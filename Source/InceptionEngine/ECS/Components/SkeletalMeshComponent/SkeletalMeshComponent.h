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

		void SetMeshPose(AnimPose const& pose);

		void SetPlane(float scale);

		void SetTriangle();

		void SetTexture(std::vector<std::string> const& textureFilePaths);

		void CreateSocket(std::string const& socketName, std::string const& parentName, Matrix4x4f const& lclTransform);

		int GetSocketParentID(std::string const& socketName);

		Matrix4x4f GetSocketLclTransform(std::string const& socketName);

		Matrix4x4f GetSocketGlobalTransform(std::string const& socketName);

		void SetVisibility(bool visible);

	private:
		friend class SkeletalMeshRenderSystem;
		friend class AnimationSystem;
#ifdef __MSVCRT__
		friend class SkeletalMeshComponent;
#endif
		friend class AnimationComponent;

		std::unique_ptr<SkeletalMeshInstance> mSkeletalMeshInstance = nullptr;

		//std::unique_ptr<AnimationController> mAnimationController = nullptr;

		EntityID mEntityID = InvalidEntityID;

		std::reference_wrapper<World> mWorld;

		bool mLoadedToDevice = false;

		bool mVisible = true;

	};
}
