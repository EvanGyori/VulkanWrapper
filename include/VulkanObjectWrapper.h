#pragma once

#include <utility>
#include <tuple>
#include <type_traits>
#include <cstddef>

#include <vulkan/vulkan.h>

#include "DefaultVkwCheck.h"

namespace vkw
{

/*
template<size_t I, typename... Types>
using nthType = std::remove_reference_t<decltype(std::get<I>(std::declval<std::tuple<Types...>>()))>;

template<typename Func>
class FunctionTraits;

template<typename Ret, typename... Params>
class FunctionTraits<Ret (*)(Params...)>
{
public:
    using ReturnType = Ret;
    template<size_t I>
    using nthParam = nthType<I, Params...>;
    
    static constexpr size_t numParams = sizeof...(Params);
};

template<auto Func, size_t I>
using nthParam = typename FunctionTraits<decltype(Func)>::template nthParam<I>;

template<auto Func>
constexpr size_t numParams = FunctionTraits<decltype(Func)>::numParams;

// Version of is_base_of that ignores pointers
template<typename Base, typename Derived>
constexpr bool isSameOrBaseOf = std::is_same_v<Base, Derived>
    || (std::is_base_of_v<std::remove_pointer_t<Base>, std::remove_pointer_t<Derived>>
	&& !std::is_same_v<std::remove_pointer_t<Base>, std::remove_pointer_t<Derived>>);

template<size_t I, auto Func, typename... Types>
class DoesTypesMatchFuncParamsHelper
{
public:
    static constexpr bool value = isSameOrBaseOf<nthParam<Func, I>, nthType<I, Types...>>
	&& DoesTypesMatchFuncParamsHelper<I - 1, Func, Types...>::value;
};

template<auto Func, typename... Types>
class DoesTypesMatchFuncParamsHelper<0, Func, Types...>
{
public:
    static constexpr bool value = isSameOrBaseOf<nthParam<Func, 0>, nthType<0, Types...>>;
};

template<auto Func, typename... Types>
class DoesTypesMatchFuncParams
{
public:
    static constexpr bool value = sizeof...(Types) == numParams<Func>
	&& DoesTypesMatchFuncParamsHelper<sizeof...(Types) - 1, Func, Types...>::value;
};

template<auto Func, typename... Types>
constexpr bool doesTypesMatchFuncParams_v = DoesTypesMatchFuncParams<Func, Types...>::value;
*/

template<typename T, auto CreateFunc, auto DestroyFunc, const char* TypeName>
class VulkanObjectWrapper
{
public:
    VulkanObjectWrapper(nullptr_t) : handle(nullptr) {}

    template<typename... Types>
    VulkanObjectWrapper(Types... args) : handle(nullptr)
    {
	VKW_CHECK_CREATION(TypeName, CreateFunc(args..., nullptr, &handle));
    }

    ~VulkanObjectWrapper()
    {
	if (handle != nullptr)
	    DestroyFunc(handle, nullptr);
    }

    operator T()
    {
	return handle;
    }

private:
    T handle;
};

}
