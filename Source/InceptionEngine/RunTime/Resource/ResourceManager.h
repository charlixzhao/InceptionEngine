#pragma once

namespace inceptionengine
{
	class Animation;
	class SkeletalMesh;

	class ResourceManager
	{
	public:
		static ResourceManager& GetInstance();

		template<typename T>
		std::shared_ptr<T> GetResource(std::string const& filePath);

	private:
		ResourceManager() = default;
		std::unordered_map<std::string, std::shared_ptr<Animation>> mAnimationCache;
		std::unordered_map<std::string, std::shared_ptr<SkeletalMesh>> mSkeletalMeshCache;
	};

	template std::shared_ptr<Animation> ResourceManager::GetResource<Animation>(std::string const&);
	template std::shared_ptr<SkeletalMesh> ResourceManager::GetResource<SkeletalMesh>(std::string const&);


}

#define gResourceMgr inceptionengine::ResourceManager::GetInstance()