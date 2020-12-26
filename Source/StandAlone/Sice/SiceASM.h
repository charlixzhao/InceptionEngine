#pragma once

#include "InceptionEngine.h"
#include "SiceScript.h"

using namespace inceptionengine;

class SiceASM : public AnimStateMachine
{
public:
	SiceASM(EntityID entityID, std::reference_wrapper<World> world, EntityID swordID)
		:AnimStateMachine(entityID, world), mSwordID(swordID)
	{
		int idle = CreateState("StandAloneResource\\sice\\sice_idle.ie_anim");
		int run = CreateState("StandAloneResource\\sice\\sice_run.ie_anim");
	
		int idle_battle = CreateState("StandAloneResource\\sice\\sice_idle_battle.ie_anim");
		int run_battle = CreateState("StandAloneResource\\sice\\sice_run_battle.ie_anim");
		
		int idle_to_battle = CreateState("StandAloneResource\\sice\\sice_idle_to_battle.ie_anim",
										 []() {},
										 [&]() {GetEntity().GetWorld().GetEntity(mSwordID).GetComponent<SkeletalMeshComponent>().SetVisibility(true); });
		
		int battle_to_idle = CreateState("StandAloneResource\\sice\\sice_battle_to_idle.ie_anim",
										 [&]() {GetEntity().GetWorld().GetEntity(mSwordID).GetComponent<SkeletalMeshComponent>().SetVisibility(false); },
										 []() {});

		//int idle_to_battle = CreateState("StandAloneResource\\sice\\sice_idle_to_battle.ie_anim");

		//int battle_to_idle = CreateState("StandAloneResource\\sice\\sice_battle_to_idle.ie_anim");

		CreateLink(idle, run, [&]() -> bool
				   {
					   float speed = (GetEntity().GetComponent<RigidbodyComponent>().GetSpeed());
					   return speed > 0.0f;
				   }, 0.15f);

		CreateLink(run, idle, [&]() -> bool
				   {
					   float speed = (GetEntity().GetComponent<RigidbodyComponent>().GetSpeed());
					   return speed == 0.0f;
				   }, 0.15f);

		CreateLink(idle_battle, run_battle, [&]() -> bool
				   {
					   float speed = (GetEntity().GetComponent<RigidbodyComponent>().GetSpeed());
					   return speed > 0.0f;
				   }, 0.15f);

		CreateLink(run_battle, idle_battle, [&]() -> bool
				   {
					   float speed = (GetEntity().GetComponent<RigidbodyComponent>().GetSpeed());
					   return speed == 0.0f;
				   }, 0.15f);

		CreateLink(idle, idle_to_battle, [&]() -> bool 
				   {
					    SiceScript* script = reinterpret_cast<SiceScript*>(GetEntity().GetComponent<NativeScriptComponent>().GetScript());
						return script->InBattleMode();
				   }, 0.01f);

		CreateLink(idle_to_battle, idle_battle, [&]() -> bool
				   {
					   return CurrentStateRemainTime() < 0.01f;
				   }, 0.01f);

		CreateLink(idle_battle, battle_to_idle, [&]() -> bool
				   {
					   SiceScript* script = reinterpret_cast<SiceScript*>(GetEntity().GetComponent<NativeScriptComponent>().GetScript());
					   return !script->InBattleMode();
				   }, 0.01f);

		CreateLink(battle_to_idle, idle, [&]() -> bool
				   {
					   return CurrentStateRemainTime() < 0.01f;
				   }, 0.01f);







		SetEntryState(0);
	}
private:

	EntityID mSwordID = InvalidEntityID;

};