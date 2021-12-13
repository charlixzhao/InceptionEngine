#pragma once

#include "EngineGlobals/EngineApiPrefix.h"

#include "RunTime/Animation/AnimPose.h"

#include "EngineGlobals/RenderGlobals.h"

#include "ECS/Entity/EntityID.h"


namespace inceptionengine
{
	class SkeletalMeshRenderSystem;

	class SkeletalMeshInstance;
	
	class AnimationController;

	class World;

	class IE_API SkeletalMeshComponent
	{
	public:

		SkeletalMeshComponent(EntityID entityID, std::reference_wrapper<World> world);

		~SkeletalMeshComponent();

		SkeletalMeshComponent(SkeletalMeshComponent const&) = delete;

		SkeletalMeshComponent& operator = (SkeletalMeshComponent const&) = delete;

		SkeletalMeshComponent(SkeletalMeshComponent&&) noexcept;

		void SetMesh(std::string const& filePath);

		void SetPlane(float scale, float level = 0.0f, std::string const& texture = "StandAloneResource/T_Ground.jpg");

		void StartAddCube();

		int AddCube(float x, float y, float z, Vec3f const& offset, int parent, std::string const& texture);

		void FinishAddCube();

		void SetTriangle();

		void SetTexture(std::vector<std::string> const& textureFilePaths);

		void CreateSocket(std::string const& socketName, std::string const& parentName, Matrix4x4f const& lclTransform);

		int GetSocketParentID(std::string const& socketName);

		Matrix4x4f GetSocketLclTransform(std::string const& socketName);

		Matrix4x4f GetSocketGlobalTransform(std::string const& socketName);

		void SetVisibility(bool visible);

		void SetShaderPath(int submeshID, std::string const& vertexShader, std::string const& fragShader);

	private:
		friend class SkeletalMeshRenderSystem;
		friend class AnimationSystem;
		friend class SkeletalMeshComponent;
		friend class AnimationComponent;

		std::unique_ptr<SkeletalMeshInstance> mSkeletalMeshInstance = nullptr;

		//std::unique_ptr<AnimationController> mAnimationController = nullptr;

		EntityID mEntityID = InvalidEntityID;

		std::reference_wrapper<World> mWorld;

		bool mLoadedToDevice = false;

		bool mVisible = true;

	};
}
