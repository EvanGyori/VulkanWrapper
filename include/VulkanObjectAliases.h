/*
 * List of Vulkan object RAII wrappers currently available.
 *
 * Provides only move semantics, no copying. Pass nullptr into the constructor to default
 * with the handle being nullptr. Otherwise pass into the constructor the same parameters
 * used in its appropiate vkCreate function (minus the last two parameters).
 *
 * To add another one, use the VKW_RAII_WRAPPER macro. If
 * the create and/or destroy functions require using
 * vkGetInstanceProcAddr (with VkDebugUtilsMessengerEXT for example),
 * use the VKW_RAII_WRAPPER_INSTANCE_EXTENSION macro
 *
 * include GLFW before including VulkanWrapper.h to get the surface
 * RAII wrapper
 */

#pragma once
#include "VulkanObjectWrapper.h"

#define VKW_RAII_WRAPPER(Alias, Type, CreateFunc, DestroyFunc) \
    constexpr const char Type ## _name[] = #Type ;\
    using Alias = VulkanObjectWrapper<Type, CreateFunc, DestroyFunc, Type ## _name>

// Use when the object's create and destroy functions need to be obtained
// through vkGetInstanceProcAddr
#define VKW_RAII_WRAPPER_INSTANCE_EXTENSION(Alias, Type, CreateFunc, DestroyFunc) \
    constexpr const char Type ## _name[] = #Type ;\
    constexpr const char CreateFunc ## _name[] = #CreateFunc ;\
    constexpr const char DestroyFunc ## _name[] = #DestroyFunc ;\
    using Alias = VulkanObjectWrapperInstanceExtension<Type, CreateFunc, PFN_ ## CreateFunc, CreateFunc ## _name, DestroyFunc, PFN_ ## DestroyFunc, DestroyFunc ## _name, Type ## _name>

namespace vkw
{

VKW_RAII_WRAPPER(Instance, VkInstance, vkCreateInstance, vkDestroyInstance);
// VkPhysicalDevice - obtained differently, no destruction needed
VKW_RAII_WRAPPER(Device, VkDevice, vkCreateDevice, vkDestroyDevice);
// VkQueue - obtained differently, no destruction needed
VKW_RAII_WRAPPER(Semaphore, VkSemaphore, vkCreateSemaphore, vkDestroySemaphore);
// Command Buffer - allocated in groups, freed individually or by pool. Pass a created one to the RAII wrapper to have it freed individually automatically
using CommandBuffer = CommandBufferNoCreateFunc;
VKW_RAII_WRAPPER(Fence, VkFence, vkCreateFence, vkDestroyFence);
VKW_RAII_WRAPPER(DeviceMemory, VkDeviceMemory, vkAllocateMemory, vkFreeMemory);
VKW_RAII_WRAPPER(Buffer, VkBuffer, vkCreateBuffer, vkDestroyBuffer);
VKW_RAII_WRAPPER(Image, VkImage, vkCreateImage, vkDestroyImage);
VKW_RAII_WRAPPER(Event, VkEvent, vkCreateEvent, vkDestroyEvent);
VKW_RAII_WRAPPER(QueryPool, VkQueryPool, vkCreateQueryPool, vkDestroyQueryPool);
VKW_RAII_WRAPPER(BufferView, VkBufferView, vkCreateBufferView, vkDestroyBufferView);
VKW_RAII_WRAPPER(ImageView, VkImageView, vkCreateImageView, vkDestroyImageView);
VKW_RAII_WRAPPER(ShaderModule, VkShaderModule, vkCreateShaderModule, vkDestroyShaderModule);
VKW_RAII_WRAPPER(PipelineCache, VkPipelineCache, vkCreatePipelineCache, vkDestroyPipelineCache);
VKW_RAII_WRAPPER(PipelineLayout, VkPipelineLayout, vkCreatePipelineLayout, vkDestroyPipelineLayout);
VKW_RAII_WRAPPER(RenderPass, VkRenderPass, vkCreateRenderPass, vkDestroyRenderPass);
// Since creation is ambiguous, create beforehand and pass device and the created pipeline into constructor
using Pipeline = VulkanObjectWrapperNoCreateFunc<VkPipeline, vkDestroyPipeline>;
VKW_RAII_WRAPPER(DescriptorSetLayout, VkDescriptorSetLayout, vkCreateDescriptorSetLayout, vkDestroyDescriptorSetLayout);
VKW_RAII_WRAPPER(Sampler, VkSampler, vkCreateSampler, vkDestroySampler);
VKW_RAII_WRAPPER(DescriptorPool, VkDescriptorPool, vkCreateDescriptorPool, vkDestroyDescriptorPool);
// VkDescriptorSet - allocated in groups, freed individually or by pool
VKW_RAII_WRAPPER(Framebuffer, VkFramebuffer, vkCreateFramebuffer, vkDestroyFramebuffer);
VKW_RAII_WRAPPER(CommandPool, VkCommandPool, vkCreateCommandPool, vkDestroyCommandPool);

VKW_RAII_WRAPPER_INSTANCE_EXTENSION(DebugUtilsMessengerEXT, VkDebugUtilsMessengerEXT, vkCreateDebugUtilsMessengerEXT, vkDestroyDebugUtilsMessengerEXT);

#ifdef GLFW_TRUE
VKW_RAII_WRAPPER(SurfaceKHR, VkSurfaceKHR, glfwCreateWindowSurface, vkDestroySurfaceKHR);
#else
using SurfaceKHR = VulkanObjectWrapperNoCreateFunc<VkSurfaceKHR, vkDestroySurfaceKHR>;
#endif

VKW_RAII_WRAPPER(SwapchainKHR, VkSwapchainKHR, vkCreateSwapchainKHR, vkDestroySwapchainKHR);

}
