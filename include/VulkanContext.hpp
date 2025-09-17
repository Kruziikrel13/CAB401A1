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
    void                          allocateBufferMemory(VkBuffer buffer, VkDeviceMemory* bufferMemory, VkDeviceSize size, VkMemoryPropertyFlags properties);
    uint32_t                      findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void                          recordCommandBuffer();
    void                          cleanup();

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

    VkCommandBuffer               commandBuffer;
    VkDescriptorSet               descriptorSet;

    VkBuffer                      bufferA, bufferB, bufferC;
    VkDeviceMemory                bufferMemoryA, bufferMemoryB, bufferMemoryC;

    uint32_t                      computeQueueFamilyIndex;
};
