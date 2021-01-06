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
	virtual void GetHit() override
	{
		EventAnimPlaySetting setting;
		setting.animFilePath = attacks[0];
		AnimSpeedRange range1;
		range1.startRatio = 0.0f;
		range1.endRatio = 1.0f;
		range1.playSpeed = attackSpeed[0];
		setting.animSpeedRanges = { range1 };

		//GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);
		std::cout << "I'm hit!\n";
	}


private:

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
			GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, mMaxWalkSpeed });
			GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(true);
			GetEntity().GetComponent<TransformComponent>().RotateForwardVecToInDuration(rotateToVec, 0.5f);
		}
		else if (!mPressedA && !mPressedD && !mPressedS && !mPressedW)
		{
			GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 0.0f });
			GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(false);
		}
	}



private:
	std::array<std::string, 5> attacks =
	{
		"StandAloneResource\\milia\\milia_combo_a1.ie_anim",
		"StandAloneResource\\milia\\milia_combo_a2.ie_anim",
		"StandAloneResource\\milia\\milia_combo_a3.ie_anim",
		"StandAloneResource\\milia\\milia_combo_a4.ie_anim",
		"StandAloneResource\\milia\\milia_combo_a5.ie_anim"
	};
	std::array<float, 5> attackSpeed = { 0.85, 0.85, 0.65, 0.7, 0.7 };

	int mAttackState = 0;

	float const mMaxWalkSpeed = 150.0f;



	bool mPressedW = false;
	bool mPressedA = false;
	bool mPressedS = false;
	bool mPressedD = false;
};