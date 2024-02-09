#pragma once
#include <Window.hpp>

namespace Jkr {
    namespace Misc {
        class MousePicker {
            using MousePickingBufferType = VulkanBufferVMA<BufferContext::Storage, MemoryType::HostVisibleAndCoherenet>;
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
            void* mMappedMemory;

        };
    }
}