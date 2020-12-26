

#include "InceptionEngine.h"

#include "Milia/MiliaScript.h"
#include "Milia/MiliaASM.h"
#include "Sice/SiceScript.h"
#include "Sice/SiceASM.h"

using namespace inceptionengine;


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

	sice.AddComponent<CameraComponent>().SetPosAndForward(Vec3f(0.0f, 180.0f, -350.0f), Vec3f(0.0f, 125.0f, 0.0f));


	sice.AddComponent<RigidbodyComponent>();
	Matrix4x4f swordSocketTransform = { {0.012678, 0.001461, -0.002478, 0.000000},
									   {-0.001110, 0.012815, 0.001880, 0.000000},
									   {0.002654, -0.001622, 0.012622, 0.000000},
									   {0.171233, -0.007158, 0.361185, 1.000000 } };

	sice.GetComponent<SkeletalMeshComponent>().CreateSocket("SwordSocket", "Bip001 R Hand", swordSocketTransform);

	world->SetGameCamera(sice.GetComponent<CameraComponent>());


	Entity const& sword = world->CreateEntity();
	EntityID swordID = sword.GetID();
	sword.AddComponent<SkeletalMeshComponent>().SetMesh("StandAloneResource\\thinsword\\thinsword_mesh.ie_skmesh");
	sword.GetComponent<SkeletalMeshComponent>().SetVisibility(false);
	sword.GetComponent<TransformComponent>().AttachToSocket(siceID, "SwordSocket");

	world->GetEntity(siceID).AddComponent<NativeScriptComponent>().SetScript<SiceScript>(swordID);
	world->GetEntity(siceID).AddComponent<AnimationComponent>().SetAnimStateMachine<SiceASM>(swordID);

	Entity const& plane = world->CreateEntity();
	plane.AddComponent<SkeletalMeshComponent>().SetPlane(1000.0f);


	engine.PlayGame();

	return 0;

}



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