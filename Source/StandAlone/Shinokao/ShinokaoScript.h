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
		
	}

public:
	virtual void GetHit(IHitable* attacker, float damage) override
	{
		EventAnimPlaySetting setting;
		setting.animFilePath = "StandAloneResource\\shinokao\\shinokao_gethit_2.ie_anim";
		setting.animStartCallback = [&]() {mCanMove = false; };
		setting.animEndCallback = [&]() {mCanMove = true; };

		AnimNotify startMove;
		startMove.ratio = 1.5f / 13.0f;
		startMove.notify = [&, damage]() { GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, -30.0f * damage }); };
		setting.animNotifies.push_back(startMove);

		AnimNotify endMove;
		endMove.ratio = 7.0f / 13.0f;
		endMove.notify = [&]() {GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 0.0f }); };
		setting.animNotifies.push_back(endMove);

		GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);

		GetEntity().GetComponent<AnimationComponent>().InsertEventAnimSpeedRangeSecond(0.01f, 0.01 + 0.03f * damage, 0.1f / damage);

		//std::cout << "I'm hit!\n";
	}

	virtual void BeBlocked() override
	{
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
		mAttack3Timer += dt;

		if (mAttack3Timer >= 3.0f && mCanMove)
		{
			mAttack3Timer = std::fmodf(mAttack3Timer, 3.0f);
			Attack3();
			/*
			if (NearMainCharacter())
			{
				GetEntity().GetComponent<RigidbodyComponent>().SetVelocity(Vec3f(0.0f, 0.0f, 0.0f));
				Attack3();
			}
			else
			{
				GetEntity().GetComponent<TransformComponent>().RotateForwardVecToInDuration(DirectionToMainCharacter(), 0.05f);
				GetEntity().GetComponent<RigidbodyComponent>().SetVelocity(Vec3f(0.0f, 0.0f, 500.0f));
			}*/

		}


	}

	bool NearMainCharacter()
	{
		float distance = VecLength((GetEntity().GetWorld().GetEntity(mMainCharacterID).GetComponent<TransformComponent>().GetWorldPosition() - GetEntity().GetComponent<TransformComponent>().GetWorldPosition()));
		float capsuleRadius = GetEntity().GetWorld().GetEntity(mMainCharacterID).GetComponent<RigidbodyComponent>().GetCapsuleRadius() + GetEntity().GetComponent<RigidbodyComponent>().GetCapsuleRadius();
		return distance <= capsuleRadius + 50.0f;
	}

	Vec3f DirectionToMainCharacter()
	{
		return NormalizeVec((GetEntity().GetWorld().GetEntity(mMainCharacterID).GetComponent<TransformComponent>().GetWorldPosition() - GetEntity().GetComponent<TransformComponent>().GetWorldPosition()));
	}

	void MoveToMainCharacterInDuration()
	{

	}

	void Attack3()
	{
		EventAnimPlaySetting setting;
		setting.animFilePath = "StandAloneResource\\shinokao\\shinokao_attack_3.ie_anim";;

		AnimNotify attackDetection;
		attackDetection.ratio = 27.0f / 60.0f;
		attackDetection.notify = std::bind(&ShinokaoScript::AttackDetection, this);

		setting.animNotifies.push_back(attackDetection);

		setting.animStartCallback = [&]() {mCanMove = false; };
		setting.animEndCallback = [&]() {mCanMove = true; };
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


private:

	float const mMaxWalkSpeed = 150.0f;

	float mAttack1Timer = 0.0f;

	float mAttack2Timer = 0.0f;

	float mAttack3Timer = 0.0f;

	float mAttack4Timer = 0.0f;

	float const mAttack1CD = 3.0f;
	float const mAttack2CD = 6.0f;
	float const mAttack3CD = 9.0f;
	float const mAttack4CD = 12.0f;

	EntityID mMainCharacterID = 0;

	bool mIsInBattleMode = true;

	bool mCanMove = true;


};