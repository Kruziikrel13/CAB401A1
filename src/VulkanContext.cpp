#include "VulkanContext.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>

// Created using Vulkan docs at
// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance and
// https://github.com/bmilde/vulkan_matrix_mul

CVulkanContext::CVulkanContext(int matrix_size) : matrixSize(matrix_size) {
    spdlog::debug("Initializing Vulkan Context [{}]", matrixSize);

    // Step One: Instance and Physical Device Selection
    createInstance();
    pickPhysicalDevice();
    createLogicalDevice();
    createCommandPool();
    createDescriptorSetLayout();
    createComputePipeline();
    createDescriptorPool();

    spdlog::debug("Vulkan Context initialized successfully.");
}

CVulkanContext::~CVulkanContext() {
    cleanup();
    spdlog::debug("Vulkan Context destroyed.");
}

void CVulkanContext::createInstance() {
    spdlog::debug("Creating Vulkan Instance.");

    // Optional application info for the Vulkan Instance
    VkApplicationInfo appInfo{};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "Matrix Multiply";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "No Engine";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_4;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan Instance!");
    }

    spdlog::debug("Vulkan Instance created successfully.");
}

void CVulkanContext::pickPhysicalDevice() {
    spdlog::debug("Selecting physical device.");

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan Support.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const VkPhysicalDevice& device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        spdlog::debug("Found device: {}", deviceProperties.deviceName);

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; ++i) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                physicalDevice          = device;
                computeQueueFamilyIndex = i;
                spdlog::debug("Selected device: {}", deviceProperties.deviceName);
                return;
            }
        }
    }

    throw std::runtime_error("Failed to find a suitable GPU with compute capabilities!");
}

void CVulkanContext::createLogicalDevice() {
    spdlog::debug("Creating a logical device.");

    float                   queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = computeQueueFamilyIndex;
    queueCreateInfo.queueCount       = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo       createInfo{};
    createInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos    = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures     = &deviceFeatures;

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a logical device.");
    }

    vkGetDeviceQueue(device, computeQueueFamilyIndex, 0, &computeQueue);

    spdlog::debug("Logical device created successfully.");
}

void CVulkanContext::createCommandPool() {
    spdlog::debug("Creating command pool");

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = computeQueueFamilyIndex;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a command pool.");
    }
    spdlog::debug("Successfully created command pool.");
}

void CVulkanContext::createDescriptorSetLayout() {
    spdlog::debug("Creating descriptor set layout.");

    std::vector<VkDescriptorSetLayoutBinding> bindings(BUF_COUNT);

    for (int i = 0; i < BUF_COUNT; i++) {
        bindings[i].binding         = i;
        bindings[i].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[i].descriptorCount = 1;
        bindings[i].stageFlags      = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings    = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a descriptor set layout");
    }

    spdlog::debug("Descriptor set layout created successfully.");
}

void CVulkanContext::createComputePipeline() {
    spdlog::debug("Creating compute pipeline.");

    spdlog::debug("Successfully created compute pipeline.");
}

void CVulkanContext::createDescriptorPool() {
    spdlog::debug("Creating descriptor pool.");
    spdlog::debug("Successfully created descriptor pool.");
}

void CVulkanContext::cleanup() {
    spdlog::debug("Cleaning up Vulkan Context Resources.");
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr); // This presently crashes
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    spdlog::debug("Vulkan Context Resources cleaned up successfully.");
}
