#pragma once

#include "EngineGlobals/EngineConcepts.h"

#include "ECS/Components/Components.h"

#include "ECS/Entity/EntityID.h"

#include <typeindex>

namespace inceptionengine
{


	class ComponentPoolBase
	{
	public:
		virtual void DeleteComponent(EntityID entityID) = 0;
		virtual bool HasEntity(EntityID entityID) = 0;
		virtual ~ComponentPoolBase() = default;
	};


	template<CComponent Component>
	class ComponentPool final : public ComponentPoolBase
	{
	public:
		using ComponentIndex = typename std::vector<Component>::size_type;

		virtual ~ComponentPool() = default;

		template<typename ... Args>
		Component& EmplaceComponent(EntityID entityID, Args&& ... args)
		{
			assert(!HasEntity(entityID) && "This entity already has this component!");
			ComponentIndex componentIndex = mComponents.size();
			mComponents.emplace_back(std::forward<Args>(args)...);
			mEntityToMap.insert(std::pair<EntityID, ComponentIndex>(entityID, componentIndex));
			return mComponents.back();
		}

		Component& GetComponent(EntityID entityID)
		{
			assert(HasEntity(entityID) && "This entity does not has this component!");
			return mComponents[mEntityToMap[entityID]];
		}

		virtual void DeleteComponent(EntityID entityID) override
		{
			if (HasEntity(entityID))
			{
				//do something
			}
			else
			{
				return;
			}
		}

		virtual bool HasEntity(EntityID entityID) override
		{
			return mEntityToMap.find(entityID) != mEntityToMap.end();
		}

		std::vector<Component>& GetComponentView()
		{
			return mComponents;
		}

	private:
		std::unordered_map<EntityID, ComponentIndex> mEntityToMap;

		std::vector<Component> mComponents;
	};


	class ComponentsPool
	{
	public:

		template<CComponent Component>
		void RegisterComponent()
		{
			using PairType = std::pair<std::type_index, std::unique_ptr<ComponentPool<Component>>>;
			std::type_index typeIndex(typeid(Component));

			//This if statement cannot be changed to PoolContainsComponentType because
			//this is the first time the component get insert, so that function will 
			//always return false and hit a assertion fail
			if (mComponentPools.find(typeIndex) != mComponentPools.end())
			{
				std::cerr << "You cannot register " << typeIndex.name() << " twice!\n";
				throw std::runtime_error("");
			}
			auto poolPtr = std::make_unique<ComponentPool<Component>>();
			mComponentPools.insert(PairType(typeIndex, std::move(poolPtr)));
		}


		template<CTuple Tuple>
		void RegisterComponents()
		{
			RegisterComponentsImpl<Tuple, 0>();
		}

		template<CTuple Tuple, size_t I>
		void RegisterComponentsImpl()
		{
			if constexpr (I == std::tuple_size_v<Tuple>)
			{
				return;
			}
			else
			{
				RegisterComponent<std::tuple_element_t<I, Tuple>>();
				RegisterComponentsImpl<Tuple, I + 1>();
			}
		}

		template<CComponent Component>
		bool PoolContainsComponentType()
		{
			std::type_index typeIndex(typeid(Component));
			bool res = mComponentPools.find(typeIndex) != mComponentPools.end();
			if (res == false)
			{
				std::cerr << "You should register " << typeIndex.name() << " first!\n";
				throw std::runtime_error("");
			}
			return res;
		}

		template<CComponent Component, typename ... Args>
		Component& AddComponent(EntityID entityID, Args&& ... args)
		{
			using PairType = std::pair<std::type_index, std::unique_ptr<ComponentPool<Component>>>;

			std::type_index typeIndex(typeid(Component));
			assert(PoolContainsComponentType<Component>() && "You should register the component first!");
			return static_cast<ComponentPool<Component>*>(mComponentPools.at(typeIndex).get())->EmplaceComponent(entityID, std::forward<Args>(args)...);
		}

		template<CComponent Component>
		Component& GetComponent(EntityID entityID)
		{
			assert(PoolContainsComponentType<Component>() && "The Pool has no such component!");
			std::type_index typeIndex(typeid(Component));
			return static_cast<ComponentPool<Component>*>(mComponentPools.at(typeIndex).get())->GetComponent(entityID);
		}

		template<CComponent Component>
		ComponentPool<Component>* GetComponentPool()
		{
			std::type_index typeIndex(typeid(Component));
			assert(PoolContainsComponentType<Component>() && "You should register the component first!");
			return static_cast<ComponentPool<Component>*>(mComponentPools.at(typeIndex).get());
		}


		template<CComponent Component>
		bool EntityHasComponent(EntityID entityID)
		{
			std::type_index typeIndex(typeid(Component));
			assert(PoolContainsComponentType<Component>() && "You should register the component first!");
			return mComponentPools.at(typeIndex)->HasEntity(entityID);
		}

		template<CComponent Component>
		void DeleteComponent(EntityID entityID)
		{
			assert(PoolContainsComponentType<Component>() && "You should register the component first!");
			std::type_index typeIndex(typeid(Component));
			mComponentPools.at(typeIndex)->DeleteComponent(entityID);
		}

		void DeleteEntity(EntityID entityID)
		{
			for (auto& pair : mComponentPools)
			{
				pair.second->DeleteComponent(entityID);
			}
		}

	private:
		std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolBase>> mComponentPools;
	};
}
