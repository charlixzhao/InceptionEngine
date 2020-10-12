
#include "IE_PCH.h"
#include "Animation.h"

namespace inceptionengine
{

	std::vector<Matrix4x4f> Animation::Interpolate(float time) const
	{
		int count = static_cast<int>((time * 30.0f));
		return mBoneTransforms[count];
	}
}

