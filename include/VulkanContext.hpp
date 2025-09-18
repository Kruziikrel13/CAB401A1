#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

// Vulkan Context Object
class CVulkanContext {
  public:
    CVulkanContext(int matrix_size);

    std::string deviceName;
    int         matrixSize;

  private:
    void                          createInstance();
    void                          pickPhysicalDevice();
    void                          createLogicalDevice();
    void                          createCommandPool();
    void                          createComputePipeline();
    void                          createDescriptorSetLayout();
    void                          createDescriptorPool();
    void                          createDescriptorSets();
    void                          createBuffers(float* matrixA, float* matrixB, float* matrixC, int matrix_size);
    void                          allocateBufferMemory(vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory, vk::MemoryPropertyFlags properties);
    uint32_t                      findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    void                          recordCommandBuffer();

    vk::raii::Context             context;
    vk::raii::Instance            instance            = nullptr;
    vk::raii::Device              device              = nullptr;
    vk::raii::PhysicalDevice      physicalDevice      = nullptr;
    vk::raii::Queue               computeQueue        = nullptr;
    vk::raii::CommandPool         commandPool         = nullptr;
    vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
    vk::raii::Pipeline            computePipeline     = nullptr;
    vk::raii::ShaderModule        computeShaderModule = nullptr;
    vk::raii::PipelineLayout      pipelineLayout      = nullptr;
    vk::raii::DescriptorPool      descriptorPool      = nullptr;
    vk::raii::CommandBuffer       commandBuffer       = nullptr;
    vk::raii::DescriptorSet       descriptorSet       = nullptr;
    vk::raii::Buffer              bufferA             = nullptr;
    vk::raii::Buffer              bufferB             = nullptr;
    vk::raii::Buffer              bufferC             = nullptr;
    vk::raii::DeviceMemory        bufferMemoryA       = nullptr;
    vk::raii::DeviceMemory        bufferMemoryB       = nullptr;
    vk::raii::DeviceMemory        bufferMemoryC       = nullptr;

    uint32_t                      computeQueueFamilyIndex;
};
