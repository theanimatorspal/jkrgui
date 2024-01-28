#include "MousePicker.hpp"

void Jkr::MousePicker::Refresh()
{
    ui ImageChannels = 4;
    auto ImageExtent = mWindow.GetSwapChainImages().front().GetImageExtent();
    auto Size = ImageChannels * ImageExtent.width * ImageExtent.height;
    mMousePickingBuffer = MakeUp<MousePickingBufferType>(mWindow.GetInstance().GetDevice(), Size);
}

void Jkr::MousePicker::Update()
{
     
}
