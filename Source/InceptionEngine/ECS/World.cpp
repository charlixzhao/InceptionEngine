#include "IE_PCH.h"

#include "World.h"

/*
include entity and component storage
*/
#include "Entity/Entity.h"
#include "Entity/EntityComponentPool.hpp"

/*
include component of the world
*/
#include "ECS/Components/SkyboxComponent/SkyboxComponent.h"

/*
include all systems
*/
#include "ECS/Systems/Systems.h"

//for test purpose
#include "RunTime/Resource/ResourceManager.h"
#include "RunTime/SkeletalMesh/Skeleton.h"
#include "RunTime/Animation/Animation.h"

namespace inceptionengine
{


    World::WorldImpl::WorldImpl(Renderer& renderer, World& world, EntityFriend const& entityFriend) :
        mWorld(world),
        mEntityFriend(entityFriend),

        /*
        Initialization of system of entities' component
        */
        mTransformSystem(mEntityComponentPool),
        mCameraSystem(renderer, mEntityComponentPool),
        mSkeletalMeshRenderSystem(renderer, mEntityComponentPool, mTransformSystem),
        mAnimationSystem(mEntityComponentPool),
        mNativeScriptSystem(mEntityComponentPool),
        mRigidbodySystem(mEntityComponentPool),
        /*
        Initialization of systems of world's component
        */
        mSkyboxSystem(renderer, mSkybox)

    {
        mEntityComponentPool.RegisterComponents<ComponentTypeList>();
    }

    Entity const& World::WorldImpl::CreateEntity()
    {
        if (!mDeletedEntities.empty())
        {
            EntityID entityID = mDeletedEntities.front();
            mDeletedEntities.pop();
            mEntities[entityID] = std::move(Entity(entityID, mWorld, mEntityFriend));
            mEntities[entityID].AddComponent<TransformComponent>();
            return mEntities[entityID];
        }
        else
        {
            EntityID entityID = mEntities.size();
            mEntities.emplace_back(entityID, mWorld, mEntityFriend).AddComponent<TransformComponent>();
            return mEntities.back();
        }
    }

    Entity const& World::WorldImpl::GetEntity(EntityID entityID)
    {
        assert(CheckValidEntityID(entityID) == true && "See log for error message!");
        return mEntities[entityID];
    }

    void World::WorldImpl::DeleteEntity(EntityID entityID)
    {
        assert(CheckValidEntityID(entityID) == true && "See log for error message!");
        mEntities[entityID].mID = Entity::InvalidID;
        mEntityComponentPool.DeleteEntity(entityID);
        mDeletedEntities.push(entityID);
    }

    bool World::WorldImpl::CheckValidEntityID(EntityID entityID)
    {
        if (entityID >= mEntities.size())
        {
            std::cerr << "Entity ID is larger than container's size!\n";
            return false;
        }
        else if (!mEntities[entityID].IsValid())
        {
            std::cerr << "Entity ID is invalid. It might be deleted!\n";
            return false;
        }
        return true;
    }

    void World::WorldImpl::WorldStart()
    {
        SystemsStart();
    }

    void World::WorldImpl::Simulate(float deltaTime, PeripheralInput keyInputs)
    {
        mNativeScriptSystem.Update(keyInputs.keyInputs, keyInputs.mouseDeltaPos, deltaTime);

        mCameraSystem.Update(deltaTime);

        mAnimationSystem.Update(deltaTime);

        mRigidbodySystem.Update(deltaTime);

        mTransformSystem.Update(deltaTime);



        mSkeletalMeshRenderSystem.Update(deltaTime);
    }

    void World::WorldImpl::WorldEnd()
    {
        SystemsEnd();
    }

    void World::WorldImpl::SetSkybox(std::array<std::string, 6> const& texturePaths)
    {
        mSkyboxSystem.SetSkybox(texturePaths);
    }

    void World::WorldImpl::SetGameCamera(CameraComponent const& camera)
    {
        mCameraSystem.SetGameCamera(camera);
    }


