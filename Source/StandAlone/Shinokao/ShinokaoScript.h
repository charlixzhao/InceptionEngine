#pragma once

#include "InceptionEngine.h"
#include <iostream>

#include "Interfaces/IHitable.h"

using namespace inceptionengine;

class ShinokaoScript : public NativeScript, public IHitable
{
public:
	ShinokaoScript(EntityID entityID, std::reference_wrapper<World> world)
		:NativeScript(entityID, world)
	{
		BindKeyInputCallback(KeyInputTypes::Keyboard_P, [&](bool press) { if (press) mCanMove = true; });

		BindKeyInputCallback(KeyInputTypes::Keyboard_O, [&](bool press) { if (press) exit(0); });
	}

public:

	
	virtual void GetHit(IHitable* attacker, float damage) override
	{
		if (!mIsInGoldBody)
		{

			HP -= 1.0f;

			if (damage > 1.1f)
			{
				GetEntity().GetComponent<AudioComponent>().PlaySound2D("StandAloneResource\\shinokao\\gethit_heavy.wav");
			}
			else
			{
				GetEntity().GetComponent<AudioComponent>().PlaySound2D("StandAloneResource\\shinokao\\gethit_light.wav");
			}

			GetEntity().GetComponent<TransformComponent>().RotateForwardVecToInDuration(DirectionToMainCharacter(), 0.05f);
			GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 0.0f });

			mMoveTimer = 0.0f;

			EventAnimPlaySetting setting;
			setting.animFilePath = "StandAloneResource\\shinokao\\shinokao_gethit_2.ie_anim";
			setting.animStartCallback = [&]() {mCanMove = false; };
			setting.animEndCallback = [&]() {mCanMove = true; };
			AnimSpeedRange range;
			range.startRatio = 0.0f;
			range.endRatio = 1.0f;
			range.playSpeed = 1.2f;
			setting.animSpeedRanges.push_back(range);

			AnimNotify startMove;
			startMove.ratio = 1.5f / 13.0f;
			startMove.notify = [&, damage]() { GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, -10.0f * damage }); };
			setting.animNotifies.push_back(startMove);

			AnimNotify endMove;
			endMove.ratio = 7.0f / 13.0f;
			endMove.notify = [&]() {GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 0.0f }); };
			setting.animNotifies.push_back(endMove);
			setting.blendInDuration = 0.1f;

			setting.animInterruptCallback = [&]()
			{

				if (RandTest(0.4f))
				{
					mCanMove = true;
					mIsInGoldBody = true;
					mMoveTimer += mMoveCD;
					int randint = rand() % 4;
					switch (randint)
					{
						case 0: mAttack1Timer += mAttack1CD; break;
						case 1: mAttack2Timer += mAttack2CD; break;
						case 2: mAttack3Timer += mAttack3CD; break;
						case 3: mAttack4Timer += mAttack4CD; break;
						default: throw std::runtime_error("impossible math result\n"); break;
					}
				}
	
			};

			GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);

			GetEntity().GetComponent<AnimationComponent>().InsertEventAnimSpeedRangeSecond(1.0f/18.0f, 1.0f / 18.0f + 0.03f * damage, 0.1f / damage);

		}
		else
		{
			GetEntity().GetComponent<AudioComponent>().PlaySound2D("StandAloneResource\\shinokao\\gethit_goldbody.wav");
			HP -= 0.5f;
		}

		std::cout << "Shinokao HP is now " << HP << std::endl;

		if (HP < 0)
		{
			std::cout << "ÄãÓ®ÁË£¡£¡£¡\n";
			exit(0);
		}

	}

	virtual void BeBlocked() override
	{
		mIsInGoldBody = false;
		mMoveTimer = 0.0f;

		EventAnimPlaySetting setting;
		setting.animFilePath = "StandAloneResource\\shinokao\\shinokao_beblocked.ie_anim";
		setting.blendOutDuration = 0.3f;

		setting.animStartCallback = [&]() {mCanMove = false; };
		setting.animEndCallback = [&]() {mCanMove = true; };

		AnimNotify startMove;
		startMove.ratio = 1.0f / 8.0f;
		startMove.notify = [&]() {GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, -30.0f }); };
		setting.animNotifies.push_back(startMove);

		AnimNotify endMove;
		endMove.ratio = 6.0f / 8.0f;
		endMove.notify = [&]() {GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 0.0f }); };
		setting.animNotifies.push_back(endMove);


		GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);
		GetEntity().GetComponent<AnimationComponent>().InsertEventAnimSpeedRangeSecond(0.0f, 0.03f, 0.08f);
	}




