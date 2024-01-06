#include "Window.hpp"
#include <SDL2/SDL_vulkan.h>

/*
from dele256/sdl_borderless_resize.cpp (Github)
*/

#define MOUSE_GRAB_PADDING 5

SDL_HitTestResult HitTestCallback(SDL_Window* Window, const SDL_Point* Area, void* Data)
{
    int Width, Height;
    SDL_GetWindowSize(Window, &Width, &Height);

    if (Area->y < MOUSE_GRAB_PADDING) {
        if (Area->x < MOUSE_GRAB_PADDING) {
            return SDL_HITTEST_RESIZE_TOPLEFT;
        } else if (Area->x > Width - MOUSE_GRAB_PADDING) {
            return SDL_HITTEST_RESIZE_TOPRIGHT;
        } else {
            return SDL_HITTEST_RESIZE_TOP;
        }
    } else if (Area->y > Height - MOUSE_GRAB_PADDING) {
        if (Area->x < MOUSE_GRAB_PADDING) {
            return SDL_HITTEST_RESIZE_BOTTOMLEFT;
        } else if (Area->x > Width - MOUSE_GRAB_PADDING) {
            return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
        } else {
            return SDL_HITTEST_RESIZE_BOTTOM;
        }
    } else if (Area->x < MOUSE_GRAB_PADDING) {
        return SDL_HITTEST_RESIZE_LEFT;
    } else if (Area->x > Width - MOUSE_GRAB_PADDING) {
        return SDL_HITTEST_RESIZE_RIGHT;
    } else if (Area->y < 25 && Area->x < 10) {
        return SDL_HITTEST_DRAGGABLE;
    }

    return SDL_HITTEST_NORMAL; // SDL_HITTEST_NORMAL <- Windows behaviour
}

void Jkr::SDLWindow::SetWindowBorderless()
{
    SDL_SetWindowBordered(mSDLWindowPtr, SDL_FALSE);
    SDL_SetWindowHitTest(mSDLWindowPtr, HitTestCallback, 0);
}

