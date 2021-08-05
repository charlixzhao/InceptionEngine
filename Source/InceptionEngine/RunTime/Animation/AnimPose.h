#pragma once


namespace inceptionengine
{

	struct AnimPose
	{
		AnimPose() = default;

		AnimPose(std::vector<Matrix4x4f> const& lclTransforms, std::vector<Vec3f> const& lclTransVels,
			std::vector<Vec3f> const& globalTransVels, std::vector<Vec3f> lclAngularVelocities,
			std::vector<Vec3f> globalAngularVelocties);

		AnimPose(std::vector<Matrix4x4f> const& lclTransforms);

		size_t Size() const;

		std::vector<Matrix4x4f> boneLclTransforms;
		std::vector<Vec3f> boneLclTranslVelocities;
		std::vector<Vec3f> boneGlobalTranslVelocities;
		std::vector<Vec3f> boneLclAngularVelocities;
		std::vector<Vec3f> boneGlobalAngularVelocties;
	};

}