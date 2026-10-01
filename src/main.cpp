/**
 * Copyright (c) 2026 AIperture-Labs & Xavier Beheydt <xavier.beheydt@gmail.com>
 */

#include <cstdlib>
#include <iostream>

#define VOLK_IMPLEMENTATION
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <volk.h>
#include <vulkan/vulkan.h>


VkInstance instance{ VK_NULL_HANDLE };

static inline void chk(VkResult result)
{
    if (result != VK_SUCCESS)
    {
        std::cerr << "Vulkan call returned an error (" << result << ")" <<std::endl;
        exit(result);
    }
}

int main(int argc, char* argv[])
{
    volkInitialize();
    // Instance
    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "How to Vulkan",
        .apiVersion = VK_API_VERSION_1_3,
    };
    uint32_t instanceExtensionCount{ 0 };
    char const* const* instanceExtensions{ SDL_Vulkan_GetInstanceExtensions(&instanceExtensionCount) };
    VkInstanceCreationInfo instanceCI{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        enabledExtensionCount = instanceExtensionCount,
        ppEnabledExtensionsNames = instanceExtensions,
    };
    chk(vkCreateInstance(&instanceCI, nullptr, &instance);

    return EXIT_SUCCESS;
}
