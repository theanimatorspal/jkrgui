#pragma once
#include "PainterParameter.hpp"
#include "Pipeline/VulkanPipelineLayout.hpp"
#include "VulkanQueue.hpp"
#include "glm/fwd.hpp"
#include <Vendor/TinyGLTF/tiny_gltf.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace ksai::kstd;
namespace tg = tinygltf;

namespace Jkr::Renderer::_3D {

class glTF_Model
{
    using ImageType = Jkr::PainterParameter<PainterParameterContext::UniformImage>;
    std::vector<uint32_t> mIndexBuffer;
    std::vector<Vertex3D> mVertexBuffer;

    void EraseVerticesAndIndices()
    {
        mIndexBuffer.clear();
        mIndexBuffer.shrink_to_fit();
        mVertexBuffer.clear();
        mVertexBuffer.shrink_to_fit();
    }

public:
    glTF_Model(const Instance &inInstance, const std::string_view inFilePath)
        : mInstance(inInstance)
    {
        tinygltf::Model glTFInput;
        tinygltf::TinyGLTF gltfContext;
        std::string error, warning;
        bool file_loaded = gltfContext.LoadASCIIFromFile(&glTFInput,
                                                         &error,
                                                         &warning,
                                                         std::string(inFilePath));
        if (file_loaded) {
            this->LoadImages(glTFInput);
            this->LoadMaterials(glTFInput);
            this->LoadTextures(glTFInput);
            const tinygltf::Scene &scene = glTFInput.scenes[0];
            for (size_t i = 0; i < scene.nodes.size(); i++) {
                const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
                this->LoadNode(node, glTFInput, nullptr, mIndexBuffer, mVertexBuffer);
            }
        }
    }

protected:
    struct Primitive {
        uint32_t mFirstIndex;
        uint32_t mIndexCount;
        int32_t mMaterialIndex;
    };

    struct PushConstant
    {
        glm::mat4 mModelMatrix;
    };

    struct Mesh {
        std::vector<Primitive> mPrimitives;
    };

    struct Node {
        Node* mParent;
        std::vector<Up<Node>> mChildren;
        Mesh mMesh;
        glm::mat4 mMatrix;
    };

    struct Material {
        glm::vec4 mBaseColorFactor = glm::vec4(1.0f);
        uint32_t mBaseColorTextureIndex;
    };

    struct Texture {
        int32_t mImageIndex;
    };

    std::vector<Texture> mTextures;
    std::vector<Material> mMaterials;
    std::vector<Up<Node>> mNodes;

    void LoadImages(tinygltf::Model &input);
    void LoadTextures(tinygltf::Model &input);
    void LoadMaterials(tinygltf::Model &input);
    void LoadNode(const tinygltf::Node &inputNode,
                  const tinygltf::Model &input,
                  glTF_Model::Node *inParent,
                  std::vector<uint32_t> &indexBuffer,
                  std::vector<Vertex3D> &vertexBuffer);
    void DrawNode(VulkanCommandBuffer &inCommandBuffer,
                  VulkanPipelineLayoutBase &inPipelineLayout,
                  glTF_Model::Node *inNode);

private:
    const Instance &mInstance;
    struct Image
    {
        Up<ImageType> mTextureImage;
    };
    std::vector<Image> mImages;
};
} // namespace Jkr::Renderer::_3D
