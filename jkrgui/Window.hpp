#pragma once
#include "MultiThreading.hpp"
#include "Offscreen.hpp"
#include "VulkanCommandBuffer.hpp"
#include <Vendor/Tracy/tracy/TracyVulkan.hpp>

namespace Jkr {
class Window : public Window_base {
    using VulkanCommandBufferArray = std::array<VulkanCommandBuffer, mMaxFramesInFlight>;
    using ThreadCommandBufferArray = ThreadCommandBuffer<mMaxFramesInFlight>;

    public:
    const VulkanCommandBufferArray &GetCommandBuffers(ParameterContext inParameter) const override;
    GETTER &GetShadowPass() { return *mShadowPass; }
    GETTER &GetDeferredPass() { return *mDeferredPass; }
    void BuildShadowPass(ui inWidth, ui inHeight);
    void BeginShadowPass(float ind);
    void EndShadowPass();

    void BuildDeferredPass(ui inWidth, ui inHeight);
    void PrepareDeferredPass(Renderer::_3D::Simple3D &inCompositionSimple3D,
                             Renderer::_3D::World3D &inWorld);
    void ExecuteDeferredComposition(Renderer::_3D::Simple3D &inCompositionSimple3D,
                                    Renderer::_3D::World3D &inWorld);
    void BeginDeferredDraws(float r, float g, float b, float a, float d);
    void EndDeferredDraws();
    void PresentDeferred();

    void BeginUpdates();
    void EndUpdates();
    void BeginDispatches();
    void EndDispatches();

    void BeginDraws(float r, float g, float b, float a, float d);
    void EndDraws();
    void Present();
    void Refresh();

    void BeginUIs();
    void EndUIs();
    void ExecuteUIs();
    void BeginThreadCommandBuffer(int inThreadId);
    void EndThreadCommandBuffer(int inThreadId);
    void ExecuteThreadCommandBuffer(int inThreadId);

    Window(const Instance &inInstance,
           const sv inTitle,
           ui inHeight,
           ui inWidth,
           ui inNumThreads,
           std::span<ui> inPerThreadBuffers,
           optref<ksai::ThreadPool> inPool = std::nullopt);
    ~Window() = default;

    private:
    optref<ksai::ThreadPool> mThreadPool;
    v<up<ThreadCommandBufferArray>> mThreadCommandBuffers;
    VulkanCommandPool mUICommandPool;
    VulkanCommandBufferArray mSecondaryCommandBuffersUI;
    Up<ShadowPass> mShadowPass;
    Up<DeferredPass> mDeferredPass;
    glm::uvec2 mFrameSize;
};

} // namespace Jkr
