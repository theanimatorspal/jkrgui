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
          Material() = default;
          Material(const sv inN, glm::vec3 inC, float inRoughness, float inMetalness)
              : mname(inN), props(Prop{.roughness = inRoughness, .metalness = inMetalness, .color = inC}) {}

          s mname;
          struct Prop {
                    float roughness;
                    float metalness;
                    glm::vec3 color;
          } props;
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
                    float mStart = std::numeric_limits<float>::max();
                    float mEnd = std::numeric_limits<float>::max();
                    float mCurrentTime = 0.0f;
          };

          using FillVertexCallBack = std::function<Vertex3D(Vertex3DExt)>;
          using FillIndexCallBack = std::function<ui(ui)>;

      public:
          GETTER& GetVerticesRef() { return mVertexBuffer; }
          GETTER& GetIndicesRef() { return mIndexBuffer; }
          GETTER& GetVertices() const { return mVertexBuffer; }
          GETTER& GetIndices() const { return mIndexBuffer; }
          GETTER& GetPainterImageParameters() const { return mImages; }
          void Load(ui inInitialVertexOffset = 0);

          glTF_Model(std::string_view inFileName) : mFileName(inFileName) {}

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

          glm::mat4 GetNodeMatrix(glTF_Model::Node* inNode);
          void UpdateJoints(glTF_Model::Node* inNode);
          void UpdateAnimation(float inDeltaTime);

          void Load(const sv inFilePath, ui inInitialVertexOffset = 0);
          void LoadImages(tinygltf::Model& input);
          void LoadTextures(tinygltf::Model& input);
          void LoadMaterials(tinygltf::Model& input);
          // TODO Remove This
          void LoadNode(
                    const tinygltf::Node& inputNode,
                    const tinygltf::Model& input,
                    glTF_Model::Node* inParent,
                    v<ui>& indexBuffer,
                    v<Vertex3D>& vertexBuffer,
                    FillVertexCallBack inVertexCallback =
                              [](Vertex3DExt inVertex) {
                                        return Vertex3D{.mPosition = inVertex.mPosition,
                                                        .mNormal = inVertex.mNormal,
                                                        .mUV = inVertex.mUV,
                                                        .mColor = inVertex.mColor};
                              },
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
};

} // namespace Jkr::Renderer::_3D
