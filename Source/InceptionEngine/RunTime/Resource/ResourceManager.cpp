
#include "IE_PCH.h"

#include "ResourceManager.h"

#include "RunTime/SkeletalMesh/SkeletalMesh.h"
#include "RunTime/Animation/Animation.h"
#include "RunTime/SkeletalMesh/Skeleton.h"
#include "RunTime/Animation/MotionMatching/MatchingDatabase.h"
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

		static_assert(std::is_same_v<T, Animation> || std::is_same_v<T, SkeletalMesh> || std::is_same_v<T, Skeleton> || std::is_same_v<T, MatchingDatabase>);


		if constexpr (std::is_same_v<T, SkeletalMesh>)
		{
			assert(PathHelper::GetFileExtension(filePath) == "ie_skmesh");
			if (mSkeletalMeshCache.find(absPath) != mSkeletalMeshCache.end())
			{
				return mSkeletalMeshCache.at(absPath);
			}
			else
			{
				std::shared_ptr<SkeletalMesh> pMesh = Serializer::Deserailize<SkeletalMesh>(absPath);
				pMesh->mSkeleton = gResourceMgr.GetResource<Skeleton>(pMesh->mPathToSkeleton);
				mSkeletalMeshCache.insert(std::pair<std::string, std::shared_ptr<SkeletalMesh>>(absPath, pMesh));
				return pMesh;
			}
		}
		else if constexpr (std::is_same_v<T, Animation>)
		{
			assert(PathHelper::GetFileExtension(filePath) == "ie_anim");
			if (mAnimationCache.find(absPath) != mAnimationCache.end())
			{
				return mAnimationCache.at(absPath);
			}
			else
			{
				std::shared_ptr<Animation> pAnimation = Serializer::Deserailize<Animation>(absPath);
				pAnimation->mSkeleton = gResourceMgr.GetResource<Skeleton>(pAnimation->mPathToSkeleton);
				mAnimationCache.insert(std::pair<std::string, std::shared_ptr<Animation>>(absPath, pAnimation));
				return pAnimation;
			}
		}
		else if constexpr (std::is_same_v<T, Skeleton>)
		{
			assert(PathHelper::GetFileExtension(filePath) == "ie_skeleton");
			if (filePath == "") return nullptr;

			if (mSkeletonCache.find(absPath) != mSkeletonCache.end())
			{
				return mSkeletonCache.at(absPath);
			}
			else
			{
				std::shared_ptr<Skeleton> pSkeleton = Serializer::Deserailize<Skeleton>(absPath);
				mSkeletonCache.insert(std::pair<std::string, std::shared_ptr<Skeleton>>(absPath, pSkeleton));
				return pSkeleton;
			}
		}
		else if constexpr (std::is_same_v<T, MatchingDatabase>)
		{
			assert(PathHelper::GetFileExtension(filePath) == "ie_mmdb");
			if (filePath == "") return nullptr;

			if (mMatchingDatabaseCache.find(absPath) != mMatchingDatabaseCache.end())
			{
				return mMatchingDatabaseCache.at(absPath);
			}
			else
			{
				std::shared_ptr<MatchingDatabase> pDB = Serializer::Deserailize<MatchingDatabase>(absPath);
				mMatchingDatabaseCache.insert(std::pair<std::string, std::shared_ptr<MatchingDatabase>>(absPath, pDB));
				return pDB;
			}
		}
		else
		{
			return nullptr;
		}
	}
}

