#pragma once

namespace inceptionengine
{
	struct MatchingFeature
	{
	public:
		static size_t constexpr NPoints = 6;
	public:
		std::array<Vec3f, NPoints> trajectory = {};  //10 frame, 20 frame, 30 frame
		std::array<Vec3f, NPoints> facingDirection = {};
		std::vector<Vec3f> featureBonePos;
		std::vector<Vec3f> featureBoneVels;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(trajectory, facingDirection, featureBonePos, featureBoneVels);
		}

	};
}

