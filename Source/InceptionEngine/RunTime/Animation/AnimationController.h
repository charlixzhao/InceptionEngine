
#pragma once


namespace inceptionengine
{

	class Animation;
	class Skeleton;

	class AnimationController
	{
	public:
		AnimationController(Skeleton const& skeleton);

		~AnimationController();

		bool Update(float deltaTime);

		std::vector<Matrix4x4f> const& GetFinalPose() const { return mFinalPose; }

		void PlayAnimation(std::string const& filePath);

		void StopAnimation();

	private:
		float mCurrentTime = 0.0f;

		std::reference_wrapper<Skeleton const> mSkeleton;

		std::shared_ptr<Animation const> mCurrentAnimation = nullptr;

		std::vector<Matrix4x4f> mFinalPose;

		bool mStopAnimFlag = false;

	};
}
