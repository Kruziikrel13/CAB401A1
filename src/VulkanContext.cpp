#include "VulkanContext.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <fstream>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

// Created using Vulkan docs at
// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance and
// https://github.com/bmilde/vulkan_matrix_mul
// https://docs.vulkan.org/tutorial/latest/00_Introduction.html

#define BUF_COUNT 3

static std::vector<char> readShaderFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file!");
    }

    std::vector<char> buffer(file.tellg());

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    file.close();

    return buffer;
}

CVulkanContext::CVulkanContext(int matrix_size) : matrixSize(matrix_size) {
    spdlog::info("Initializing Vulkan Context [{}]", matrixSize);

    try {
        // Step One: Instance and Physical Device Selection
        createInstance();
        pickPhysicalDevice();
        createLogicalDevice();
        spdlog::info("Vulkan initialized! With GPU: {}", deviceName);

        createCommandPool();
        createDescriptorSetLayout();
        createComputePipeline();
        createDescriptorPool();
        spdlog::info("Vulkan setup completed and compute shader successfully loaded.");

    } catch (const vk::SystemError& err) { throw std::runtime_error("Vulkan System Error: " + std::string(err.what())); }

    spdlog::info("Vulkan Context initialized successfully.");
}

void CVulkanContext::createInstance() {
    spdlog::debug("Creating Vulkan Instance.");

    // Optional application info for the Vulkan Instance
    constexpr vk::ApplicationInfo appInfo{.pApplicationName   = "CAB401 Matrix Multiply",
                                          .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                                          .pEngineName        = "No Engine",
                                          .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
                                          .apiVersion         = vk::ApiVersion14};

    vk::InstanceCreateInfo        createInfo{.pApplicationInfo = &appInfo};

    instance = vk::raii::Instance(context, createInfo);

    spdlog::debug("Vulkan Instance created successfully.");
}

void CVulkanContext::pickPhysicalDevice() {
    spdlog::debug("Selecting physical device.");

    auto devices = instance.enumeratePhysicalDevices();
    if (devices.empty()) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support.");
    }

    for (const auto& device : devices) {
        auto deviceProperties = device.getProperties();
        spdlog::debug("Found device: {}", deviceProperties.deviceName.data());

        auto queueFamilyProperties = device.getQueueFamilyProperties();
        for (uint32_t i = 0; i < queueFamilyProperties.size(); ++i) {
            if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute) {
                physicalDevice          = device;
                computeQueueFamilyIndex = i;
                deviceName              = deviceProperties.deviceName.data();
                spdlog::debug("Selected device with compute support: {}", deviceName);
                return;
            }
        }
    };

    throw std::runtime_error("Failed to find a suitable GPU with compute capabilities.");
}

void CVulkanContext::createLogicalDevice() {
    spdlog::debug("Creating a logical device.");

    float                     queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo{.queueFamilyIndex = computeQueueFamilyIndex, .queueCount = 1, .pQueuePriorities = &queuePriority};
    vk::DeviceCreateInfo      createInfo{
             .queueCreateInfoCount = 1,
             .pQueueCreateInfos    = &queueCreateInfo,
    };

    device       = vk::raii::Device(physicalDevice, createInfo);
    computeQueue = vk::raii::Queue(device, computeQueueFamilyIndex, 0);

    spdlog::debug("Logical device created successfully.");
}

void CVulkanContext::createCommandPool() {
    spdlog::debug("Creating command pool");

    vk::CommandPoolCreateInfo poolInfo{
        .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = computeQueueFamilyIndex,
    };

    commandPool = vk::raii::CommandPool(device, poolInfo);

    spdlog::debug("Successfully created command pool.");
}

void CVulkanContext::createDescriptorSetLayout() {
    spdlog::debug("Creating descriptor set layout.");

    std::vector<vk::DescriptorSetLayoutBinding> bindings(BUF_COUNT);

    for (int i = 0; i < BUF_COUNT; i++) {
        bindings[i] = vk::DescriptorSetLayoutBinding{
            .binding         = i,
            .descriptorType  = vk::DescriptorType::eStorageBuffer,
            .descriptorCount = 1,
            .stageFlags      = vk::ShaderStageFlagBits::eCompute,
        };
    }

    vk::DescriptorSetLayoutCreateInfo layoutInfo{
        .bindingCount = bindings.size(),
        .pBindings    = bindings.data(),
    };

    descriptorSetLayout = vk::raii::DescriptorSetLayout(device, layoutInfo);

    spdlog::debug("Descriptor set layout created successfully.");
}

void CVulkanContext::createComputePipeline() {
    spdlog::debug("Creating compute pipeline.");
    auto                       shaderCode = readShaderFile("shaders/matrix_comp.spv");

    vk::ShaderModuleCreateInfo shaderModuleInfo{
        .codeSize = shaderCode.size() * sizeof(char),
        .pCode    = reinterpret_cast<const uint32_t*>(shaderCode.data()),
    };

    computeShaderModule = vk::raii::ShaderModule(device, shaderModuleInfo);
    spdlog::debug("Created compute shader module.");

    vk::PipelineShaderStageCreateInfo shaderStageInfo{
        .stage  = vk::ShaderStageFlagBits::eCompute,
        .module = computeShaderModule,
        .pName  = "main",
    };

    vk::PushConstantRange pushConstantRange{
        .stageFlags = vk::ShaderStageFlagBits::eCompute,
        .offset     = 0,
        .size       = sizeof(uint32_t),
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
        .setLayoutCount         = 1,
        .pSetLayouts            = &*descriptorSetLayout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges    = &pushConstantRange,
    };
    pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);
    spdlog::debug("Created pipeline layout.");

    const vk::ComputePipelineCreateInfo pipelineInfo{
        .stage  = shaderStageInfo,
        .layout = pipelineLayout,
    };
    computePipeline = device.createComputePipeline(VK_NULL_HANDLE, pipelineInfo);
    spdlog::debug("Created compute pipeline.");

    spdlog::debug("Successfully created compute pipeline.");
}

void CVulkanContext::createDescriptorPool() {
    spdlog::debug("Creating Descriptor Pool.");

    vk::DescriptorPoolSize poolSize{
        .type            = vk::DescriptorType::eStorageBuffer,
        .descriptorCount = BUF_COUNT,
    };

    vk::DescriptorPoolCreateInfo poolInfo{
        .maxSets       = 1,
        .poolSizeCount = 1,
        .pPoolSizes    = &poolSize,
    };

    descriptorPool = vk::raii::DescriptorPool(device, poolInfo);

    spdlog::debug("Descriptor Pool created successfully.");
}
