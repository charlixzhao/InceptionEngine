
#include "IE_PCH.h"
#include "CameraComponent.h"
#include "ECS/Entity/Entity.h"
#include "ECS/World.h"
#include "ECS/Components/TransformComponent/TransformComponent.h"


namespace inceptionengine
{
	CameraComponent::CameraComponent(EntityID entityID, std::reference_wrapper<World> world)
		:mEntityID(entityID), mWorld(world)
	{

	}




	void CameraComponent::SetPosAndForward(Vec3f position, Vec3f forward)
	{
		mPosition = Vec4f(position.x, position.y, position.z, 1.0f);
		mForwardPoint = Vec4f(forward.x, forward.y, forward.z, 1.0f);
		Vec3f a = mPosition;
		Vec3f b = NormalizeVec(Vec3f(mForwardPoint - mPosition));
		float t1 = -a[0] / b[0];
		float t2 = -a[2] / b[2];
		if (std::isnan(t1) && std::isnan(t2))
			throw std::runtime_error("impossible math result");
		float t = std::isnan(t1) ? t2 : t1;
		//if (std::abs(t1 - t2) > 0.01f)
			//throw std::runtime_error("not look to the y axis\n");
		mLookAtPoint = Vec4f(0.0f, a[1] + b[1] * t, 0.0f, 1.0f);
		std::cout << "look at position is " << VecToString(mLookAtPoint) << std::endl;

	}

	/*
	move this to camera system because it need to communicate with transform component
	*/
	Matrix4x4f CameraComponent::CameraMatrix() const
	{
		Matrix4x4f globalPosition = mWorld.get().GetEntity(mEntityID).GetComponent<TransformComponent>().GetWorldTransform();
		globalPosition = Translate(globalPosition[3]);

		Vec4f worldPosition = globalPosition * mPosition;
		Vec4f worldForward = globalPosition * mForwardPoint;

		return LookAt(Vec3f(worldPosition.x, worldPosition.y, worldPosition.z), Vec3f(worldForward.x, worldForward.y, worldForward.z), Vec3f(0.0f, 1.0f, 0.0f));
	}


	void CameraComponent::RotateHorizontal(float degree)
	{
		auto lookAtDirection = mForwardPoint - mPosition;
		auto rotateionAxis = CrossProduct(Vec3f(lookAtDirection.x, lookAtDirection.y, lookAtDirection.z), Vec3f(0.0f, 1.0f, 0.0f));
		mPosition = mLookAtPoint + Rotate(mPosition - mLookAtPoint, degree, rotateionAxis);
		mForwardPoint = mLookAtPoint + Rotate(mForwardPoint - mLookAtPoint, degree, rotateionAxis);
	}

	void CameraComponent::RotateVertical(float degree)
	{
		mPosition = Rotate(mPosition, degree, Vec3f(0.0f, 1.0f, 0.0f));
		mForwardPoint = Rotate(mForwardPoint, degree, Vec3f(0.0f, 1.0f, 0.0f));
		if (mCameraControlYaw)
		{
			TransformComponent& transformComponent = mWorld.get().GetEntity(mEntityID).GetComponent<TransformComponent>();
			transformComponent.Rotate(Vec3f(0.0f, 1.0f, 0.0f), degree);
		}
	}

	void CameraComponent::PrintCameraPramameter()
	{
		std::cout << "position: " << VecToString(mPosition) << std::endl;
		std::cout << "position: " << VecToString(mForwardPoint) << std::endl;
	}

	void CameraComponent::LiftUp(float amount)
	{
		mPosition.y += amount;
		mForwardPoint.y += amount;
	}

	Vec3f CameraComponent::GetForwardVec() const
	{
		Vec3f temp = Vec3f(mForwardPoint - mPosition);
		temp.y = 0;
		return NormalizeVec(temp);
	}
	void CameraComponent::SetCameraControlYaw(bool enable)
	{
		mCameraControlYaw = enable;
	}
	bool CameraComponent::GetCameraControlYaw() const
	{
		return mCameraControlYaw;
	}
	Vec4f CameraComponent::GetCameraRefPosition() const
	{
		return mPosition;
	}

	Vec4f CameraComponent::GetLootAtDirection() const
	{
		return NormalizeVec(mForwardPoint - mPosition);
	}

	void CameraComponent::MoveForward(float amount)
	{
		Vec4f currentPostion = mPosition;
		Vec4f currentForward = mForwardPoint;

		currentPostion += amount * GetLootAtDirection();
		currentForward += amount * GetLootAtDirection();

		if (VecLength(currentPostion - mLookAtPoint) <= 80.0f ||
			VecLength(currentPostion - mLookAtPoint) >= 350.0f)
		{
			return;
		}
		else
		{
			mPosition = currentPostion;
			mForwardPoint = currentForward;
		}
	}

}
