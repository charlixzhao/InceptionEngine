
#pragma once

#include "EngineGlobals/EngineApiPrefix.h"
#include "ECS/Entity/EntityID.h"

namespace inceptionengine
{
	class CameraSystem;
	class World;

	class IE_API CameraComponent
	{
	public:
		CameraComponent(EntityID entityID, std::reference_wrapper<World> world);
		
		void SetPosAndForward(Vec3f position, Vec3f forward);

		Matrix4x4f CameraMatrix() const;

		void RotateHorizontal(float degree);

		void RotateVertical(float degree);

		void PrintCameraPramameter();

		void LiftUp(float amount);

		Vec3f GetForwardVec() const;

		void SetCameraControlYaw(bool enable);

		bool GetCameraControlYaw() const;

		Vec4f GetCameraRefPosition() const;

	private:

		Vec4f mPosition = { 0.0f,0.0f,1.0f,1.0f };
		
		Vec4f mForwardPoint = { 0.0f,0.0f,0.0f,1.0f };

		EntityID mEntityID;

		std::reference_wrapper<World> mWorld;

		bool mCameraControlYaw = false;
	};
}