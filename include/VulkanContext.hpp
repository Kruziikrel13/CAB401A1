#pragma once

#include <iostream>
#include <vector>
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
    void                  createDescriptorSetLayout();
    void                  createComputePipeline();
    void                  cleanup();

    VkInstance            instance;
    VkDevice              device;
    VkQueue               computeQueue;
    VkCommandPool         commandPool;
    VkPhysicalDevice      physicalDevice;
    VkDescriptorSetLayout descriptorSetLayout;
    uint32_t              computeQueueFamilyIndex;

    int                   matrixSize;
};
