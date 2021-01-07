#pragma once


#include "InceptionEngine.h"
#include <iostream>
#include "Interfaces/IHitable.h"

using namespace inceptionengine;

struct ActionStates
{
	bool CanMove() const
	{
		return !isAttacking &&
			!isEquipping &&
			!isBlocking &&
			!isRolling &&
			!isBeingAttacked;
	}

	bool CanAttack() const
	{
		return (
			!isEquipping &&
			!isBlocking &&
			!isRolling &&
			!isBeingAttacked) && (!isAttacking || isInComboCritical);
	}
	bool CanRoll() const
	{
		return CanMove();
	}
	bool CanBlock() const
	{
		return CanMove();
	}
	bool CanEquip() const
	{
		return CanMove();
	}

	bool isAttacking = false;
	bool isInComboCritical = false;

	bool isEquipping = false;
	bool isBlocking = false;
	bool isRolling = false;
	bool isBeingAttacked = false;
};

class SiceScript : public NativeScript, public IHitable
{
public:
	SiceScript(EntityID entityID, std::reference_wrapper<World> world, EntityID swordID)
		:NativeScript(entityID, world), mSwordID(swordID)
	{
		
		BindKeyInputCallback(KeyInputTypes::Keyboard_D, std::bind(&SiceScript::OnKey_D, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_A, std::bind(&SiceScript::OnKey_A, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_W, std::bind(&SiceScript::OnKey_W, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_S, std::bind(&SiceScript::OnKey_S, this, std::placeholders::_1));

		BindKeyInputCallback(KeyInputTypes::Keyboard_1, std::bind(&SiceScript::OnKey_1, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_2, std::bind(&SiceScript::OnKey_2, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_3, std::bind(&SiceScript::OnKey_3, this, std::placeholders::_1));

		BindKeyInputCallback(KeyInputTypes::Keyboard_Space, std::bind(&SiceScript::OnSpace, this, std::placeholders::_1));

		BindKeyInputCallback(KeyInputTypes::Mouse_Left, std::bind(&SiceScript::OnMouse_Left, this, std::placeholders::_1));

		BindKeyInputCallback(KeyInputTypes::Mouse_Right, std::bind(&SiceScript::OnMouse_Right, this, std::placeholders::_1));

		BindKeyInputCallback(KeyInputTypes::Mouse_Scroll, std::bind(&SiceScript::OnMouse_Scroll, this, std::placeholders::_1));

	}

	bool InBattleMode() const { return mInBattleMode; }

	virtual void GetHit(IHitable* attacker) override
	{
		if (mIsPerfectBlocking && attacker != nullptr)
		{
			attacker->BeBlocked();
			BlockSuccess();

		}
	}

	virtual void BeBlocked() override
	{
		;
	}


private:
	void BlockSuccess()
	{
		mIsPerfectBlocking = false;
		float currentSecond = GetEntity().GetComponent<AnimationComponent>().GetCurrentEventAnimTime();
		GetEntity().GetComponent<AnimationComponent>().InsertEventAnimSpeedRangeSecond(currentSecond, currentSecond + 0.03f, 0.15f);
		GetEntity().GetComponent<AudioComponent>().PlaySound2D("StandAloneResource\\thinsword\\sword_block2.WAV");
	}

	virtual void OnBegin() override
	{
		GetEntity().GetComponent<AnimationComponent>().SetAimIkChain({ "Bip001 Spine1", "Bip001 Neck", "Bip001 Head" },	
																	 { 0.2f, 0.4f, 1.0f });
	}

	virtual void OnTick(float dt) override
	{
		
		mAimIkTimer += dt;
		if (mAimIkTimer >= mTestAimIkInterval)
		{
			mAimIkTimer = fmodf(mAimIkTimer, mTestAimIkInterval);
			if (GetEntity().GetComponent<AnimationComponent>().GetCurrentAsmActiveState() == 0 &&
				GetEntity().GetComponent<AnimationComponent>().GetCurrentAsmActiveStateRunningSecond() > 5.0f)
			{
				Vec3f modelForward = NormalizeVec(GetEntity().GetComponent<TransformComponent>().GetGlobalForward());
				Vec3f cameraForward = -GetEntity().GetComponent<CameraComponent>().GetForwardVec();
				if (std::abs(RadsBetween(modelForward, cameraForward)) < PI / 3.0f)
				{
					AimToCamera();
				}
				else
				{
					GetEntity().GetComponent<AnimationComponent>().DeactivateAimIk(0.3f);
				}
			}
		}

	}

	virtual void OnMouseDeltaPos(MouseDeltaPos mouseDeltaPos) override
	{
		if (mUseMouseToControlCamera)
		{
			GetEntity().GetComponent<CameraComponent>().RotateVertical(-mouseDeltaPos.deltaXPos * mCameraRotateVerticalSpeed);
			GetEntity().GetComponent<CameraComponent>().RotateHorizontal(-mouseDeltaPos.deltaYPos * mCameraRotateHoritonzallSpeed);
		}

	}



	void OnKey_1(bool press)
	{
		if (press)
		{
			mInBattleMode = !mInBattleMode;
			//GetEntity().GetComponent<AnimationComponent>().TestAimAxis();
			//GetEntity().GetWorld().GetEntity(mSwordID).GetComponent<SkeletalMeshComponent>().SetVisibility(mInBattleMode);
		}
		
	}


	void AimToCamera()
	{
		Vec4f cameraPosition = GetEntity().GetComponent<CameraComponent>().GetCameraRefPosition();
		Matrix4x4f modelTranslate = Translate(GetEntity().GetComponent<TransformComponent>().GetWorldTransform()[3]);
		GetEntity().GetComponent<AnimationComponent>().ChainAimToInDuration(modelTranslate * cameraPosition, Vec3f(50.0f, 0.0f, 0.0f), 0.3f);
	}


	void OnKey_3(bool press)
	{
		if (press)
		{
			
			if (!GetEntity().GetComponent<AnimationComponent>().IsAimIkActive())
			{
				Vec4f cameraPosition = GetEntity().GetComponent<CameraComponent>().GetCameraRefPosition();
				//GetEntity().GetComponent<AnimationComponent>().TestAimAxis();
				Matrix4x4f modelTranslate = Translate(GetEntity().GetComponent<TransformComponent>().GetWorldTransform()[3]);
				GetEntity().GetComponent<AnimationComponent>().ChainAimToInDuration(modelTranslate * cameraPosition, Vec3f(50.0f, 0.0f, 0.0f), 0.6f);
			}
			else
			{
				GetEntity().GetComponent<AnimationComponent>().DeactivateAimIk();
			}
			
		}

	}


	void OnKey_2(bool press)
	{
		if (press)
		{
			mUseMouseToControlCamera = !mUseMouseToControlCamera;
			InceptionEngine::GetInstance().SetMouseVisibility(!mUseMouseToControlCamera);
		}
	}



	void OnKey_W(bool press)
	{
		mPressedW = press;
		Vec3f cameraForward = GetEntity().GetComponent<CameraComponent>().GetForwardVec();
		ControlMovement(press, cameraForward);
	}

	void OnKey_A(bool press)
	{
		mPressedA = press;
		Vec3f cameraForward = GetEntity().GetComponent<CameraComponent>().GetForwardVec();
		Vec3f rotateToVec = RotateVec(cameraForward, 90.0f, Vec3f(0.0f, 1.0f, 0.0f));
		ControlMovement(press, rotateToVec);
	}

	void OnKey_S(bool press)
	{
		mPressedS = press;
		Vec3f cameraForward = GetEntity().GetComponent<CameraComponent>().GetForwardVec();
		Vec3f rotateToVec = RotateVec(cameraForward, 180.0f, Vec3f(0.0f, 1.0f, 0.0f));
		ControlMovement(press, rotateToVec);
	}

	void OnKey_D(bool press)
	{
		mPressedD = press;
		Vec3f cameraForward = GetEntity().GetComponent<CameraComponent>().GetForwardVec();
		Vec3f rotateToVec = RotateVec(cameraForward, -90.0f, Vec3f(0.0f, 1.0f, 0.0f));
		ControlMovement(press, rotateToVec);
	}

	void ControlMovement(bool start, Vec3f const& rotateToVec)
	{
		if (start)
		{
			if (mSiceActionState.CanMove())
			{
				GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, mMaxWalkSpeed });
				GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(true);
				GetEntity().GetComponent<TransformComponent>().RotateForwardVecToInDuration(rotateToVec, 0.25f);
			}
			
		}
		else if (!mPressedA && !mPressedD && !mPressedS && !mPressedW && mAttackState == 0)
		{
			GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 0.0f });
			GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(false);
		}
	}

	void OnMouse_Scroll(bool scrollUp)
	{
		if (scrollUp)
		{
			GetEntity().GetComponent<CameraComponent>().MoveForward(20.0f);
		}
		else
		{
			GetEntity().GetComponent<CameraComponent>().MoveForward(-20.0f);
		}
	}

	void StartAction(bool rotateToForward)
	{
		if(rotateToForward) GetEntity().GetComponent<TransformComponent>().RotateForwardVecToInDuration(GetEntity().GetComponent<CameraComponent>().GetForwardVec(), 0.15f);
		GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 0.0f });
		GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(true);
	}

	void AttackDetection()
	{
		Vec3f bottom = GetEntity().GetComponent<SkeletalMeshComponent>().GetSocketGlobalTransform("SwordStart")[3];
		Vec3f top = GetEntity().GetComponent<SkeletalMeshComponent>().GetSocketGlobalTransform("SwordEnd")[3];
		std::vector<SphereTraceResult> traceRes = GetEntity().GetWorld().SphereTrace(bottom, top, 10.0f);
		if (traceRes.size() > 0)
		{
			float currentSecond = GetEntity().GetComponent<AnimationComponent>().GetCurrentEventAnimTime();
			GetEntity().GetComponent<AnimationComponent>().InsertEventAnimSpeedRangeSecond(currentSecond, currentSecond + 0.03f, 0.1f);
		}
		for (SphereTraceResult const& res : traceRes)
		{
			EntityID hitEntity = res.entityID;
			if (GetEntity().GetWorld().GetEntity(hitEntity).HasComponent<NativeScriptComponent>())
			{
				IHitable* hitable = dynamic_cast<IHitable*>(GetEntity().GetWorld().GetEntity(hitEntity).GetComponent<NativeScriptComponent>().GetScript());
				if (hitable != nullptr)
				{
					hitable->GetHit(dynamic_cast<IHitable*>(this));
				}
				else
				{
					std::cout << "hit is nullptr\n";
				}
			}

		}
	}

	void RestoreMovement()
	{
		if (mSiceActionState.CanMove())
		{
			
			if (mPressedA || mPressedD || mPressedS || mPressedW)
			{
				Vec3f cameraForward = GetEntity().GetComponent<CameraComponent>().GetForwardVec();
				Vec3f rotateToVec;

				if (mPressedW) rotateToVec = cameraForward;
				else if (mPressedA)  rotateToVec = RotateVec(cameraForward, 90.0f, Vec3f(0.0f, 1.0f, 0.0f));
				else if (mPressedS) rotateToVec = RotateVec(cameraForward, 180.0f, Vec3f(0.0f, 1.0f, 0.0f));
				else if (mPressedD) rotateToVec = RotateVec(cameraForward, -90.0f, Vec3f(0.0f, 1.0f, 0.0f));
				GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, mMaxWalkSpeed });
				GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(true);
				GetEntity().GetComponent<TransformComponent>().RotateForwardVecToInDuration(rotateToVec, 0.25f);
			}
			else
			{
				GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 0.0f });
			}
			
		}
	}

	void OnMouse_Left(bool press)
	{
		if (press && mInBattleMode && mSiceActionState.CanAttack())
		{
			
			StartAction(true);
			mSiceActionState.isInComboCritical = false;

			EventAnimPlaySetting setting;
			setting.animFilePath = mAttackAnims[mAttackState];
			AnimSpeedRange range1;
			range1.startRatio = 0.0f;
			range1.endRatio = 1.0f;
			range1.playSpeed = mAttackSpeed[mAttackState];
			setting.animSpeedRanges = { range1 };
	
			AnimNotify attackDetection;
			attackDetection.ratio = 8.0f / 14.0f;
			attackDetection.notify = std::bind(&SiceScript::AttackDetection, this);
			setting.animNotifies.push_back(attackDetection);


			if (mAttackState <= mAttackAnims.size() - 2)
			{
				AnimNotify comboStartNotify;
				comboStartNotify.ratio = mComboStartRatio[mAttackState];
				comboStartNotify.notify = [&]() {mSiceActionState.isInComboCritical = true; };
				setting.animNotifies.push_back(comboStartNotify);
			}


			setting.animStartCallback = [&]() {mSiceActionState.isAttacking = true; };
			//setting.animInterruptCallback = [&]() {std::cout<<"iterrupt\n"; mAttackState = 0; mSiceActionState.isAttacking = false; mSiceActionState.isInComboCritical = false; };
			setting.animEndCallback = std::bind(&SiceScript::ReturnFromAttack, this, mAttackState);


			mAttackState += 1;
			mAttackState = mAttackState % mAttackAnims.size();

			GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);
		}
	}

	void ReturnFromAttack(int attackState)
	{
		EventAnimPlaySetting returnPlaySetting;
		returnPlaySetting.animFilePath = mAttackReturnAnims[attackState];
		returnPlaySetting.animEndCallback = [&]()
		{
			mAttackState = 0;
			GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(false);
			mSiceActionState.isAttacking = false;
			RestoreMovement();
		};

		if (attackState <= mAttackAnims.size() - 2)
		{
			AnimNotify endComboNotify;
			endComboNotify.ratio = mComboEndRatio[attackState];
			endComboNotify.notify = [&]() {mSiceActionState.isInComboCritical = false; };

			returnPlaySetting.animNotifies.push_back(endComboNotify);
		}

		//returnPlaySetting.animInterruptCallback = [&]() {mAttackState = 0; mSiceActionState.isAttacking = false; mSiceActionState.isInComboCritical = false; };
		returnPlaySetting.blendOutDuration = 0.05f;
		
		AnimSpeedRange range;
		range.startRatio = 0.0f;
		range.endRatio = 1.0f;
		range.playSpeed = 1.2f;
		returnPlaySetting.animSpeedRanges.push_back(range);

		GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(returnPlaySetting);
	}

	void OnSpace(bool press)
	{
		if (press && mInBattleMode && mSiceActionState.CanRoll())
		{
			StartAction(false);

			mSiceActionState.isRolling = true;
			EventAnimPlaySetting rollSetting;
			rollSetting.animFilePath = "StandAloneResource\\sice\\sice_roll.ie_anim";
		
			rollSetting.animStartCallback = [&]() {GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 300.0f }); };
			
			rollSetting.animEndCallback = [&]()
			{
				GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(false);
				mSiceActionState.isRolling = false;
				RestoreMovement();
			};

			AnimNotify movePoint1;
			movePoint1.ratio = 4.0f / 15.0f;
			movePoint1.notify = [&]() {GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 1200.0f }); };
			rollSetting.animNotifies.push_back(movePoint1);

			AnimNotify movePoint2;
			movePoint2.ratio = 9.0f / 15.0f;
			movePoint2.notify = [&]() {GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 700.0f }); };
			rollSetting.animNotifies.push_back(movePoint2);

			AnimNotify movePoint3;
			movePoint3.ratio = 12.0f / 15.0f;
			movePoint3.notify = [&]() {GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 300.0f }); };
			rollSetting.animNotifies.push_back(movePoint3);
			rollSetting.blendOutDuration = 0.08f;


			AnimSpeedRange range;
			range.startRatio = 0.0f;
			range.endRatio = 1.0f;
			range.playSpeed = 0.8f;
			rollSetting.animSpeedRanges.push_back(range);

			GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(rollSetting);
		}
	}

	void OnMouse_Right(bool press)
	{
		if (press && mInBattleMode && mSiceActionState.CanBlock())
		{
			StartAction(true);

			//mAttackState = 0;

			mSiceActionState.isBlocking = true;
			EventAnimPlaySetting setting;
			setting.animFilePath = "StandAloneResource\\sice\\sice_block.ie_anim";

			AnimNotify startBlock;
			startBlock.ratio = 6.0f / 17.0f;
			startBlock.notify = [&]() {mIsPerfectBlocking = true; };

			AnimNotify endBlock;
			endBlock.ratio = 15.0f / 17.0f;
			endBlock.notify = [&]() {mIsPerfectBlocking = false; GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(false); };

			setting.animNotifies.push_back(startBlock);
			setting.animNotifies.push_back(endBlock);
			setting.blendOutDuration = 0.2f;

			setting.animInterruptCallback = [&]() {mIsPerfectBlocking = false; GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(false); };
			setting.animBlendOutFinishCallback = [&]() {mSiceActionState.isBlocking = false; };

			AnimSpeedRange range;
			range.startRatio = 0.0f;
			range.endRatio = 1.0f;

			range.playSpeed = 1.4f;
			setting.animSpeedRanges.push_back(range);

			GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);
		}
	}

