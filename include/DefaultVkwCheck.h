/*
 * Provides VKW_CHECK macro function to be wrapped around Vulkan function calls as a shorthand
 * to throw an error if a bad VkResult was returned
 *
 * Provides VKW_CHECK_CREATION macro function which is used inside the RAII wrapper objects
 * to print what object type failed to create if an error occurred
 *
 * Both VKW_CHECK and VKW_CHECK_CREATION can be defined separately beforehand for
 * custom error handling
 */

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

// Use as a shorthand to check for any errors from a vulkan function
// and to print the file and line the error occured
#define VKW_CHECK(result) vkw::checkResult((result), __FILE__, __LINE__)
#endif

#ifndef VKW_CHECK_CREATION
inline void checkCreationResult(const char* type, VkResult result)
{
    if (result < VK_SUCCESS) {
	throw std::runtime_error(std::string("VKW ERROR: failed to create ") + type + ". Result code: " + std::to_string(result));
    }
}

// Used in the RAII wrapper objects to print what type of object failed
// to create since printing the file and line would be useless
// (it would be inside a class template)
#define VKW_CHECK_CREATION(type, result) vkw::checkCreationResult((type), (result))
#endif

}
