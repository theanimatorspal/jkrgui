#include "JkrLuaExe.hpp"
#include "Misc/ThreeD/World3D.hpp"
#include "PainterParameter.hpp"
#include "Renderers/TwoD/Shape.hpp"
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
                                                                   &Jkr::Renderer::CustomPainterImage::GetImageToVector,
                                                                   "Register",
                                                                   &Jkr::Renderer::CustomPainterImage::Register);

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
                                           &Uniform3D::AddUniformBuffer,
                                           "UpdateByGLTFAnimation",
                                           &Uniform3D::UpdateByGLTFAnimation);

               using namespace Jkr::Misc::_3D;
               Jkr.new_usertype<Camera3D>("Camera3D",
                                          sol::call_constructor,
                                          sol::factories([]() { return mu<Camera3D>(); }),
                                          "SetAttributes",
                                          &Camera3D::SetAttributes,
                                          "MoveForward",
                                          &Camera3D::MoveForward,
                                          "MoveBackward",
                                          &Camera3D::MoveBackward,
                                          "MoveLeft",
                                          &Camera3D::MoveLeft,
                                          "MoveRight",
                                          &Camera3D::MoveRight,
                                          "SetPerspective",
                                          sol::overload(sol::resolve<void(float, float, float, float)>(&Camera3D::SetPerspective), sol::resolve<void(void)>(&Camera3D::SetPerspective)),
                                          "GetMatrix",
                                          &Camera3D::GetMatrix);

               Jkr.new_usertype<World3D>("World3D",
                                         sol::call_constructor,
                                         sol::factories(&World3D::CreateWorld3D),
                                         "BuildBasic",
                                         &World3D::BuildBasic,
                                         "BuildDemo",
                                         &World3D::BuildDemo,
                                         "AddCamera",
                                         &World3D::AddCamera,
                                         "AddGLTFModel",
                                         &World3D::AddGLTFModel,
                                         "AddObject",
                                         &World3D::AddObject,
                                         "DrawBackgrounds",
                                         &World3D::DrawBackgrounds,
                                         "AddSimple3D",
                                         &World3D::AddSimple3D,
                                         "DrawObjects",
                                         &World3D::DrawObjects,
                                         "Event",
                                         &World3D::Event,
                                         "Update",
                                         &World3D::Update,
                                         "GetCamera",
                                         &World3D::GetCamera,
                                         "GetCurrentCamera",
                                         &World3D::GetCurrentCamera,
                                         "GetGLTFModel",
                                         &World3D::GetGLTFModel,
                                         "GetUniform",
                                         &World3D::GetUniform,
                                         "GetSimple3D",
                                         &World3D::GetSimple3D,
                                         "SetCurrentCamera",
                                         &World3D::SetCurrentCamera);
}

} // namespace JkrEXE