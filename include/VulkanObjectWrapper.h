/*
 * The template disaster for the RAII wrapper class for Vulkan objects
 */

#pragma once

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
    using DependencyType = NthParam<DestroyFunc, 0>;

    static constexpr bool hasDestroyDependency = true;
    DependencyType destroyDependency;

    DestroyDependencyBase() : destroyDependency(nullptr) {}

    DestroyDependencyBase(DestroyDependencyBase&& rhs) : destroyDependency(rhs.destroyDependency)
    {
	rhs.destroyDependency = nullptr;
    }

    DestroyDependencyBase& operator=(DestroyDependencyBase&& rhs)
    {
	DependencyType temp = destroyDependency;
	destroyDependency = rhs.destroyDependency;
	rhs.destroyDependency = temp;

	return *this;
    }
};

template<typename T, auto DestroyFunc>
class VulkanObjectWrapperBase : public DestroyDependencyBase<DestroyFunc>
{
public:
    VulkanObjectWrapperBase(const VulkanObjectWrapperBase&) = delete;
    VulkanObjectWrapperBase& operator=(const VulkanObjectWrapperBase&) = delete;

    VulkanObjectWrapperBase(VulkanObjectWrapperBase&& rhs) :
	DestroyDependencyBase<DestroyFunc>(std::move(rhs)),
	handle(rhs.handle)
    {
	rhs.handle = nullptr;
    }

    VulkanObjectWrapperBase& operator=(VulkanObjectWrapperBase&& rhs)
    {
	DestroyDependencyBase<DestroyFunc>::operator=(std::move(rhs));

	T temp = handle;
	handle = rhs.handle;
	rhs.handle = temp;

	return *this;
    }

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
    // the parameter pack needs to be isolated from the other template parameters
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

	InnerHelper(InnerHelper&&) = default;
	InnerHelper& operator=(InnerHelper&&) = default;
    };
};

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

	InnerHelper(InnerHelper&&) = default;
	InnerHelper& operator=(InnerHelper&&) = default;
    };
};

template<typename T, auto CreateFunc, typename CreateFuncType, const char* CreateFuncName, auto DestroyFunc, typename DestroyFuncType, const char* DestroyFuncName, const char* TypeName>
using VulkanObjectWrapperInstanceExtension = ApplyRemoveLastNFunctionParams<CreateFunc, 2, VulkanObjectWrapperInstanceExtensionHelper<T, CreateFunc, CreateFuncType, CreateFuncName, DestroyFunc, DestroyFuncType, DestroyFuncName, TypeName>::template InnerHelper>;

}
