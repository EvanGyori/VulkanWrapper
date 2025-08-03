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
    VulkanStructWrapper(T&& rhs = {}) : T{std::forward<T>(rhs)} { rhs.sType = sType; }
};

}
