#include <iostream>

#include "VulkanStructAliases.h"
#include "VulkanObjectAliases.h"

int main()
{
    vkw::ApplicationInfo appInfo({
	.pApplicationName = "testing VulkanWrapper",
	.applicationVersion = 0,
	.apiVersion = VK_API_VERSION_1_0
    });

    vkw::InstanceCreateInfo createInfo({
	.pApplicationInfo = &appInfo
    });

    try {
	vkw::Instance instance(&createInfo);
    } catch(std::runtime_error& err) {
	std::cout << err.what() << '\n';
    }

    std::cout << sizeof(vkw::Instance) << ' ' << sizeof(VkInstance) << '\n';
    std::cout << vkw::Instance::hasDestroyDependency << '\n';
}
