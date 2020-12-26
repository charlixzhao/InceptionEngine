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

		void PlayEventAnimation(EventAnimPlaySetting const& setting);

		void HandReachTarget(Matrix4x4f const& EndEffector);

		void TestAxis();

		void StopAnimation();

		bool IsPlayingAnimation();

		Matrix4x4f GetSocketRefTransformation(std::string const& socketName);

		template<typename T, typename ... Args>
		void SetAnimStateMachine(Args&& ... args)
		{
			mAnimationController->SetAnimStateMachine<T>(mEntityID, mWorld, std::forward<Args>(args)...);
		}

	private:
		friend class AnimationSystem;
		friend class SkeletalMeshComponent;
		friend class SkeletalMeshRenderSystem;

		std::unique_ptr<AnimationController> mAnimationController = nullptr;
		EntityID mEntityID = InvalidEntityID;
		std::reference_wrapper<World> mWorld;

	};
}