#pragma once

#include "EngineGlobals/EngineConcepts.h"


namespace inceptionengine
{
	class SkeletalMeshRenderSystem;
	class SkyboxSystem;
	class TransformSystem;
	class NativeScriptSystem;
	class CameraSystem;
	class AnimationSystem;

	template<CComponent>
	struct SystemType;

	template<>
	struct SystemType<TransformComponent>
	{
		using Type = TransformSystem;
	};

	template<>
	struct SystemType<SkeletalMeshComponent>
	{
		using Type = SkeletalMeshRenderSystem;
	};

	template<>
	struct SystemType<NativeScriptComponent>
	{
		using Type = NativeScriptSystem;
	};

	template<>
	struct SystemType<CameraComponent>
	{
		using Type = CameraSystem;
	};

	template<>
	struct SystemType<AnimationComponent>
	{
		using Type = AnimationSystem;
	};

}