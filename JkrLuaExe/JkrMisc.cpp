#include "JkrLuaExe.hpp"
#include "PainterParameter.hpp"
#include "sol/sol.hpp"

namespace JkrEXE {
extern void CreateMainBindings(sol::state& s);

void CreateMiscBindings(sol::state& inState) {
               auto Jkr = inState["Jkr"].get_or_create<sol::table>();

               Jkr.new_usertype<Jkr::Misc::RecycleBin<int>>("RecycleBin",
                                                            sol::call_constructor,
                                                            sol::factories([]() { return Jkr::Misc::RecycleBin<int>(); }),
                                                            "Add",
                                                            &Jkr::Misc::RecycleBin<int>::Add,
                                                            "Remove",
                                                            &Jkr::Misc::RecycleBin<int>::Remove,
                                                            "Exists",
                                                            &Jkr::Misc::RecycleBin<int>::Exists,
                                                            "IsEmpty",
                                                            &Jkr::Misc::RecycleBin<int>::IsEmpty,
                                                            "Get",
                                                            &Jkr::Misc::RecycleBin<int>::Get);

               Jkr.new_usertype<Jkr::Renderer::CustomPainterImage>("CustomPainterImage",
                                                                   sol::call_constructor,
                                                                   sol::factories([](Jkr::Instance& inInstance, Jkr::Window& inWindow, int inWidth, int inHeight) {
                                                                                  return mu<Jkr::Renderer::CustomPainterImage>(inInstance, inWindow, inWidth, inHeight);
                                                                   }),
                                                                   "GetImageToVector",
                                                                   &Jkr::Renderer::CustomPainterImage::GetImageToVector);

               Jkr.new_usertype<DefaultCustomImagePainterPushConstant>("DefaultCustomImagePainterPushConstant",
                                                                       sol::call_constructor,
                                                                       sol::factories([]() { return DefaultCustomImagePainterPushConstant(); }),
                                                                       "x",
                                                                       &DefaultCustomImagePainterPushConstant::x,
                                                                       "y",
                                                                       &DefaultCustomImagePainterPushConstant::y,
                                                                       "z",
                                                                       &DefaultCustomImagePainterPushConstant::z);

               Jkr.new_usertype<Jkr::Renderer::CustomImagePainter>(
                         "CustomImagePainter",
                         sol::call_constructor,
                         sol::factories([](sv inCacheFileName, sv inComputeShader) { return mu<Jkr::Renderer::CustomImagePainter>(inCacheFileName, inComputeShader); }),
                         "Load",
                         &Jkr::Renderer::CustomImagePainter::Load,
                         "Store",
                         &Jkr::Renderer::CustomImagePainter::Store,
                         "Bind",
                         &Jkr::Renderer::CustomImagePainter::Bind,
                         "BindImageFromImage",
                         &Jkr::Renderer::CustomImagePainter::BindImageFromImage,
                         "Draw",
                         &Jkr::Renderer::CustomImagePainter::Draw<DefaultCustomImagePainterPushConstant>);

               using Uniform3D = Jkr::Misc::_3D::Uniform3D;
               Jkr.new_usertype<Uniform3D>("Uniform3D",
                                           sol::call_constructor,
                                           sol::factories([](Jkr::Instance& inI, Jkr::Renderer::_3D::Simple3D& inSimple3D) { return mu<Uniform3D>(inI, inSimple3D); },
                                                          [](Jkr::Instance& inInstance) { return mu<Uniform3D>(inInstance); },
                                                          &Uniform3D::CreateByGLTFNodeIndex),
                                           "AddTexture",
                                           &Uniform3D::AddTexture,
                                           "Bind",
                                           &Uniform3D::Bind,
                                           "AddBuffer",
                                           &Uniform3D::AddUniformBuffer);
}

} // namespace JkrEXE