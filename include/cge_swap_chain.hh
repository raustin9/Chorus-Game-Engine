#pragma once
#ifndef SWAP_CHAIN
#define SWAP_CHAIN


// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>
#include <memory>
#include "cge_device.hh"

namespace cge {

    class CGE_SwapChain {
     public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    
        CGE_SwapChain(CGE_Device &deviceRef, VkExtent2D windowExtent);
        CGE_SwapChain(CGE_Device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<CGE_SwapChain> previous);
        ~CGE_SwapChain();
    
        CGE_SwapChain(const CGE_SwapChain &) = delete;
        CGE_SwapChain& operator=(const CGE_SwapChain &) = delete;
    
        VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
        VkRenderPass getRenderPass() { return renderPass; }
        VkImageView getImageView(int index) { return swapChainImageViews[index]; }
        size_t imageCount() { return swapChainImages.size(); }
        VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
        VkExtent2D getSwapChainExtent() { return swapChainExtent; }
        uint32_t width() { return swapChainExtent.width; }
        uint32_t height() { return swapChainExtent.height; }
    
        float extentAspectRatio() {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }
        VkFormat findDepthFormat();
    
        VkResult acquireNextImage(uint32_t *imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

        bool compareSwapFormats(const CGE_SwapChain& swapChain) const {
            return swapChain.swapChainDepthFormat == swapChainDepthFormat && swapChain.swapChainImageFormat == swapChainImageFormat;
        }
    
     private:
        void init();
        void createSwapChain();
        void createImageViews();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();
    
        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(
                const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    
        VkFormat swapChainImageFormat;
        VkFormat swapChainDepthFormat;
        VkExtent2D swapChainExtent;
    
        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkRenderPass renderPass;
    
        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemorys;
        std::vector<VkImageView> depthImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;
    
        CGE_Device &device;
        VkExtent2D windowExtent;
    
        VkSwapchainKHR swapChain;
        std::shared_ptr<CGE_SwapChain> oldSwapChain;
    
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;
    };

}  /* end cge namespace */

#endif /* SWAP_CHAIN */

