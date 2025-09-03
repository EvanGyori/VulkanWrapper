#pragma once

#include "VulkanGetArrayWrapper.h"

namespace vkw
{

// Instance and Device Enumeration
const auto& enumerateInstanceExtensionProperties = VulkanGetArrayWrapper<vkEnumerateInstanceExtensionProperties>;
const auto& enumerateInstanceLayerProperties = VulkanGetArrayWrapper<vkEnumerateInstanceLayerProperties>;
const auto& enumeratePhysicalDevices = VulkanGetArrayWrapper<vkEnumeratePhysicalDevices>;
const auto& enumerateDeviceExtensionProperties = VulkanGetArrayWrapper<vkEnumerateDeviceExtensionProperties>;
const auto& enumerateDeviceLayerProperties = VulkanGetArrayWrapper<vkEnumerateDeviceLayerProperties>;

// Queue Family & Surface Queries
const auto& getPhysicalDeviceQueueFamilyProperties = VulkanGetArrayWrapper<vkGetPhysicalDeviceQueueFamilyProperties>;
const auto& getPhysicalDeviceSparseImageFormatProperties = VulkanGetArrayWrapper<vkGetPhysicalDeviceSparseImageFormatProperties>;
const auto& getPhysicalDeviceSurfacePresentModesKHR = VulkanGetArrayWrapper<vkGetPhysicalDeviceSurfacePresentModesKHR>;
const auto& getPhysicalDeviceSurfaceFormatsKHR = VulkanGetArrayWrapper<vkGetPhysicalDeviceSurfaceFormatsKHR>;
const auto& getPhysicalDeviceDisplayPropertiesKHR = VulkanGetArrayWrapper<vkGetPhysicalDeviceDisplayPropertiesKHR>;
const auto& getPhysicalDeviceDisplayPlanePropertiesKHR = VulkanGetArrayWrapper<vkGetPhysicalDeviceDisplayPlanePropertiesKHR>;
const auto& getDisplayModePropertiesKHR = VulkanGetArrayWrapper<vkGetDisplayModePropertiesKHR>;
const auto& getDisplayPlaneSupportedDisplaysKHR = VulkanGetArrayWrapper<vkGetDisplayPlaneSupportedDisplaysKHR>;

// Swapchain & Device Groups
const auto& getSwapchainImagesKHR = VulkanGetArrayWrapper<vkGetSwapchainImagesKHR>;
const auto& getPhysicalDevicePresentRectanglesKHR = VulkanGetArrayWrapper<vkGetPhysicalDevicePresentRectanglesKHR>;

// Extensions and Tools
const auto& getPhysicalDeviceToolProperties = VulkanGetArrayWrapper<vkGetPhysicalDeviceToolProperties>;
//const auto& getPhysicalDeviceCalibrateableTimeDomainsEXT = VulkanGetArrayWrapper<vkGetPhysicalDeviceCalibrateableTimeDomainsEXT>;
//const auto& getPhysicalDeviceCooperativeMatrixPropertiesKHR = VulkanGetArrayWrapper<vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR>;
//const auto& getPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = VulkanGetArrayWrapper<vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV>;

}
