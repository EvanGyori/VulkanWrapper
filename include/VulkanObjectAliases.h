#pragma once
#include "VulkanObjectWrapper.h"

#define VKW_RAII_WRAPPER(Alias, Type, CreateFunc, DestroyFunc) constexpr const char Type ## _name[] = #Type ;\
    using Alias = VulkanObjectWrapper<Type, CreateFunc, DestroyFunc, Type ## _name>

namespace vkw
{

VKW_RAII_WRAPPER(Instance, VkInstance, vkCreateInstance, vkDestroyInstance);
    //VulkanObjectWrapper<VkInstance, vkCreateInstance, vkDestroyInstance>;

}
