#pragma once

namespace inceptionengine
{
	class AnimInstance;
	struct EventAnimPlaySetting;
	class AnimationController;

	class EventAnimController
	{
	public:
		EventAnimController(std::reference_wrapper<AnimationController> animationController);

		~EventAnimController();

		void PlayEventAnimation(EventAnimPlaySetting const& setting);

		bool IsPlayingAnimation() const;

		void Update(float dt);

		std::vector<Matrix4x4f> const& GetCurrentPose() const;

	private:
		std::unique_ptr<AnimInstance> mAnimInstance = nullptr;
		float mRunningTime = 0.0f;
		std::vector<Matrix4x4f> mCurrentPose;
		std::reference_wrapper<AnimationController> mAnimationController;

	};
}