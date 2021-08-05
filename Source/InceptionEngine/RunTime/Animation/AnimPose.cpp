#include "IE_PCH.h"
#include "AnimPose.h"

namespace inceptionengine
{
	AnimPose::AnimPose(std::vector<Matrix4x4f> const& lclTransforms, std::vector<Vec3f> const& lclTransVels,
		std::vector<Vec3f> const& globalTransVels, std::vector<Vec3f> lclAngularVelocities,
		std::vector<Vec3f> globalAngularVelocties)

		:boneLclTransforms(lclTransforms), boneLclTranslVelocities(lclTransVels),
		boneGlobalTranslVelocities(globalTransVels), boneLclAngularVelocities(lclAngularVelocities),
		boneGlobalAngularVelocties(globalAngularVelocties)
	{
		assert(lclTransforms.size() == lclTransVels.size() &&
			lclTransVels.size() == globalTransVels.size() &&
			globalTransVels.size() == lclAngularVelocities.size() &&
			lclAngularVelocities.size() == globalAngularVelocties.size());
	}

	AnimPose::AnimPose(std::vector<Matrix4x4f> const& lclTransforms)
		:boneLclTransforms(lclTransforms)
	{
	}

	size_t AnimPose::Size() const
	{
		return boneLclTransforms.size();
	}
}