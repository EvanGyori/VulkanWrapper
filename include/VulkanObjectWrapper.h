#pragma once

#include <utility>
#include <tuple>
#include <type_traits>
#include <cstddef>

#include <vulkan/vulkan.h>

#include "DefaultVkwCheck.h"

namespace vkw
{

template<size_t N, typename... T>
using NthType = std::remove_reference_t<decltype(std::get<N>(std::declval<std::tuple<T...>>()))>;

template<typename Func>
class FunctionTraits;

template<typename Ret, typename... Params>
class FunctionTraits<Ret (*)(Params...)>
{
public:
    using ReturnType = Ret;
    template<size_t N>
    using NthParam = NthType<N, Params...>;
    static constexpr size_t paramCount = sizeof...(Params);
};

template<auto Func>
using ReturnType = typename FunctionTraits<decltype(Func)>::ReturnType;

template<auto Func, size_t N>
using NthParam = typename FunctionTraits<decltype(Func)>::template NthParam<N>;

template<auto Func>
constexpr size_t paramCount = FunctionTraits<decltype(Func)>::paramCount;

template<size_t N, typename T1, typename... T2>
inline auto getNthArg(T1 arg, T2... args)
{
    if constexpr(N == 0) {
	return arg;
    } else {
	return getNthArg<N - 1, T2...>(args...);
    }
}


/*


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

template<auto DestroyFunc, bool HasDestroyDependency = 2 < paramCount<DestroyFunc>>
class DestroyDependencyBase {};

template<auto DestroyFunc>
class DestroyDependencyBase<DestroyFunc, true>
{
protected:
    NthParam<DestroyFunc, 0> destroyDependency;
};

template<typename T, auto CreateFunc, auto DestroyFunc, const char* TypeName>
class VulkanObjectWrapper : public DestroyDependencyBase<DestroyFunc>
{
public:
    // When the number of parameters for a vkDestroy function is greater than 2,
    // it requires an object used during creation (maybe not for some extension objects)
    static constexpr bool hasDestroyDependency = paramCount<DestroyFunc> > 2;

    T handle;

public:
    VulkanObjectWrapper(nullptr_t) : handle(nullptr) {}

    template<typename... Types>
    VulkanObjectWrapper(Types... args) : handle(nullptr)
    {
	if constexpr(hasDestroyDependency) {
	    this->destroyDependency = getNthArg<0>(args...);
	}

	VKW_CHECK_CREATION(TypeName, CreateFunc(args..., nullptr, &handle));
    }

    ~VulkanObjectWrapper()
    {
	if (handle != nullptr) {
	    if constexpr(hasDestroyDependency) {
		DestroyFunc(this->destroyDependency, handle, nullptr);
	    } else {
		DestroyFunc(handle, nullptr);
	    }
	}
    }

    operator T()
    {
	return handle;
    }
};

}
