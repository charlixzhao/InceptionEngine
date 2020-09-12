#include "SkeletalMeshInstance.h"

namespace inceptionengine
{
	SkeletalMeshInstance::~SkeletalMeshInstance() = default;

	RenderObejctID SkeletalMeshInstance::GetRenderObejctID() const
	{
		return mRenderObejctID;
	}

	std::vector<RenderUnit>& SkeletalMeshInstance::GetRenderUnits(unsigned int i)
	{
		return mRenderUnits[i];
	}
	void SkeletalMeshInstance::InitializeRenderObjects(size_t numOfSubMeshes)
	{
		mVertexBuffers.resize(numOfSubMeshes);
		mIndexBuffers.resize(numOfSubMeshes);
		mTextures.resize(numOfSubMeshes);
		mPipelines.resize(numOfSubMeshes);
		mUniformBufferDescriptions.resize(numOfSubMeshes);
	}
}