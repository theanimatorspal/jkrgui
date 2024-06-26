#pragma once
#include "VulkanImage.hpp"
#include "VulkanVMA.hpp"

namespace ksai {
class VulkanImageVMA : public VulkanImageBase {
    public:
    VulkanImageVMA(const VulkanVMA& inVMA,
                   const VulkanDevice& inDevice,
                   ui inWidth,
                   ui inHeight,
                   ImageContext inImageContext,
                   ui inChannel                         = 4,
                   ui inLayerCount                      = 1,
                   ui inSamples                         = 1,
                   ui inMips                            = 1,
                   opt<vk::ImageUsageFlags> inUsageBits = std::nullopt,
                   opt<vk::ImageLayout> inFormat        = std::nullopt);
    ~VulkanImageVMA();

    private:
    const VulkanVMA& mVMA;
    VmaAllocation mAllocation;
};
} // namespace ksai
