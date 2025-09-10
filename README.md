# VulkanWrapper

A faster and more bare bones alternative to vulkan.hpp!

Provides RAII wrappers, struct aliases that auto-set sType, shortcut for enumerating arrays, and error handling macros.

vulkan.hpp does all this and more but it has two flaws: VERY long compilation times and I found myself scrolling through their huge header files too much. These problems can be solved easily using pre-compiled headers and auto-complete but screw it, I want simplicity, here I am (please don't look at the template code).

In order to increase compile-time speed, I used templates so that essentially code is only generated for whatever objects are used in a file. This brings the number of lines of code to roughly 1000. Opposed to vulkan.hpp's 300,000 lines of code for a single include!

## Cons

- Requires C++20 but I should be able to add support for as low as C++11 if I feel like it.
- I only added struct and RAII object aliases for the most commonly used stuff. To add support for certain extensions, look at `include/VulkanStructAliases.h` or `include/VulkanObjectAliases.h`.
- You must either initialize the Vulkan struct aliases using designated initializers or set the members after using the default constructor. See [Usage](#usage) for more info. This will change once I get my hands on C++26's compile-time reflection.
- Uses Vulkan functions directly meaning the Vulkan Loader is required at compile-time. Can't DLL load at runtime.

## Usage

Make sure your program has access to all the files in `include` and then include the file `VulkanWrapper.h`. I could create a single-header-file if needed.

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

To have the RAII object hold nothing do:

```C++
vkw::Instance instance(nullptr);
```

### Shortcut for Enumerating Arrays

Instead of the usual:

```C++
uint32_t count;
vkEnumeratePhysicalDevices(instance, &count, nullptr);
std::vector<VkPhysicalDevice> physicalDevices(count);
vkEnumeratePhysicalDevices(instance, &count, elements.data());
```

You can do:

```C++
std::vector<VkPhysicalDevice> physicalDevices = vkw::enumeratePhysicalDevices(instance);
```

The same applies for all functions that match that scheme of first getting the number of elements, then getting the elements.

### Error Handling

Wrap Vulkan function calls with the `VKW_CHECK` macro to throw an std::runtime_error if the returned VkResult is a nono. To override error handling, define VKW_CHECK(result) before including `VulkanWrapper.h`.

If an object fails to be created inside an RAII object, a std::runtime_error will be thrown stating what type of object failed to be made. To override how the VkResult code is handled, define VKW_CHECK_CREATION(typeName, result) before including `VulkanWrapper.h`.

```C++
VKW_CHECK(vkEndCommandBuffer(commandBuffer));
```

### GLFW Bonus
If GLFW is included before including `VulkanWrapper.h`, the `vkw::Surface`'s constructor will be based on `glfwCreateWindowSurface`.

Also, `GLFWwindowWrapper` is provided as an RAII wrapper for creating and destroying a `GLFWwindow*`. No error handling is done if glfwCreateWindow returns a nullptr.

```C++
glfwWindowHint(...)
vkw::GLFWwindowWrapper window(width, height, title, monitor);
```

### Use everything else as normal

Just use Vulkan as normal except only replacing vk with vkw:: if you want some special functionality, otherwise keep vk at the front of each function call. The vkw structs and RAII wrappers can be mingled with the non-vkw stuff.
