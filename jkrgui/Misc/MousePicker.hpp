#pragma once
#include <Window.hpp>

namespace Jkr {

class MousePicker {
    using MousePickingBufferType = VulkanBuffer<BufferContext::Staging, MemoryType::HostVisibleAndCoherenet>;
public:
    MousePicker(Window& inWindow)
        : mWindow(inWindow)
    {
        Refresh();
    }
    void Refresh();
    void Dispatch();

private:
    Window& mWindow;
    Up<MousePickingBufferType> mMousePickingBuffer;

};

}