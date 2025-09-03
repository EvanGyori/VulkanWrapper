/*
 * Very simple wrapper for Vulkan structures that auto sets sType and can be used
 * as normal after construction
 *
 * Once C++ adds compile-time reflection, (...) without designated initializers
 * could be used instead of ({...}) with designated initializers
 */

#pragma once

#include <utility>

#include <vulkan/vulkan.h>

namespace vkw
{

template<typename T, VkStructureType sType>
struct VulkanStructWrapper : T
{
    // Causes narrowing conversion warning in many use cases
    //template<typename... MemberTypes> VulkanStructWrapper(MemberTypes... members) : T{} { T x = {sType, nullptr, members...}; T::operator=(x); }

    // Causes narrowing conversion error
    //template<typename... MemberTypes> VulkanStructWrapper(MemberTypes... members) : T{sType, nullptr, members...} {}

    // Allows designated initializers by doing ({ ... }). Can't do {...} though.
    // Not using designated initializers inside ({ ... }) would require setting sType
    VulkanStructWrapper(T&& rhs = {}) : T{std::forward<T>(rhs)} { this->sType = sType; } // forward may be unnecessary
};

}
