
#include "IE_PCH.h"
#include "Animation.h"
#include "AnimPose.h"

namespace inceptionengine
{


	std::vector<Matrix4x4f> Animation::Interpolate(float time) const
	{
		if (time <= 0)
		{
			return mBoneTransforms[0];
		}

		time = std::fmod(time, mDuration);

		int count = static_cast<int>((time * 30.0f));

		if(count >= mBoneTransforms.size() - 1)
			return mBoneTransforms.back();

		float countTime1 = static_cast<float>(count) * (1.0f / 30.0f);
		float countTime2 = countTime1 + (1.0f / 30.0f);
		assert(time >= countTime1 && time <= countTime2);
		float alpha = 1.0f - ((time - countTime1) / (1.0f / 30.0f)); //weights of the first pose

		return BlendPose(mBoneTransforms[count], mBoneTransforms[count + 1], alpha);
	}
}

