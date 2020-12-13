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

		void SetPlane();

		void SetTriangle();


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

	};
}
