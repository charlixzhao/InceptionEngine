#pragma once

#include "EngineGlobals/EngineApiPrefix.h"
#include "ECS/Entity/EntityID.h"

namespace inceptionengine
{
	enum class ColliderType
	{
		None,
		Capsule,
		Sphere,
		Box
	};

	class World;

	class IE_API RigidbodyComponent
	{
	public:

		RigidbodyComponent(EntityID entityID, std::reference_wrapper<World> world);

		~RigidbodyComponent();

		RigidbodyComponent(RigidbodyComponent const&) = delete;

		RigidbodyComponent(RigidbodyComponent&&) noexcept;


		float GetSpeed() const;

		Vec3f GetVelocity() const;

		Vec3f GetAngularVelocity() const;

		void SetVelocity(Vec3f const& v);

		void RotateInDuration(float degree, Vec3f axis, float duration);

	private:
		friend class RigidbodySystem;

		//forth position is always 0
		Vec4f mVelocity = {};

		Vec3f mAngulerVelocity = {};

		EntityID mEntityID = InvalidEntityID;
		std::reference_wrapper<World> mWorld;

	};

}