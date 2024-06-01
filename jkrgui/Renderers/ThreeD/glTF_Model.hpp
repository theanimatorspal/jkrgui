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

class glTF_Model {
    public:
    // TODO Make this consistent, Change BoundingBox::min  to mMin
    struct BoundingBox {
        glm::vec3 min;
        glm::vec3 max;
        bool valid = false;
        BoundingBox();
        BoundingBox(glm::vec3 min, glm::vec3 max);
        BoundingBox GetAABB(glm::mat4 m);
        bool IsPointInside(glm::vec3 inVec);
    };

    struct Texture {
        uint32_t mWidth, mHeight;
        uint32_t mMipLevels;
        uint32_t mLayerCount;
        int32_t mImageIndex;
    };

    struct TextureCoordinateSets {
        uint8_t mBaseColor          = 0;
        uint8_t mMetallicRoughness  = 0;
        uint8_t mSpecularGlossiness = 0;
        uint8_t mNormal             = 0;
        uint8_t mOcclusion          = 0;
        uint8_t mEmissive           = 0;
    };

    struct Extension {
        int32_t mSpecularGlossinessTextureIndex = -1;
        int32_t mDiffuseTextureIndex            = -1;
        glm::vec4 mDiffuseFactor                = glm::vec4(1.0f);
        glm::vec3 mSpecularFactor               = glm::vec3(0.0f);
    };

    struct PbrWorkflows {
        bool mMetallicRoughness  = true;
        bool mSpecularGlossiness = false;
    };

    struct Material {
        enum class AlphaMode { Opaque, Mask, Blend };
        AlphaMode mAlphaMode                   = AlphaMode::Opaque;
        float mAlphaCutOff                     = -1.0f;
        float mMetallicFactor                  = 1.0f;
        float mRoughnessFactor                 = 1.0f;
        glm::vec4 mBaseColorFactor             = glm::vec4(1.0f);
        glm::vec4 mEmissiveFactor              = glm::vec4(1.0f);
        int32_t mBaseColorTextureIndex         = -1;
        int32_t mMetallicRoughnessTextureIndex = -1;
        int32_t mNormalTextureIndex            = -1;
        int32_t mOcclusionTextureIndex         = -1;
        int32_t mEmissiveTextureIndex          = -1;
        bool mDoubleSided                      = false;
        TextureCoordinateSets mTextureCoordinateSets;
        Extension mExtension;
        PbrWorkflows mPbrWorkflows;
        uint32_t mIndex        = 0;
        bool unlit             = false;
        float emissiveStrength = 1.0f;
    };

    struct Primitive {
        ui mFirstIndex;
        ui mIndexCount;
        ui mVertexCount;
        int32_t mMaterialIndex = -1;
        bool mHasIndices;
        BoundingBox mBB;
    };

    struct Mesh {
        v<Primitive> mPrimitives;
        v<ui> mNodeIndex;
        BoundingBox mBB;
        BoundingBox mAABB;
    };

    struct Node {
        glm::mat4 GetLocalMatrix();

        Node* mParent;
        uint32_t mIndex;
        v<Up<Node>> mChildren;

        Mesh mMesh;
        glm::vec3 mTranslation{};
        glm::vec3 mScale{1.0f};
        glm::quat mRotation{};

        int mSkin = -1;
        glm::mat4 mMatrix;
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
        float mEnd         = std::numeric_limits<float>::min();
        float mCurrentTime = 0.0f;
    };

    using FillVertexCallBack   = std::function<Vertex3D(Vertex3DExt)>;
    using FillIndexCallBack    = std::function<ui(ui)>;
    using PushCallBack         = std::function<void(glm::mat4)>;
    using DrawCallBack         = std::function<void(ui, opt<Texture>)>;
    using UpdateJointsCallBack = std::function<void(v<glm::mat4>&)>;

