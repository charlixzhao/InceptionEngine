


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
"Runtime/KeyInputCallbackRegistry.h"
"Runtime/Animation/AnimPose.h"
"Runtime/Animation/Animation.h"
"Runtime/Animation/Animation.cpp"
"Runtime/Animation/AnimationController.h"
"Runtime/Animation/AnimationController.cpp"
"Runtime/SkeletalMesh/SkeletalMesh.h"
"Runtime/SkeletalMesh/SkeletalMesh.cpp"
"Runtime/SkeletalMesh/SkeletalMeshInstance.h"
"Runtime/SkeletalMesh/SkeletalMeshInstance.cpp"
"Runtime/SkeletalMesh/SkeletalMesh.cpp"
"Runtime/SkeletalMesh/Skeleton.h"
"Runtime/SkeletalMesh/Skeleton.cpp"
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
${PROJECT_SOURCE_DIR}/ThirdParty/ThirdPartyLibrary/glfw/lib/glfw3.lib 
${PROJECT_SOURCE_DIR}/ThirdParty/ThirdPartyLibrary/vulkan/Lib/vulkan-1.lib)

target_compile_options(InceptionEngine PUBLIC /wd26812 /wd26451 /wd26495)

target_precompile_headers(InceptionEngine PRIVATE "IE_PCH.h")

add_custom_command(TARGET InceptionEngine POST_BUILD 
 COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_BINARY_DIR}/Source/InceptionEngine/Debug/InceptionEngine.dll"
        "${PROJECT_BINARY_DIR}/Source/StandAlone/Debug")

