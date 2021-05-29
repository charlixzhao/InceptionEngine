#pragma once

#include "IE_PCH.h"

#include "RunTime/SkeletalMesh/SkeletalMesh.h"

#include "RunTime/Animation/Animation.h"

#include "RunTime/SkeletalMesh/Skeleton.h"

#include "Serialization/Serializer.h"

#include "PathHelper.h"


#define gResourceMgr inceptionengine::ResourceManager::GetInstance()

namespace inceptionengine
{
	struct Animation;
	struct SkeletalMesh;
	struct Skeleton;

	class ResourceManager
	{

	public:
        static ResourceManager& GetInstance()
        {
            static ResourceManager gManager;
            return gManager;
        }
        template<typename T>
        std::shared_ptr<T> GetResource(std::string const& filePath)
        {
            std::string absPath = PathHelper::GetAbsolutePath(filePath);

            static_assert(std::is_same_v<T, Animation> || std::is_same_v<T, SkeletalMesh> || std::is_same_v<T, Skeleton>);


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

            else
            {
                return nullptr;
            }
        }
	private:
		ResourceManager() = default;
		std::unordered_map<std::string, std::shared_ptr<Animation>> mAnimationCache;
		std::unordered_map<std::string, std::shared_ptr<SkeletalMesh>> mSkeletalMeshCache;
		std::unordered_map<std::string, std::shared_ptr<Skeleton>> mSkeletonCache;

	};

	template std::shared_ptr<Animation> ResourceManager::GetResource<Animation>(std::string const&);
	template std::shared_ptr<SkeletalMesh> ResourceManager::GetResource<SkeletalMesh>(std::string const&);
	template std::shared_ptr<Skeleton> ResourceManager::GetResource<Skeleton>(std::string const&);


}
