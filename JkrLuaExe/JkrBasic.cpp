#include "EventManager.hpp"
#include "JkrLuaExe.hpp"
#include "WindowMulT.hpp"

namespace JkrEXE {

void CreateBasicBindings(sol::state& s) {
    auto Jkr = s["Jkr"].get_or_create<sol::table>();
    Jkr.new_usertype<Jkr::Instance>(
         "Instance",
         sol::call_constructor,
         sol::factories([](int inDescriptorSize, int inPoolSize, bool inEnableValidation) {
             return std::make_unique<Jkr::Instance>(
                  inDescriptorSize, inPoolSize, inEnableValidation);
         }));
    Jkr.new_usertype<Jkr::Window>("WindowBase");
    Jkr.new_usertype<Jkr::WindowMulT>(
         "Window",
         sol::base_classes,
         sol::bases<Jkr::Window>(),
         sol::call_constructor,
         sol::factories([](Jkr::Instance& inInstance,
                           std::string_view inTitle,
                           int inWidth,
                           int inHeight,
                           int inThreadsCount) {
             int NoOfCmdBufferPerThread = 2; // TODO Don't hardcode
             std::vector<ui> CmdBufferCountPerThreadVec;
             CmdBufferCountPerThreadVec.resize(inThreadsCount, NoOfCmdBufferPerThread);
             return std::make_unique<Jkr::WindowMulT>(inInstance,
                                                      inTitle,
                                                      inHeight,
                                                      inWidth,
                                                      inThreadsCount,
                                                      CmdBufferCountPerThreadVec,
                                                      inInstance.GetThreadPool());
         }),
         "BuildShadowPass",
         &Jkr::WindowMulT::BuildShadowPass,

         "BeginShadowPass",
         &Jkr::WindowMulT::BeginShadowPass,
         "EndShadowPass",
         &Jkr::WindowMulT::EndShadowPass,

         "SetTitle",
         &Jkr::WindowMulT::SetTitle,
         "BeginUpdates",
         &Jkr::WindowMulT::BeginUpdates,
         "EndUpdates",
         &Jkr::WindowMulT::EndUpdates,
         "BeginDispatches",
         &Jkr::WindowMulT::BeginDispatches,
         "EndDispatches",
         &Jkr::WindowMulT::EndDispatches,
         "BeginDraws",
         &Jkr::WindowMulT::BeginDraws,
         "EndDraws",
         &Jkr::WindowMulT::EndDraws,
         "Present",
         &Jkr::WindowMulT::Present,

         "BeginUIs",
         &Jkr::WindowMulT::BeginUIs,
         "EndUIs",
         &Jkr::WindowMulT::EndUIs,
         "ExecuteUIs",
         &Jkr::WindowMulT::ExecuteUIs,

         "BeginThreadCommandBuffer",
         &Jkr::WindowMulT::BeginThreadCommandBuffer,
         "EndThreadCommandBuffer",
         &Jkr::WindowMulT::EndThreadCommandBuffer,
         "ExecuteThreadCommandBuffer",
         &Jkr::WindowMulT::ExecuteThreadCommandBuffer,

         "GetWindowCurrentTime",
         &Jkr::WindowMulT::GetWindowCurrentTime,
         "GetWindowDimension",
         &Jkr::WindowMulT::GetWindowDimension,

         "SetScissor",
         &Jkr::WindowMulT::SetScissor,
         "SetDefaultScissor",
         &Jkr::WindowMulT::SetDefaultScissor,
         "SetViewport",
         &Jkr::WindowMulT::SetViewport,
         "SetDefaultViewport",
         &Jkr::WindowMulT::SetDefaultViewport);

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
                                        [](Jkr::EventManager& inManager, sol::function inFunction) {
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
         [](Jkr::Instance& inInstance, PipelinePropertiesContext inContext) {
             return mu<Jkr::PainterCache>(inInstance, inContext);
         },
         "Store",
         [](Jkr::PainterCache& inCache,
            string_view inFilename,
            string_view inVertexShader,
            string_view inFragmentShader,
            string_view inComputeShader) {
             inCache.Store(string(inFilename),
                           string(inVertexShader),
                           string(inFragmentShader),
                           string(inComputeShader));
         },
         "Load",
         [](Jkr::PainterCache& inCache, string_view inFilename) {
             inCache.Load(string(inFilename));
         }
         // TODO Sort
         // Variable
         // Descriptors
    );
}

} // namespace JkrEXE