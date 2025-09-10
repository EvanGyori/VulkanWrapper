#pragma once

#include <vector>
#include <type_traits>

#include <vulkan/vulkan.h>

#include "DefaultVkwCheck.h"

namespace vkw
{

template<auto GetFunc>
class VulkanGetArrayWrapperHelper
{
public:
    template<typename... Params>
    class InnerHelper
    {
    private:
	using ElementType = std::remove_pointer_t<NthParam<GetFunc, paramCount<GetFunc> - 1>>;

    public:
	static std::vector<ElementType> get(Params... args)
	{
	    // Some get/enumerate functions don't return VkResult and thus wont and can't be wrapped in VKW_CHECK
	    static constexpr bool returnsErrorCode = std::is_same_v<ReturnType<GetFunc>, VkResult>;

	    uint32_t count;
	    if constexpr(returnsErrorCode) {
		VKW_CHECK(GetFunc(args..., &count, nullptr));
	    } else {
		GetFunc(args..., &count, nullptr);
	    }

	    std::vector<ElementType> elements(count);

	    if constexpr(returnsErrorCode) {
		VKW_CHECK(GetFunc(args..., &count, elements.data()));
	    } else {
		GetFunc(args..., &count, elements.data());
	    }

	    return elements;
	}
    };
};

template<auto GetFunc>
using VulkanGetArrayWrapper = ApplyRemoveLastNFunctionParams<GetFunc, 2, VulkanGetArrayWrapperHelper<GetFunc>::template InnerHelper>;

}
