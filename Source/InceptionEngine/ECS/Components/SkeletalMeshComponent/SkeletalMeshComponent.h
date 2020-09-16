#pragma once

#include "EngineGlobals/EngineApiPrefix.h"

#include "RunTime/Animation/AnimPose.h"

#include "EngineGlobals/RenderGlobals.h"


namespace inceptionengine
{
	class SkeletalMeshRenderSystem;

	class SkeletalMeshInstance;
	
	class IE_API SkeletalMeshComponent
	{
	public:

		explicit SkeletalMeshComponent(SkeletalMeshRenderSystem& system);

		~SkeletalMeshComponent();

		SkeletalMeshComponent(SkeletalMeshComponent const&) = delete;

		SkeletalMeshComponent& operator = (SkeletalMeshComponent const&) = delete;

		SkeletalMeshComponent(SkeletalMeshComponent&&) noexcept;

		void SetMesh(std::string const& meshFilePath);

		void SetMeshPose(AnimPose const& pose);

		void SetPlane();

	private:
		void InitializeRenderObjects(size_t numOfSubMeshes);

	private:
		friend class SkeletalMeshRenderSystem;

		std::reference_wrapper<SkeletalMeshRenderSystem> mSystem;

		std::unique_ptr<SkeletalMeshInstance> mSkeletalMeshInstance = nullptr;

		bool mLoadedToDevice = false;

	};
}
