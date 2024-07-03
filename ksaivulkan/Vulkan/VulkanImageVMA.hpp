#pragma once
#include "VulkanImage.hpp"
#include "VulkanVMA.hpp"

namespace ksai {
class VulkanImageVMA : public VulkanImageBase {
    public:
    struct CreateInfo {
        const VulkanVMA *inVMA;
        const VulkanDevice *inDevice;
        ui inWidth;
        ui inHeight;
        ImageContext inImageContext          = ImageContext::Default;
        ui inChannel                         = 4;
        ui inLayerCount                      = 1;
        ui inSamples                         = 1;
        ui inMips                            = 1;
        opt<vk::ImageUsageFlags> inUsageBits = std::nullopt;
        opt<vk::ImageLayout> inLayout        = std::nullopt;
        opt<vk::Format> inFormat             = std::nullopt;
    };

    VulkanImageVMA() = default;
    ~VulkanImageVMA();
    VulkanImageVMA(const VulkanImageVMA &other)            = delete;
    VulkanImageVMA &operator=(const VulkanImageVMA &other) = delete;
    VulkanImageVMA(VulkanImageVMA &&other)                 = default;
    VulkanImageVMA &operator=(VulkanImageVMA &&other)      = default;

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanImageVMA(const VulkanVMA &inVMA,
                   const VulkanDevice &inDevice,
                   ui inWidth,
                   ui inHeight,
                   ImageContext inImageContext,
                   ui inChannel                         = 4,
                   ui inLayerCount                      = 1,
                   ui inSamples                         = 1,
                   ui inMips                            = 1,
                   opt<vk::ImageUsageFlags> inUsageBits = std::nullopt,
                   opt<vk::ImageLayout> inLayout        = std::nullopt,
                   opt<vk::Format> inFormat             = std::nullopt);

    private:
    const VulkanVMA *mVMA;
    VmaAllocation mAllocation;
    bool mInitialized = false;
};
} // namespace ksai
