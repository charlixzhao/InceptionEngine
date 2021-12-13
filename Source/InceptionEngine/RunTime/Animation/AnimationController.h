
#pragma once

#include "ECS/Entity/EntityID.h"

#include "AnimBlender.h"

namespace inceptionengine::dynamics
{
	struct KinematicsTree;
}

namespace inceptionengine
{
	struct Animation;
	struct Skeleton;
	struct IkChain;
	class World;
	class AnimStateMachine;
	struct EventAnimPlaySetting;
	class EventAnimController;
	class IkController;
	struct AnimNotify;
	class MotionMatchingController;

	

	class AnimationController
	{
	public:
		AnimationController();

		~AnimationController();

		void Initialize(std::shared_ptr<Skeleton const> skeleton);

		bool Update(float deltaTime);

		AnimPose const& GetFinalPose() const { return mFinalPose; }

		void PlayEventAnimation(EventAnimPlaySetting const& setting);

		void StopAnimation();

		void FlipFlopStopAnimation();

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

		void EventAnimFinish(float blendOutDuration, std::function<void()> blendFinishCallback);

		Matrix4x4f GetSocketRefTransformation(std::string const& socket);

		float GetCurrentEventAnimTime() const;

		float GetCurrentEventAnimDuration() const;

		void InsertEventAnimSpeedRange(float startRatio, float endRatio, float playSpeed);

		//position should be model space
		
		void TestAimAxis();

		void SetAimIkChain(std::vector<std::string> const& boneNames, std::vector<float> const& weights);

		void ActivateAimIk();

		void DeactivateAimIk(float blendOutDuration);

		void ChainAimToInDuration(Matrix4x4f modelTransform, Vec3f const& targetPosition, Vec3f const& eyeOffsetInHeadCoord, float duration);

		bool IsAimIkActive() const;

		int GetCurrentAsmActiveState() const;

		float GetCurrentAsmActiveStateRunningSecond() const;

		void InsertAnimNotify(AnimNotify const& notify);

		void SetMatchingDatabase(std::string const& filePath);

		void SetInputControl(Vec3f const& input);

		void SetKinematicsTree();

		void ApplyExtForce(int bodyID, Vec3f const& force, Vec3f const& location, float time);

	private:
		friend class IkController;

		Matrix4x4f GetBoneModelTransform(std::vector<Matrix4x4f> const& lclPose, int boneID) const;
		Matrix4x4f GetBoneGlobalTransform(Matrix4x4f const& modelTransform, std::vector<Matrix4x4f> const& lclPose, int boneID) const;

	private:
		float mCurrentTime = 0.0f;

		std::shared_ptr<Skeleton const> mSkeleton = nullptr;

		std::shared_ptr<Animation const> mCurrentAnimation = nullptr;

		//local final pose
		AnimPose mFinalPose;

		//global bone velocities
		std::vector<Vec3f> mBoneVelocities;

		std::unique_ptr<AnimStateMachine> mAnimStateMachine = nullptr;

		std::unique_ptr<EventAnimController> mEventAnimController = nullptr;

		AnimBlender mBlender;

		bool mStopAnim = false;

		std::unique_ptr<IkController> mIkController = nullptr;

		//first one is used to blend to the ik pose when aim ik occur
		//second one is uesed to blend to normal pose (ASM or EventAnim) when aim ik is inactivated
		std::pair<AnimBlender, AnimBlender> mAimIkBlender;

	private:
		std::unique_ptr<MotionMatchingController> mMotionMatchingController = nullptr;
		float mFeatureQueryTimer = 0.0f;
		float const mFeatureQueryInterval = 5.0f/30.0f;

	private:

		struct TimerEvent
		{
			float remainingTime = 0.0f;
			std::function<void()> End = []() {; };
		};

		std::unique_ptr<dynamics::KinematicsTree> mKinematicsTree = nullptr;
		std::list<TimerEvent> mTimerEvents;
		std::vector<dynamics::SpatialForce> mExtForces;
	};
}
