#pragma once

#include <vulkan/vulkan.hpp>

#define BUF_COUNT 3

// Vulkan Context Object
class CVulkanContext {
  public:
    CVulkanContext(int matrix_size);
    ~CVulkanContext();

  private:
    void                  createInstance();
    void                  pickPhysicalDevice();
    void                  createLogicalDevice();
    void                  createCommandPool();
    void                  createComputePipeline();
    void                  createDescriptorSetLayout();
    void                  createDescriptorPool();
    void                  createDescriptorSets();
    void                  createBuffers(float* matrixA, float* matrixB, float* matrixC, int matrix_size);
    void                  allocateBufferMemory(VkBuffer buffer, VkDeviceMemory* bufferMemory, VkDeviceSize size, VkMemoryPropertyFlags properties);
    uint32_t              findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void                  recordCommandBuffer();
    void                  cleanup();

    VkInstance            instance;
    VkDevice              device;
    VkPhysicalDevice      physicalDevice;
    VkQueue               computeQueue;
    uint32_t              computeQueueFamilyIndex;
    VkCommandPool         commandPool;
    VkPipeline            computePipeline;
    VkPipelineLayout      pipelineLayout;
    VkShaderModule        computeShaderModule;
    VkCommandBuffer       commandBuffer;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool      descriptorPool;
    VkDescriptorSet       descriptorSet;

    VkBuffer              bufferA, bufferB, bufferC;
    VkDeviceMemory        bufferMemoryA, bufferMemoryB, bufferMemoryC;

    int                   matrixSize;
};
