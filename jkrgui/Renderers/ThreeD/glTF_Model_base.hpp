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

namespace Jkr::Renderer::glTF {

class glTF_Model_base
{
    using ImageType = Jkr::PainterParameter<PainterParameterContext::UniformImage>;

private:
    const Instance &mInstance;
    std::vector<uint32_t> mIndexBuffer;
    std::vector<Vertex3D> mVertexBuffer;

public:
    glTF_Model_base(const Instance &inInstance, const std::string_view inFilePath)
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
            // TODO: this->LoadImages()
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

    struct Image
    {
        Up<ImageType> mTextureImage;
    };
    
    std::vector<Image> mImages;
    std::vector<Texture> mTextures;
    std::vector<Material> mMaterials;
    std::vector<Up<Node>> mNodes;

    void LoadImages(tinygltf::Model &input);
    void LoadTextures(tinygltf::Model &input);
    void LoadMaterials(tinygltf::Model &input);
    void LoadNode(const tinygltf::Node &inputNode,
                  const tinygltf::Model &input,
                  glTF_Model_base::Node *inParent,
                  std::vector<uint32_t> &indexBuffer,
                  std::vector<Vertex3D> &vertexBuffer);
    void DrawNode(VulkanCommandBuffer &inCommandBuffer,
                  VulkanPipelineLayoutBase &inPipelineLayout,
                  glTF_Model_base::Node *inNode);
};
} // namespace Jkr::Renderer::glTF
