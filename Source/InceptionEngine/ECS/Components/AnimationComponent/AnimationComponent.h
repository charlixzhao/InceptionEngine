#pragma once
#include "EngineGlobals/EngineApiPrefix.h"
#include "ECS/Entity/EntityID.h"
#include "RunTime/Animation/AnimationController.h"
#include "AnimStateMachine.h"
#include "EventAnimPlaySetting.h"

namespace inceptionengine
{
	class Entity;
	class World;
	class AnimationController;



	class IE_API AnimationComponent
	{
	public:
		AnimationComponent(EntityID entityID, std::reference_wrapper<World> world);

		~AnimationComponent();

		AnimationComponent(AnimationComponent const&) = delete;

		AnimationComponent(AnimationComponent&&) noexcept;

		/*
		Overall control
		*/

		void StopAnimation();

		/*
		ASM API
		*/

		template<typename T, typename ... Args>
		void SetAnimStateMachine(Args&& ... args)
		{
			mAnimationController->SetAnimStateMachine<T>(mEntityID, mWorld, std::forward<Args>(args)...);
		}

		/*
		return -1 if ASM is not active, which means either the ASM is blending between state, or
		EvenAnim is controlling animation. Otherwise, return
		the current state of ASM
		*/
		int GetCurrentAsmActiveState() const;

		float GetCurrentAsmActiveStateRunningSecond() const;

		/*
		EventAnim API
		*/

		void PlayEventAnimation(EventAnimPlaySetting const& setting);

		bool IsPlayingEventAnimation() const;

		float GetCurrentEventAnimDuration() const;

		float GetCurrentEventAnimTime() const;

		float GetCurrentEventAnimRatio() const;

		void InsertEventAnimSpeedRangeRatio(float startRatio, float endRatio, float playSpeed);

		void InsertEventAnimSpeedRangeSecond(float startSecond, float endSecond, float playSpeed);


		/*
		Socket system API
		*/
		Matrix4x4f GetSocketRefTransformation(std::string const& socketName);
		

		/*
		ik API
		*/
		void SetAimIkChain(std::vector<std::string>const& chainBoneNames, std::vector<float> const& weights);

		void ChainAimToInDuration(Vec3f const& targetPosition, Vec3f const& eyeOffsetInHeadCoord, float duration);

		bool IsAimIkActive() const;

		void DeactivateAimIk(float blendOutDuration = 0.5f);

		void HandReachTarget(Matrix4x4f const& EndEffector);
		void TestAxis();
		void TestAimAxis();

	private:
		friend class AnimationSystem;
		friend class SkeletalMeshComponent;
		friend class SkeletalMeshRenderSystem;

		std::unique_ptr<AnimationController> mAnimationController = nullptr;
		EntityID mEntityID = InvalidEntityID;
		std::reference_wrapper<World> mWorld;

	};
}