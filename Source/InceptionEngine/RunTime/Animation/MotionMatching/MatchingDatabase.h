#pragma once

#include "MatchingFeature.h"

namespace inceptionengine
{
	struct Animation;

	struct MatchingDatabase
	{
	public:
		std::vector<std::string> animPaths;
		std::vector<std::string> featureBones;
		std::vector<std::vector<MatchingFeature>> features;
		std::array<Vec3f, MatchingFeature::NPoints> trajectorySD = {};
		std::array<Vec3f, MatchingFeature::NPoints> facingDirectionSD = {};
		Vec3f currentFacingSD = { NAN, NAN, NAN };
		std::vector<Vec3f> featureBonePosSDs;
		std::vector<Vec3f> featureBoneVelSDs;

	public:
		std::pair<int, int> Query(MatchingFeature const& f1) const;
		float FeatureDistance(MatchingFeature const& f1, MatchingFeature const& f2, int animIndex) const;
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(animPaths, featureBones, features, trajectorySD, facingDirectionSD, currentFacingSD, featureBonePosSDs, featureBoneVelSDs);
		}
	};
}
