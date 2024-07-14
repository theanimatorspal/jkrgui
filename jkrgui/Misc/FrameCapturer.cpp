#include "FrameCapturer.hpp"

using namespace Jkr::Misc;

void FrameCapturer::Refresh() {
    ui ImageChannels    = 4;
    auto ImageExtent    = mWindow.GetSwapChainImages().front().GetImageExtent();
    auto Size           = ImageChannels * ImageExtent.width * ImageExtent.height;
    mMousePickingBuffer = MakeUp<MousePickingBufferType>(mWindow.GetInstance().GetVMA(),
                                                         mWindow.GetInstance().GetDevice(),
                                                         Size,
                                                         ksai::BufferContext::Storage,
                                                         ksai::MemoryType::HostVisibleAndCoherenet);
    mMousePickingBuffer->MapMemoryRegion(&mMappedMemory);
}

void FrameCapturer::Dispatch() {
    auto &ImageTobeCopiedFrom = mWindow.GetSwapChainImages().front();
    auto Extent               = ImageTobeCopiedFrom.GetImageExtent();
    mWidth                    = Extent.width;
    mHeight                   = Extent.height;
    mChannelCount             = 4; // TODO Get From Image

    if (ImageTobeCopiedFrom.GetImageHandle()) {
        // mWindow.CmdCopySwapChainImageToBufferPostRendering(*mMousePickingBuffer);
    }
}
