#pragma once

#include <numeric>

namespace inceptionengine
{
	using EntityID = unsigned long long;
	EntityID constexpr InvalidEntityID = std::numeric_limits<EntityID>().max();

	//struct EntityID
	//{
		//using Type = unsigned long long;
		//static constexpr Type InvalidID = std::numeric_limits<Type>().max();
		//EntityID(Type id) : ID(id) { ; }
		//operator Type() const { return ID; }
		//Type ID;
	//};
}