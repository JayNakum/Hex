#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#if defined(HEX_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>
