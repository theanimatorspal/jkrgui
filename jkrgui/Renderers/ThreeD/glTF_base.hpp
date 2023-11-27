#pragma once
#include <Vendor/TinyGLTF/tiny_gltf.hpp>
#include <glm/glm.hpp>
#include "PainterParameter.hpp"

using namespace ksai::kstd;
namespace tg = tinygltf;

namespace Jkr::Renderer::glTF {

class glTF_base {
    using ImageType = Jkr::PainterParameter<PainterParameterContext::UniformImage>;

public:
    glTF_base(const Instance &inInstance)
        : mInstance(inInstance)
    {}

private:
    const Instance &mInstance;

private:
    struct Primitive {
        uint32_t mFirstIndex;
        uint32_t mIndexCount;
        int32_t mMaterialIndex;
    };

    struct Mesh {
        std::vector<Primitive> mPrimitives;
    };

    struct Node {
        Node* mParent;
        std::vector<Node*> mChildren;
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
    std::vector<Node*> mNodes;

    void LoadImages(tg::Model &input)
    {
        // Images can be stored inside the glTF (which is the case for the sample model), so instead of directly
        // loading them from disk, we fetch them from the glTF loader and upload the buffers
        mImages.resize(input.images.size());
        for (size_t i = 0; i < input.images.size(); i++) {
            tinygltf::Image &glTFImage = input.images[i];
            // Get the image data from the glTF loader
            unsigned char *buffer = nullptr;
            VkDeviceSize bufferSize = 0;
            bool deleteBuffer = false;
            // We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
            if (glTFImage.component == 3) {
                bufferSize = glTFImage.width * glTFImage.height * 4;
                buffer = new unsigned char[bufferSize];
                unsigned char *rgba = buffer;
                unsigned char *rgb = &glTFImage.image[0];
                for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i) {
                    memcpy(rgba, rgb, sizeof(unsigned char) * 3);
                    rgba += 4;
                    rgb += 3;
                }
                deleteBuffer = true;
            } else {
                buffer = &glTFImage.image[0];
                bufferSize = glTFImage.image.size();
            }
            // Load texture from image buffer
            Image an_image;
            an_image.mTextureImage = MakeUp<ImageType>(mInstance);
            an_image.mTextureImage->Setup(reinterpret_cast<void **>(&buffer),
                                          glTFImage.width,
                                          glTFImage.height,
                                          4);
            mImages.push_back(std::move(an_image));
            if (deleteBuffer) {
                delete[] buffer;
            }
        }
    }

    void loadTextures(tinygltf::Model &input)
    {
        mTextures.resize(input.textures.size());
        for (size_t i = 0; i < input.textures.size(); i++) {
            mTextures[i].mImageIndex = input.textures[i].source;
        }
    }

    void loadMaterials(tinygltf::Model &input)
    {
        mMaterials.resize(input.materials.size());
        for (size_t i = 0; i < input.materials.size(); i++) {
            // We only read the most basic properties required for our sample
            tinygltf::Material glTFMaterial = input.materials[i];
            // Get the base color factor
            if (glTFMaterial.values.find("baseColorFactor") != glTFMaterial.values.end()) {
                mMaterials[i].mBaseColorFactor = glm::make_vec4(
                    glTFMaterial.values["baseColorFactor"].ColorFactor().data());
            }
            // Get base color texture index
            if (glTFMaterial.values.find("baseColorTexture") != glTFMaterial.values.end()) {
                mMaterials[i].mBaseColorTextureIndex = glTFMaterial.values["baseColorTexture"]
                                                           .TextureIndex();
            }
        }
    }

    void loadNode(const tinygltf::Node &inputNode,
                  const tinygltf::Model &input,
                  glTF_base::Node *parent,
                  std::vector<uint32_t> &indexBuffer,
                  std::vector<Vertex3D> &vertexBuffer)
    {}
};
} // namespace Jkr::Renderer::glTF