private:
	std::array<std::string, 4> const mAttackAnims =
	{
		"StandAloneResource\\sice\\sice_combo_a1.ie_anim",
		"StandAloneResource\\sice\\sice_combo_a2.ie_anim",
		"StandAloneResource\\sice\\sice_combo_a3.ie_anim",
		"StandAloneResource\\sice\\sice_combo_a4.ie_anim",
	};
	std::array<std::string, 4> const mAttackReturnAnims =
	{
		"StandAloneResource\\sice\\sice_return_a1.ie_anim",
		"StandAloneResource\\sice\\sice_return_a2.ie_anim",
		"StandAloneResource\\sice\\sice_return_a3.ie_anim",
		"StandAloneResource\\sice\\sice_return_a4.ie_anim",
	};

	std::array<float, 4> mAttackSpeed = { 0.6, 0.4, 0.7, 0.7 };
	std::array<float, 4> mComboStartRatio = { 12.0f / 14.0f, 8.0f / 10.0f,  12.0f / 16.0f, 1.0f };
	std::array<float, 4> mComboEndRatio = { 3.0f / 20.0f, 3.0f / 20.0f, 1.0f / 20.0f, 0.0f };

	int mAttackState = 0;

	float const mMaxWalkSpeed = 300.0f;
	bool mUseMouseToControlCamera = false;
	float const mCameraRotateVerticalSpeed = 0.03f;
	float const mCameraRotateHoritonzallSpeed = 0.03f;


	bool mPressedW = false;
	bool mPressedA = false;
	bool mPressedS = false;
	bool mPressedD = false;

	bool mInBattleMode = false;

	EntityID mSwordID = InvalidEntityID;

	float mAimIkTimer = 0.0f;
	float mTestAimIkInterval = 3.0f;

	bool mIsPerfectBlocking = false;

	bool mIsDoingAction = false;

	std::function<void()> DoingAction = [&]() {std::cout << "animstart\n";  mIsDoingAction = true; };

	std::function<void()> StopDoingAction = [&]() {std::cout<<"animfinish\n"; mIsDoingAction = false; };

	ActionStates mSiceActionState;
};