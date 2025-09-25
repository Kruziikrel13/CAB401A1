#include "VulkanContext.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <fstream>

// Created using Vulkan docs at
// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance and
// https://github.com/bmilde/vulkan_matrix_mul
// https://docs.vulkan.org/tutorial/latest/00_Introduction.html

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

CVulkanContext::~CVulkanContext() {
    spdlog::trace("Destroying Vulkan Context.");

    // cleanup arrays
    for (auto& buffer : buffers) {
        buffer = nullptr;
    }

    for (auto& memory : bufferMemories) {
        memory = nullptr;
    }

    // Resources are automatically cleaned up by vk::raii destructors
    spdlog::trace("Vulkan Context destroyed successfully.");
}

void CVulkanContext::createInstance() {
    spdlog::trace("Creating Vulkan Instance.");

    // Optional application info for the Vulkan Instance
    constexpr vk::ApplicationInfo appInfo{.pApplicationName   = "CAB401 Matrix Multiply",
                                          .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                                          .pEngineName        = "No Engine",
                                          .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
                                          .apiVersion         = vk::ApiVersion14};
    vk::InstanceCreateInfo        createInfo{.pApplicationInfo = &appInfo};

    instance = vk::raii::Instance(context, createInfo);

    spdlog::trace("Vulkan Instance created successfully.");
}

void CVulkanContext::pickPhysicalDevice() {
    spdlog::trace("Selecting physical device.");

    std::vector<vk::raii::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
    if (devices.empty())
        throw std::runtime_error("Failed to find GPUs with Vulkan support.");

    for (const vk::raii::PhysicalDevice& device : devices) {
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
    spdlog::trace("Creating a logical device.");

    float                      queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo  queueCreateInfo{.queueFamilyIndex = computeQueueFamilyIndex, .queueCount = 1, .pQueuePriorities = &queuePriority};

    vk::PhysicalDeviceFeatures features{};
#ifdef NDEBUG
    features.robustBufferAccess = VK_FALSE;
#endif
    spdlog::trace("Retrieved device features and queue info for logical device creation.");
    vk::DeviceCreateInfo createInfo{
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos    = &queueCreateInfo,
        .pEnabledFeatures     = &features,
    };
    //FIXME: ASan detects a memory leak coming from here, use vulkan validation layers to verify what's going wrong
    device       = physicalDevice.createDevice(createInfo);
    computeQueue = device.getQueue(computeQueueFamilyIndex, 0);

    spdlog::trace("Logical device created successfully.");
}

void CVulkanContext::createCommandPool() {
    spdlog::trace("Creating command pool");

    vk::CommandPoolCreateInfo poolInfo{
        .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = computeQueueFamilyIndex,
    };

    commandPool = device.createCommandPool(poolInfo);

    spdlog::trace("Successfully created command pool.");
}

void CVulkanContext::createDescriptorSetLayout() {
    spdlog::trace("Creating descriptor set layout.");

    std::vector<vk::DescriptorSetLayoutBinding> bindings(buffers.size());

    for (size_t i = 0; i < buffers.size(); i++) {
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

    descriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);

    spdlog::trace("Descriptor set layout created successfully.");
}

void CVulkanContext::createComputePipeline() {
    spdlog::trace("Creating compute pipeline.");
    auto                       shaderCode = readShaderFile("shaders/matrix_comp.spv");

    vk::ShaderModuleCreateInfo shaderModuleInfo{
        .codeSize = shaderCode.size() * sizeof(char),
        .pCode    = reinterpret_cast<const uint32_t*>(shaderCode.data()),
    };

    computeShaderModule = device.createShaderModule(shaderModuleInfo);
    spdlog::trace("Created compute shader module.");

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
    spdlog::trace("Created pipeline layout.");

    const vk::ComputePipelineCreateInfo pipelineInfo{
        .stage  = shaderStageInfo,
        .layout = pipelineLayout,
    };
    computePipeline = device.createComputePipeline(VK_NULL_HANDLE, pipelineInfo);
    spdlog::trace("Created compute pipeline.");

    spdlog::trace("Successfully created compute pipeline.");
}

void CVulkanContext::createDescriptorPool() {
    spdlog::trace("Creating Descriptor Pool.");

    vk::DescriptorPoolSize       poolSize{.type = vk::DescriptorType::eStorageBuffer, .descriptorCount = buffers.size()};

    vk::DescriptorPoolCreateInfo poolInfo{
        .maxSets       = 1,
        .poolSizeCount = 1,
        .pPoolSizes    = &poolSize,
    };

    descriptorPool = device.createDescriptorPool(poolInfo);

    spdlog::trace("Descriptor Pool created successfully.");
}

uint32_t CVulkanContext::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    spdlog::trace("Finding suitable memory type.");
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            spdlog::trace("Found suitable memory type: {}", i);
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type.");
}

void CVulkanContext::allocateBufferMemory(vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory, vk::MemoryPropertyFlags properties) {
    vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{
        .allocationSize  = memRequirements.size,
        .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties),
    };
    bufferMemory = vk::raii::DeviceMemory(device, allocInfo);
    buffer.bindMemory(*bufferMemory, 0);
}

void CVulkanContext::runComputeShader(float* matrixA, float* matrixB, float* matrixC) {
    spdlog::info("Running compute shader.");

    spdlog::trace("Resetting descriptor pool.");
    descriptorPool.reset();
}
