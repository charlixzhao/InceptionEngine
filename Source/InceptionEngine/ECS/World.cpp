
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

#include <cassert>

namespace inceptionengine
{
    class World::WorldImpl
    {
    public:
        WorldImpl(Renderer& renderer, World* pWorld, EntityFriend const& entityFriend);

        EntityID CreateEntity();

        Entity const& GetEntity(EntityID entityID);

        void DeleteEntity(EntityID entityID);

        bool CheckValidEntityID(EntityID entityID);

        void WorldStart();

        void Simulate(float deltaTime);

        void WorldEnd();

        void SetSkybox(std::array<std::string, 6> const& texturePaths);

        ComponentsPool& GetComponentsPool();

        template<typename Component>
        typename SystemType<Component>::Type& GetSystem()
        {
            if constexpr (std::is_same_v<Component, SkeletalMeshComponent>)
            {
                return mSkeletalMeshRenderSystem;
            }
            else if constexpr (std::is_same_v<Component, TransformComponent>)
            {
                return mTransformSystem;
            }
            else if constexpr (std::is_same_v<Component, NativeScriptComponent>)
            {
                return mNativeScriptSystem;
            }
            else if constexpr (std::is_same_v<Component, CameraComponent>)
            {
                return mCameraSystem;
            }
        }

    private:
        void SystemsStart();

        void SystemsEnd();

    private:
        std::reference_wrapper<EntityFriend const> mEntityFriend;

    private:
        World* mWorld = nullptr;

    private:
        friend class Entity;

        std::vector<Entity> mEntities;

        std::queue<EntityID> mDeletedEntities;

        /*
        Storage for all components of entities
        */
        ComponentsPool mEntityComponentPool;

    private:
        /*
        All components of the world. These are components unique in world. Other example includes sunlight, world fog,
        and others
        */
        SkyboxComponent mSkybox;

    private:
        /*
        Systems controlling components of entites
        */
        SkeletalMeshRenderSystem mSkeletalMeshRenderSystem;
        TransformSystem mTransformSystem;
        NativeScriptSystem mNativeScriptSystem;
        CameraSystem mCameraSystem;

    private:
        /*
        Systems controlling components of world
        */
        SkyboxSystem mSkyboxSystem;
    };

    World::WorldImpl::WorldImpl(Renderer& renderer, World* pWorld, EntityFriend const& entityFriend) :
        mWorld(pWorld),
        mEntityFriend(entityFriend),

        /*
        Initialization of system of entities' component
        */
        mSkeletalMeshRenderSystem(renderer, mEntityComponentPool),
        mNativeScriptSystem(mEntityComponentPool),
        mTransformSystem(mEntityComponentPool),
        mCameraSystem(mEntityComponentPool),

        /*
        Initialization of systems of world's component
        */
        mSkyboxSystem(renderer, mSkybox)

    {
        mEntityComponentPool.RegisterComponents<ComponentTypeList>();
    }

    EntityID World::WorldImpl::CreateEntity()
    {
        if (!mDeletedEntities.empty())
        {
            EntityID entityID = mDeletedEntities.front();
            mDeletedEntities.pop();
            mEntities[entityID] = std::move(Entity(entityID, mWorld, mEntityFriend));
            return entityID;
        }
        else
        {
            EntityID entityID = mEntities.size();
            mEntities.emplace_back(entityID, mWorld, mEntityFriend);
            return entityID;
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

    void World::WorldImpl::Simulate(float deltaTime)
    {
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

}

namespace inceptionengine
{
    World::World(Renderer& renderer)
    {
        mWorldImpl = std::make_unique<WorldImpl>(renderer, this, mEntityFriend);
    }

    World::~World() = default;

    EntityID World::CreateEntity()
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

    void World::Simulate(float deltaTime)
    {       
        mWorldImpl->Simulate(deltaTime);
    }

    template<typename Component>
    typename SystemType<Component>::Type& inceptionengine::World::GetSystem()
    {
        return mWorldImpl->GetSystem<Component>();
    }

    template SystemType<SkeletalMeshComponent>::Type& World::GetSystem<SkeletalMeshComponent>();
    template SystemType<TransformComponent>::Type& World::GetSystem<TransformComponent>();
    template SystemType<NativeScriptComponent>::Type& World::GetSystem<NativeScriptComponent>();
    template SystemType<CameraComponent>::Type& World::GetSystem<CameraComponent>();
}

