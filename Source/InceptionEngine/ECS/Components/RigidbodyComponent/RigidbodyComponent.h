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
	struct CapsuleCollider;

	class IE_API RigidbodyComponent
	{
	public:

		RigidbodyComponent(EntityID entityID, std::reference_wrapper<World> world);

		~RigidbodyComponent();

		RigidbodyComponent(RigidbodyComponent const&) = delete;

		RigidbodyComponent(RigidbodyComponent&&) noexcept;

		//first field is whether collision occur, second field is whether the collison is blocking
		std::pair<bool, bool> DetectCapsuleCollision(RigidbodyComponent const& other);

		float GetSpeed() const;

		Vec3f GetVelocity() const;

		Vec3f GetAngularVelocity() const;

		void SetVelocity(Vec3f const& v);

		void SetCollider(ColliderType colliderType);

		void SetCapsuleColliderProperties(Vec3f const& bottom, Vec3f const& top, float radius);

		void SetCapsuleColliderBlocking(bool blocking);

	

	private:
		friend class RigidbodySystem;

		//forth position is always 0
		Vec4f mVelocity = {};

		Vec3f mAngulerVelocity = {};

		EntityID mEntityID = InvalidEntityID;
		std::reference_wrapper<World> mWorld;
		
		std::unique_ptr<CapsuleCollider> mCapsuleCollider = nullptr;

	};

}