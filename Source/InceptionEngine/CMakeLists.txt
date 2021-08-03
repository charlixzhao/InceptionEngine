


set(SRC_BUILD_FILES
"IE_PCH.h"
"InceptionEngine.h"
"InceptionEngine.cpp"
"EngineGlobals/EngineApiPrefix.h"
"EngineGlobals/EngineConcepts.h"
"EngineGlobals/RenderGlobals.h"
"ECS/World.h"
"ECS/World.cpp"
"ECS/ComponentSystemTypeTraits.h"
"ECS/Entity/EntityID.h"
"ECS/Entity/EntityFriend.h"
"ECS/Entity/Entity.h"
"ECS/Entity/Entity.cpp"
"ECS/Entity/EntityComponentPool.hpp"
"ECS/Components/Components.h"
"ECS/Components/AudioComponent/AudioComponent.h"
"ECS/Components/AudioComponent/AudioComponent.cpp"
"ECS/Components/ColliderComponent/ColliderComponent.h"
"ECS/Components/ColliderComponent/ColliderComponent.cpp"
"ECS/Components/CameraComponent/CameraComponent.h"
"ECS/Components/CameraComponent/CameraComponent.cpp"
"ECS/Components/NativeScriptComponent/NativeScript.h"
"ECS/Components/NativeScriptComponent/NativeScript.cpp"
"ECS/Components/NativeScriptComponent/KeyInputTypes.h"
"ECS/Components/NativeScriptComponent/NativeScriptComponent.h"
"ECS/Components/NativeScriptComponent/NativeScriptComponent.cpp"
"ECS/Components/SkeletalMeshComponent/SkeletalMeshComponent.h"
"ECS/Components/SkeletalMeshComponent/SkeletalMeshComponent.cpp"
"ECS/Components/SkyboxComponent/SkyboxComponent.h"
"ECS/Components/SkyboxComponent/SkyboxComponent.cpp"
"ECS/Components/TransformComponent/TransformComponent.h"
"ECS/Components/TransformComponent/TransformComponent.cpp"
"ECS/Components/AnimationComponent/AnimationComponent.h"
"ECS/Components/AnimationComponent/AnimationComponent.cpp"
"ECS/Components/AnimationComponent/AnimStateMachine.h"
"ECS/Components/AnimationComponent/AnimStateMachine.cpp"
"ECS/Components/AnimationComponent/EventAnimPlaySetting.h"
"ECS/Components/AnimationComponent/EventAnimPlaySetting.cpp"
"ECS/Components/AnimationComponent/AnimNotify.h"
"ECS/Components/AnimationComponent/AnimNotify.cpp"
"ECS/Components/AnimationComponent/AnimNotifyState.h"
"ECS/Components/AnimationComponent/AnimNotifyState.cpp"
"ECS/Components/AnimationComponent/AnimSpeedRange.h"
"ECS/Components/AnimationComponent/AnimSpeedRange.cpp"
"ECS/Components/RigidbodyComponent/RigidbodyComponent.h"
"ECS/Components/RigidbodyComponent/RigidbodyComponent.cpp"
"ECS/Components/RigidbodyComponent/SphereTraceResult.h"
"ECS/Systems/Systems.h"
"ECS/Systems/SystemBase.h"
"ECS/Systems/SystemBase.cpp"
"ECS/Systems/AnimationSystem/AnimationSystem.h"
"ECS/Systems/AnimationSystem/AnimationSystem.cpp"
"ECS/Systems/CameraSystem/CameraSystem.h"
"ECS/Systems/CameraSystem/CameraSystem.cpp"
"ECS/Systems/NativeScriptSystem/NativeScriptSystem.h"
"ECS/Systems/NativeScriptSystem/NativeScriptSystem.cpp"
"ECS/Systems/TransformSystem/TransformSystem.h"
"ECS/Systems/TransformSystem/TransformSystem.cpp"
"ECS/Systems/SkeletalMeshRenderSystem/SkeletalMeshRenderSystem.h"
"ECS/Systems/SkeletalMeshRenderSystem/SkeletalMeshRenderSystem.cpp"
"ECS/Systems/SkyboxSystem/SkyboxSystem.h"
"ECS/Systems/SkyboxSystem/SkyboxSystem.cpp"
"ECS/Systems/RigidbodySystem/RigidbodySystem.h"
"ECS/Systems/RigidbodySystem/RigidbodySystem.cpp"
"Runtime/KeyInputCallbackRegistry.h"
"Runtime/Animation/AnimPose.h"
"Runtime/Animation/Animation.h"
"Runtime/Animation/Animation.cpp"
"Runtime/Animation/AnimInstance.h"
"Runtime/Animation/AnimInstance.cpp"
"Runtime/Animation/AnimSpeedBar.h"
"Runtime/Animation/AnimSpeedBar.cpp"
"Runtime/Animation/AnimationController.h"
"Runtime/Animation/AnimationController.cpp"
"Runtime/Animation/EventAnimController.h"
"Runtime/Animation/EventAnimController.cpp"
"Runtime/Animation/AnimBlender.h"
"Runtime/Animation/AnimBlender.cpp"
"Runtime/Animation/AnimMask.h"
"Runtime/Animation/AnimMask.cpp"
"Runtime/Animation/IkController.h"
"Runtime/Animation/IkController.cpp"
"Runtime/Animation/MotionMatching/MatchingFeature.h"
"Runtime/Animation/MotionMatching/MatchingFeature.cpp"
"Runtime/Animation/MotionMatching/MatchingDatabase.h"
"Runtime/Animation/MotionMatching/MatchingDatabase.cpp"
"Runtime/Animation/MotionMatching/MotionMatchingController.h"
"Runtime/Animation/MotionMatching/MotionMatchingController.cpp"
"Runtime/SkeletalMesh/SkeletalMesh.h"
"Runtime/SkeletalMesh/SkeletalMesh.cpp"
"Runtime/SkeletalMesh/SkeletalMeshInstance.h"
"Runtime/SkeletalMesh/SkeletalMeshInstance.cpp"
"Runtime/SkeletalMesh/SkeletalMesh.cpp"
"Runtime/SkeletalMesh/Skeleton.h"
"Runtime/SkeletalMesh/Skeleton.cpp"
"Runtime/Collision/CapsuleCollider.h"
"Runtime/Collision/CapsuleCollider.cpp"
"Runtime/RHI/Renderer/Renderer.h"
"Runtime/RHI/Renderer/Renderer.cpp"
"Runtime/RHI/Renderer/Vertex.h"
"Runtime/RHI/Renderer/IRenderObject.h"
"Runtime/RHI/WindowHandler/WindowHandler.h"
"Runtime/RHI/WindowHandler/WindowHandler.cpp"
"Runtime/RHI/WindowHandler/PeripheralInput.h"
"Runtime/Resource/ResourceManager.h"
"Runtime/Resource/ResourceManager.cpp"
)


