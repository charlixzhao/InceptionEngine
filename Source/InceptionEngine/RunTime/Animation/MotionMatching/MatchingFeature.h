#pragma once

namespace inceptionengine
{
	struct MatchingFeature
	{
	public:
		static size_t constexpr NPoints = 3;
	public:
		std::array<Vec3f, NPoints> futureTrajectory = {};  //10 frame, 20 frame, 30 frame
		std::array<Vec3f, NPoints> futureFacing = {};

		Vec3f currentFacing;
		std::vector<Vec3f> featureBonePos;
		std::vector<Vec3f> featureBoneVels;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(futureTrajectory, futureFacing, currentFacing, featureBonePos, featureBoneVels);
		}

	};
}

