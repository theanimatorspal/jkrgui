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
    void BuildShadowPass();
    void BeginShadowPass(float ind);
    void EndShadowPass();

    ///
    ///
    ///
    ///
    ///
    ///
    ///
    ///
    /// @todo Name this Correctly
    void PrepareDeferredPass();
    /// @todo Name this Correctly
    void BuildDeferredPass(Renderer::_3D::Simple3D &inCompositionSimple3D,
                           Renderer::_3D::World3D &inWorld);
    void ExecuteDeferredComposition(Renderer::_3D::Simple3D &inCompositionSimple3D,
                                    Renderer::_3D::World3D &inWorld);
    void BeginDeferredDraws(float r, float g, float b, float a, float d);
    void EndDeferredDraws();
    void PresentDeferred();

    ///
    ///
    ///
    ///
    ///
    ///
    ///
    ///@note remove these two, @todo these are to be unified to one function
    /// maybe call it "Wait"
    void BeginUpdates();
    void EndUpdates();
    void BeginDispatches();
    void EndDispatches();

    ///
    ///
    ///
    ///
    ///
    ///
    ///
    ///
    void BeginDraws(float r, float g, float b, float a, float d);
    void EndDraws();
    void Present();

    ///@note this is for Windowless Window <to only submit but not present>
    void Submit();
    void Refresh();

    ///
    ///
    ///
    ///
    ///
    ///
    ///
    ///
    void BeginUIs();
    void EndUIs();
    void ExecuteUIs();
    void BeginThreadCommandBuffer(int inThreadId);
    void EndThreadCommandBuffer(int inThreadId);
    void ExecuteThreadCommandBuffer(int inThreadId);

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
};

} // namespace Jkr
