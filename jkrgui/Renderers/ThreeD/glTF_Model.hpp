#pragma once
#include <functional>
#include <limits>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Global/Standards.hpp"
#include "TestUtils.hpp"
#include "glm/fwd.hpp"
#include "ksai_config.hpp"
#include <Vendor/TinyGLTF/tiny_gltf.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Jkr::Renderer::_3D {
using namespace ksai::kstd;
using namespace ksai;
struct Material {
               struct Prop {
                              float roughness;
                              float metalness;
                              glm::vec3 color;
               } props;

               Material() = default;
               Material(const sv inN, glm::vec3 inC, float inRoughness, float inMetalness) : mname(inN), props(Prop{.roughness = inRoughness, .metalness = inMetalness, .color = inC}) {}
               s mname;
               glm::vec4 mBaseColorFactor = glm::vec4(1.0f);
               ui mBaseColorTextureIndex;
};

class glTF_Model {
       public:
               struct Primitive {
                              ui mFirstIndex;
                              ui mIndexCount;
                              int32_t mMaterialIndex;
               };

               struct Mesh {
                              v<Primitive> mPrimitives;
               };

               struct Node {
                              glm::mat4 GetLocalMatrix();

                              Node* mParent;
                              uint32_t mIndex;
                              v<Up<Node>> mChildren;

                              Mesh mMesh;
                              glm::vec3 mTranslation;
                              glm::vec3 mScale{1.0f};
                              glm::quat mRotation{};

                              int mSkin = -1;
                              glm::mat4 mMatrix;
               };

               struct Texture {
                              int32_t mImageIndex;
               };

               struct Image {
                              v<uc> mTextureImage;
                              ui mWidth;
                              ui mHeight;
               };

               //==========================
               // SKIN
               //==========================
               struct Skin {
                              s mName;
                              Node* mSkeletonRoot = nullptr;
                              v<glm::mat4> mInverseBindMatrices;
                              v<Node*> mJoints;
               };
               //==========================
               // ANIMATION
               //==========================
               struct AnimationSampler {
                              s mInterpolation;
                              v<float> mInputs;
                              v<glm::vec4> mOutputsVec4;
               };
               struct AnimationChannel {
                              s mPath;
                              Node* mNode;
                              ui mSamplerIndex;
               };
               struct Animation {
                              s mName;
                              v<AnimationSampler> mSamplers;
                              v<AnimationChannel> mChannels;
                              float mStart       = std::numeric_limits<float>::max();
                              float mEnd         = std::numeric_limits<float>::max();
                              float mCurrentTime = 0.0f;
               };

               using FillVertexCallBack   = std::function<Vertex3D(Vertex3DExt)>;
               using FillIndexCallBack    = std::function<ui(ui)>;
               using PushCallBack         = std::function<void(glm::mat4)>;
               using DrawCallBack         = std::function<void(ui, opt<Texture>)>;
               using UpdateJointsCallBack = std::function<void(v<glm::mat4>&)>;

               GETTER GetFileName() const { return sv(mFileName); }
               GETTER& GetVertices() const { return mVertexBuffer; }
               GETTER& GetIndices() const { return mIndexBuffer; }
               GETTER& GetVerticesRef() { return mVertexBuffer; }
               GETTER& GetIndicesRef() { return mIndexBuffer; }
               GETTER& GetImagesRef() { return mImages; }
               GETTER& GetTexturesRef() { return mTextures; }
               GETTER& GetMaterialsRef() { return mMaterials; }
               GETTER& GetNodesRef() { return mNodes; }
               GETTER& GetSkinsRef() { return mSkins; }
               GETTER& GetAnimationsRef() { return mAnimations; }
               GETTER GetVerticesSize() { return mVertexBuffer.size(); }
               GETTER GetIndicesSize() { return mIndexBuffer.size(); }
               GETTER GetImagesSize() { return mImages.size(); }
               GETTER GetTexturesSize() { return mTextures.size(); }
               GETTER GetMaterialsSize() { return mMaterials.size(); }
               GETTER GetNodesSize() { return mNodes.size(); }
               GETTER GetSkinsSize() { return mSkins.size(); }
               GETTER GetJointsCount(ui inIndex) { return mSkins[inIndex].mInverseBindMatrices.size(); }
               GETTER GetAnimationsSize() { return mAnimations.size(); }
               GETTER GetActiveAnimation() const { return mActiveAnimation; }
               SETTER SetActiveAnimation(ui inAnimation) { mActiveAnimation = inAnimation; };

               void Load(ui inInitialVertexOffset = 0);
               void Load(FillVertexCallBack inVertexCallback, FillIndexCallBack inIndexCallback);
               void Draw(glTF_Model::Node& inNode, PushCallBack inBindDataCallBack, DrawCallBack inDrawCallBack);
               glm::mat4 GetNodeMatrix(glTF_Model::Node* inNode);
               void UpdateJoints(glTF_Model::Node* inNode, UpdateJointsCallBack inCallback = [](v<glm::mat4>&) {});
               void UpdateAnimation(float inDeltaTime, UpdateJointsCallBack inCallback);
               void Load(const sv inFilePath, ui inInitialVertexOffset = 0);
               void LoadImages(tinygltf::Model& input);
               void LoadTextures(tinygltf::Model& input);
               void LoadMaterials(tinygltf::Model& input);
               void LoadNode(
                         const tinygltf::Node& inputNode,
                         const tinygltf::Model& input,
                         glTF_Model::Node* inParent,
                         ui inNodeIndex,
                         v<ui>& indexBuffer,
                         v<Vertex3D>& vertexBuffer,
                         FillVertexCallBack inVertexCallback =
                                   [](Vertex3DExt inVertex) { return Vertex3D{.mPosition = inVertex.mPosition, .mNormal = inVertex.mNormal, .mUV = inVertex.mUV, .mColor = inVertex.mColor}; },
                         FillIndexCallBack inIndexCallback = [](ui inIndex) { return inIndex; });
               void LoadSkins(tinygltf::Model& input);
               void LoadAnimations(tinygltf::Model& input);
               void EraseVerticesAndIndices() {
                              mIndexBuffer.clear();
                              mIndexBuffer.shrink_to_fit();
                              mVertexBuffer.clear();
                              mVertexBuffer.shrink_to_fit();
               }
               Node* FindNode(Node* inParent, ui inIndex);
               Node* NodeFromIndex(ui inIndex);

               glTF_Model(const std::string_view inFileName) : mFileName(inFileName) {}

       private:
               const std::string mFileName;
               v<ui> mIndexBuffer;
               v<Vertex3D> mVertexBuffer;
               v<Image> mImages;
               v<Texture> mTextures;
               v<Material> mMaterials;
               v<Up<Node>> mNodes;
               v<Skin> mSkins;
               v<Animation> mAnimations;
               ui mActiveAnimation = 0;

               tinygltf::Model glTFInput;
               tinygltf::TinyGLTF gltfContext;
};

} // namespace Jkr::Renderer::_3D
