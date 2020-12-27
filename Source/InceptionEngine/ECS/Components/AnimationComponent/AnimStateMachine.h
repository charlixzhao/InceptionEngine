#pragma once
#include "EngineGlobals/EngineApiPrefix.h"

#include "ECS/Entity/EntityID.h"
#include "RunTime/Animation/AnimBlender.h"

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

			std::function<void()> enterCallback = []() {};
			std::function<void()> leaveCallback = []() {};
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
		int CreateState(std::string const& animFilePath, 
						std::function<void()> enterCallback = []() {},
						std::function<void()> leaveCallback = []() {});
		void CreateLink(int fromState, int toState, std::function<bool()> translationfunc, float translationDuration);
		void SetEntryState(int state);
		float CurrentStateRemainTime() const;

	protected:
		//private data member
		std::vector<State> mStates;
		int mEntryState = -1;
		int mCurrentState = -1;

	private:
		friend class AnimationController;
		void Update(float dt);
		void Restart();
		int FindRestartState(int state);

	private:
		std::reference_wrapper<World> mWorld;
		EntityID mEntityID = InvalidEntityID;

		std::vector<Matrix4x4f> mFinalPose;

		Link* mActiveLink = nullptr;

		int mRestartState = -1;

		AnimBlender mTransitionBlender;
		
	};
}