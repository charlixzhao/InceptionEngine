
#pragma once

#include "ECS/Entity/EntityID.h"

#include "AnimBlender.h"

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

		bool IsPlayingEventAnimation() const;

		void HandReachTarget(IkChain const& ikChain, Matrix4x4f const& EndEffector);

		void TestAxis(IkChain const& ikChain);

		bool IsBlendingOccuring() const;

		template<typename T, typename ... Args>
		void SetAnimStateMachine(EntityID entityID, std::reference_wrapper<World> world, Args&& ... args)
		{
			static_assert(std::is_base_of_v<AnimStateMachine, T>);
			mAnimStateMachine = std::make_unique<T>(entityID, world, std::forward<Args>(args)...);
		}

		void StartAnimStateMachine();

		void EventAnimFinish(float blendOutDuration);


		Matrix4x4f GetSocketRefTransformation(std::string const& socket);

		float GetCurrentEventAnimTime() const;

		float GetCurrentEventAnimDuration() const;

		void InsertEventAnimSpeedRange(float startRatio, float endRatio, float playSpeed);

	private:
		Matrix4x4f GetBoneGlobalTransform(int boneID);

		float mCurrentTime = 0.0f;

		std::shared_ptr<Skeleton const> mSkeleton = nullptr;

		std::shared_ptr<Animation const> mCurrentAnimation = nullptr;

		//local final pose
		std::vector<Matrix4x4f> mFinalPose;

		std::unique_ptr<AnimStateMachine> mAnimStateMachine = nullptr;

		std::unique_ptr<EventAnimController> mEventAnimController = nullptr;

		AnimBlender mBlender;

	};
}
