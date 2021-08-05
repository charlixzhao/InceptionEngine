#pragma once
#include "AnimPose.h"

namespace inceptionengine
{
	enum class AnimBlendType
	{
		Linear, 
		Cubic,
		Step,
		Inertialization
	};

	class AnimBlender
	{
	public:
		void StartBlending(AnimPose const& fromPose,
			AnimPose const& toPose,
			float blendingDuration,
			AnimBlendType blendingType = AnimBlendType::Linear,
			std::function<void()> blendFinishCallback = []() {});

		std::optional<AnimPose> Blend(float dt, bool indicateStop = true);

		std::optional<AnimPose> Blend(AnimPose const& blendToPose, float dt, bool indicateStop = true);

		bool IsBlending() const;

		void InterruptBlending();

	private:
		AnimPose LinearBlend(float alpha) const;

	private:
		AnimPose mBlendFromPose;
		AnimPose mBlendToPose;
		float mBlendingDuration = 0.0f;
		float mCurrentBlendingTime = 0.0f;
		AnimBlendType mBlendingType = AnimBlendType::Linear;
		std::function<void()> mBlendFinishCallback = []() {};

	};
}