#include <Renderers/ThreeD/World3D.hpp>
#include <Renderers/ThreeD/Simple3D.hpp>
#include "JkrLuaExe.hpp"
#include <Misc/JkrFile.hpp>

namespace JkrEXE {
using namespace std;
struct DefaultPushConstant3D {
    glm::mat4 m1;
    glm::mat4 m2;
};

struct DefaultPushConstant3DCompute {
    glm::vec4 v1, v2;
};

void CreateRenderer3DBindings(sol::state &s) {
    auto Jkr = s["Jkr"].get_or_create<sol::table>();
    using namespace Jkr::Renderer::_3D;
    Jkr.new_usertype<kstd::Vertex3D>("Vertex3D",
                                     sol::call_constructor,
                                     sol::default_constructor,
                                     "mPosition",
                                     &kstd::Vertex3D::mPosition,
                                     "mNormal",
                                     &kstd::Vertex3D::mNormal,
                                     "mUV",
                                     &kstd::Vertex3D::mUV,
                                     "mColor",
                                     &kstd::Vertex3D::mColor);
    Jkr.new_usertype<DefaultPushConstant3D>("DefaultPushConstant3D",
                                            sol::call_constructor,
                                            sol::default_constructor,
                                            "m1",
                                            &DefaultPushConstant3D::m1,
                                            "m2",
                                            &DefaultPushConstant3D::m2);
    Jkr.new_usertype<DefaultPushConstant3DCompute>("DefaultPushConstant3DCompute",
                                                   sol::call_constructor,
                                                   sol::default_constructor,
                                                   "v1",
                                                   &DefaultPushConstant3DCompute::v1,
                                                   "v2",
                                                   &DefaultPushConstant3DCompute::v2);
    Jkr.new_enum<false>("PipelineContext",
                        "Default",
                        PipelineContext::Default,
                        "DefaultWithGeometry",
                        PipelineContext::DefaultWithGeometry,
                        "Skybox",
                        PipelineContext::Skybox);
    Jkr.new_enum<false>("CompileContext",
                        "Default",
                        Simple3D::CompileContext::Default,
                        "ShadowPass",
                        Simple3D::CompileContext::ShadowPass,
                        "Deferred",
                        Simple3D::CompileContext::Deferred,
                        "DeferredComposition",
                        Simple3D::CompileContext::DeferredComposition);
    Jkr.new_usertype<Simple3D>("Simple3D",
                               sol::call_constructor,
                               sol::factories([](Jkr::Instance &inI, Jkr::Window_base &inW) {
                                   return mu<Simple3D>(inI, inW);
                               }),
                               "Compile",
                               &Simple3D::CompileDefault,
                               "CompileEXT",
                               &Simple3D::Compile,
                               "CompileForShadowOffscreen",
                               &Simple3D::CompileForShadowOffscreen,

                               "Draw",
                               &Simple3D::Draw<DefaultPushConstant3D>,
                               "Dispatch",
                               &Simple3D::Dispatch<DefaultPushConstant3DCompute>,
                               "Bind",
                               &Simple3D::Bind,
                               "SetPipelineContext",
                               &Simple3D::SetPipelineContext);

    // TODO Make this consistent, Change BoundingBox::min  to mMin
    Jkr.new_usertype<Jkr::Renderer::_3D::glTF_Model::BoundingBox>(
         "BoundingBox",
         sol::call_constructor,
         sol::default_constructor,
         "min",
         &Jkr::Renderer::_3D::glTF_Model::BoundingBox::min,
         "max",
         &Jkr::Renderer::_3D::glTF_Model::BoundingBox::max,
         "GetAABB",
         &Jkr::Renderer::_3D::glTF_Model::BoundingBox::GetAABB);

    Jkr.new_usertype<Jkr::Renderer::_3D::glTF_Model::Primitive>(
         "Primitive",
         sol::call_constructor,
         sol::default_constructor,
         "mFirstIndex",
         &Jkr::Renderer::_3D::glTF_Model::Primitive::mFirstIndex,
         "mIndexCount",
         &Jkr::Renderer::_3D::glTF_Model::Primitive::mIndexCount,
         "mVertexCount",
         &Jkr::Renderer::_3D::glTF_Model::Primitive::mVertexCount,
         "mMaterialIndex",
         &Jkr::Renderer::_3D::glTF_Model::Primitive::mMaterialIndex,
         "mHasIndices",
         &Jkr::Renderer::_3D::glTF_Model::Primitive::mHasIndices,
         "mBB",
         &Jkr::Renderer::_3D::glTF_Model::Primitive::mBB);

    Jkr.new_usertype<Jkr::Renderer::_3D::glTF_Model::Mesh>(
         "Mesh",
         sol::call_constructor,
         sol::default_constructor,
         "mPrimitives",
         &Jkr::Renderer::_3D::glTF_Model::Mesh::mPrimitives,
         "mNodeIndex",
         &Jkr::Renderer::_3D::glTF_Model::Mesh::mNodeIndex,
         "mBB",
         &Jkr::Renderer::_3D::glTF_Model::Mesh::mBB,
         "mAABB",
         &Jkr::Renderer::_3D::glTF_Model::Mesh::mAABB);

    Jkr.new_usertype<Jkr::Renderer::_3D::glTF_Model::Animation>(
         "Animation",
         sol::call_constructor,
         sol::default_constructor,
         "mStart",
         &Jkr::Renderer::_3D::glTF_Model::Animation::mStart,
         "mEnd",
         &Jkr::Renderer::_3D::glTF_Model::Animation::mEnd,
         "mCurrentTime",
         &Jkr::Renderer::_3D::glTF_Model::Animation::mCurrentTime);

    Jkr.new_usertype<Jkr::Renderer::_3D::glTF_Model::Node>(
         "GLTFNode",
         sol::call_constructor,
         sol::default_constructor,
         "GetLocalMatrix",
         &Jkr::Renderer::_3D::glTF_Model::Node::GetLocalMatrix,
         "mIndex",
         &Jkr::Renderer::_3D::glTF_Model::Node::mIndex,
         "mMesh",
         &Jkr::Renderer::_3D::glTF_Model::Node::mMesh,
         "mTranslation",
         &Jkr::Renderer::_3D::glTF_Model::Node::mTranslation,
         "mScale",
         &Jkr::Renderer::_3D::glTF_Model::Node::mScale,
         "mRotation",
         &Jkr::Renderer::_3D::glTF_Model::Node::mRotation,
         "mSkin",
         &Jkr::Renderer::_3D::glTF_Model::Node::mSkin,
         "mMatrix",
         &Jkr::Renderer::_3D::glTF_Model::Node::mMatrix,
         "mChildren",
         &Jkr::Renderer::_3D::glTF_Model::Node::mChildren);

    Jkr.new_enum<glTF_Model::Material::AlphaMode>(
         "AlphaMode",
         {{"Opaque", glTF_Model::Material::AlphaMode::Opaque},
          {"Mask", glTF_Model::Material::AlphaMode::Mask},
          {"Blend", glTF_Model::Material::AlphaMode::Blend}});

    // Bind TextureCoordinateSets struct
    Jkr.new_usertype<glTF_Model::TextureCoordinateSets>(
         "TextureCoordinateSets",
         "mBaseColor",
         &glTF_Model::TextureCoordinateSets::mBaseColor,
         "mMetallicRoughness",
         &glTF_Model::TextureCoordinateSets::mMetallicRoughness,
         "mSpecularGlossiness",
         &glTF_Model::TextureCoordinateSets::mSpecularGlossiness,
         "mNormal",
         &glTF_Model::TextureCoordinateSets::mNormal,
         "mOcclusion",
         &glTF_Model::TextureCoordinateSets::mOcclusion,
         "mEmissive",
         &glTF_Model::TextureCoordinateSets::mEmissive);

    // Bind Extension struct
    Jkr.new_usertype<glTF_Model::Extension>("Extension",
                                            "mSpecularGlossinessTextureIndex",
                                            &glTF_Model::Extension::mSpecularGlossinessTextureIndex,
                                            "mDiffuseTextureIndex",
                                            &glTF_Model::Extension::mDiffuseTextureIndex,
                                            "mDiffuseFactor",
                                            &glTF_Model::Extension::mDiffuseFactor,
                                            "mSpecularFactor",
                                            &glTF_Model::Extension::mSpecularFactor);

    // Bind PbrWorkflows struct
    Jkr.new_usertype<glTF_Model::PbrWorkflows>("PbrWorkflows",
                                               "mMetallicRoughness",
                                               &glTF_Model::PbrWorkflows::mMetallicRoughness,
                                               "mSpecularGlossiness",
                                               &glTF_Model::PbrWorkflows::mSpecularGlossiness);

    // Bind Material struct
    Jkr.new_usertype<glTF_Model::Material>("Material",
                                           sol::call_constructor,
                                           sol::default_constructor,
                                           "mAlphaMode",
                                           &glTF_Model::Material::mAlphaMode,
                                           "mAlphaCutOff",
                                           &glTF_Model::Material::mAlphaCutOff,
                                           "mMetallicFactor",
                                           &glTF_Model::Material::mMetallicFactor,
                                           "mRoughnessFactor",
                                           &glTF_Model::Material::mRoughnessFactor,
                                           "mBaseColorFactor",
                                           &glTF_Model::Material::mBaseColorFactor,
                                           "mEmissiveFactor",
                                           &glTF_Model::Material::mEmissiveFactor,
                                           "mBaseColorTextureIndex",
                                           &glTF_Model::Material::mBaseColorTextureIndex,
                                           "mMetallicRoughnessTextureIndex",
                                           &glTF_Model::Material::mMetallicRoughnessTextureIndex,
                                           "mNormalTextureIndex",
                                           &glTF_Model::Material::mNormalTextureIndex,
                                           "mOcclusionTextureIndex",
                                           &glTF_Model::Material::mOcclusionTextureIndex,
                                           "mEmissiveTextureIndex",
                                           &glTF_Model::Material::mEmissiveTextureIndex,
                                           "mDoubleSided",
                                           &glTF_Model::Material::mDoubleSided,
                                           "mTextureCoordinateSets",
                                           &glTF_Model::Material::mTextureCoordinateSets,
                                           "mExtension",
                                           &glTF_Model::Material::mExtension,
                                           "mPbrWorkflows",
                                           &glTF_Model::Material::mPbrWorkflows,
                                           "mIndex",
                                           &glTF_Model::Material::mIndex,
                                           "unlit",
                                           &glTF_Model::Material::unlit,
                                           "emissiveStrength",
                                           &glTF_Model::Material::emissiveStrength);

    Jkr.new_usertype<Jkr::Renderer::_3D::glTF_Model>(
         "glTF_Model",
         sol::call_constructor,
         sol::factories([](v<char>& inFileInMemory) { return mu<glTF_Model>(inFileInMemory); }),

         "IsNodeParentOfByIndex",
         &Jkr::Renderer::_3D::glTF_Model::IsNodeParentOfByIndex,
         "IsNodeParentOf",
         &Jkr::Renderer::_3D::glTF_Model::IsNodeParentOf,

         "GetMeshesSize",
         &Jkr::Renderer::_3D::glTF_Model::GetMeshesSize,
         "GetVerticesSize",
         &Jkr::Renderer::_3D::glTF_Model::GetVerticesSize,
         "GetIndicesSize",
         &Jkr::Renderer::_3D::glTF_Model::GetIndicesSize,
         "GetImagesSize",
         &Jkr::Renderer::_3D::glTF_Model::GetImagesSize,
         "GetTexturesSize",
         &Jkr::Renderer::_3D::glTF_Model::GetTexturesSize,
         "GetNodesSize",
         &Jkr::Renderer::_3D::glTF_Model::GetNodesSize,
         "GetMaterialsSize",
         &Jkr::Renderer::_3D::glTF_Model::GetMaterialsSize,
         "GetSkinsSize",
         &Jkr::Renderer::_3D::glTF_Model::GetSkinsSize,
         "GetAnimationsSize",
         &Jkr::Renderer::_3D::glTF_Model::GetAnimationsSize,
         "GetAnimationById",
         &Jkr::Renderer::_3D::glTF_Model::GetAnimationById,
         "GetJointsCount",
         &Jkr::Renderer::_3D::glTF_Model::GetJointsCount,
         "GetNodeMatrixByIndex",
         &Jkr::Renderer::_3D::glTF_Model::GetNodeMatrixByIndex,
         "UpdateAnimation",
         &Jkr::Renderer::_3D::glTF_Model::UpdateAnimation,
         "UpdateAnimationNormalizedTime",
         &Jkr::Renderer::_3D::glTF_Model::UpdateAnimationNormalizedTime,
         "UpdateBlendCombineAnimation",
         &Jkr::Renderer::_3D::glTF_Model::UpdateBlendCombineAnimation,
         "GetNodeIndexByMeshIndex",
         &Jkr::Renderer::_3D::glTF_Model::GetNodeIndexByMeshIndex,
         "GetMeshesRef",
         &Jkr::Renderer::_3D::glTF_Model::GetMeshesRef,
         "GetNodesRef",
         &Jkr::Renderer::_3D::glTF_Model::GetNodesRef,
         "GetMaterialsRef",
         &Jkr::Renderer::_3D::glTF_Model::GetMaterialsRef,
         "GetMaterials",
         &Jkr::Renderer::_3D::glTF_Model::GetMaterials);
    Jkr.new_usertype<Jkr::Renderer::_3D::Shape>(
         "Shape3D",
         sol::call_constructor,
         sol::factories([](Jkr::Instance &inInstance, Jkr::Window_base &inCompatibleWindow) {
             return mu<Jkr::Renderer::_3D::Shape>(inInstance, inCompatibleWindow);
         }),
         "Bind",
         &Jkr::Renderer::_3D::Shape::Bind,
         "Add",
         sol::overload(
              sol::resolve<glTF_Model &>(&Jkr::Renderer::_3D::Shape::AddEXT),
              sol::resolve<Jkr::Generator &, glm::vec3>(&Jkr::Renderer::_3D::Shape::AddEXT)),
         "Dispatch",
         &Jkr::Renderer::_3D::Shape::Dispatch,
         "GetVertexCount",
         &Jkr::Renderer::_3D::Shape::GetVertexCount,
         "GetIndexCount",
         &Jkr::Renderer::_3D::Shape::GetIndexCount,
         "GetVertexOffsetAbsolute",
         &Jkr::Renderer::_3D::Shape::GetVertexOffsetAbsolute,
         "GetIndexOffsetAbsolute",
         &Jkr::Renderer::_3D::Shape::GetIndexOffsetAbsolute,
         "CheckAndResize",
         sol::resolve<void(size_t, size_t)>(&Jkr::Renderer::_3D::Shape::CheckAndResize));
}
} // namespace JkrEXE