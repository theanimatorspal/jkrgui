#include "EventManager.hpp"
#include "Painter.hpp"
#include "Window.hpp"
#include "Renderers/ThreeD/Uniform3D.hpp"
#include "Renderers/ThreeD/World3D.hpp"
#include "JkrLuaExe.hpp"

namespace JkrEXE {
using namespace std;
using namespace Jkr;
void CreateBasicBindings(sol::state &s) {
    auto Jkr = s["Jkr"].get_or_create<sol::table>();
    Jkr.new_usertype<Jkr::Instance>(
         "Instance", sol::call_constructor, sol::factories([](int inDescriptorSize, int inPoolSize, bool inEnableValidation) {
             return std::make_unique<Jkr::Instance>(inDescriptorSize, inPoolSize, inEnableValidation);
         }));
    Jkr.new_usertype<Jkr::Window_base>("WindowBase");
    Jkr.new_usertype<Jkr::Window>(
         "Window",
         sol::base_classes,
         sol::bases<Jkr::Window_base>(),
         sol::call_constructor,
         sol::factories([](Jkr::Instance &inInstance, std::string_view inTitle, int inWidth, int inHeight, int inThreadsCount) {
             int NoOfCmdBufferPerThread = 2; // TODO Don't hardcode
             std::vector<ui> CmdBufferCountPerThreadVec;
             CmdBufferCountPerThreadVec.resize(inThreadsCount, NoOfCmdBufferPerThread);
             return std::make_unique<Jkr::Window>(
                  inInstance, inTitle, inHeight, inWidth, inThreadsCount, CmdBufferCountPerThreadVec, inInstance.GetThreadPool());
         }),
         "BuildShadowPass",
         &Jkr::Window::BuildShadowPass,
         "BeginShadowPass",
         &Jkr::Window::BeginShadowPass,
         "EndShadowPass",
         &Jkr::Window::EndShadowPass,

         "BuildDeferredPass",
         &Jkr::Window::BuildDeferredPass,
         "PrepareDeferredPass",
         &Jkr::Window::PrepareDeferredPass,
         "BeginDeferredDraws",
         &Jkr::Window::BeginDeferredDraws,
         "EndDeferredDraws",
         &Jkr::Window::EndDeferredDraws,

         "SetTitle",
         &Jkr::Window::SetTitle,
         "BeginUpdates",
         &Jkr::Window::BeginUpdates,
         "EndUpdates",
         &Jkr::Window::EndUpdates,
         "BeginDispatches",
         &Jkr::Window::BeginDispatches,
         "EndDispatches",
         &Jkr::Window::EndDispatches,
         "BeginDraws",
         &Jkr::Window::BeginDraws,
         "EndDraws",
         &Jkr::Window::EndDraws,
         "Present",
         &Jkr::Window::Present,

         "BeginUIs",
         &Jkr::Window::BeginUIs,
         "EndUIs",
         &Jkr::Window::EndUIs,
         "ExecuteUIs",
         &Jkr::Window::ExecuteUIs,

         "BeginThreadCommandBuffer",
         &Jkr::Window::BeginThreadCommandBuffer,
         "EndThreadCommandBuffer",
         &Jkr::Window::EndThreadCommandBuffer,
         "ExecuteThreadCommandBuffer",
         &Jkr::Window::ExecuteThreadCommandBuffer,

         "GetWindowCurrentTime",
         &Jkr::Window::GetWindowCurrentTime,
         "GetWindowDimension",
         &Jkr::Window::GetWindowDimension,
         "GetVulkanDrawableSize",
         &Jkr::Window::GetVulkanDrawableSize,

         "SetScissor",
         &Jkr::Window::SetScissor,
         "SetDefaultScissor",
         &Jkr::Window::SetDefaultScissor,
         "SetViewport",
         &Jkr::Window::SetViewport,
         "SetDefaultViewport",
         &Jkr::Window::SetDefaultViewport);

    using namespace Jkr;
    Jkr.new_usertype<Jkr::EventManager>("EventManager",
                                        sol::call_constructor,
                                        sol::factories([]() { return mu<Jkr::EventManager>(); }),
                                        "ShouldQuit",
                                        &Jkr::EventManager::ShouldQuit,
                                        "ProcessEvents",
                                        &EventManager::ProcessEvents,
                                        "ShouldQuit",
                                        &EventManager::ShouldQuit,
                                        "GetEventHandle",
                                        &EventManager::GetEventHandle,
                                        "GetMousePos",
                                        &EventManager::GetMousePos,
                                        "GetRelativeMousePos",
                                        &EventManager::GetRelativeMousePos,
                                        "GetMouseButtonValue",
                                        &EventManager::GetMouseButtonValue,
                                        "IsKeyPressedContinous",
                                        &EventManager::IsKeyPressedContinous,
                                        "IsLeftButtonPressedContinous",
                                        &EventManager::IsLeftButtonPressedContinous,
                                        "IsRightButtonPressedContinous",
                                        &EventManager::IsRightButtonPressedContinous,
                                        "IsLeftButtonPressed",
                                        &EventManager::IsLeftButtonPressed,
                                        "IsRightButtonPressed",
                                        &EventManager::IsRightButtonPressed,
                                        "IsKeyReleased",
                                        &EventManager::IsKeyReleased,
                                        "IsKeyPressed",
                                        &EventManager::IsKeyPressed,
                                        "StartTextInput",
                                        &EventManager::StartTextInput,
                                        "StopTextInput",
                                        &EventManager::StopTextInput,
                                        "IsMouseWithinAtTopOfStack",
                                        &EventManager::IsMouseWithinAtTopOfStack,
                                        "IsMouseWithin",
                                        &EventManager::IsMouseWithin,
                                        "SetBoundedRect",
                                        &EventManager::SetBoundedRect,
                                        "UpdateBoundedRect",
                                        &EventManager::UpdateBoundedRect,
                                        "SetEventCallBack",
                                        [](Jkr::EventManager &inManager, sol::function inFunction) {
                                            inManager.SetEventCallBack([=]() {
                                                auto result = inFunction();
                                                if (not result.valid()) {
                                                    sol::error error = result;
                                                    std::cout << error.what();
                                                    ksai_print(error.what());
                                                }
                                            });
                                        });

    auto PainterCacheTypeEnum = Jkr.new_enum<false>("PainterType",
                                                    "Default",
                                                    PipelinePropertiesContext::Default,
                                                    "Line",
                                                    PipelinePropertiesContext::Line,
                                                    "LineStrip",
                                                    PipelinePropertiesContext::LineStrip);
    Jkr.new_usertype<Jkr::PainterCache>(
         "PainterCache",
         sol::call_constructor,
         [](Jkr::Instance &inInstance, PipelinePropertiesContext inContext) {
             return mu<Jkr::PainterCache>(inInstance, inContext);
         },
         "Store",
         [](Jkr::PainterCache &inCache,
            string_view inFilename,
            string_view inVertexShader,
            string_view inFragmentShader,
            string_view inComputeShader) {
             inCache.Store(string(inFilename), string(inVertexShader), string(inFragmentShader), string(inComputeShader));
         },
         "Load",
         [](Jkr::PainterCache &inCache, string_view inFilename) { inCache.Load(string(inFilename)); }
         // TODO Sort
         // Variable
         // Descriptors
    );
}

} // namespace JkrEXE