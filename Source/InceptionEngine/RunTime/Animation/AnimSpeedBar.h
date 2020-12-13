
#pragma once

namespace inceptionengine
{
	class AnimSpeedBar
	{
	public:
		//return the index of the newly created speed range
		int CreateAnimSpeedRange(float startRatio, float endRatio, float playSpeed = 1.0f);
		

	private:
		//internal data representation
		struct AnimSpeedRange
		{
			float startRatio = 0.0f;
			float endRatio = 1.0f;
			float playSpeed = 1.0f;
			int index = -1;
		};

	private:
		//private methods
		void SortSpeedRanges();

	private:
		//private data member

		std::vector<AnimSpeedRange> mAnimSpeedRanges;

		
	};
}