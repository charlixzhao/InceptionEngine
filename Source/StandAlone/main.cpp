

#include "InceptionEngine.h"

#include <iostream>



using namespace inceptionengine;
float TestX = -50.0f;
float TestY = 150.0f;
float TestZ = -20.0f;

Matrix4x4f TestTarget = Translate(TestX, TestY, TestZ);

class HornetScript : public NativeScript
{
public:
	HornetScript(EntityID entityID, std::reference_wrapper<World> world)
		:NativeScript(entityID, world)
	{
		BindKeyInputCallback(KeyInputTypes::Keyboard_D, std::bind(&HornetScript::OnKey_D, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_A, std::bind(&HornetScript::OnKey_A, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_W, std::bind(&HornetScript::OnKey_W, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_S, std::bind(&HornetScript::OnKey_S, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_Space, std::bind(&HornetScript::OnKey_Space, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_1, std::bind(&HornetScript::OnKey_1, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_2, std::bind(&HornetScript::OnKey_2, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_3, std::bind(&HornetScript::OnKey_3, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_4, std::bind(&HornetScript::OnKey_4, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_5, std::bind(&HornetScript::OnKey_5, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_6, std::bind(&HornetScript::OnKey_6, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Mouse_Left, std::bind(&HornetScript::OnMouse_Left, this, std::placeholders::_1));
	}


private:
	virtual void OnBegin() override
	{
		std::cout << "Hello Script!\n";
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
			GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, -200.0f });
		}
		else
		{
			GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 0.0f });
		}
		/*
		if (press)
		{
			TestX += 5;
			TestTarget = Translate(TestX, TestY, TestZ);
			GetEntity().GetWorld().GetEntity(1).GetComponent<TransformComponent>().SetWorldTransform(TestTarget * Scale(0.1f, 0.1f, 0.1f));
		}*/
	}

	
	void OnKey_2(bool press)
	{
		if (press)
		{
			mUseMouseToControlCamera = ! mUseMouseToControlCamera;
			InceptionEngine::GetInstance().SetMouseVisibility(!mUseMouseToControlCamera);
		}
	}
	void OnKey_3(bool press)
	{
		if (press)
		{
			TestY += 5;
			TestTarget = Translate(TestX, TestY, TestZ);
			GetEntity().GetWorld().GetEntity(1).GetComponent<TransformComponent>().SetWorldTransform(TestTarget * Scale(0.1f, 0.1f, 0.1f));
		}
	}
	void OnKey_4(bool press)
	{
		if (press)
		{
			TestY += -5;
			TestTarget = Translate(TestX, TestY, TestZ);
			GetEntity().GetWorld().GetEntity(1).GetComponent<TransformComponent>().SetWorldTransform(TestTarget * Scale(0.1f, 0.1f, 0.1f));
		}
	}
	void OnKey_5(bool press)
	{
		if (press)
		{
			TestZ += 5;
			TestTarget = Translate(TestX, TestY, TestZ);
			GetEntity().GetWorld().GetEntity(1).GetComponent<TransformComponent>().SetWorldTransform(TestTarget * Scale(0.1f, 0.1f, 0.1f));
		}
	}
	void OnKey_6(bool press)
	{
		if (press)
		{
			TestZ += -5;
			TestTarget = Translate(TestX, TestY, TestZ);
			GetEntity().GetWorld().GetEntity(1).GetComponent<TransformComponent>().SetWorldTransform(TestTarget * Scale(0.1f, 0.1f, 0.1f));
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
			GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, mMaxWalkSpeed });
			GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(true);
			GetEntity().GetComponent<TransformComponent>().RotateForwardVecToInDuration(rotateToVec, 0.5f);
		}
		else if(!mPressedA && !mPressedD && !mPressedS && !mPressedW)
		{
			GetEntity().GetComponent<RigidbodyComponent>().SetVelocity({ 0.0f,0.0f, 0.0f });
			GetEntity().GetComponent<CameraComponent>().SetCameraControlYaw(false);
		}
	}

	void OnKey_Space(bool press)
	{
		
		/*
		if (press)
		{
			if (!GetEntity().GetComponent<AnimationComponent>().IsPlayingAnimation())
			{
				GetEntity().GetComponent<AnimationComponent>().PlayAnimation("StandAloneResource\\milia\\milia_walk.ie_anim");
			}
			else
			{
				GetEntity().GetComponent<AnimationComponent>().StopAnimation();
			}
			
		}*/

		/*
		if (press)
		{
			GetEntity().GetComponent<SkeletalMeshComponent>().HandReachTarget(TestTarget);
			//GetEntity().GetComponent<SkeletalMeshComponent>().TestAxis();
		}*/
	}

	void OnMouse_Left(bool press)
	{
		if (press)
		{
			EventAnimPlaySetting setting;
			setting.animFilePath = attcks[mAttackState];
			AnimSpeedRange range1;
			range1.startRatio = 0.0f;
			range1.endRatio = 0.3f;
			range1.playSpeed = 0.8f;
			AnimSpeedRange range2;
			range2.startRatio = 0.5f;
			range2.endRatio = 0.8f;
			range2.playSpeed = 0.1f;
			setting.animSpeedRanges = { range1, range2 };

			mAttackState += 1;
			if (mAttackState >= attcks.size()) mAttackState = 0;
			setting.animEndCallback = [&]() {mAttackState = 0; };
			GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);
		}
	}

	private:
		std::array<std::string, 5> attcks =
		{
			"StandAloneResource\\milia\\milia_combo_a1.ie_anim",
			"StandAloneResource\\milia\\milia_combo_a2.ie_anim",
			"StandAloneResource\\milia\\milia_combo_a3.ie_anim",
			"StandAloneResource\\milia\\milia_combo_a4.ie_anim",
			"StandAloneResource\\milia\\milia_combo_a5.ie_anim"
		};

		int mAttackState = 0;

		float const mMaxWalkSpeed = 150.0f;
		bool mUseMouseToControlCamera = false;
		float const mCameraRotateVerticalSpeed = 0.03f;
		float const mCameraRotateHoritonzallSpeed = 0.03f;

		
		bool mPressedW = false;
		bool mPressedA = false;
		bool mPressedS = false;
		bool mPressedD = false;
};

class MiliaASM : public AnimStateMachine
{
public:
	MiliaASM(EntityID entityID, std::reference_wrapper<World> world)
		:AnimStateMachine(entityID, world)
	{
		CreateState("StandAloneResource\\milia\\milia_idle.ie_anim");
		CreateState("StandAloneResource\\milia\\milia_walk.ie_anim");
		CreateLink(0, 1, [&]() -> bool
				   {
					   float speed = (GetEntity().GetComponent<RigidbodyComponent>().GetSpeed());
					   return speed > 0.0f;
				   }, 0.15f);

		CreateLink(1, 0, [&]() -> bool
				   {
					   float speed = (GetEntity().GetComponent<RigidbodyComponent>().GetSpeed());
					   return speed == 0.0f;
				   }, 0.15f);

		SetEntryState(0);
	}

private:
};


int main()
{
	auto& engine = InceptionEngine::GetInstance();

	World* world = engine.CreateWorld();
	
	std::array<std::string, 6> skyboxTexturePath =
	{
		"StandAloneResource\\skybox\\front.png",
		"StandAloneResource\\skybox\\back.png",
		"StandAloneResource\\skybox\\top.png",
		"StandAloneResource\\skybox\\bottom.png",
		"StandAloneResource\\skybox\\right.png",
		"StandAloneResource\\skybox\\left.png"
	};
	

	world->SetSkybox(skyboxTexturePath);
	

	Entity const& entityOne = world->CreateEntity();
	

	entityOne.AddComponent<SkeletalMeshComponent>().SetMesh("StandAloneResource\\milia\\milia_mesh.ie_skmesh");
	entityOne.AddComponent<AnimationComponent>().SetAnimStateMachine<MiliaASM>();

	entityOne.AddComponent<NativeScriptComponent>().SetScript<HornetScript>();
	entityOne.AddComponent<RigidbodyComponent>();

	entityOne.AddComponent<CameraComponent>().SetPosAndForward(Vec3f(0.0f, 160.0f, -300.0f), Vec3f(0.0f, 100.0f, 0.0f));


	world->SetGameCamera(entityOne.GetComponent<CameraComponent>());

	Entity const& entityTwo = world->CreateEntity();

	entityTwo.AddComponent<SkeletalMeshComponent>().SetPlane(1000.0f);


	engine.PlayGame();

	return 0;

}