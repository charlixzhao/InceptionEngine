
#pragma once


namespace inceptionengine
{
	struct Animation;
	struct Skeleton;

	class AnimationController
	{
	public:
		AnimationController();

		~AnimationController();

		bool Update(float deltaTime);

		std::vector<Matrix4x4f> const& GetFinalPose() const { return mFinalPose; }

		void PlayAnimation(std::shared_ptr<Animation const> pAnimation);

		void StopAnimation();

		bool IsPlayingAnimation();

	private:
		float mCurrentTime = 0.0f;

		std::shared_ptr<Animation const> mCurrentAnimation = nullptr;

		std::vector<Matrix4x4f> mFinalPose;

	};
}
