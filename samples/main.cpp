#include <iostream>
#include <vector>
#include <type_traits>

#include "VulkanWrapper.h"

static const char* getMessageSeverityString(VkDebugUtilsMessageSeverityFlagBitsEXT severityFlag)
{
    switch (severityFlag) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
	    return "verbose";
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
	    return "info";
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
	    return "WARNING";
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
	    return "ERROR";
	default:
	    return "unknownSeverity";
    }
}

static std::string getMessageTypesString(VkDebugUtilsMessageTypeFlagsEXT typeFlags)
{
    std::string types = "";

    if (typeFlags & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
	types = "general";
    }

    if (typeFlags & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
	if (types != "") {
	    types += ", ";
	}
	types += "validation";
    }

    if (typeFlags & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
	if (types != "") {
	    types += ", ";
	}
	types += "performance";
    }

    return (types == "") ? "unknownType" : types;
}

static VkBool32 VKAPI_PTR debugMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT types,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void* userData)
{
    std::cout << "VULKAN: " << getMessageSeverityString(severity)
	<< " TYPES: " << getMessageTypesString(types)
	<< " MESSAGE: " << callbackData->pMessage << std::endl;

    return VK_FALSE;
}

void run()
{
    vkw::DebugUtilsMessengerCreateInfoEXT debuggerInfo({
	.messageSeverity =
	    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
	.messageType =
	    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
	.pfnUserCallback = debugMessengerCallback
    });

    vkw::ApplicationInfo appInfo({
	.pApplicationName = "testing VulkanWrapper",
	.applicationVersion = 0,
	.apiVersion = VK_API_VERSION_1_0
    });

    std::vector<const char*> extensions { "VK_EXT_debug_utils" };

    vkw::InstanceCreateInfo createInfo({
	//.pNext = &debuggerInfo,
	.pApplicationInfo = &appInfo,
	.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
	.ppEnabledExtensionNames = extensions.data()
    });

    //vkw::Instance instance(&createInfo);

    //vkw::DebugUtilsMessengerEXT debugger(instance, &debuggerInfo);

    std::vector<vkw::Instance> instances;
    for (int i = 0; i < 3; ++i)
	instances.emplace_back(&createInfo);

    std::vector<vkw::Instance> instances2 = std::move(instances);
}

int main()
{
    try {
	run();
    } catch(std::runtime_error& err) {
	std::cout << err.what() << '\n';
    }
}
