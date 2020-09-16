#pragma once

#include "Skeleton.h"
#include "RunTime/RHI/Renderer/Vertex.h"
#include "RunTime/RHI/Renderer/ShaderPath.h"

namespace inceptionengine
{

	class ShaderPath;

	struct SubMesh
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::string texturePath;
		ShaderPath shaderPath;
	};


	class SkeletalMesh
	{
	public:

	private:
		friend class SkeletalMeshComponent;
		friend class SkeletalMeshRenderSystem;

		std::vector<SubMesh> mSubMeshes;
		Skeleton mSkeleton;
		
	};
}
