#pragma once


#include "InceptionEngine.h"
#include <iostream>
#include "Interfaces/IHitable.h"

using namespace inceptionengine;

class SiceScript : public NativeScript
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
	}

	bool InBattleMode() const { return mInBattleMode; }


private:
	
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
			if (mAttackState == 0)
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



	void OnMouse_Left(bool press)
	{
		if (press)
		{
			GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 0.0f });
			GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(true);

			//GetEntity().GetComponent<AudioComponent>().PlaySound2D("StandAloneResource\\sice\\explosion.wav");

			EventAnimPlaySetting setting;
			setting.animFilePath = attacks[mAttackState];
			AnimSpeedRange range1;
			range1.startRatio = 0.0f;
			range1.endRatio = 1.0f;
			range1.playSpeed = attackSpeed[mAttackState];
			setting.animSpeedRanges = { range1 };
			setting.blendOutDuration = 0.3f;
			if (mAttackState == 0)
			{
				AnimNotify attackDetection;
				attackDetection.ratio = 8.0f / 14.0f;
				attackDetection.notify = [&]()
				{
					Vec3f bottom = GetEntity().GetComponent<SkeletalMeshComponent>().GetSocketGlobalTransform("SwordStart")[3];
					Vec3f top = GetEntity().GetComponent<SkeletalMeshComponent>().GetSocketGlobalTransform("SwordEnd")[3];
					std::vector<SphereTraceResult> traceRes = GetEntity().GetWorld().SphereTrace(bottom, top, 10.0f);
					if (traceRes.size() > 0)
					{
						float currentRatio = GetEntity().GetComponent<AnimationComponent>().GetCurrentEventAnimRatio();
						GetEntity().GetComponent<AnimationComponent>().InsertEventAnimSpeedRange(currentRatio, currentRatio + 0.04f, 0.1f);
					} 
					for (SphereTraceResult const& res : traceRes)
					{
						EntityID hitEntity = res.entityID;
						if (GetEntity().GetWorld().GetEntity(hitEntity).HasComponent<NativeScriptComponent>())
						{
							IHitable* hitable = dynamic_cast<IHitable*>(GetEntity().GetWorld().GetEntity(hitEntity).GetComponent<NativeScriptComponent>().GetScript());
							if (hitable != nullptr)
							{
								hitable->GetHit();
							}
							else
							{
								std::cout << "hit is nullptr\n";
							}
						}

					}
				};

				setting.animNotifies.push_back(attackDetection);
			}

			mAttackState += 1;
			if (mAttackState >= attacks.size()) mAttackState = 0;
			setting.animEndCallback = [&]() {mAttackState = 0; GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(false); };

			GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);
		}
	}

	void OnSpace(bool press)
	{
		if (press)
		{
			//GetEntity().GetComponent<AnimationComponent>().StopAnimation();
		}
	}

private:
	std::array<std::string, 4> attacks =
	{
		"StandAloneResource\\sice\\sice_combo_a1.ie_anim",
		"StandAloneResource\\sice\\sice_combo_a2.ie_anim",
		"StandAloneResource\\sice\\sice_combo_a3.ie_anim",
		"StandAloneResource\\sice\\sice_combo_a4.ie_anim",
	};
	std::array<float, 4> attackSpeed = { 0.6, 0.4, 0.7, 0.7 };

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
};