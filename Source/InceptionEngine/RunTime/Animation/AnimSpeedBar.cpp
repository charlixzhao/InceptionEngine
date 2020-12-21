#include "IE_PCH.h"
#include "AnimSpeedBar.h"

namespace inceptionengine
{

	void AnimSpeedBar::AddAnimSpeedRanges(std::vector<AnimSpeedRange> const& ranges)
	{
		for (auto const& range : ranges)
		{
			if (CheckOverlap(range))
			{
				throw std::runtime_error("animspeed range overlap");
			}
			mAnimSpeedRanges.push_back(range);
		}


		SortSpeedRanges();
	}


	float AnimSpeedBar::QueryAnimSpeed(float ratio) const
	{
		for (auto const& range : mAnimSpeedRanges)
		{
			if (InRange(range, ratio)) return range.playSpeed;
		}
		return 1.0f;
	}

	void AnimSpeedBar::SortSpeedRanges()
	{
		std::sort(mAnimSpeedRanges.begin(), mAnimSpeedRanges.end(), CompareRange);
	}

	bool AnimSpeedBar::InRange(AnimSpeedRange const& range, float ratio) const
	{
		return ratio > range.startRatio && ratio < range.endRatio;
	}

	bool AnimSpeedBar::CheckOverlap(AnimSpeedRange const& range) const
	{
		for (auto const& r : mAnimSpeedRanges)
		{
			if ((r.startRatio < range.startRatio && range.startRatio < r.endRatio) ||
				(range.startRatio < r.startRatio && r.startRatio < range.endRatio)) return true;
		}
		return false;
	}
	bool AnimSpeedBar::CompareRange(AnimSpeedRange const& lhs, AnimSpeedRange const& rhs)
	{
		return lhs.startRatio < rhs.startRatio;
	}
}

