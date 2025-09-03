# VulkanWrapper

A faster and more bare bones alternative to vulkan.hpp!

Provides RAII wrappers for Vulkan objects, struct aliases that auto-set sType, and error handling macros.

vulkan.hpp does all this and more but it has two flaws: VERY long compilation times and I found myself scrolling through their huge header files to see how to do something in certain scenarios. These problems can be solved easily using pre-compiled headers and auto-complete but screw it, I want simplicity, here I am (please don't look at the template code).

In order to increase compile-time speed, I used templates so that essentially code is only generated for whatever objects are used in a file. This brings the number of lines of code to roughly 1000. Opposed to vulkan.hpp's 300,000 lines of code for a single include!

## Cons

- Requires C++20 but I should be able to add support as low as C++11 if I feel like it.
- I only added struct and RAII object aliases for the most commonly used stuff. To add support for certain extensions, look at `include/VulkanStructAliases.h` or `include/VulkanObjectAliases.h`.
- You either must initialize the Vulkan struct aliases using designated initializers or set the members after using the default constructor. See [Usage](#usage) for more info. This will change once I get my hands on C++26's compile-time reflection.

## Usage

### Vulkan Structure Aliases

You no longer need to set sType. Replace vk with vkw:: and use like so:

```C++
vkw::ApplicationInfo appInfo({
    .pApplicationName = "test",
    .apiVersion = VK_API_VERSION_2_0
});
```

or

```C++
vkw::ApplicationInfo appInfo;
appInfo.pApplicationName = "test";
appInfo.apiVersion = VK_API_VERSION_2_0;
```

After that, use it like normal.

### Vulkan RAII Object Wrappers

No more needing to call vkDestroy. Replace Vk with vkw:: and pass the usual vkCreate arguments to the constructor (without the last two) like so:

```C++
vkw::Instance instance(&createInfo);
```

Instead of

```C++
VkInstance instance;
vkCreateInstance(&createInfo, nullptr, &instance);

// ...

vkDestroyInstance(instance, nullptr);
```

To use have the RAII object hold nothing do:

```C++
vkw::Instance instance(nullptr);
```

### Error Handling

Wrap Vulkan function calls with the `VKW_CHECK` macro to throw an error if the returned VkResult is a nono. To override error handling, define VKW_CHECK(result) before including `VulkanWrapper.h`.

If an object fails to be created inside an RAII object, an error will be thrown stating what type of object failed to be made. To override how the VkResult code is handled, define VKW_CHECK_CREATION(typeName, result) before including `VulkanWrapper.h`.

```C++
VKW_CHECK(vkEndCommandBuffer(commandBuffer));
```
