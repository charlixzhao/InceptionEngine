#pragma once
#include "AnimPose.h"

namespace inceptionengine
{
	struct Skeleton;

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
			std::function<void()> blendFinishCallback = []() {},
			std::shared_ptr<Skeleton> skeleton = nullptr);

		std::optional<AnimPose> Blend(float dt, bool indicateStop = true);

		std::optional<AnimPose> Blend(AnimPose const& blendToPose, float dt, bool indicateStop = true);

		bool IsBlending() const;

		void InterruptBlending();

	private:
		AnimPose LinearBlend(float alpha) const;
		AnimPose InertializedBlend(float t) const;

	private:
		//template<typename T>
		struct InertializationParams
		{
			InertializationParams() = default;
			InertializationParams(float fromVal, float toVal, float v0, float t1);
			Vec3f fromPos = { 0.0f, 0.0f, 0.0f };
			Vec3f translVelDirection = { 0.0f, 0.0f, 0.0f };

			Quaternion4f fromRot;
			Vec3f rotateAxis = { 0.0f, 0.0f, 0.0f };

			float toVal_ = 0.0f;
			float x0_ = 0.0f;
			float v0_ = 0.0f;
			float a0_ = 0.0f;
			float t1_ = 0.0f;
			float A_ = 0.0f;
			float B_ = 0.0f;
			float C_ = 0.0f;

			float Evaluate(float t) const;
		};

	private:
		void ConstructInertializationParams(AnimPose const& fromPose, AnimPose const& toPose, float t1);
	private:
		AnimPose mBlendFromPose;
		AnimPose mBlendToPose;
		float mBlendingDuration = 0.0f;
		float mCurrentBlendingTime = 0.0f;
		AnimBlendType mBlendingType = AnimBlendType::Linear;
		std::vector<InertializationParams> mTranslParams;
		std::vector<InertializationParams> mRotParams;
		std::function<void()> mBlendFinishCallback = []() {};

	};
}