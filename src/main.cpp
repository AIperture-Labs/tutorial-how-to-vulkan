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
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>


VkInstance instance{ VK_NULL_HANDLE };
VkDevice device{ VK_NULL_HANDLE };
VkQueue queue{ VK_NULL_HANDLE };
VmaAllocator allocator{ VK_NULL_HANDLE };

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

    // Queues
    uint32_t queueFamilyCount{ 0 };
    vkGetPhysicalDeviceQueueFamilyProperties(devices[deviceIndex], &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(devices[deviceIndex], &queueFamilyCount, queueFamilies.data());
    std::cout << "There is " << queueFamilyCount << " family queue(s) available." << std::endl;
    uint32_t queueFamily{ 0 };
    for (size_t i = 0; i < queueFamilies.size(); i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueFamily = i;
            break;
        }
    }
    chk(SDL_Vulkan_GetPresentationSupport(instance, devices[deviceIndex], queueFamily));
    const float qfpriorities{ 1.0f };
    VkDeviceQueueCreateInfo queueCI{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamily,
        .queueCount = 1,
        .pQueuePriorities = &qfpriorities
    };

    // Device setup
    const std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    VkPhysicalDeviceVulkan12Features enabledVk12Features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .descriptorIndexing = true,
        .shaderSampledImageArrayNonUniformIndexing = true,
        .descriptorBindingVariableDescriptorCount = true,
        .runtimeDescriptorArray = true,
        .bufferDeviceAddress = true,
    };
    VkPhysicalDeviceVulkan13Features enabledVk13Features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &enabledVk12Features,
        .synchronization2 = true,
        .dynamicRendering = true,
    };
    VkPhysicalDeviceFeatures enabledVk10Features{
        .samplerAnisotropy = VK_TRUE,
    };
    VkDeviceCreateInfo deviceCI{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &enabledVk13Features,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCI,
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = &enabledVk10Features,
    };
    chk(vkCreateDevice(devices[deviceIndex], &deviceCI, nullptr, &device));
    vkGetDeviceQueue(device, queueFamily, 0, &queue);

    // WMA
    VmaVulkanFunctions vkFunctions{
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
        .vkCreateImage = vkCreateImage
    };
    VmaAllocatorCreateInfo allocatorCI{
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = devices[deviceIndex],
        .device = device,
        .pVulkanFunctions = &vkFunctions,
        .instance = instance,
    };
    chk(vmaCreateAllocator(&allocatorCI, &allocator));


    // Tear Down
    SDL_Quit();
	// vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);
    return EXIT_SUCCESS;
}
