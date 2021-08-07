

#include "InceptionEngine.h"

#include "Milia/MiliaScript.h"
#include "Milia/MiliaASM.h"
#include "Sice/SiceScript.h"
#include "Sice/SiceASM.h"
#include "Shinokao/ShinokaoASM.h"
#include "Shinokao/ShinokaoScript.h"

using namespace inceptionengine;


#pragma once


#include "InceptionEngine.h"
#include <iostream>
#include "Interfaces/IHitable.h"

using namespace inceptionengine;



class HumanoidScript : public NativeScript
{
public:
	HumanoidScript(EntityID entityID, std::reference_wrapper<World> world)
		:NativeScript(entityID, world)
	{
		BindKeyInputCallback(KeyInputTypes::Keyboard_2, std::bind(&HumanoidScript::OnKey_2, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Mouse_Scroll, std::bind(&HumanoidScript::OnMouse_Scroll, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_W, std::bind(&HumanoidScript::OnKey_W, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_A, std::bind(&HumanoidScript::OnKey_A, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_S, std::bind(&HumanoidScript::OnKey_S, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_D, std::bind(&HumanoidScript::OnKey_D, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_Space, std::bind(&HumanoidScript::OnKey_Space, this, std::placeholders::_1));
	}


private:
	

	virtual void OnMouseDeltaPos(MouseDeltaPos mouseDeltaPos) override
	{
		if (mUseMouseToControlCamera)
		{
			GetEntity().GetComponent<CameraComponent>().RotateVertical(-mouseDeltaPos.deltaXPos * mCameraRotateVerticalSpeed);
			GetEntity().GetComponent<CameraComponent>().RotateHorizontal(-mouseDeltaPos.deltaYPos * mCameraRotateHoritonzallSpeed);
		}

	}

	virtual void OnJoystickInput(JoyStick joystickInput) override
	{
		Vec3f direction = { joystickInput.horizontal, 0.0f, joystickInput.vertical };
		if (VecLength(direction) >= 0.01f)
		{
			direction = NormalizeVec(direction);
			GetEntity().GetComponent<AnimationComponent>().SetInputControl(direction);
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

	void OnKey_Space(bool press)
	{
		if (press)
		{
			GetEntity().GetComponent<AnimationComponent>().SetInputControl(Vec3f(0.0f, 0.0f, 0.0f));
		}
	}


	void OnKey_W(bool press)
	{
		if (press)
		{
			GetEntity().GetComponent<AnimationComponent>().SetInputControl(Vec3f(0.0f, 0.0f, -1.0f));
		}
	}
	void OnKey_A(bool press)
	{
		if (press)
		{
			GetEntity().GetComponent<AnimationComponent>().SetInputControl(Vec3f(-1.0f, 0.0f, 0.0f));
		}
	}
	void OnKey_S(bool press)
	{
		if (press)
		{
			GetEntity().GetComponent<AnimationComponent>().SetInputControl(Vec3f(0.0f, 0.0f, 1.0f));
		}
	}
	void OnKey_D(bool press)
	{
		if (press)
		{
			GetEntity().GetComponent<AnimationComponent>().SetInputControl(Vec3f(1.0f, 0.0f, 0.0f));
		}
	}

	

private:
	bool mUseMouseToControlCamera = false;
	float const mCameraRotateVerticalSpeed = 0.03f;
	float const mCameraRotateHoritonzallSpeed = 0.03f;

};


class HumanoidASM : public AnimStateMachine
{
public:
	HumanoidASM(EntityID entityID, std::reference_wrapper<World> world)
		:AnimStateMachine(entityID, world)
	{
		CreateState("StandAloneResource\\humanoid\\obstacles1_subject1.ie_anim");

		SetEntryState(0);
	}

private:
};

//Scene 3, motion matching
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


	Entity const& humanoid = world->CreateEntity();
	EntityID humanoidID = humanoid.GetID();
	humanoid.AddComponent<SkeletalMeshComponent>().SetMesh("StandAloneResource\\humanoid\\humanoid_mesh.ie_skmesh");

	humanoid.GetComponent<SkeletalMeshComponent>().SetTexture({ "EngineResource\\texture\\brown.png",
															  "EngineResource\\texture\\grey.png" ,
															  "EngineResource\\texture\\brown.png" ,
															  "EngineResource\\texture\\grey.png" ,
															  "EngineResource\\texture\\brown.png" });

	//humanoid.GetComponent<SkeletalMeshComponent>().SetShaderPath(-1, "EngineResource\\shader\\spv\\vertex.spv",
	//															 "EngineResource\\shader\\spv\\highlight.spv");


	humanoid.AddComponent<CameraComponent>().SetPosAndForward(Vec3f(0.0f, 200.0f, 200.0f), Vec3f(0.0f, 145.0f, 0.0f));
	world->SetGameCamera(humanoid.GetComponent<CameraComponent>());

	humanoid.AddComponent<RigidbodyComponent>();
	humanoid.GetComponent<RigidbodyComponent>().SetCollider(ColliderType::Capsule);
	humanoid.GetComponent<RigidbodyComponent>().SetCapsuleColliderProperties(Vec3f(0.0f, 0.0f, 0.0f),
																		 Vec3f(0.0f, 150.0f, 0.0f),
																		 40.0f);


	world->GetEntity(humanoidID).AddComponent<NativeScriptComponent>().SetScript<HumanoidScript>();
	world->GetEntity(humanoidID).AddComponent<AnimationComponent>().SetAnimStateMachine<HumanoidASM>();
	world->GetEntity(humanoidID).GetComponent<AnimationComponent>().SetMatchingDatabase("StandAloneResource/humanoid/walk.ie_mmdb");
	world->GetEntity(humanoidID).GetComponent<AnimationComponent>().SetRootMotion(true);

	Entity const& plane = world->CreateEntity();
	plane.AddComponent<SkeletalMeshComponent>().SetPlane(2000.0f);

	engine.PlayGame();

	return 0;

}

/*
//Scene two: sice locomotion and attack
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


	Entity const& sice = world->CreateEntity();
	EntityID siceID = sice.GetID();
	sice.AddComponent<SkeletalMeshComponent>().SetMesh("StandAloneResource\\sice\\sice_mesh.ie_skmesh");

	sice.GetComponent<SkeletalMeshComponent>().SetTexture({ "StandAloneResource\\sice\\hair.BMP",
															   "StandAloneResource\\sice\\skin.HDR" ,
															   "StandAloneResource\\sice\\cloth.BMP" });

	sice.GetComponent<SkeletalMeshComponent>().SetShaderPath(-1, "EngineResource\\shader\\spv\\vertex.spv",
															 "EngineResource\\shader\\spv\\highlight.spv");


	sice.AddComponent<CameraComponent>().SetPosAndForward(Vec3f(0.0f, 200.0f, 200.0f), Vec3f(0.0f, 145.0f, 0.0f));

	sice.AddComponent<RigidbodyComponent>();
	sice.GetComponent<RigidbodyComponent>().SetCollider(ColliderType::Capsule);
	sice.GetComponent<RigidbodyComponent>().SetCapsuleColliderProperties(Vec3f(0.0f, 0.0f, 0.0f),
																		 Vec3f(0.0f, 150.0f, 0.0f),
																		 40.0f);

	sice.AddComponent<AudioComponent>();

	Matrix4x4f swordSocketTransform = { {0.012678, 0.001461, -0.002478, 0.000000},
									   {-0.001110, 0.012815, 0.001880, 0.000000},
									   {0.002654, -0.001622, 0.012622, 0.000000},
									   {0.171233, -0.007158, 0.361185, 1.000000 } };

	Matrix4x4f swordStart = { {1.000000, -0.000000, 0.000000, 0.000000},
							 {-0.000000, 1.000000, -0.000000, 0.000000},
							 {0.000000, -0.000000, 1.000000, 0.000000},
							 {0.161875, 0.016064, 0.158630, 1.000000} };

	Matrix4x4f swordEnd = { {1.000000, -0.000000, 0.000000, 0.000000},
						   {-0.000000, 1.000000, -0.000000, 0.000000},
						   {0.000000, -0.000000, 1.000000, 0.000000},
						   {0.516013, -0.056450, 0.869853, 1.000000} };


	sice.GetComponent<SkeletalMeshComponent>().CreateSocket("SwordSocket", "Bip001 R Hand", swordSocketTransform);
	sice.GetComponent<SkeletalMeshComponent>().CreateSocket("SwordStart", "Bip001 R Hand", swordStart);
	sice.GetComponent<SkeletalMeshComponent>().CreateSocket("SwordEnd", "Bip001 R Hand", swordEnd);

	world->SetGameCamera(sice.GetComponent<CameraComponent>());


	Entity const& sword = world->CreateEntity();
	EntityID swordID = sword.GetID();
	sword.AddComponent<SkeletalMeshComponent>().SetMesh("StandAloneResource\\thinsword\\thinsword_mesh.ie_skmesh");
	sword.GetComponent<SkeletalMeshComponent>().SetVisibility(false);
	sword.GetComponent<TransformComponent>().AttachToSocket(siceID, "SwordSocket");
	sword.GetComponent<SkeletalMeshComponent>().SetShaderPath(-1, "EngineResource\\shader\\spv\\vertex.spv",
															  "EngineResource\\shader\\spv\\highlight.spv");

	world->GetEntity(siceID).AddComponent<NativeScriptComponent>().SetScript<SiceScript>(swordID);
	world->GetEntity(siceID).AddComponent<AnimationComponent>().SetAnimStateMachine<SiceASM>(swordID);

	Entity const& plane = world->CreateEntity();
	plane.AddComponent<SkeletalMeshComponent>().SetPlane(2000.0f);

	Entity const& shinokao = world->CreateEntity();
	shinokao.GetComponent<TransformComponent>().SetWorldTransform(Translate(Vec3f(0.0f, 0.0f, -500.0f)));
	shinokao.AddComponent<RigidbodyComponent>().SetCollider(ColliderType::Capsule);
	shinokao.GetComponent<RigidbodyComponent>().SetCapsuleColliderProperties(Vec3f(0.0f, 0.0f, 0.0f),
																			 Vec3f(0.0f, 150.0f * 1.8f, 0.0f),
																			 150.0f);

	shinokao.AddComponent<SkeletalMeshComponent>().SetMesh("StandAloneResource\\shinokao\\shinokao_mesh.ie_skmesh");

	shinokao.AddComponent<AnimationComponent>().SetAnimStateMachine<ShinokaoASM>();
	shinokao.AddComponent<NativeScriptComponent>().SetScript<ShinokaoScript>();
	shinokao.GetComponent<TransformComponent>().SetUniformScale(1.3f);
	shinokao.GetComponent<SkeletalMeshComponent>().CreateSocket("SwordStart", "Sword_joint", Matrix4x4f(1.0f));
	shinokao.GetComponent<SkeletalMeshComponent>().CreateSocket("SwordEnd", "joint4_R", Matrix4x4f(1.0f));
	shinokao.AddComponent<AudioComponent>();

	engine.PlayGame();

	return 0;

}*/



//Scene one: milia walk and attack
/*

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
	EntityID entityOneID = entityOne.GetID();

	entityOne.AddComponent<SkeletalMeshComponent>().SetMesh("StandAloneResource\\milia\\milia_mesh.ie_skmesh");
	entityOne.AddComponent<AnimationComponent>().SetAnimStateMachine<MiliaASM>();

	entityOne.AddComponent<NativeScriptComponent>().SetScript<HornetScript>();
	entityOne.AddComponent<RigidbodyComponent>();

	entityOne.AddComponent<CameraComponent>().SetPosAndForward(Vec3f(0.0f, 180.0f, -300.0f), Vec3f(0.0f, 125.0f, 0.0f));
	Matrix4x4f fireswordSocket = Matrix4x4f(1.0f);
	fireswordSocket[0] = (Vec4f(-0.95429f, -0.072989f, 0.289827f, 0.0f));
	fireswordSocket[1] = (Vec4f(0.052727f, -0.995626f, -0.077125f, 0.0f));
	fireswordSocket[2] = (Vec4f(0.294189f, -0.058318f, 0.953966f, 0.0f));
	fireswordSocket[3] = Vec4f(2.31845f, 2.314819f, -16.453683f, 1.0f);

	entityOne.GetComponent<SkeletalMeshComponent>().CreateSocket("FireSword", "Hand_R", fireswordSocket);

	world->SetGameCamera(entityOne.GetComponent<CameraComponent>());

	Entity const& sice = world->CreateEntity();
	
	sice.AddComponent<SkeletalMeshComponent>().SetMesh("StandAloneResource\\sice\\sice_mesh.ie_skmesh");
	sice.AddComponent<AnimationComponent>().SetAnimStateMachine<SiceASM>();
	sice.GetComponent<SkeletalMeshComponent>().SetTexture({ "StandAloneResource\\sice\\hair.BMP",
															   "StandAloneResource\\sice\\skin.HDR" ,
															   "StandAloneResource\\sice\\cloth.BMP" });

	Entity const& entityThree = world->CreateEntity();
	entityThree.AddComponent<SkeletalMeshComponent>().SetMesh("StandAloneResource\\firesword\\firesword_mesh.ie_skmesh");
	entityThree.GetComponent<SkeletalMeshComponent>().SetTexture({ "StandAloneResource\\firesword\\handle.BMP",
													 "StandAloneResource\\firesword\\fireblade.jpg" });
	entityThree.GetComponent<TransformComponent>().AttachToSocket(entityOneID, "FireSword");

	Entity const& entityFour = world->CreateEntity();
	entityFour.AddComponent<SkeletalMeshComponent>().SetPlane(1000.0f);

	engine.PlayGame();

	return 0;

}*/