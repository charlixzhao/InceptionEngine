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
						   AnimBlendType blendingType = AnimBlendType::Linear);

		std::optional<std::vector<Matrix4x4f>> Blend(float dt);

		bool IsBlending() const;

		void InterruptBlending();

	private:
		std::vector<Matrix4x4f> BlendPose(float alpha) const;

	private:
		std::vector<Matrix4x4f> mBlendFromPose;
		std::vector<Matrix4x4f> mBlendToPose;
		float mBlendingDuration = 0.0f;
		float mCurrentBlendingTime = 0.0f;
		AnimBlendType mBlendingType = AnimBlendType::Linear;

	};
}