    Matrix4x4f GetBoneModelTransform(std::vector<Matrix4x4f> const& lcl, Skeleton const& sk, int boneID)
    {
        Matrix4x4f globalTransform = lcl[boneID];
        auto const& bone = sk.mBones[boneID];
        int parentID = bone.parentID;
        while (parentID != -1)
        {
            globalTransform = lcl[parentID] * globalTransform;
            parentID = sk.mBones[parentID].parentID;
        }
        return globalTransform;
    }

    void World::WorldImpl::DrawAnimationTest()
    {
        auto anim = gResourceMgr.GetResource<Animation>("StandAloneResource\\milia_sm\\milia_a_pose.ie_anim");
        for (int i = 0; i < anim->mBoneTransforms[0].size(); i++)
        {
            Entity const& ent = CreateEntity();
            ent.AddComponent<SkeletalMeshComponent>().SetMesh("StandAloneResource\\cube\\cube_mesh.ie_skmesh");
            ent.GetComponent<TransformComponent>().SetWorldTransform(GetBoneModelTransform(anim->mBoneTransforms[10], *anim->mSkeleton, i) * Scale(0.1f, 0.1f, 0.1f));
        }
        
    }

    ComponentsPool& World::WorldImpl::GetComponentsPool()
    {
        return mEntityComponentPool;
    }

    void World::WorldImpl::SystemsStart()
    {
        /*
        system of entties' components start
        */
        mNativeScriptSystem.Start();
        mAnimationSystem.Start();
        mSkeletalMeshRenderSystem.Start();

        /*
        systems of world's components start
        */
        mSkyboxSystem.Start();
    }

    void World::WorldImpl::SystemsEnd()
    {
        /*
        system of entties' components end
        */
        mNativeScriptSystem.End();
        mSkeletalMeshRenderSystem.End();

        /*
        systems of world's components end
        */
        mSkyboxSystem.End();
    }

    std::vector<SphereTraceResult> World::WorldImpl::SphereTrace(Vec3f const& bottom, Vec3f const& top, float radius)
    {
        return mRigidbodySystem.SphereTrace(bottom, top, radius);
    }

}

namespace inceptionengine
{
    World::World(Renderer& renderer)
    {
        mWorldImpl = std::make_unique<WorldImpl>(renderer, *this, mEntityFriend);
    }

    World::~World() = default;

    Entity const& World::CreateEntity()
    {
        return mWorldImpl->CreateEntity();
    }

    Entity const& World::GetEntity(EntityID entityID)
    {
        return mWorldImpl->GetEntity(entityID);
    }

    void World::DeleteEntity(EntityID entityID)
    {
        mWorldImpl->DeleteEntity(entityID);
    }

    bool World::CheckValidEntityID(EntityID entityID)
    {
        return mWorldImpl->CheckValidEntityID(entityID);
    }

    void World::SetSkybox(std::array<std::string, 6> const& texturePaths)
    {
        mWorldImpl->SetSkybox(texturePaths);
    }

    void World::SetGameCamera(CameraComponent const& camera)
    {
        mWorldImpl->SetGameCamera(camera);
    }

    void World::DrawAnimationTest()
    {
        mWorldImpl->DrawAnimationTest();
    }

    void World::WorldStart()
    {
        mWorldImpl->WorldStart();
    }

    void World::WorldEnd()
    {
        mWorldImpl->WorldEnd();
    }

    ComponentsPool& World::GetComponentsPools()
    {
        return mWorldImpl->GetComponentsPool();
    }


    void World::Simulate(float deltaTime, PeripheralInput keyInputs)
    {
        mWorldImpl->Simulate(deltaTime, keyInputs);
    }

    std::vector<SphereTraceResult> World::SphereTrace(Vec3f const& bottom, Vec3f const& top, float radius)
    {
        return mWorldImpl->SphereTrace(bottom, top, radius);
    }
}

