#include "JkrLuaExe.hpp"
#include "Renderers/ThreeD/World3D.hpp"
#include "Renderers/ThreeD/Simple3D.hpp"
#include "Renderers/TwoD/Shape.hpp"
#include <Misc/RecycleBin.hpp>

namespace JkrEXE {
extern void CreateMainBindings(sol::state& s);

void CreateMiscBindings(sol::state& inState) {
    auto Jkr = inState["Jkr"].get_or_create<sol::table>();

    Jkr.new_usertype<Jkr::Misc::RecycleBin<int>>(
         "RecycleBin",
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

    Jkr.new_usertype<Jkr::Renderer::CustomPainterImage>(
         "CustomPainterImage",
         sol::call_constructor,
         sol::factories(
              [](Jkr::Instance& inInstance, Jkr::Window_base& inWindow, int inWidth, int inHeight) {
                  return mu<Jkr::Renderer::CustomPainterImage>(
                       inInstance, inWindow, inWidth, inHeight);
              }),
         "GetImageToVector",
         &Jkr::Renderer::CustomPainterImage::GetImageToVector,
         "Register",
         &Jkr::Renderer::CustomPainterImage::Register);

    Jkr.new_usertype<DefaultCustomImagePainterPushConstant>(
         "DefaultCustomImagePainterPushConstant",
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
         sol::factories([](sv inCacheFileName, sv inComputeShader) {
             return mu<Jkr::Renderer::CustomImagePainter>(inCacheFileName, inComputeShader);
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
         &Jkr::Renderer::CustomImagePainter::Draw<DefaultCustomImagePainterPushConstant>);

    using Uniform3D = Jkr::Renderer::_3D::Uniform3D;
    using Simple3D  = Jkr::Renderer::_3D::Simple3D;
    using namespace Jkr::Renderer::_3D;
    Jkr.new_usertype<Uniform3D>(
         "Uniform3D",
         sol::call_constructor,
         sol::factories(
              [](Jkr::Instance& inI, Jkr::Renderer::_3D::Simple3D& inSimple3D) {
                  return mu<Uniform3D>(inI, inSimple3D);
              },
              [](Jkr::Instance& inInstance) { return mu<Uniform3D>(inInstance); },
              &Uniform3D::CreateByGLTFNodeIndex),
         "AddTexture",
         &Uniform3D::AddTexture,
         "Bind",
         &Uniform3D::Bind,
         "AddBuffer",
         &Uniform3D::AddUniformBuffer,
         "UpdateByGLTFAnimation",
         &Uniform3D::UpdateByGLTFAnimation,
         "Build",
         sol::overload(
              sol::resolve<void(Jkr::Renderer::_3D::Simple3D&)>(&Uniform3D::Build),
              [](Uniform3D& inUniform3D,
                 Simple3D& inS,
                 glTF_Model& inModel,
                 ui inNodeIndex,
                 bool inShouldskin,
                 bool inShouldTexture) {
                  inUniform3D.Build(inS, inModel, inNodeIndex, inShouldskin, inShouldTexture);
              },
              sol::resolve<void(Simple3D&, glTF_Model&, ui, bool, bool, bool)>(&Uniform3D::Build),
              sol::resolve<void(Simple3D&, glTF_Model&, glTF_Model::Primitive&)>(
                   &Uniform3D::Build)),
         "BuildByMaterial",
         &Uniform3D::BuildByMaterial,
         "AddTextureToUniform3D",
         &Uniform3D::AddTextureToUniform3D,
         "AddUniformBufferToUniform3D",
         &Uniform3D::AddUniformBufferToUniform3D,
         "AddStorageBufferToUniform3D",
         &Uniform3D::AddStorageBufferToUniform3D,
         "AddBindingsToUniform3DGLTF",
         &Uniform3D::AddBindingsToUniform3DGLTF,
         "AddTextureFromShapeImage",
         &Uniform3D::AddTextureFromShapeImage,
         "AddGenerateBRDFLookupTable",
         &Uniform3D::AddGenerateBRDFLookupTable,
         "AddGenerateIrradianceCube",
         &Uniform3D::AddGenerateIrradianceCube,
         "AddGeneratePrefilteredCube",
         &Uniform3D::AddGeneratePrefilteredCube);

    using namespace Jkr::Renderer::_3D;
    Jkr.new_usertype<Camera3D>(
         "Camera3D",
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
         sol::overload(sol::resolve<void(float, float, float, float)>(&Camera3D::SetPerspective),
                       sol::resolve<void(void)>(&Camera3D::SetPerspective)),
         "GetMatrix",
         &Camera3D::GetMatrix);

    Jkr.new_usertype<World3D::Object3D>(
         "Object3D",
         sol::call_constructor,
         sol::constructors<World3D::Object3D(), World3D::Object3D(World3D::Object3D)>(),

         "mId",
         &World3D::Object3D::mId,
         "mAssociatedModel",
         &World3D::Object3D::mAssociatedModel,
         "mAssociatedUniform",
         &World3D::Object3D::mAssociatedUniform,
         "mAssociatedSimple3D",
         &World3D::Object3D::mAssociatedSimple3D,
         "mIndexCount",
         &World3D::Object3D::mIndexCount,
         "mFirstIndex",
         &World3D::Object3D::mFirstIndex,
         "mTranslation",
         &World3D::Object3D::mTranslation,
         "mScale",
         &World3D::Object3D::mScale,
         "mRotation",
         &World3D::Object3D::mRotation,
         "mMatrix",
         &World3D::Object3D::mMatrix,
         "mColor",
         &World3D::Object3D::mColor,

         "mMatrix2",
         &World3D::Object3D::mMatrix2,

         "AppyTransforms",
         &World3D::Object3D::ApplyTransforms,

         "mBoundingBox",
         &World3D::Object3D::mBoundingBox,
         "IsCollidingWith",
         &World3D::Object3D::IsCollidingWith,
         "GetCollisionThreashold",
         &World3D::Object3D::GetCollisionThreashold,
         "GetOverlap",
         &World3D::Object3D::GetOverlap,
         "GetContactPoint",
         &World3D::Object3D::GetContactPoint,
         "GetContactNormal",
         &World3D::Object3D::GetContactNormal,

         "mMass",
         &World3D::Object3D::mMass,
         "mMomentum",
         &World3D::Object3D::mMomentum,
         "mIBody",
         &World3D::Object3D::mIBody,
         "mVelocity",
         &World3D::Object3D::mVelocity,
         "mAngularVelocity",
         &World3D::Object3D::mAngularVelocity,
         "mAngularMomentum",
         &World3D::Object3D::mAngularMomentum,
         "mForce",
         &World3D::Object3D::mForce,
         "mTorque",
         &World3D::Object3D::mTorque,

         "SetParent",
         &World3D::Object3D::SetParent,
         "GetLocalMatrix",
         &World3D::Object3D::GetLocalMatrix,
         "GetMatrix",
         &World3D::Object3D::GetMatrix);

    Jkr.new_usertype<World3D>("World3D",
                              sol::call_constructor,
                              sol::factories(&World3D::CreateWorld3D),
                              "BuildBasic",
                              &World3D::BuildBasic,
                              "AddCamera",
                              &World3D::AddCamera,
                              "AddGLTFModel",
                              &World3D::AddGLTFModel,
                              "AddUniform3D",
                              &World3D::AddUniform3D,
                              "AddSimple3D",
                              &World3D::AddSimple3D,
                              "AddLight3D",
                              &World3D::AddLight3D,
                              "AddWorldInfoToUniform3D",
                              &World3D::AddWorldInfoToUniform3D,
                              "AddShadowMapToUniform3D",
                              &World3D::AddShadowMapToUniform3D,
                              "DrawObjectsExplicit",
                              &World3D::DrawObjectsExplicit,
                              "Event",
                              &World3D::Event,
                              "Update",
                              &World3D::Update,
                              "GetCamera3D",
                              &World3D::GetCamera3D,
                              "GetCurrentCamera",
                              &World3D::GetCurrentCamera,
                              "GetGLTFModel",
                              &World3D::GetGLTFModel,
                              "GetUniform3D",
                              &World3D::GetUniform3D,
                              "GetSimple3D",
                              &World3D::GetSimple3D,
                              "GetSkyboxImageBase",
                              &World3D::GetSkyboxImageBase,

                              "MakeExplicitObjectsVector",
                              &World3D::MakeExplicitObjectsVector,
                              "SetCurrentCamera",
                              &World3D::SetCurrentCamera,

                              "AddSkyboxToUniform3D",
                              &World3D::AddSkyboxToUniform3D);
}

} // namespace JkrEXE