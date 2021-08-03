#pragma once

namespace inceptionengine
{
	struct MatchingFeature
	{
		std::array<Vec3f, 3> trajectory = {};  //10 frame, 20 frame, 30 frame
		std::array<Vec3f, 3> facingDirection = {};
		Vec3f leftFootPosition = { 0.0f, 0.0f,0.0f };
		Vec3f rightFootPosition = { 0.0f, 0.0f,0.0f };
		Vec3f leftFootVelocity = { 0.0f, 0.0f,0.0f };
		Vec3f rightFootVelocity = { 0.0f, 0.0f,0.0f };
		Vec3f hipVelocity = { 0.0f, 0.0f,0.0f };

		//static float FeatureDistance(MatchingFeature const& f1, MatchingFeature const& f2);

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(trajectory, facingDirection, leftFootPosition,
				rightFootPosition, leftFootVelocity, rightFootVelocity, hipVelocity);
		}
	};
}

