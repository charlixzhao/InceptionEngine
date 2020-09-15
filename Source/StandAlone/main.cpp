
#include <iostream>


#include "InceptionEngine.h"


using namespace inceptionengine;

class HornetScript : public NativeScript
{
public:
	HornetScript(std::reference_wrapper<Entity const> entity)
		:NativeScript(entity)
	{
		BindKeyInputCallback(KeyInputTypes::Mouse_Left, std::bind(&HornetScript::OnMouse_Left, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_H, std::bind(&HornetScript::OnKey_H, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Keyboard_B, std::bind(&HornetScript::OnKey_B, this, std::placeholders::_1));
		BindKeyInputCallback(KeyInputTypes::Mouse_Scroll, std::bind(&HornetScript::OnMouse_Scroll, this, std::placeholders::_1));
	}

private:
	virtual void OnBegin() override
	{
		std::cout << "Hellooo Script\n";
	}

	void OnMouse_Left(bool press)
	{
		char* output = press ? "press_mouse_left\n" : "release_mouse_left\n";
		std::cout << output;
	}

	void OnKey_H(bool press)
	{
		char* output = press ? "press_key_h\n" : "release_key_h\n";
		std::cout << output;
	}
	void OnKey_B(bool press)
	{
		char* output = press ? "press_key_b\n" : "release_key_b\n";
		std::cout << output;
	}

	void OnMouse_Scroll(bool press)
	{
		char* output = press ? "scroll_mouse_up\n" : "scroll_mouse_down\n";
		std::cout << output;
	}

};


int main()
{
	Vec3f v(1.0f,1.0f,1.0f);
	std::cout << v.x << v.y << v.z << std::endl;

	auto& engine = InceptionEngine::GetInstance();

	World* world = engine.CreateWorld();
	
	std::array<std::string, 6> skyboxTexturePath =
	{
		"D:\\Inception\\Content\\Textures\\Skybox\\front.png",
		"D:\\Inception\\Content\\Textures\\Skybox\\back.png",
		"D:\\Inception\\Content\\Textures\\Skybox\\top.png",
		"D:\\Inception\\Content\\Textures\\Skybox\\bottom.png",
		"D:\\Inception\\Content\\Textures\\Skybox\\right.png",
		"D:\\Inception\\Content\\Textures\\Skybox\\left.png"
	};
	
	world->SetSkybox(skyboxTexturePath);
	

	EntityID entityOneID = world->CreateEntity();
	
	Entity const& entityOne = world->GetEntity(entityOneID);


	entityOne.AddComponent<TransformComponent, int const&, int const&>(1, 2).Hello();

	entityOne.AddComponent<SkeletalMeshComponent>().SetPlane();

	entityOne.AddComponent<NativeScriptComponent>().SetScript<HornetScript>();

	engine.PlayGame();

	return 0;

}