private:

	virtual void OnTick(float dt) override
	{
	
		if (mMoveToCountDown > 0)
		{
			mMoveToCountDown -= dt;

			GetEntity().GetComponent<TransformComponent>().RotateForwardVecToInDuration(DirectionToMainCharacter(), 0.05f);
			
			if (mMoveToCountDown < 0.0f)
			{
				mMoveToCountDown = 0.0f;
				mMoveToFinishCallback();
				return;
			}
			else if (NearMainCharacter())
			{
				mMoveToCountDown = 0.0f;
				mMoveToFinishCallback();
				return;
			}

			return;
		}

		UpdateTimer(dt);


		if (mCanMove)
		{
			if (mMoveTimer >= mMoveCD && AttackCDFinish())
			{
				mMoveTimer = std::fmodf(mMoveTimer, mMoveCD);


				float distance = VecLength(PositionNearMainCharacter() - GetEntity().GetComponent<TransformComponent>().GetWorldPosition());
				float duration = distance / 500.0f;

				if(RandTest(0.6))
					mIsInGoldBody = true;
				GetEntity().GetComponent<TransformComponent>().RotateForwardVecToInDuration(DirectionToMainCharacter(), 0.05f, 
																							[&, duration]() 
																							{
																								if (NearMainCharacter())
																								{
																									Attack();
																								}
																								else
																								{
																									mIsInGoldBody = false;
																									MoveForDuration(500.0f, duration, [&]() {Attack(); });
																								}	
																							});

			}
			
		}

	}


	void Attack()
	{
		mCanMove = false;
		GetEntity().GetComponent<RigidbodyComponent>().SetVelocity(Vec3f(0.0f, 0.0f, 0.0f));
		GetEntity().GetComponent<TransformComponent>().RotateForwardVecToInDuration(DirectionToMainCharacter(), 0.05f,
																					[&]()
																					{
																						mCanMove = true;
																						//attack logic
																						
																						if (mAttack4Timer >= mAttack4CD)
																						{
																							mAttack4Timer = std::fmodf(mAttack4Timer, mAttack4CD);
																							Attack4();
																						}
																						else if (mAttack3Timer >= mAttack3CD)
																						{
																							mAttack3Timer = std::fmodf(mAttack3Timer, mAttack3CD);
																							Attack3();
																						}
																						else if (mAttack2Timer >= mAttack2CD)
																						{
																							mAttack2Timer = std::fmodf(mAttack2Timer, mAttack2CD);
																							Attack2();
																						}
																						else if (mAttack1Timer >= mAttack1CD)
																						{
																							mAttack1Timer = std::fmodf(mAttack1Timer, mAttack1CD);
																							Attack1();
																						}
																					});
	}

	bool AttackCDFinish() const
	{
		return (mAttack4Timer >= mAttack4CD) ||
			(mAttack3Timer >= mAttack3CD) ||
			(mAttack2Timer >= mAttack2CD) ||
			(mAttack1Timer >= mAttack1CD);
	}

	bool NearMainCharacter()
	{
		float distance = VecLength((GetEntity().GetWorld().GetEntity(mMainCharacterID).GetComponent<TransformComponent>().GetWorldPosition() - GetEntity().GetComponent<TransformComponent>().GetWorldPosition()));
		float capsuleRadius = GetEntity().GetWorld().GetEntity(mMainCharacterID).GetComponent<RigidbodyComponent>().GetCapsuleRadius() + GetEntity().GetComponent<RigidbodyComponent>().GetCapsuleRadius();
		return distance <= capsuleRadius + 100.0f;
	}

	float DistanceToMainCharacter()
	{
		return VecLength((GetEntity().GetWorld().GetEntity(mMainCharacterID).GetComponent<TransformComponent>().GetWorldPosition() - GetEntity().GetComponent<TransformComponent>().GetWorldPosition()));
	}

	Vec3f DirectionToMainCharacter()
	{
		return NormalizeVec((GetEntity().GetWorld().GetEntity(mMainCharacterID).GetComponent<TransformComponent>().GetWorldPosition() - GetEntity().GetComponent<TransformComponent>().GetWorldPosition()));
	}

	Vec3f PositionNearMainCharacter()
	{
		float capsuleRadius = GetEntity().GetWorld().GetEntity(mMainCharacterID).GetComponent<RigidbodyComponent>().GetCapsuleRadius() + GetEntity().GetComponent<RigidbodyComponent>().GetCapsuleRadius();
		return GetEntity().GetWorld().GetEntity(mMainCharacterID).GetComponent<TransformComponent>().GetWorldPosition() + (capsuleRadius + 50.0f) * -DirectionToMainCharacter();
	}


	void MoveToInSpeed(Vec3f const& position, float speed, std::function<void()> finishCallback)
	{
		mMoveToPosition = position;
		mMoveToSpeed = speed;
		mMoveToFinishCallback = finishCallback;
		Vec3f direction = NormalizeVec(position - GetEntity().GetComponent<TransformComponent>().GetWorldPosition());
		GetEntity().GetComponent<TransformComponent>().RotateForwardVecToInDuration(direction, 0.05f, [&]() {GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f,mMoveToSpeed }); });
	}

	void MoveForDuration(float speed, float duration, std::function<void()> finishCallback)
	{

		mMoveToCountDown = duration;
		mMoveToFinishCallback = finishCallback;
		GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f,speed });

		
	}

	Vec3f mMoveToPosition;
	float mMoveToSpeed = 0.0f;
	float mMoveToCountDown = 0.0f;
	std::function<void()> mMoveToFinishCallback = []() {; };

	void Attack1()
	{
		EventAnimPlaySetting setting;
		setting.animFilePath = "StandAloneResource\\shinokao\\shinokao_attack_1.ie_anim";

		AnimSpeedRange range;
		range.startRatio = 0.0f;
		range.endRatio = 6.0f / 30.0f;
		range.playSpeed = 0.25f;
		setting.animSpeedRanges.push_back(range);

		AnimNotify attackDetection;
		attackDetection.ratio = 8.0f / 30.0f;
		attackDetection.notify = std::bind(&ShinokaoScript::AttackDetection, this);

		setting.animNotifies.push_back(attackDetection);

		setting.animStartCallback = StartAttack;
		setting.animEndCallback = EndAttack;
		GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);
	}

	void Attack2()
	{
		EventAnimPlaySetting setting;
		setting.animFilePath = "StandAloneResource\\shinokao\\shinokao_attack_2.ie_anim";;

		AnimSpeedRange range;
		range.startRatio = 0.0f;
		range.endRatio = 5.0f / 30.0f;
		range.playSpeed = 0.15f;
		setting.animSpeedRanges.push_back(range);

		AnimNotify attackDetection;
		attackDetection.ratio = 6.5f / 30.0f;
		attackDetection.notify = std::bind(&ShinokaoScript::AttackDetection, this);

		setting.animNotifies.push_back(attackDetection);

		setting.animStartCallback = StartAttack;
		setting.animEndCallback = EndAttack;
		GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);
	}
	
	void Attack3()
	{
		EventAnimPlaySetting setting;
		setting.animFilePath = "StandAloneResource\\shinokao\\shinokao_attack_3.ie_anim";;

		AnimNotify attackDetection;
		attackDetection.ratio = 27.0f / 60.0f;
		attackDetection.notify = std::bind(&ShinokaoScript::AttackDetection, this);

		setting.animNotifies.push_back(attackDetection);

		AnimNotify attackDetection2;
		attackDetection2.ratio = 41.5f / 60.0f;
		attackDetection2.notify = std::bind(&ShinokaoScript::AttackDetection, this);
		setting.animNotifies.push_back(attackDetection2);

		setting.animStartCallback = StartAttack;
		setting.animEndCallback = EndAttack;
		GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);
	}

	void Attack4()
	{
		EventAnimPlaySetting setting;
		setting.animFilePath = "StandAloneResource\\shinokao\\shinokao_attack_4.ie_anim";;

		AnimNotify attackDetection;
		attackDetection.ratio = 49.0f / 79.0f;
		attackDetection.notify = std::bind(&ShinokaoScript::AttackDetection, this);
		setting.animNotifies.push_back(attackDetection);

		AnimNotify attackDetection2;
		attackDetection2.ratio = 57.0f / 79.0f;
		attackDetection2.notify = std::bind(&ShinokaoScript::AttackDetection, this);
		setting.animNotifies.push_back(attackDetection2);

		setting.animStartCallback = StartAttack;
		setting.animEndCallback = EndAttack;
		GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);
	}

	
	void AttackDetection()
	{
		//Vec3f bottom = GetEntity().GetComponent<SkeletalMeshComponent>().GetSocketGlobalTransform("SwordStart")[3];
		//Vec3f top = GetEntity().GetComponent<SkeletalMeshComponent>().GetSocketGlobalTransform("SwordEnd")[3];

		float capsuleRadius = GetEntity().GetComponent<RigidbodyComponent>().GetCapsuleRadius();
		Vec3f bottom = GetEntity().GetComponent<TransformComponent>().GetWorldPosition() + capsuleRadius * 1.7f * NormalizeVec(GetEntity().GetComponent<TransformComponent>().GetGlobalForward());
		Vec3f top = bottom;
		top.y = 200.0f;
		std::vector<SphereTraceResult> traceRes = GetEntity().GetWorld().SphereTrace(bottom, top, 150.0f);

		for (SphereTraceResult const& res : traceRes)
		{
			EntityID hitEntity = res.entityID;
			//ignore self
			if (hitEntity != GetEntity().GetID())
			{
				float currentSecond = GetEntity().GetComponent<AnimationComponent>().GetCurrentEventAnimTime();
				GetEntity().GetComponent<AnimationComponent>().InsertEventAnimSpeedRangeSecond(currentSecond, currentSecond + 0.03f, 0.1f);
				break;
			}

		}

		for (SphereTraceResult const& res : traceRes)
		{
			EntityID hitEntity = res.entityID;
			//ignore self
			if (hitEntity != GetEntity().GetID())
			{
				if (GetEntity().GetWorld().GetEntity(hitEntity).HasComponent<NativeScriptComponent>())
				{
					IHitable* hitable = dynamic_cast<IHitable*>(GetEntity().GetWorld().GetEntity(hitEntity).GetComponent<NativeScriptComponent>().GetScript());
					if (hitable != nullptr)
					{
						hitable->GetHit(dynamic_cast<IHitable*>(this), 0.5f);
					}
				}
			}
		}
	}

	void UpdateTimer(float dt)
	{
		mMoveTimer += dt;
		mAttack1Timer += dt;
		mAttack2Timer += dt;
		mAttack3Timer += dt;
		mAttack4Timer += dt;
	}


private:

	std::function<void()> StartAttack = [&]() {mCanMove = false; mIsInGoldBody = true; };

	std::function<void()> EndAttack = [&]() {mCanMove = true; mIsInGoldBody = false; };

	float const mMaxWalkSpeed = 150.0f;

	float mMoveTimer = 0.0f;

	float mAttack1Timer = 0.0f;

	float mAttack2Timer = 0.0f;

	float mAttack3Timer = 0.0f;

	float mAttack4Timer = 0.0f;

	float const mAttack1CD = 9.0f;
	float const mAttack2CD = 15.0f;
	float const mAttack3CD = 25.0f;
	float const mAttack4CD = 35.0f;
	float const mMoveCD = 1.0f;

	EntityID mMainCharacterID = 0;

	bool mIsInBattleMode = true;

	bool mCanMove = false;

	bool mIsInGoldBody = false;


	float HP = 30.0f;

};