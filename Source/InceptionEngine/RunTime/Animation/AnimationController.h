
#pragma once


namespace inceptionengine
{

	class Animation;

	class AnimationController
	{
	public:
		AnimationController();

		~AnimationController();

		bool Update(float deltaTime);

		std::vector<Matrix4x4f> const& GetFinalPose() const { return mFinalPose; }

		void PlayAnimation(std::string const& filePath);

		void StopAnimation();

	private:
		float mCurrentTime = 0.0f;

		std::shared_ptr<Animation const> mCurrentAnimation = nullptr;

		std::vector<Matrix4x4f> mFinalPose;

		bool mStopAnimFlag = false;

	};
}
