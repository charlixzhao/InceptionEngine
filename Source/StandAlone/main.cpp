

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
	}

private:
	virtual void OnBegin() override
	{
		std::cout << "Hello Script!\n";
	}

	void OnKey_D(bool press)
	{
		if(press) GetEntity().GetComponent<CameraComponent>().RotateVertical(10.0f);
	}

	void OnKey_1(bool press)
	{
		if (press)
		{
			TestX += 5;
			TestTarget = Translate(TestX, TestY, TestZ);
			GetEntity().GetWorld().GetEntity(1).GetComponent<TransformComponent>().SetWorldTransform(TestTarget * Scale(0.1f, 0.1f, 0.1f));
		}
	}
	void OnKey_2(bool press)
	{
		if (press)
		{
			TestX += -5;
			TestTarget = Translate(TestX, TestY, TestZ);
			GetEntity().GetWorld().GetEntity(1).GetComponent<TransformComponent>().SetWorldTransform(TestTarget * Scale(0.1f, 0.1f, 0.1f));
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


	void OnKey_A(bool press)
	{
		if (press) GetEntity().GetComponent<CameraComponent>().RotateVertical(-10.0f);
	}

	void OnKey_W(bool press)
	{
		if (press) GetEntity().GetComponent<CameraComponent>().RotateHorizontal(-10.0f);
	}

	void OnKey_S(bool press)
	{
		if (press) GetEntity().GetComponent<CameraComponent>().RotateHorizontal(10.0f);
	}

	void OnKey_Space(bool press)
	{
		
		
		if (press)
		{
			if (!GetEntity().GetComponent<SkeletalMeshComponent>().IsPlayingAnimation())
			{
				GetEntity().GetComponent<SkeletalMeshComponent>().PlayAnimation("StandAloneResource\\milia\\milia_walk.ie_anim");
			}
			else
			{
				GetEntity().GetComponent<SkeletalMeshComponent>().StopAnimation();
			}
			
		}

		
		
		/*
		if (press)
		{
			GetEntity().GetComponent<SkeletalMeshComponent>().HandReachTarget(TestTarget);
			//GetEntity().GetComponent<SkeletalMeshComponent>().TestAxis();
		}*/
	}
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
	//entityOne.GetComponent<TransformComponent>().SetWorldTransform(Rotate(-PI/2.0f, Vec3f(1.0f, 0.0f, 0.0f)));

	entityOne.AddComponent<NativeScriptComponent>().SetScript<HornetScript>();

	entityOne.AddComponent<CameraComponent>().SetPosAndForward(Vec3f(0.0f, 80.0f, 280.0f), Vec3f(0.0f, 80.0f, 0.0f));

	world->SetGameCamera(entityOne.GetComponent<CameraComponent>());

	Entity const& entityTwo = world->CreateEntity();

	//entityTwo.AddComponent<SkeletalMeshComponent>().SetMesh("StandAloneResource\\cube\\cube_mesh.ie_skmesh");
	entityTwo.GetComponent<TransformComponent>().SetWorldTransform(TestTarget * Scale(0.1f, 0.1f, 0.1f));

	//world->DrawAnimationTest();

	engine.PlayGame();

	return 0;

}