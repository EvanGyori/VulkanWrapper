#pragma once

#include <stdexcept>
#include <string>

#include <vulkan/vulkan.h>

namespace vkw
{

#ifndef VKW_CHECK
inline void checkResult(VkResult result, const char* file, const char* line)
{
    if (result < VK_SUCCESS) {
	throw std::runtime_error(std::string("VKW ERROR: in ") + file + " at line " + line + ". Result code: " + std::to_string(result));
    }
}

#define VKW_CHECK(result) vkw::checkResult((result), __FILE__, __LINE__)
#endif

#ifndef VKW_CHECK_CREATION
inline void checkCreationResult(const char* type, VkResult result)
{
    if (result < VK_SUCCESS) {
	throw std::runtime_error(std::string("VKW ERROR: failed to create ") + type + ". Result code: " + std::to_string(result));
    }
}

#define VKW_CHECK_CREATION(type, result) vkw::checkCreationResult((type), (result))
#endif

}
