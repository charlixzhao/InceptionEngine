#pragma once

#include "EngineGlobals/EngineApiPrefix.h"

#include "RunTime/Animation/AnimPose.h"

#include "EngineGlobals/RenderGlobals.h"


namespace inceptionengine
{
	class SkeletalMeshRenderSystem;

	class SkeletalMeshInstance;
	
	class AnimationController;

	class IE_API SkeletalMeshComponent
	{
	public:

		SkeletalMeshComponent();

		~SkeletalMeshComponent();

		SkeletalMeshComponent(SkeletalMeshComponent const&) = delete;

		SkeletalMeshComponent& operator = (SkeletalMeshComponent const&) = delete;

		SkeletalMeshComponent(SkeletalMeshComponent&&) noexcept;

		void SetMesh(std::string const& filePath);

		void SetMeshPose(AnimPose const& pose);

		void SetPlane();

		void SetTriangle();

		void PlayAnimation(std::string const& filePath);

		void StopAnimation();

		bool IsPlayingAnimation();

	private:
		friend class SkeletalMeshRenderSystem;
		friend class AnimationSystem;

		std::unique_ptr<SkeletalMeshInstance> mSkeletalMeshInstance = nullptr;

		std::unique_ptr<AnimationController> mAnimationController = nullptr;

		bool mLoadedToDevice = false;

	};
}