    Node* FindNode(Node* inParent, ui inIndex);
    Node* NodeFromIndex(ui inIndex);
    GETTER GetFileName() const { return sv(mFileName); }
    GETTER& GetVertices() const { return mVertexBuffer; }
    GETTER& GetIndices() const { return mIndexBuffer; }
    GETTER& GetVerticesExtRef() { return mVertexBufferExt; }
    GETTER& GetVerticesRef() { return mVertexBuffer; }
    GETTER& GetIndicesRef() { return mIndexBuffer; }
    GETTER& GetImagesRef() { return mImages; }
    GETTER& GetTexturesRef() { return mTextures; }
    GETTER& GetMaterialsRef() { return mMaterials; }
    GETTER& GetNodesRef() { return mNodes; }
    GETTER& GetSkinsRef() { return mSkins; }
    GETTER& GetAnimationsRef() { return mAnimations; }
    GETTER& GetMeshesRef() { return mMeshes; }
    GETTER& GetAnimationById(int inId) { return mAnimations[inId]; }
    GETTER GetVerticesSize() { return mVertexBuffer.size(); }
    GETTER GetIndicesSize() { return mIndexBuffer.size(); }
    GETTER GetImagesSize() { return mImages.size(); }
    GETTER GetTexturesSize() { return mTextures.size(); }
    GETTER GetMaterialsSize() { return mMaterials.size(); }
    GETTER GetNodesSize() { return mNodes.size(); }
    GETTER GetSkinsSize() { return mSkins.size(); }
    GETTER GetMeshesSize() { return mMeshes.size(); }
    GETTER GetJointsCount(ui inIndex) { return mSkins[inIndex].mInverseBindMatrices.size(); }
    GETTER GetAnimationsSize() { return mAnimations.size(); }
    GETTER GetNodeIndexByMeshIndex(int inIndex) const { return mMeshes[inIndex].mNodeIndex; }
    glm::mat4 GetNodeMatrix(glTF_Model::Node* inNode);
    glm::mat4 GetNodeMatrixByIndex(int inIndex);

    void Load(ui inInitialVertexOffset = 0);
    void Load(FillVertexCallBack inVertexCallback, FillIndexCallBack inIndexCallback);
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
              [](Vertex3DExt inVertex) {
                  return Vertex3D{.mPosition = inVertex.mPosition,
                                  .mNormal   = inVertex.mNormal,
                                  .mUV       = inVertex.mUV,
                                  .mColor    = inVertex.mColor};
              },
         FillIndexCallBack inIndexCallback = [](ui inIndex) { return inIndex; });
    void LoadSkins(tinygltf::Model& input);
    void LoadAnimations(tinygltf::Model& input);
    void
    UpdateJoints(glTF_Model::Node* inNode, UpdateJointsCallBack inCallBack = [](v<glm::mat4>&) {});
    void UpdateAnimation(ui inActiveAnimation, float inDeltaTime, bool inShouldLoop = true);
    void BlendCombineAnimationToArbritaryTime(float inDestinationTime,
                                              ui inDestinationAnimationIndex,
                                              float inBlendFactor,
                                              bool inShouldLoop = true);

    void UpdateBlendCombineAnimation(float inDelTime,
                                     ui inDestinationAnimationIndex,
                                     float inBlendFactor,
                                     bool inShouldLoop               = true,
                                     bool inShouldResetByBlendFactor = false);

    void UpdateAllJoints(UpdateJointsCallBack inCallBack);
    void EraseVerticesAndIndices() {
        mIndexBuffer.clear();
        mIndexBuffer.shrink_to_fit();
        mVertexBuffer.clear();
        mVertexBuffer.shrink_to_fit();
    }
    void
    Draw(glTF_Model::Node& inNode, PushCallBack inBindDataCallBack, DrawCallBack inDrawCallBack);

    glTF_Model(const std::string_view inFileName) : mFileName(inFileName) {}

    private:
    std::mutex mUpdateMutex;
    const std::string mFileName;
    v<ui> mIndexBuffer;
    v<Vertex3D> mVertexBuffer;
    v<Vertex3DExt> mVertexBufferExt;
    v<Image> mImages;
    v<Texture> mTextures;
    v<Material> mMaterials;
    v<Up<Node>> mNodes;
    v<Skin> mSkins;
    v<Animation> mAnimations;

    v<Mesh> mMeshes;
};

} // namespace Jkr::Renderer::_3D
