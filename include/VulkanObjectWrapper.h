#pragma once

#include <utility>
#include <tuple>
#include <type_traits>
#include <cstddef>

#include <vulkan/vulkan.h>

#include "DefaultVkwCheck.h"
#include "VkwHelpers.h"

namespace vkw
{

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
using VulkanObjectWrapper = ApplyRemoveLastNFunctionParams<CreateFunc, 2, VulkanObjectWrapperHelper<T, CreateFunc, DestroyFunc, TypeName>::template InnerHelper>;

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
using VulkanObjectWrapperInstanceExtension = ApplyRemoveLastNFunctionParams<CreateFunc, 2, VulkanObjectWrapperInstanceExtensionHelper<T, CreateFunc, CreateFuncType, CreateFuncName, DestroyFunc, DestroyFuncType, DestroyFuncName, TypeName>::template InnerHelper>;

}
