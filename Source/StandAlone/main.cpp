
#include <iostream>
#include "InceptionEngine.h"


using namespace inceptionengine;

class HornetScript : public NativeScript
{
public:
	HornetScript(std::reference_wrapper<Entity const> entity)
		:NativeScript(entity)
	{

	}

private:
	virtual void OnBegin() override
	{
		std::cout << "Hello Script\n";
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