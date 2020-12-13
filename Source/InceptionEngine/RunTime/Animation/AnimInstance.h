
#pragma once
#include "AnimSpeedBar.h"
namespace inceptionengine
{
	struct Animation;

	enum class AnimInterpType
	{
		Linear,
		Step
	};

	class AnimInstance
	{
	public:
		AnimInstance(std::string const& animFilePath);

		std::vector<Matrix4x4f> Sample(float time, AnimInterpType interpType = AnimInterpType::Linear);

		~AnimInstance();

		
	private:
		std::shared_ptr<Animation const> animationResource;
		std::string animationFilePath;
		bool rootMotion = false;
		AnimSpeedBar animSpeedBar;
		Matrix4x4f rootTransform = {};
		AnimInterpType interpType = AnimInterpType::Linear;
	};
}