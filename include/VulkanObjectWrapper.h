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

template<auto Func, typename FuncType = decltype(Func)>
class FunctionTraits;

template<auto Func, typename Ret, typename... ParamTypes>
class FunctionTraits<Func, Ret (*)(ParamTypes...)>
{
public:
    using ReturnType = Ret;
    template<size_t N>
    using NthParam = NthType<N, ParamTypes...>;
    static constexpr size_t paramCount = sizeof...(ParamTypes);

    class ParamPack
    {
    public:
	template<template<typename...> class T>
	using apply = T<ParamTypes...>;
    };
};

template<auto Func>
using ReturnType = typename FunctionTraits<Func>::ReturnType;

template<auto Func, size_t N>
using NthParam = typename FunctionTraits<Func>::template NthParam<N>;

template<auto Func>
constexpr size_t paramCount = FunctionTraits<Func>::paramCount;

template<size_t N, typename T1, typename... T2>
inline auto getNthArg(T1 arg, T2... args)
{
    if constexpr(N == 0) {
	return arg;
    } else {
	return getNthArg<N - 1, T2...>(args...);
    }
}

template<typename... Types>
struct Pack
{
    template<template<typename...> class T>
    using apply = T<Types...>;

    template<template<typename...> class T>
    using addInt = T<Types..., int>;
};

template<size_t N>
struct RemoveFirstN
{
    template<typename... Types>
    struct Pack;

    template<typename First, typename... Types>
    struct Pack<First, Types...>
    {
	template<template<typename...> class T>
	using apply = typename RemoveFirstN<N - 1>::template Pack<Types...>::template apply<T>;
    };
};

template<>
struct RemoveFirstN<0>
{
    template<typename... Types>
    struct Pack
    {
	template<template<typename...> class T>
	using apply = T<Types...>;
    };
};

struct ReverseTypes
{
    template<typename... Types>
    struct Pack
    {
    private:
	template<typename...>
	struct Helper;

	template<typename First, typename... Forward>
	struct Helper<First, Forward...>
	{
	    template<typename... Backward>
	    struct InnerHelper
	    {
		template<template<typename...> class T>
		using apply = typename Helper<Forward...>::template InnerHelper<First, Backward...>::template apply<T>;
	    };
	};

	template<>
	struct Helper<>
	{
	    template<typename... Backward>
	    struct InnerHelper
	    {
		template<template<typename...> class T>
		using apply = T<Backward...>;
	    };
	};

    public:
	template<template<typename...> class T>
	using apply = typename Helper<Types...>::template InnerHelper<>::template apply<T>;
    };
};


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

// When the number of parameters for a vkDestroy function is greater than 2,
// it requires an object used during creation (maybe not for some extension objects)
template<auto DestroyFunc, bool HasDestroyDependency = 2 < paramCount<DestroyFunc>>
class DestroyDependencyBase
{
protected:
    static constexpr bool hasDestroyDependency = HasDestroyDependency;
};

template<auto DestroyFunc>
class DestroyDependencyBase<DestroyFunc, true>
{
protected:
    static constexpr bool hasDestroyDependency = true;
    NthParam<DestroyFunc, 0> destroyDependency;
};

template<typename T, auto DestroyFunc>
class VulkanObjectWrapperBase : public DestroyDependencyBase<DestroyFunc>
{
public:
    operator T()
    {
	return handle;
    }

protected:
    VulkanObjectWrapperBase() : handle(nullptr) {}

    T handle;
};

template<typename T, auto CreateFunc, auto DestroyFunc, const char* TypeName>
class VulkanObjectWrapperHelper
{
public:
    // In order to get the create parameters from CreateFunc with the last two removed,
    // some template tricks need to be done.
    template<typename... CreateParams>
    class InnerHelper : public VulkanObjectWrapperBase<T, DestroyFunc>
    {
    public:
	InnerHelper(nullptr_t) {}

	InnerHelper(CreateParams... args)
	{
	    if constexpr(this->hasDestroyDependency) {
		this->destroyDependency = getNthArg<0>(args...);
	    }

	    VKW_CHECK_CREATION(TypeName, CreateFunc(args..., nullptr, &this->handle));
	}

	~InnerHelper()
	{
	    if (this->handle != nullptr) {
		if constexpr(this->hasDestroyDependency) {
		    DestroyFunc(this->destroyDependency, this->handle, nullptr);
		} else {
		    DestroyFunc(this->handle, nullptr);
		}
	    }
	}
    };
};

// Holy moly
template<typename T, auto CreateFunc, auto DestroyFunc, const char* TypeName>
using VulkanObjectWrapper = FunctionTraits<CreateFunc>::ParamPack::template apply<ReverseTypes::Pack>::template apply<RemoveFirstN<2>::Pack>::template apply<ReverseTypes::Pack>::template apply<VulkanObjectWrapperHelper<T, CreateFunc, DestroyFunc, TypeName>::template InnerHelper>;

template<typename T, auto CreateFunc, typename CreateFuncType, const char* CreateFuncName, auto DestroyFunc, typename DestroyFuncType, const char* DestroyFuncName, const char* TypeName>
class VulkanObjectWrapperInstanceExtensionHelper
{
public:
    template<typename... CreateParams>
    class InnerHelper : public VulkanObjectWrapperBase<T, DestroyFunc>
    {
    public:
	static_assert(VulkanObjectWrapperBase<T, DestroyFunc>::hasDestroyDependency, "Instance is not given during creation but is needed to get create and destroy function pointers");

	InnerHelper(nullptr_t) {}
	
	InnerHelper(CreateParams... args)
	{
	    this->destroyDependency = getNthArg<0>(args...);

	    auto createFunc = reinterpret_cast<CreateFuncType>(
		vkGetInstanceProcAddr(this->destroyDependency, CreateFuncName));
	    VKW_CHECK_CREATION(TypeName, createFunc(args..., nullptr, &this->handle));
	}

	~InnerHelper()
	{
	    if (this->handle != nullptr) {
		auto destroyFunc = reinterpret_cast<DestroyFuncType>(
		    vkGetInstanceProcAddr(this->destroyDependency, DestroyFuncName));
		destroyFunc(this->destroyDependency, this->handle, nullptr);
	    }
	}
    };
};

template<typename T, auto CreateFunc, typename CreateFuncType, const char* CreateFuncName, auto DestroyFunc, typename DestroyFuncType, const char* DestroyFuncName, const char* TypeName>
using VulkanObjectWrapperInstanceExtension = FunctionTraits<CreateFunc>::ParamPack::template apply<ReverseTypes::Pack>::template apply<RemoveFirstN<2>::Pack>::template apply<ReverseTypes::Pack>::template apply<VulkanObjectWrapperInstanceExtensionHelper<T, CreateFunc, CreateFuncType, CreateFuncName, DestroyFunc, DestroyFuncType, DestroyFuncName, TypeName>::template InnerHelper>;

}
