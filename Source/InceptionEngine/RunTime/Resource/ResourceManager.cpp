
#include "IE_PCH.h"

#include "ResourceManager.h"

#include "RunTime/SkeletalMesh/SkeletalMesh.h"

#include "RunTime/Animation/Animation.h"

#include "Serialization/Serializer.h"

#include "PathHelper.h"

namespace inceptionengine
{

	ResourceManager& ResourceManager::GetInstance()
	{
		static ResourceManager gManager;
		return gManager;
	}

	/*
	this can be done with concepts and use typeid to implement cache
	*/
	template<typename T>
	std::shared_ptr<T> ResourceManager::GetResource(std::string const& filePath)
	{
		std::string absPath = PathHelper::GetAbsolutePath(filePath);

		static_assert(std::is_same_v<T, Animation> || std::is_same_v<T, SkeletalMesh>);

		if constexpr (std::is_same_v<T, Animation>)
		{
			if (mAnimationCache.find(absPath) != mAnimationCache.end())
			{
				return mAnimationCache.at(absPath);
			}
			else
			{
				std::shared_ptr<Animation> pAnimation = Serializer::Deserailize<Animation>(absPath);
				mAnimationCache.insert(std::pair<std::string, std::shared_ptr<Animation>>(absPath, pAnimation));
				return pAnimation;
			}
		}
		else if constexpr (std::is_same_v<T, SkeletalMesh>)
		{
			if (mSkeletalMeshCache.find(absPath) != mSkeletalMeshCache.end())
			{
				return mSkeletalMeshCache.at(absPath);
			}
			else
			{

				std::shared_ptr<SkeletalMesh> pMesh = Serializer::Deserailize<SkeletalMesh>(absPath);
				mSkeletalMeshCache.insert(std::pair<std::string, std::shared_ptr<SkeletalMesh>>(absPath, pMesh));
				return pMesh;
			}
		}
		else
		{
			return nullptr;
		}
	}
}

