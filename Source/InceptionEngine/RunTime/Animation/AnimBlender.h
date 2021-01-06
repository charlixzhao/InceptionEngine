#pragma once

namespace inceptionengine
{
	enum class AnimBlendType
	{
		Linear, 
		Cubic,
		Step
	};

	class AnimBlender
	{
	public:
		void StartBlending(std::vector<Matrix4x4f> const& fromPose,
						   std::vector<Matrix4x4f> const& toPose,
						   float blendingDuration,
						   AnimBlendType blendingType = AnimBlendType::Linear,
						   std::function<void()> blendFinishCallback = []() {});

		std::optional<std::vector<Matrix4x4f>> Blend(float dt, bool indicateStop = true);

		std::optional<std::vector<Matrix4x4f>> Blend(std::vector<Matrix4x4f> const& blendToPose, float dt, bool indicateStop = true);

		bool IsBlending() const;

		void InterruptBlending();

	public:
		//For testing
		//std::vector<Matrix4x4f> GetToPose() const { return mBlendToPose; }

	private:
		std::vector<Matrix4x4f> BlendPose(float alpha) const;

	private:
		std::vector<Matrix4x4f> mBlendFromPose;
		std::vector<Matrix4x4f> mBlendToPose;
		float mBlendingDuration = 0.0f;
		float mCurrentBlendingTime = 0.0f;
		AnimBlendType mBlendingType = AnimBlendType::Linear;
		std::function<void()> mBlendFinishCallback = []() {};

	};
}