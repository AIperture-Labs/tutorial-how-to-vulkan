/**
 * Copyright (c) 2026 AIperture-Labs & Xavier Beheydt <xavier.beheydt@gmail.com>
 */

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

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

static inline void chk(bool result)
{
    if (!result)
    {
        std::cerr << "Call returned an error!" << std::endl;
        exit(result);
    }
}

int main(int argc, char* argv[])
{
    chk(SDL_Init(SDL_INIT_VIDEO));
	chk(SDL_Vulkan_LoadLibrary(NULL));
    volkInitialize();
    // Instance
    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "How to Vulkan",
        .apiVersion = VK_API_VERSION_1_3,
    };
    uint32_t instanceExtensionsCount{ 0 };
    char const* const* instanceExtensions{ SDL_Vulkan_GetInstanceExtensions(&instanceExtensionsCount) };
    VkInstanceCreateInfo instanceCI{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = instanceExtensionsCount,
        .ppEnabledExtensionNames = instanceExtensions,
    };
    chk(vkCreateInstance(&instanceCI, nullptr, &instance));
    volkLoadInstance(instance);

    // Device selection
    uint32_t deviceCount{ 0 };
    chk(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));
    std::vector<VkPhysicalDevice> devices(deviceCount);
    chk(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()));
    std::cout << "There is " << deviceCount << " device(s) available." << std::endl;
    uint32_t deviceIndex{ 0 };
    if (argc > 1)
    {
        deviceIndex = std::stoi(argv[1]);
        assert(deviceIndex < deviceCount);
    }
    VkPhysicalDeviceProperties2 deviceProperties{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
    vkGetPhysicalDeviceProperties2(devices[deviceIndex], &deviceProperties);
    std::cout << "Selected device: " << deviceProperties.properties.deviceName << std::endl;


    // Tear Down
    SDL_Quit();
	// vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);
    return EXIT_SUCCESS;
}
