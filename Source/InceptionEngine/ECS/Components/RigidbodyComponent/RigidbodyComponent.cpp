#include "IE_PCH.h"
#include "RigidbodyComponent.h"

namespace inceptionengine
{
	RigidbodyComponent::RigidbodyComponent(EntityID entityID, std::reference_wrapper<World> world)
		:mEntityID(entityID), mWorld(world)
	{

	}

	RigidbodyComponent::~RigidbodyComponent() = default;

	RigidbodyComponent::RigidbodyComponent(RigidbodyComponent&&) noexcept = default;

	float RigidbodyComponent::GetSpeed() const
	{
		return VecLength(mVelocity);
	}
	Vec3f RigidbodyComponent::GetVelocity() const
	{
		return mVelocity;
	}
	Vec3f RigidbodyComponent::GetAngularVelocity() const
	{
		return mAngulerVelocity;
	}
	void RigidbodyComponent::SetVelocity(Vec3f const& v)
	{
		mVelocity = Vec4f(v[0], v[1], v[2], 0.0f);
	}
}