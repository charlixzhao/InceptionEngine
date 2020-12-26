#include "IE_PCH.h"
#include "AnimStateMachine.h"

#include "ECS/Entity/Entity.h"
#include "ECS/World.h"

#include "RunTime/Animation/AnimInstance.h"
#include "RunTime/Animation/AnimPose.h"



namespace inceptionengine
{
    AnimStateMachine::AnimStateMachine(EntityID entityID, std::reference_wrapper<World> world)
        :mWorld(world), mEntityID(entityID)
    {

    }

    AnimStateMachine::~AnimStateMachine() = default;

    AnimStateMachine& AnimStateMachine::operator=(AnimStateMachine&&) noexcept = default;

 
    void AnimStateMachine::Update(float dt)
    {
        if (mCurrentBlendingTime >= 0.0f)
        {
            mCurrentBlendingTime += dt;
            if (mCurrentBlendingTime <= mBlendingDuration)
            {
                //continue blending
                float alpha = 1.0f - mCurrentBlendingTime / mBlendingDuration;
                mFinalPose = BlendPose(mBlendFromPose, mBlendToPose, alpha);
            }
            else
            {
                //finish blending
                mCurrentBlendingTime = mBlendingDuration = -1.0f;
                mCurrentState = mActiveLink->toState;
                mStates[mCurrentState].enterCallback();
                mStates[mCurrentState].runningTime = 0.0f;
                mActiveLink = nullptr;
            }

            return;
        }

        if (mCurrentState == -1)
            return;

        auto& currentState = mStates[mCurrentState];
        currentState.runningTime += dt;
        currentState.runningTime = std::fmod(currentState.runningTime, currentState.animInstance->GetDuration());
        for (auto& link : currentState.links)
        {
            if (link.translationFunc())
            {
                currentState.leaveCallback();
                auto& blendToState = mStates[link.toState];
                mBlendFromPose = currentState.animInstance->Sample(currentState.runningTime);
                mBlendToPose = blendToState.animInstance->Sample(0.0f);
                mBlendingDuration = link.translationDuration;
                mCurrentBlendingTime = 0.0f;
                mActiveLink = &link;
                return;
            }
        }

        mFinalPose = currentState.animInstance->Sample(currentState.runningTime);
    }

    int AnimStateMachine::FindRestartState(int state)
    {
        for (auto& link : mStates[state].links)
        {
            if (link.translationFunc())
            {
                return FindRestartState(link.toState);
            }
        }

        return state;
    }

    void AnimStateMachine::Restart()
    {
        mCurrentState = FindRestartState(mRestartState);

        mStates[mCurrentState].runningTime = 0.0f;
        mCurrentBlendingTime = mBlendingDuration = -1.0f;
        mActiveLink = nullptr;
    }


    Entity const& AnimStateMachine::GetEntity()
    {
        return mWorld.get().GetEntity(mEntityID);
    }

    int AnimStateMachine::CreateState(std::string const& animFilePath,
                                      std::function<void()> enterCallback,
                                      std::function<void()> leaveCallback)
    {
        int index = mStates.size();
        mStates.emplace_back();
        mStates.back().animInstance = std::make_unique<AnimInstance>(animFilePath);
        mStates.back().enterCallback = enterCallback;
        mStates.back().leaveCallback = leaveCallback;
        return index;
    }

    void AnimStateMachine::CreateLink(int fromState, int toState, std::function<bool()> translationfunc, float translationDuration)
    {
        assert(fromState >= 0 && fromState < mStates.size());
        assert(toState >= 0 && toState < mStates.size());
        Link link;
        link.fromState = fromState;
        link.toState = toState;
        link.translationFunc = translationfunc;
        link.translationDuration = translationDuration;
        mStates[fromState].links.push_back(link);
    }

    void AnimStateMachine::SetEntryState(int state)
    {
        mEntryState = state;
    }

    AnimStateMachine::State::~State() = default;


    float AnimStateMachine::CurrentStateRemainTime() const
    {
        return mStates[mCurrentState].animInstance->GetDuration() - mStates[mCurrentState].runningTime;
    }

}