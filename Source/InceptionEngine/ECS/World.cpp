
#include "World.h"
#include "Components/Components.h"
#include "RunTime/RHI/Renderer/Renderer.h"
#include "RunTime/RHI/WindowHandler/WindowHandler.h"

#include <cassert>
#include <iostream>

namespace inceptionengine
{
    World::World(Renderer& renderer) :
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

    EntityID World::CreateEntity()
    {
        if (!mDeletedEntities.empty())
        {
            EntityID entityID = mDeletedEntities.front();
            mDeletedEntities.pop();
            mEntities[entityID] = Entity(entityID, this, EntityFriend());
            return entityID;
        }
        else
        {
            EntityID entityID = mEntities.size();
            mEntities.emplace_back(entityID, this, EntityFriend());
            return entityID;
        }
    }

    Entity const& World::GetEntity(EntityID entityID)
    {
        assert(CheckValidEntityID(entityID) == true && "See log for error message!");
        return mEntities[entityID];
    }

    void World::DeleteEntity(EntityID entityID)
    {
        assert(CheckValidEntityID(entityID) == true && "See log for error message!");
        mEntities[entityID].mID = Entity::InvalidID;
        mEntityComponentPool.DeleteEntity(entityID);
        mDeletedEntities.push(entityID);
    }

    bool World::CheckValidEntityID(EntityID entityID)
    {
        if (entityID >= mEntities.size())
        {
            std::cerr << "Entity ID is larger than container's size!\n";
            return false;
        }
        else if (! mEntities[entityID].IsValid())
        {
            std::cerr << "Entity ID is invalid. It might be deleted!\n";
            return false;
        }
        return true;
    }

    void World::SetSkybox(std::array<std::string, 6> const& texturePaths)
    {
        mSkyboxSystem.SetSkybox(texturePaths);
    }

    void World::WorldStart()
    {
        SystemsStart();
    }

    void World::WorldEnd()
    {
        SystemsEnd();
    }

    void World::SystemsStart()
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

    void World::SystemsEnd()
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


    void World::Simulate(float deltaTime)
    {       
        mSkeletalMeshRenderSystem.Update(deltaTime);
    }

  
}

