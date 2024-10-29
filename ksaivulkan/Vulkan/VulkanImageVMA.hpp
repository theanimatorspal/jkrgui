#pragma once
#include "VulkanImage.hpp"
#include "VulkanVMA.hpp"

namespace ksai {
class VulkanImageVMA : public VulkanImageBase {
    public:
    struct CreateInfo {
        VulkanVMA *inVMA;
        VulkanDevice *inDevice;
        ui inWidth;
        ui inHeight;
        ImageContext inImageContext = ImageContext::Default;
        ui inChannel                = 4;
        ui inLayerCount             = 1;
        ui inSamples                = 1;
        ui inMips                   = 1;

        ///@note Todo Remove this
        bool inCreateOnlyImageView             = false;

        opt<vk::ImageUsageFlags> inUsageBits   = std::nullopt;
        opt<vk::ImageLayout> inLayout          = std::nullopt;
        opt<vk::Format> inFormat               = std::nullopt;
        opt<vk::ImageViewType> inImageViewType = std::nullopt;

        /// @note  If this one is given then others are not required (except VMA and InDevice)
        opt<ImageProperties> inImageProperties = std::nullopt;

        /// @warning This doesn't appear to be used anywhere, comment if it is used somewhere, or
        /// remove this
        ui inImageViewBaseLayer = 0;
    };

    VulkanImageVMA() = default;
    ~VulkanImageVMA();
    VulkanImageVMA(VulkanImageVMA &other)             = delete;
    VulkanImageVMA &operator=(VulkanImageVMA &other)  = delete;
    VulkanImageVMA(VulkanImageVMA &&other)            = default;
    VulkanImageVMA &operator=(VulkanImageVMA &&other) = default;

    void Init(CreateInfo inCreateInfo);
    void Init(ImageProperties inProperties);
    void Destroy();

    VulkanImageVMA(VulkanVMA &inVMA,
                   VulkanDevice &inDevice,
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
    VulkanVMA *mVMA;
    VmaAllocation mAllocation;
    bool mInitialized = false;
};
} // namespace ksai
