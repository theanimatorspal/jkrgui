#pragma once
#include "MultiThreading.hpp"
#include "Offscreen.hpp"
#include "VulkanCommandBuffer.hpp"
#include <Vendor/Tracy/tracy/TracyVulkan.hpp>

namespace Jkr {
class Window : public Window_base {
    using VulkanCommandBufferArray = std::array<VulkanCommandBuffer, mMaxFramesInFlight>;
    using ThreadCommandBufferArray = ThreadCommandBuffer<mMaxFramesInFlight>;
    using VCBArray                 = VulkanCommandBufferArray;
    using PC                       = ParameterContext;

    public:
    /// @brief
    ///
    ///
    ///
    ///
    ///
    ///
    ///
    auto GetCommandBuffers(PC in) -> VCBArray & override;
    GETTER &GetShadowPass() { return *mShadowPass; }
    GETTER &GetDeferredPass() { return *mDeferredPass; }
    GETTER &GetArbritaryPassses() { return mArbritaryPasses; }
    void BuildShadowPass();
    void BeginShadowPass(int inI, float ind);
    void EndShadowPass();

    ///@brief Arbritary passes, (do whatever you want here)
    void BuildArbritaryPasses(int inNo, int inWidth, int inHeight);
    void BeginArbritaryPass(int index, int inWidth, int inHeight, glm::vec4 inClearValue);
    void EndArbritaryPass();

    ///@brief Deferred pass
    void BuildDeferredPass();
    void PrepareDeferredPass(Renderer::_3D::Simple3D &inCompositionSimple3D,
                             Renderer::_3D::World3D &inWorld);
    void ExecuteDeferredComposition(Renderer::_3D::Simple3D &inCompositionSimple3D,
                                    Renderer::_3D::World3D &inWorld);
    void BeginDeferredDraws(float r, float g, float b, float a, float d);
    void EndDeferredDraws();
    void PresentDeferred();

    ///@brief Main Functions
    void Wait();
    void AcquireImage();
    ///@brief begins the command buffer recording (for primary command buffer)
    ///@note all the dispatches should happen *after* the call of BeginRecording
    void BeginRecording();
    void EndRecording();

    ///@brief begins the renderpass
    void BeginDraws(float r, float g, float b, float a, float d);
    void EndDraws();
    void BlitImage();
    void Present();
    ///@param inShouldNotSubmit if true, it will only end the command buffer but not submit it
    void Submit();
    void Refresh();

    void BeginUIs();
    void EndUIs();
    void ExecuteUIs();
    void BeginThreadCommandBuffer(int inThreadId);
    void EndThreadCommandBuffer(int inThreadId);
    void ExecuteThreadCommandBuffer(int inThreadId);

    void BeginImmediate();
    void EndImmediate();

    static void SyncSubmitPresent(Window &inSubmitWindow, Window &inPresentWindow);

    Window(Instance &inInstance,
           const sv inTitle,
           ui inHeight,
           ui inWidth,
           ui inOffscreenFrameHeight,
           ui inOffscreenFrameWidth,
           ui inNumThreads,
           std::span<ui> inPerThreadBuffers,
           optref<ksai::ThreadPool> inPool = std::nullopt);
    Window(Instance &inInstance,
           std::span<ui> inPerThreadBuffers,
           ui inOffscreenFrameHeight,
           ui inOffscreenFrameWidth,
           ui inNumThreads,
           optref<ksai::ThreadPool> inPool = std::nullopt);
    ~Window() = default;

    private:
    optref<ksai::ThreadPool> mThreadPool;
    v<up<ThreadCommandBufferArray>> mThreadCommandBuffers;
    VulkanCommandPool mUICommandPool;
    VulkanCommandBufferArray mSecondaryCommandBuffersUI;
    Up<ShadowPass> mShadowPass;
    Up<DeferredPass> mDeferredPass;
    v<Up<ArbritaryPass>> mArbritaryPasses;
};

} // namespace Jkr