add_library(InceptionEngine SHARED ${SRC_BUILD_FILES})


source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
         FILES ${SRC_BUILD_FILES})

target_include_directories(InceptionEngine PUBLIC 
${PROJECT_SOURCE_DIR}/Source/InceptionEngine 
${PROJECT_SOURCE_DIR}/Source/InceptionBase 
${PROJECT_SOURCE_DIR}/ThirdParty)

target_link_libraries(InceptionEngine 
InceptionBase 
${PROJECT_SOURCE_DIR}/ThirdParty/ThirdPartyLibraries/glfw/lib/glfw3.lib 
${PROJECT_SOURCE_DIR}/ThirdParty/ThirdPartyLibraries/vulkan/Lib/vulkan-1.lib
${PROJECT_SOURCE_DIR}/ThirdParty/ThirdPartyLibraries/irrKlang/lib/Winx64-visualStudio/irrKlang.lib)

target_compile_options(InceptionEngine PUBLIC /wd26812 /wd26451 /wd26495)

target_precompile_headers(InceptionEngine PRIVATE "IE_PCH.h")

add_custom_command(TARGET InceptionEngine POST_BUILD 
 COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_BINARY_DIR}/Source/InceptionEngine/Debug/InceptionEngine.dll"
        "${PROJECT_BINARY_DIR}/Source/StandAlone/Debug")

add_custom_command(TARGET InceptionEngine POST_BUILD 
 COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_SOURCE_DIR}/ThirdParty/ThirdPartyLibraries/irrKlang/bin/winx64-visualStudio/ikpFlac.dll"
        "${PROJECT_BINARY_DIR}/Source/StandAlone/Debug")

add_custom_command(TARGET InceptionEngine POST_BUILD 
 COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_SOURCE_DIR}/ThirdParty/ThirdPartyLibraries/irrKlang/bin/winx64-visualStudio/ikpMP3.dll"
        "${PROJECT_BINARY_DIR}/Source/StandAlone/Debug")

add_custom_command(TARGET InceptionEngine POST_BUILD 
 COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_SOURCE_DIR}/ThirdParty/ThirdPartyLibraries/irrKlang/bin/winx64-visualStudio/irrKlang.dll"
        "${PROJECT_BINARY_DIR}/Source/StandAlone/Debug")

