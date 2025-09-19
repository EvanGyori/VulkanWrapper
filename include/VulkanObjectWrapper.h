/*
 * The template disaster for the RAII wrapper class for Vulkan objects
 */

#pragma once

#include <cstddef>
#include <algorithm>

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

    DestroyDependencyBase() noexcept : destroyDependency(nullptr) {}

    DestroyDependencyBase(DestroyDependencyBase&& rhs) noexcept : destroyDependency(rhs.destroyDependency)
    {
	rhs.destroyDependency = nullptr;
    }

    DestroyDependencyBase& operator=(DestroyDependencyBase&& rhs) noexcept
    {
	std::swap(destroyDependency, rhs.destroyDependency);
	return *this;
    }
};

template<typename T, auto DestroyFunc>
class VulkanObjectWrapperBase : public DestroyDependencyBase<DestroyFunc>
{
public:
    VulkanObjectWrapperBase(const VulkanObjectWrapperBase&) = delete;
    VulkanObjectWrapperBase& operator=(const VulkanObjectWrapperBase&) = delete;

    VulkanObjectWrapperBase(VulkanObjectWrapperBase&& rhs) noexcept :
	DestroyDependencyBase<DestroyFunc>(std::move(rhs)),
	handle(rhs.handle)
    {
	rhs.handle = nullptr;
    }

    VulkanObjectWrapperBase& operator=(VulkanObjectWrapperBase&& rhs) noexcept
    {
	DestroyDependencyBase<DestroyFunc>::operator=(std::move(rhs));
	std::swap(handle, rhs.handle);
	return *this;
    }

    operator T() const noexcept
    {
	return handle;
    }

protected:
    VulkanObjectWrapperBase() noexcept : handle(nullptr) {}

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
	    if constexpr(this->hasDestroyDependency) {
		DestroyFunc(this->destroyDependency, this->handle, nullptr);
	    } else {
		DestroyFunc(this->handle, nullptr);
	    }
	}

	InnerHelper(InnerHelper&&) = default;
	InnerHelper& operator=(InnerHelper&&) = default;
    };
};

// Used for all objects except for some special case scenarios covered below (doesn't fit the pattern)
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
	    if (this->destroyDependency != nullptr) {
		auto destroyFunc = reinterpret_cast<DestroyFuncType>(
		    vkGetInstanceProcAddr(this->destroyDependency, DestroyFuncName));
		destroyFunc(this->destroyDependency, this->handle, nullptr);
	    }
	}

	InnerHelper(InnerHelper&&) = default;
	InnerHelper& operator=(InnerHelper&&) = default;
    };
};

// Used when the create and/or destroy functions must be obtained by vkGetInstanceProcAddr
template<typename T, auto CreateFunc, typename CreateFuncType, const char* CreateFuncName, auto DestroyFunc, typename DestroyFuncType, const char* DestroyFuncName, const char* TypeName>
using VulkanObjectWrapperInstanceExtension = ApplyRemoveLastNFunctionParams<CreateFunc, 2, VulkanObjectWrapperInstanceExtensionHelper<T, CreateFunc, CreateFuncType, CreateFuncName, DestroyFunc, DestroyFuncType, DestroyFuncName, TypeName>::template InnerHelper>;

// Used when the create function is ambiguous, so instead the handle is passed in to the RAII object after the handle's creation
template<typename T, auto DestroyFunc>
class VulkanObjectWrapperNoCreateFunc : public VulkanObjectWrapperBase<T, DestroyFunc>
{
public:
    static_assert(VulkanObjectWrapperBase<T, DestroyFunc>::hasDestroyDependency, "VulkanObjectWrapperNoCreateFunc currently only supports objects with three destroy function parameters, aka it has a destroy dependency");

    VulkanObjectWrapperNoCreateFunc(nullptr_t) {}

    VulkanObjectWrapperNoCreateFunc(VulkanObjectWrapperBase<T, DestroyFunc>::DependencyType destroyDependency, T handle)
    {
	this->destroyDependency = destroyDependency;
	this->handle = handle;
    }

    ~VulkanObjectWrapperNoCreateFunc()
    {
	DestroyFunc(this->destroyDependency, this->handle, nullptr);
    }

    VulkanObjectWrapperNoCreateFunc(VulkanObjectWrapperNoCreateFunc&&) = default;
    VulkanObjectWrapperNoCreateFunc& operator=(VulkanObjectWrapperNoCreateFunc&&) = default;
};

// Specialize for VkCommandBuffer to free a single command buffer
class CommandBufferNoCreateFunc
{
public:
    CommandBufferNoCreateFunc(nullptr_t) :
	device(nullptr),
	commandPool(nullptr),
	handle(nullptr)
    {
    }

    CommandBufferNoCreateFunc(
	VkDevice device,
	VkCommandPool commandPool,
	VkCommandBuffer handle)
	: device(device),
	commandPool(commandPool),
	handle(handle)
    {
    }

    CommandBufferNoCreateFunc(const CommandBufferNoCreateFunc&) = delete;
    CommandBufferNoCreateFunc& operator=(const CommandBufferNoCreateFunc&) = delete;

    CommandBufferNoCreateFunc(CommandBufferNoCreateFunc&& rhs) noexcept :
	device(rhs.device),
	commandPool(rhs.commandPool),
	handle(rhs.handle)
    {
	rhs.device = nullptr;
	rhs.commandPool = nullptr;
	rhs.handle = nullptr;
    }

    CommandBufferNoCreateFunc& operator=(CommandBufferNoCreateFunc&& rhs) noexcept
    {
	std::swap(device, rhs.device);
	std::swap(commandPool, rhs.commandPool);
	std::swap(handle, rhs.handle);
	return *this;
    }

    ~CommandBufferNoCreateFunc()
    {
	vkFreeCommandBuffers(device, commandPool, 1, &handle);
    }

    operator VkCommandBuffer() const noexcept
    {
	return handle;
    }

private:
    VkDevice device;
    VkCommandPool commandPool;
    VkCommandBuffer handle;
};

}
