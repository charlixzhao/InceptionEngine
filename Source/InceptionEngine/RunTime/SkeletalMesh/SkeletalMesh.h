#pragma once

#include "Skeleton.h"
#include "RunTime/RHI/Renderer/Vertex.h"
#include "RunTime/RHI/Renderer/ShaderPath.h"

namespace inceptionengine
{

	class ShaderPath;

	struct SubMesh
	{
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(vertices, indices, texturePath, shaderPath);
		}

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::string texturePath;
		ShaderPath shaderPath;
	};


	class SkeletalMesh
	{
	public:
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(mSubMeshes, mSkeleton);
		}

		std::vector<SubMesh> mSubMeshes;
		Skeleton mSkeleton;
	};
}
