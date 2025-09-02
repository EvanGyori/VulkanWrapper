#pragma once
#include "VulkanObjectWrapper.h"

#define VKW_RAII_WRAPPER(Alias, Type, CreateFunc, DestroyFunc) \
    constexpr const char Type ## _name[] = #Type ;\
    using Alias = VulkanObjectWrapper<Type, CreateFunc, DestroyFunc, Type ## _name>

#define VKW_RAII_WRAPPER_INSTANCE_EXTENSION(Alias, Type, CreateFunc, DestroyFunc) \
    constexpr const char Type ## _name[] = #Type ;\
    constexpr const char CreateFunc ## _name[] = #CreateFunc ;\
    constexpr const char DestroyFunc ## _name[] = #DestroyFunc ;\
    using Alias = VulkanObjectWrapperInstanceExtension<Type, CreateFunc, PFN_ ## CreateFunc, CreateFunc ## _name, DestroyFunc, PFN_ ## DestroyFunc, DestroyFunc ## _name, Type ## _name>

namespace vkw
{

VKW_RAII_WRAPPER(Instance, VkInstance, vkCreateInstance, vkDestroyInstance);

VKW_RAII_WRAPPER_INSTANCE_EXTENSION(DebugUtilsMessengerEXT, VkDebugUtilsMessengerEXT, vkCreateDebugUtilsMessengerEXT, vkDestroyDebugUtilsMessengerEXT);

}
