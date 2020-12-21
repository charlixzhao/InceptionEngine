#pragma once
#include "EngineGlobals/EngineApiPrefix.h"

#include "ECS/Entity/EntityID.h"

namespace inceptionengine
{
	class AnimInstance;
	class Entity;
	class World;

	class IE_API AnimStateMachine
	{
	public:
		AnimStateMachine(EntityID entityID, std::reference_wrapper<World> world);

		virtual ~AnimStateMachine();

		AnimStateMachine& operator=(AnimStateMachine&&) noexcept;



	protected:
		struct State;
		struct Link;

		//internal data representation
		struct State
		{
			State() = default;
			~State();
			State(State&&) = default;

			std::unique_ptr<AnimInstance> animInstance = nullptr;
			std::string animInstanceFilePath;
			std::vector<Link> links;
			float runningTime = 0.0f;
		};

		struct Link
		{
			int fromState = -1;
			int toState = -1;
			std::function<bool()> translationFunc;
			float translationDuration = 0.5f;
		};


	protected:
		Entity const& GetEntity();
		int CreateState(std::string const& animFilePath);
		void CreateLink(int fromState, int toState, std::function<bool()> translationfunc, float translationDuration);
		void SetEntryState(int state);

	protected:
		//private data member
		std::vector<State> mStates;
		int mEntryState = -1;
		int mCurrentState = -1;

	private:
		friend class AnimationController;
		void Update(float dt);
		void Restart();

		std::reference_wrapper<World> mWorld;
		EntityID mEntityID = InvalidEntityID;
		float mCurrentBlendingTime = -1.0f;
		float mBlendingDuration = -1.0f;
		std::vector<Matrix4x4f> mFinalPose;
		std::vector<Matrix4x4f> mBlendFromPose;
		std::vector<Matrix4x4f> mBlendToPose;
		Link* mActiveLink = nullptr;
	};
}