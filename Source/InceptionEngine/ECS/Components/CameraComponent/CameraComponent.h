
#pragma once

#include "EngineGlobals/EngineApiPrefix.h"

namespace inceptionengine
{
	class CameraSystem;

	class IE_API CameraComponent
	{
	public:
		CameraComponent(CameraSystem& system);

		CameraComponent(CameraSystem& system, Vec4f position, Vec4f forward);
		
		void SetPosAndForward(Vec3f position, Vec3f forward);

		Matrix4x4f CameraMatrix() const;

		void RotateHorizontal(float degree);

		void RotateVertical(float degree);

		void PrintCameraPramameter();

		void LiftUp(float amount);

		Vec3f GetForwardVec() const;

	private:
		std::reference_wrapper<CameraSystem> mSystem;

		Vec4f mPosition = { 0.0f,0.0f,1.0f,1.0f };
		
		Vec4f mForwardPoint = { 0.0f,0.0f,0.0f,1.0f };
	};
}