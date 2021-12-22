
#pragma once
#include <list>
#include "ECS/Components/AnimationComponent/AnimSpeedRange.h"

namespace inceptionengine
{
	class AnimSpeedBar
	{
	public:
		//return the index of the newly created speed range
		void AddAnimSpeedRanges(std::vector<AnimSpeedRange> const& ranges);

		float QueryAnimSpeed(float ratio) const;

		//insert without check overlap
		void InsertAnimSpeedRange(AnimSpeedRange const& range);
	private:
		//private methods
		void SortSpeedRanges();
		bool InRange(AnimSpeedRange const& range, float ratio) const;
		bool CheckOverlap(AnimSpeedRange const& range) const;
		static bool CompareRange(AnimSpeedRange const& lhs, AnimSpeedRange const& rhs);

	private:
		//private data member

		std::list<AnimSpeedRange> mAnimSpeedRanges;
	};
}
