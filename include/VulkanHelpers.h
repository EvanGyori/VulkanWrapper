#pragma once

#include <vulkan/vulkan.h>

inline bool operator==(const VkOffset2D& lhs, const VkOffset2D& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator==(const VkOffset3D& lhs, const VkOffset3D& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

inline bool operator==(const VkExtent2D& lhs, const VkExtent2D& rhs)
{
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

inline bool operator==(const VkExtent3D& lhs, const VkExtent3D& rhs)
{
    return lhs.width == rhs.width && lhs.height == rhs.height && lhs.depth == rhs.depth;
}

inline bool operator==(const VkRect2D& lhs, const VkRect2D& rhs)
{
    return lhs.offset == rhs.offset && lhs.extent == rhs.extent;
}
