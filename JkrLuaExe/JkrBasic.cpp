#include "JkrLuaExe.hpp"

namespace JkrEXE {

void CreateBasicBindings(sol::state& s) {
          auto Jkr = s["Jkr"].get_or_create<sol::table>();
          Jkr.new_usertype<Jkr::Instance>("Instance", sol::call_constructor, sol::factories([](int inDescriptorSize, int inPoolSize) {
                                                    return std::make_unique<Jkr::Instance>(inDescriptorSize, inPoolSize);
                                          }));
          Jkr.new_usertype<Jkr::Window>("WindowBase");
          Jkr.new_usertype<Jkr::WindowMulT>(
           "Window",
           sol::base_classes,
           sol::bases<Jkr::Window>(),
           sol::call_constructor,
           sol::factories([](Jkr::Instance& inInstance, std::string_view inTitle, int inWidth, int inHeight, int inThreadsCount) {
                     int NoOfCmdBufferPerThread = 2; // TODO Don't hardcode
                     std::vector<ui> CmdBufferCountPerThreadVec;
                     CmdBufferCountPerThreadVec.resize(inThreadsCount, NoOfCmdBufferPerThread);
                     return std::make_unique<Jkr::WindowMulT>(
                      inInstance, inTitle, inHeight, inWidth, inThreadsCount, CmdBufferCountPerThreadVec, inInstance.GetThreadPool());
           }),
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

          Jkr.new_usertype<Jkr::EventManager>("EventManager",
                                              sol::call_constructor,
                                              sol::factories([]() { return mu<Jkr::EventManager>(); }),
                                              "ShouldQuit",
                                              &Jkr::EventManager::ShouldQuit,
                                              "ProcessEvents",
                                              &Jkr::EventManager::ProcessEvents);

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
           [](Jkr::Instance& inInstance, PipelinePropertiesContext inContext) { return mu<Jkr::PainterCache>(inInstance, inContext); },
           "Store",
           [](Jkr::PainterCache& inCache,
              string_view inFilename,
              string_view inVertexShader,
              string_view inFragmentShader,
              string_view inComputeShader) {
                     inCache.Store(string(inFilename), string(inVertexShader), string(inFragmentShader), string(inComputeShader));
           },
           "Load",
           [](Jkr::PainterCache& inCache, string_view inFilename) { inCache.Load(string(inFilename)); }
           // TODO Sort
           // Variable
           // Descriptors
          );
}

} // namespace JkrEXE