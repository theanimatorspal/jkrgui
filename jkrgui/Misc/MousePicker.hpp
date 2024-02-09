#pragma once
#include <Window.hpp>
#include <Vendor/stbi/stb_image_write.h>

namespace Jkr {
    namespace Misc {
        class FrameCapturer {
            using MousePickingBufferType = VulkanBufferVMA<BufferContext::Storage, MemoryType::HostVisibleAndCoherenet>;
        public:
            FrameCapturer(Window& inWindow)
                : mWindow(inWindow)
            {
                Refresh();
            }
            void Refresh();
            void Dispatch();
            void CaptureToFile(const std::string_view inFileName)
            {
                // TODO RGBA Mistake
                stbi_write_bmp(inFileName.data(), mWidth, mHeight, mChannelCount, mMappedMemory);
            }

        private:
            Window& mWindow;
            Up<MousePickingBufferType> mMousePickingBuffer;
            void* mMappedMemory;
            int mWidth, mHeight, mChannelCount;

        };
    }
}