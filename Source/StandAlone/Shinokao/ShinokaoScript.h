#pragma once

#include "InceptionEngine.h"
#include <iostream>

#include "Interfaces/IHitable.h"

using namespace inceptionengine;

class ShinokaoScript : public NativeScript, public IHitable
{
public:
	ShinokaoScript(EntityID entityID, std::reference_wrapper<World> world)
		:NativeScript(entityID, world)
	{
		
	}

public:
	virtual void GetHit(IHitable* attacker) override
	{
		EventAnimPlaySetting setting;
		setting.animFilePath = "StandAloneResource\\shinokao\\shinokao_gethit_2.ie_anim";

		GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);

		GetEntity().GetComponent<AnimationComponent>().InsertEventAnimSpeedRangeSecond(0.0f, 0.03f, 0.1f);

		//std::cout << "I'm hit!\n";
	}

	virtual void BeBlocked() override
	{
		EventAnimPlaySetting setting;
		setting.animFilePath = "StandAloneResource\\shinokao\\shinokao_beblocked.ie_anim";
		setting.blendOutDuration = 0.3f;
		GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);
		GetEntity().GetComponent<AnimationComponent>().InsertEventAnimSpeedRangeSecond(0.0f, 0.03f, 0.1f);
	}

	


private:

	virtual void OnTick(float dt) override
	{
		mAttack3Timer += dt;
		if (mAttack3Timer >= mAttack3CD)
		{
			mAttack3Timer = std::fmodf(mAttack3Timer, mAttack3CD);

			EventAnimPlaySetting setting;
			setting.animFilePath = "StandAloneResource\\shinokao\\shinokao_attack_3.ie_anim";;

			AnimNotify attackDetection;
			attackDetection.ratio = 27.0f / 60.0f;
			attackDetection.notify = [&]()
			{
				Vec3f bottom = GetEntity().GetComponent<SkeletalMeshComponent>().GetSocketGlobalTransform("SwordStart")[3];
				Vec3f top = GetEntity().GetComponent<SkeletalMeshComponent>().GetSocketGlobalTransform("SwordEnd")[3];
				std::vector<SphereTraceResult> traceRes = GetEntity().GetWorld().SphereTrace(bottom, top, 15.0f);
				if (traceRes.size() > 0)
				{
					float currentSecond = GetEntity().GetComponent<AnimationComponent>().GetCurrentEventAnimTime();
					GetEntity().GetComponent<AnimationComponent>().InsertEventAnimSpeedRangeSecond(currentSecond, currentSecond + 0.03f, 0.15f);
				}
				for (SphereTraceResult const& res : traceRes)
				{
					EntityID hitEntity = res.entityID;
					if (GetEntity().GetWorld().GetEntity(hitEntity).HasComponent<NativeScriptComponent>())
					{
						IHitable* hitable = dynamic_cast<IHitable*>(GetEntity().GetWorld().GetEntity(hitEntity).GetComponent<NativeScriptComponent>().GetScript());
						if (hitable != nullptr)
						{
							hitable->GetHit(dynamic_cast<IHitable*>(this));
						}
						else
						{
							std::cout << "hit is nullptr\n";
						}
					}

				}
			};

			setting.animNotifies.push_back(attackDetection);
			GetEntity().GetComponent<AnimationComponent>().PlayEventAnimation(setting);
		}
	}

	



private:

	float const mMaxWalkSpeed = 150.0f;

	float mAttack3Timer = 0.0f;

	float const mAttack3CD = 5.0f;


};