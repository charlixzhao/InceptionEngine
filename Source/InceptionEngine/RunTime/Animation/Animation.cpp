
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

		time = std::fmodf(time, mDuration);

		int count = static_cast<int>((static_cast<double>(time) * 30.0));

		if(count >= mBoneTransforms.size() - 1)
			return mBoneTransforms.back();

		double countTime1 = static_cast<double>(count) / 30.0;
		double countTime2 = countTime1 + (1.0 / 30.0);
		assert(time >= countTime1 && time <= countTime2);
		float alpha = 1.0f - ((time - static_cast<float>(countTime1)) / (1.0f / 30.0f)); //weights of the first pose

		return BlendPose(mBoneTransforms[count], mBoneTransforms[count + 1], alpha);
	}
}

