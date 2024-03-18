#include "Shape.hpp"
#include <Misc/RecycleBin.hpp>
#include <sol/sol.hpp>

namespace JkrEXE {
using namespace ksai;
void CreateMiscBindings(sol::state& inState) {
          auto Jkr = inState["Jkr"].get_or_create<sol::table>();
          Jkr.new_usertype<Jkr::Misc::Recycle<int>>("RecycleBin",
                                                    sol::call_constructor,
                                                    sol::factories([]() { return Jkr::Misc::Recycle<int>(); }),
                                                    "Add",
                                                    &Jkr::Misc::Recycle<int>::Add,
                                                    "Remove",
                                                    &Jkr::Misc::Recycle<int>::Remove,
                                                    "Exists",
                                                    &Jkr::Misc::Recycle<int>::Exists,
                                                    "IsEmpty",
                                                    &Jkr::Misc::Recycle<int>::IsEmpty,
                                                    "Get",
                                                    &Jkr::Misc::Recycle<int>::Get);

          Jkr.new_usertype<Jkr::Renderer::CustomPainterImage>(
           "CustomPainterImage",
           sol::call_constructor,
           sol::factories([](Jkr::Instance& inInstance, Jkr::Window& inWindow, int inWidth, int inHeight) {
                     return mu<Jkr::Renderer::CustomPainterImage>(inInstance, inWindow, inWidth, inHeight);
           }),
           "GetImageToVector",
           &Jkr::Renderer::CustomPainterImage::GetImageToVector);

          struct DefaultCustomImagePushConstant {
                    glm::vec4 x, y, z;
          };

          // Jkr.new_usertype<DefaultCustomImagePushConstant>(
          //  "DefaultCustomImagePushConstant", "x", &DefaultCustomImagePushConstant::x, "y",
          //  &DefaultCustomImagePushConstant::y, "z", &DefaultCustomImagePushConstant::z);

          Jkr.new_usertype<Jkr::Renderer::CustomImagePainter>("CustomImagePainter",
                                                              sol::call_constructor,
                                                              sol::factories([](sv inCacheFileName, sv inComputeShader) {
                                                                        return mu<Jkr::Renderer::CustomImagePainter>(inCacheFileName,
                                                                                                                     inComputeShader);
                                                              }),
                                                              "Load",
                                                              &Jkr::Renderer::CustomImagePainter::Load,
                                                              "Store",
                                                              &Jkr::Renderer::CustomImagePainter::Store,
                                                              "Bind",
                                                              &Jkr::Renderer::CustomImagePainter::Bind,
                                                              "BindImageFromImage",
                                                              &Jkr::Renderer::CustomImagePainter::BindImageFromImage,
                                                              "Draw",
                                                              &Jkr::Renderer::CustomImagePainter::Draw<DefaultCustomImagePushConstant>);
}
} // namespace JkrEXE