Jkr::SDLWindow::SDLWindow(std::string_view inName, int inHeight, int inWidth)
    : mName(inName)
    , mHeight(inHeight)
    , mWidth(inWidth)
{
    mSDLWindowPtr = SDL_CreateWindow(
        mName.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        mWidth,
        mHeight,
        SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
}

Jkr::SDLWindow::~SDLWindow()
{
    SDL_DestroyWindow(mSDLWindowPtr);
}

void Jkr::SDLWindow::SetSize(int inWidth, int inHeight)
{
}

void Jkr::SDLWindow::SetTitle(std::string inString) const
{
    SDL_SetWindowTitle(mSDLWindowPtr, inString.c_str());
}

std::pair<int, int> Jkr::SDLWindow::GetWindowSize() const
{
    int w, h;
    SDL_Vulkan_GetDrawableSize(mSDLWindowPtr, &w, &h);
    return std::make_pair(w, h);
}

void Jkr::SDLWindow::ToggleWindowFullScreen()
{
    if (!mWindowIsFullScreen) {
        SDL_SetWindowFullscreen(mSDLWindowPtr, SDL_WINDOW_FULLSCREEN_DESKTOP);
        mWindowIsFullScreen = true;
    } else {
        SDL_SetWindowFullscreen(mSDLWindowPtr, 0);
        SDL_SetWindowSize(mSDLWindowPtr, mWidth, mHeight);
        mWindowIsFullScreen = false;
    }
}

void Jkr::SDLWindow::Minimize()
{
    SDL_SetWindowFullscreen(mSDLWindowPtr, 0);
    SDL_SetWindowSize(mSDLWindowPtr, mWidth, mHeight);
    SDL_MinimizeWindow(mSDLWindowPtr);
    mWindowIsFullScreen = false;
}

Jkr::Window::Window(const Instance& inInstance, std::string_view inTitle, int inHeight, int inWidth)
    : SDLWindow(inTitle, inHeight, inWidth)
    , mInstance(inInstance)
    , mSurface(mInstance.GetVulkanInstance(), mSDLWindowPtr)
    , mSwapChain(mInstance.GetDevice(),
          mInstance.GetQueueContext(),
          mSurface.ProcessCurrentSurfaceConditions(mInstance.GetPhysicalDevice())
              .ProcessCurrentSurfaceExtents(mInstance.GetPhysicalDevice()))
    , mDepthImage(mInstance.GetDevice(), mSurface)
    , mRenderPass(mInstance.GetDevice(), mSurface, mDepthImage)
    , mSwapChainImages { mSwapChain.GetVulkanImages(mInstance.GetDevice(), mSurface) }
    , mFrameBuffers { FrameBufferType(mInstance.GetDevice(),
                          mRenderPass,
                          mSwapChainImages[0],
                          mDepthImage),
        FrameBufferType(mInstance.GetDevice(),
            mRenderPass,
            mSwapChainImages[1],
            mDepthImage) }
    , mImageAvailableSemaphores { VulkanSemaphore(mInstance.GetDevice()),
        VulkanSemaphore(mInstance.GetDevice()) }
    , mRenderFinishedSemaphores { VulkanSemaphore(mInstance.GetDevice()),
        VulkanSemaphore(mInstance.GetDevice()) }
    , mFences { VulkanFence(mInstance.GetDevice()), VulkanFence(mInstance.GetDevice()) }
    , mCommandPool(inInstance.GetDevice(), inInstance.GetQueueContext())
    , mCommandBuffers { VulkanCommandBuffer(inInstance.GetDevice(), mCommandPool),
        VulkanCommandBuffer(inInstance.GetDevice(), mCommandPool) }
{
}

void Jkr::Window::Draw(float r, float g, float b, float a, float d)
{
    mUpdateFunction(mData);
    std::array<float, 5>
        ClearValues = { r, g, b, a, d };
    mFences[mCurrentFrame].Wait();
    std::pair<uint32_t, uint32_t> SwapChainResult = mSwapChain.AcquireNextImage(mImageAvailableSemaphores[mCurrentFrame]);
    mFences[mCurrentFrame].Reset();
    auto& CommandBuffers = this->GetCommandBuffers();
    if (!mSwapChain.ImageIsNotOptimal(SwapChainResult)) {
        mAcquiredImageIndex = SwapChainResult.second;
        CommandBuffers[mCurrentFrame].Reset();
        CommandBuffers[mCurrentFrame].Begin();

        mComputeDispatchFunction(mData);

        CommandBuffers[mCurrentFrame].BeginRenderPass(
            mRenderPass,
            mSurface,
            mFrameBuffers[mAcquiredImageIndex], // यो स्थानमा जहिल्यै झुक्किन्छ । फ्रेम बफर एउटा हुने हो ।  "Acquired Image Index" अनुसार ।
            ClearValues);

        mDrawFunction(mData);

        CommandBuffers[mCurrentFrame].EndRenderPass();

        CommandBuffers[mCurrentFrame].End();

        mInstance.GetGraphicsQueue().Submit<SubmitContext::ColorAttachment>(
            mImageAvailableSemaphores[mCurrentFrame],
            mRenderFinishedSemaphores[mCurrentFrame],
            mFences[mCurrentFrame],
            CommandBuffers[mCurrentFrame]);
        uint32_t Result = mInstance.GetGraphicsQueue().Present<mMaxFramesInFlight, SubmitContext::ColorAttachment>(
            mSwapChain,
            mRenderFinishedSemaphores[mCurrentFrame],
            mAcquiredImageIndex);
        if (mSwapChain.ImageIsNotOptimal(Result)) {
            Refresh();
        } else {
            mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
        }
    } else {
        Refresh();
    }
}

void Jkr::Window::Refresh()
{
    mSurface.ProcessCurrentSurfaceConditions(mInstance.GetPhysicalDevice());
    mSwapChain = VulkanSwapChain<mMaxFramesInFlight>(mInstance.GetDevice(), mInstance.GetQueueContext(), mSurface);
    mSwapChainImages = mSwapChain.GetVulkanImages(mInstance.GetDevice(), mSurface);
    mDepthImage = VulkanImage<ImageContext::DepthImage>(mInstance.GetDevice(), mSurface);
    mFrameBuffers = {
        FrameBufferType(mInstance.GetDevice(), mRenderPass, mSwapChainImages[0], mDepthImage),
        FrameBufferType(mInstance.GetDevice(), mRenderPass, mSwapChainImages[1], mDepthImage)
    };
}
