#pragma once

#include <concepts>
#include <tuple>
#include <type_traits>

namespace inceptionengine
{

	class TransformComponent;
	class NativeScriptComponent;
	class SkeletalMeshComponent;
	class CameraComponent;

	using ComponentTypeList = std::tuple<
		TransformComponent,
		NativeScriptComponent,
		SkeletalMeshComponent,
		CameraComponent>;

	template<typename T, size_t I>
	struct CComponentChecker;

	template<typename T>
	struct CComponentChecker<T, 0>
	{
		static constexpr bool Value = std::is_same_v<T, std::tuple_element_t<0, ComponentTypeList>>;
	};

	template<typename T, size_t I>
	struct CComponentChecker
	{
		static constexpr bool Value = std::is_same_v<T, std::tuple_element_t<I, ComponentTypeList>> || CComponentChecker<T, I - 1>::Value;
	};

	template<typename>
	struct is_std_tuple
	{
		static constexpr bool Value = false;
	};

	template<typename ... Args>
	struct is_std_tuple<std::tuple<Args...>>
	{
		static constexpr bool Value = true;
	};

	template<typename T>
	concept CComponent = CComponentChecker<T, std::tuple_size_v<ComponentTypeList> - 1>::Value;

	template<typename T>
	concept CTuple = is_std_tuple<T>::Value;

}
