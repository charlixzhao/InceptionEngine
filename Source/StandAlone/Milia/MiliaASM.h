#pragma once

#include "InceptionEngine.h"
using namespace inceptionengine;

class MiliaASM : public AnimStateMachine
{
public:
	MiliaASM(EntityID entityID, std::reference_wrapper<World> world)
		:AnimStateMachine(entityID, world)
	{
		CreateState("StandAloneResource\\milia\\milia_idle.ie_anim");
		CreateState("StandAloneResource\\milia\\milia_walk.ie_anim");
		CreateLink(0, 1, [&]() -> bool
				   {
					   float speed = (GetEntity().GetComponent<RigidbodyComponent>().GetSpeed());
					   return speed > 0.0f;
				   }, 0.15f);

		CreateLink(1, 0, [&]() -> bool
				   {
					   float speed = (GetEntity().GetComponent<RigidbodyComponent>().GetSpeed());
					   return speed == 0.0f;
				   }, 0.15f);

		SetEntryState(0);
	}

private:
};