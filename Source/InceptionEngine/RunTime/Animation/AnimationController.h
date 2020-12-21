
#pragma once

#include "ECS/Entity/EntityID.h"

namespace inceptionengine
{
	struct Animation;
	struct Skeleton;
	struct IkChain;
	class World;
	class AnimStateMachine;
	struct EventAnimPlaySetting;
	class EventAnimController;

	class AnimationController
	{
	public:
		AnimationController();

		~AnimationController();

		void Initialize(std::shared_ptr<Skeleton const> skeleton);

		bool Update(float deltaTime);

		std::vector<Matrix4x4f> const& GetFinalPose() const { return mFinalPose; }

		void PlayEventAnimation(EventAnimPlaySetting const& setting);

		void StopAnimation();

		bool IsPlayingAnimation();

		void HandReachTarget(IkChain const& ikChain, Matrix4x4f const& EndEffector);

		void TestAxis(IkChain const& ikChain);

		template<typename T>
		void SetAnimStateMachine(EntityID entityID, std::reference_wrapper<World> world)
		{
			static_assert(std::is_base_of_v<AnimStateMachine, T>);
			mAnimStateMachine = std::make_unique<T>(entityID, world);
		}

		void StartAnimStateMachine();

		void EventAnimFinish();

	private:
		Matrix4x4f GetBoneGlobalTransform(int boneID);

		float mCurrentTime = 0.0f;

		std::shared_ptr<Skeleton const> mSkeleton = nullptr;

		std::shared_ptr<Animation const> mCurrentAnimation = nullptr;

		std::vector<Matrix4x4f> mFinalPose;

		std::unique_ptr<AnimStateMachine> mAnimStateMachine = nullptr;

		std::unique_ptr<EventAnimController> mEventAnimController = nullptr;

	};
}
