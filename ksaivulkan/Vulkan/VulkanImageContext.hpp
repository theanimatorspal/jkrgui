#pragma once
#include <vulkan/vulkan.hpp>

namespace ksai {
enum class ImageContext {
    Default,
    Unknown,
    ExternalHandled,
    DepthImage,
    Storage,
    CubeCompatible,
    ColorAttach
};

struct ImageProperties {
    vk::Format mImageFormat                       = vk::Format::eR16G16B16A16Sfloat;
    vk::FormatFeatureFlagBits mImageFormatFeature = vk::FormatFeatureFlagBits::eSampledImage;
    vk::ImageType mImageType                      = vk::ImageType::e2D;
    vk::ImageUsageFlags mImageUsage               = vk::ImageUsageFlagBits::eSampled;
    vk::SampleCountFlagBits mSampleCountFlagBits  = vk::SampleCountFlagBits::e1;
    uint32_t mMipLevels                           = 1;
    uint32_t mArrayLayers                         = 1;
    vk::ImageTiling mTiling                       = vk::ImageTiling::eOptimal;
    vk::ImageAspectFlagBits mImageAspect          = vk::ImageAspectFlagBits::eColor;
    vk::MemoryPropertyFlagBits mMemoryProperty    = vk::MemoryPropertyFlagBits::eDeviceLocal;
    vk::ImageViewType mImageViewType              = vk::ImageViewType::e2D;
    vk::SharingMode mSharingMode                  = vk::SharingMode::eExclusive;
    vk::ImageLayout mInitialImageLayout           = vk::ImageLayout::eUndefined;
    vk::Extent2D mExtent;
    vk::ImageCreateFlags mFlags         = vk::ImageCreateFlags();
    vk::ImageLayout mCurrentImageLayout = mInitialImageLayout;
};
} // namespace ksai
