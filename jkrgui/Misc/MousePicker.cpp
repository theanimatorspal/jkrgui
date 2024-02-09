#include "MousePicker.hpp"

using namespace Jkr::Misc;

void MousePicker::Refresh()
{
    ui ImageChannels = 4;
    auto ImageExtent = mWindow.GetSwapChainImages().front().GetImageExtent();
    auto Size = ImageChannels * ImageExtent.width * ImageExtent.height;
    mMousePickingBuffer = MakeUp<MousePickingBufferType>(mWindow.GetInstance().GetVMA(), mWindow.GetInstance().GetDevice(), Size);
}

void MousePicker::Dispatch()
{
    auto& ImageTobeCopiedFrom = mWindow.GetSwapChainImages().front();
    if (ImageTobeCopiedFrom.GetImageHandle()) {
        mWindow.CmdCopySwapChainImageToBufferPostRendering(*mMousePickingBuffer);
    }
}
