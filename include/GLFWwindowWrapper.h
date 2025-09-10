/*
 * An RAII wrapper for GLFWwindow. Any window hints must be specified before the object is created
 */

#pragma once

namespace vkw
{

class GLFWwindowWrapper
{
public:
    GLFWwindowWrapper(nullptr_t) : handle(nullptr) {}

    GLFWwindowWrapper(int width, int height, const char* title, GLFWmonitor* monitor)
    {
	handle = glfwCreateWindow(width, height, title, monitor, nullptr);
    }

    ~GLFWwindowWrapper()
    {
	glfwDestroyWindow(handle);
    }

    GLFWwindowWrapper(GLFWwindowWrapper const&) = delete;
    GLFWwindowWrapper& operator=(GLFWwindowWrapper const&) = delete;

    GLFWwindowWrapper(GLFWwindowWrapper&& rhs) noexcept : handle(rhs.handle)
    {
	rhs.handle = nullptr;
    }

    GLFWwindowWrapper& operator=(GLFWwindowWrapper&& rhs) noexcept
    {
	GLFWwindow* temp = handle;
	handle = rhs.handle;
	rhs.handle = temp;

	return *this;
    }

    operator GLFWwindow*() const noexcept
    {
	return handle;
    }

private:
    GLFWwindow* handle;
};

}
