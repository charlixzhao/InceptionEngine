#pragma once

#include "MatchingFeature.h"

namespace inceptionengine
{
	struct Animation;

	struct MatchingDatabase
	{
		std::vector<MatchingFeature> features;

		std::array<Vec3f, 3> trajectorySD = {};
		std::array<Vec3f, 3> facingDirectionSD = {};
		Vec3f leftFootPositionSD = { NAN, NAN, NAN };
		Vec3f rightFootPositionSD = { NAN, NAN, NAN };
		Vec3f leftFootVelocitySD = { NAN, NAN, NAN };
		Vec3f rightFootVelocitySD = { NAN, NAN, NAN };
		Vec3f hipVelocitySD = { NAN, NAN, NAN };

		std::vector<std::string> animPaths;

		int Query(MatchingFeature const& f1);
		float FeatureDistance(MatchingFeature const& f1, MatchingFeature const& f2);

		static MatchingDatabase ConstructFromAnim(Animation const& anim);

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(animPaths, features, trajectorySD, facingDirectionSD, leftFootPositionSD,
				rightFootPositionSD, leftFootVelocitySD, rightFootVelocitySD, hipVelocitySD);
		}
	};






	
}
