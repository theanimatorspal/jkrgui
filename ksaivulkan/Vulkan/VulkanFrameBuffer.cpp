#include "VulkanFrameBuffer.hpp"
using namespace ksai;

VulkanFrameBuffer &VulkanFrameBuffer::operator=(VulkanFrameBuffer &&Other) {
    ExplicitlyDestroy();
    mBuffer       = std::move(Other.mBuffer);
    mExtent       = std::move(Other.mExtent);
    Other.mBuffer = nullptr;
    return *this;
}

void VulkanFrameBuffer::Destroy() {
    ExplicitlyDestroy();
    mInitialized = false;
}

void VulkanFrameBuffer::ExplicitlyDestroy() {
    if (mBuffer) {
        mDevice->waitIdle();
        mDevice->destroyFramebuffer(mBuffer);
    }
    mBuffer = nullptr;
}