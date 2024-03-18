#pragma once
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
          v<ui> mIndexBuffer;
          v<Vertex3D> mVertexBuffer;

          void EraseVerticesAndIndices() {
                    mIndexBuffer.clear();
                    mIndexBuffer.shrink_to_fit();
                    mVertexBuffer.clear();
                    mVertexBuffer.shrink_to_fit();
          }

      public:
          glTF_Model(const sv inFilePath) {
                    tinygltf::Model glTFInput;
                    tinygltf::TinyGLTF gltfContext;
                    s error, warning;
                    bool file_loaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, s(inFilePath));
                    if (file_loaded) {
                              this->LoadImages(glTFInput);
                              this->LoadMaterials(glTFInput);
                              this->LoadTextures(glTFInput);
                              const tinygltf::Scene& scene = glTFInput.scenes[0];
                              for (size_t i = 0; i < scene.nodes.size(); i++) {
                                        const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
                                        this->LoadNode(node, glTFInput, nullptr, mIndexBuffer, mVertexBuffer);
                              }
                    }
          }

          GETTER& GetVerticesRef() { return mVertexBuffer; }
          GETTER& GetIndicesRef() { return mIndexBuffer; }
          GETTER& GetVertices() const { return mVertexBuffer; }
          GETTER& GetIndices() const { return mIndexBuffer; }
          GETTER& GetPainterImageParameters() const { return mImages; }

      protected:
          struct Primitive {
                    ui mFirstIndex;
                    ui mIndexCount;
                    int32_t mMaterialIndex;
          };

          struct PushConstant {
                    glm::mat4 mModelMatrix;
          };

          struct Mesh {
                    v<Primitive> mPrimitives;
          };

          struct Node {
                    Node* mParent;
                    v<Up<Node>> mChildren;
                    Mesh mMesh;
                    glm::mat4 mMatrix;
          };

          struct Texture {
                    int32_t mImageIndex;
          };

          v<Texture> mTextures;
          v<Material> mMaterials;
          v<Up<Node>> mNodes;

          void LoadImages(tinygltf::Model& input);
          void LoadTextures(tinygltf::Model& input);
          void LoadMaterials(tinygltf::Model& input);
          void LoadNode(
           const tinygltf::Node& inputNode, const tinygltf::Model& input, glTF_Model::Node* inParent, v<ui>& indexBuffer, v<Vertex3D>& vertexBuffer);

      private:
          struct Image {
                    v<uc> mTextureImage;
          };
          v<Image> mImages;
};

} // namespace Jkr::Renderer::_3D
