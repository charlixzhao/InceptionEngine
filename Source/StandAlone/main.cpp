

#include "InceptionEngine.h"

#include <iostream>


using namespace inceptionengine;

class HornetScript : public NativeScript
{
public:
	HornetScript(std::reference_wrapper<Entity const> entity)
		:NativeScript(entity)
	{
		BindKeyInputCallback(KeyInputTypes::Keyboard_D, std::bind(&HornetScript::OnKey_D, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_A, std::bind(&HornetScript::OnKey_A, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_W, std::bind(&HornetScript::OnKey_W, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_S, std::bind(&HornetScript::OnKey_S, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_Space, std::bind(&HornetScript::OnKey_Space, this, std::placeholders::_1));
	}

private:
	virtual void OnBegin() override
	{
		std::cout << "Hello Script!\n";
	}

	void OnKey_D(bool press)
	{
		if(press) mEntity.get().GetComponent<CameraComponent>().RotateVertical(10.0f);
	}

	void OnKey_A(bool press)
	{
		if (press) mEntity.get().GetComponent<CameraComponent>().RotateVertical(-10.0f);
	}

	void OnKey_W(bool press)
	{
		if (press) mEntity.get().GetComponent<CameraComponent>().RotateHorizontal(-10.0f);
	}

	void OnKey_S(bool press)
	{
		if (press) mEntity.get().GetComponent<CameraComponent>().RotateHorizontal(10.0f);
	}

	void OnKey_Space(bool press)
	{
		if (press) mEntity.get().GetComponent<AnimationComponent>().PlayAnimation("StandAloneResource\\maria\\maria_dance.ie_anim");
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
	

	EntityID entityOneID = world->CreateEntity();
	
	Entity const& entityOne = world->GetEntity(entityOneID);

	entityOne.AddComponent<TransformComponent, int const&, int const&>(1, 2);

	entityOne.AddComponent<SkeletalMeshComponent>().ImportMesh("StandAloneResource\\maria\\maria_mesh.ie_skmesh");

	entityOne.AddComponent<NativeScriptComponent>().SetScript<HornetScript>();

	entityOne.AddComponent<CameraComponent>().SetPosAndForward(Vec3f(-280.0f, 80.0f, 0.0f), Vec3f(0.0f, 80.0f, 0.0f));

	entityOne.AddComponent<AnimationComponent>();

	world->SetGameCamera(entityOne.GetComponent<CameraComponent>());

	engine.PlayGame();

	return 0;

}