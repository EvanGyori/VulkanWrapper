#pragma once

#include "VulkanGetArrayWrapper.h"

namespace vkw
{

namespace
{
// Instance and Device Enumeration
const auto& enumerateInstanceExtensionProperties = VulkanGetArrayWrapper<vkEnumerateInstanceExtensionProperties>::get;
const auto& enumerateInstanceLayerProperties = VulkanGetArrayWrapper<vkEnumerateInstanceLayerProperties>::get;
const auto& enumeratePhysicalDevices = VulkanGetArrayWrapper<vkEnumeratePhysicalDevices>::get;
const auto& enumerateDeviceExtensionProperties = VulkanGetArrayWrapper<vkEnumerateDeviceExtensionProperties>::get;
const auto& enumerateDeviceLayerProperties = VulkanGetArrayWrapper<vkEnumerateDeviceLayerProperties>::get;

// Queue Family & Surface Queries
const auto& getPhysicalDeviceQueueFamilyProperties = VulkanGetArrayWrapper<vkGetPhysicalDeviceQueueFamilyProperties>::get;
const auto& getPhysicalDeviceSparseImageFormatProperties = VulkanGetArrayWrapper<vkGetPhysicalDeviceSparseImageFormatProperties>::get;
const auto& getPhysicalDeviceSurfacePresentModesKHR = VulkanGetArrayWrapper<vkGetPhysicalDeviceSurfacePresentModesKHR>::get;
const auto& getPhysicalDeviceSurfaceFormatsKHR = VulkanGetArrayWrapper<vkGetPhysicalDeviceSurfaceFormatsKHR>::get;
const auto& getPhysicalDeviceDisplayPropertiesKHR = VulkanGetArrayWrapper<vkGetPhysicalDeviceDisplayPropertiesKHR>::get;
const auto& getPhysicalDeviceDisplayPlanePropertiesKHR = VulkanGetArrayWrapper<vkGetPhysicalDeviceDisplayPlanePropertiesKHR>::get;
const auto& getDisplayModePropertiesKHR = VulkanGetArrayWrapper<vkGetDisplayModePropertiesKHR>::get;
const auto& getDisplayPlaneSupportedDisplaysKHR = VulkanGetArrayWrapper<vkGetDisplayPlaneSupportedDisplaysKHR>::get;

// Swapchain & Device Groups
const auto& getSwapchainImagesKHR = VulkanGetArrayWrapper<vkGetSwapchainImagesKHR>::get;
const auto& getPhysicalDevicePresentRectanglesKHR = VulkanGetArrayWrapper<vkGetPhysicalDevicePresentRectanglesKHR>::get;

// Extensions and Tools
const auto& getPhysicalDeviceToolProperties = VulkanGetArrayWrapper<vkGetPhysicalDeviceToolProperties>::get;
//const auto& getPhysicalDeviceCalibrateableTimeDomainsEXT = VulkanGetArrayWrapper<vkGetPhysicalDeviceCalibrateableTimeDomainsEXT>;
//const auto& getPhysicalDeviceCooperativeMatrixPropertiesKHR = VulkanGetArrayWrapper<vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR>;
//const auto& getPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = VulkanGetArrayWrapper<vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV>;
}

}
