#include "IE_PCH.h"

#include "AnimInstance.h"
#include "Animation.h"
#include "RunTime/Resource/ResourceManager.h"

namespace inceptionengine
{
	AnimInstance::AnimInstance(std::string const& animFilePath)
	{
		std::shared_ptr<Animation> pAnimation = gResourceMgr.GetResource<Animation>(animFilePath);
		assert(pAnimation != nullptr);
		animationResource = pAnimation;
		animationFilePath = animFilePath;
	}

	std::vector<Matrix4x4f> AnimInstance::Sample(float time, AnimInterpType interpType)
	{
		return animationResource->Interpolate(time);
	}

	AnimInstance::~AnimInstance() = default;
}

