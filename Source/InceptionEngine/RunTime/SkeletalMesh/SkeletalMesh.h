#pragma once

#include "Skeleton.h"

#include "RunTime/RHI/Renderer/Vertex.h"
#include "RunTime/RHI/Renderer/ShaderPath.h"


namespace inceptionengine
{

	struct SubMesh
	{
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(mName, vertices, indices, texturePath, shaderPath);
		}

		std::string mName;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::string texturePath;
		ShaderPath shaderPath;
	};


	struct SkeletalMesh
	{
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(mName, mPathToSkeleton, mSubMeshes);
		}

		std::string mName;
		std::string mPathToSkeleton;
		std::vector<SubMesh> mSubMeshes;
		std::shared_ptr<Skeleton> mSkeleton = nullptr;
	};
}
