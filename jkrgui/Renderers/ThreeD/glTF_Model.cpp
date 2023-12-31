#include "glTF_Model.hpp"

using namespace Jkr::Renderer::_3D;

void glTF_Model::LoadImages(tinygltf::Model& input)
{
    // Images can be stored inside the glTF (which is the case for the sample model), so instead of directly
    // loading them from disk, we fetch them from the glTF loader and upload the buffers
    mImages.resize(input.images.size());
    for (size_t i = 0; i < input.images.size(); i++) {
        tinygltf::Image& glTFImage = input.images[i];
        // Get the image data from the glTF loader
        v<uc> buffvec;
        uc* buffer = nullptr;
        VkDeviceSize bufferSize = 0;
        // We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
        if (glTFImage.component == 3) {
            bufferSize = glTFImage.width * glTFImage.height * 4;
            buffvec.resize(bufferSize);
            buffer = buffvec.data();
            unsigned char* rgba = buffer;
            unsigned char* rgb = &glTFImage.image[0];
            for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i) {
                memcpy(rgba, rgb, sizeof(unsigned char) * 3);
                rgba += 4;
                rgb += 3;
            }
        } else {
            buffer = &glTFImage.image[0];
            // bufferSize = glTFImage.image.size();
        }
        // Load texture from image buffer
        Image an_image;
        an_image.mTextureImage = MakeUp<ImageType>(mInstance);
        an_image.mTextureImage->Setup(reinterpret_cast<void**>(&buffer),
            glTFImage.width,
            glTFImage.height,
            4);
        mImages.push_back(std::move(an_image));
    }
}

void glTF_Model::LoadTextures(tinygltf::Model& input)
{
    mTextures.resize(input.textures.size());
    for (size_t i = 0; i < input.textures.size(); i++) {
        mTextures[i].mImageIndex = input.textures[i].source;
    }
}

void glTF_Model::LoadMaterials(tinygltf::Model& input)
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

void glTF_Model::LoadNode(const tinygltf::Node& inputNode,
    const tinygltf::Model& input,
    Node* inParent,
    std::vector<uint32_t>& indexBuffer,
    std::vector<Vertex3D>& vertexBuffer)
{
    Up<glTF_Model::Node> Node = MakeUp<glTF_Model::Node>();
    Node->mMatrix = glm::mat4(1.0f);
    Node->mParent = inParent;

    /* Node Matrix is either made from translation, rotation, scale or a 4x4 matrix */
    if (inputNode.translation.size() == 3) {
        Node->mMatrix = glm::translate(Node->mMatrix,
            glm::vec3(glm::make_vec3(inputNode.translation.data())));
    }

    if (inputNode.rotation.size() == 4) {
        glm::quat q = glm::make_quat(inputNode.rotation.data());
        Node->mMatrix *= glm::mat4(q);
    }
    if (inputNode.scale.size() == 3) {
        Node->mMatrix = glm::scale(Node->mMatrix, glm::vec3(glm::make_vec3(inputNode.scale.data())));
    }
    if (inputNode.matrix.size() == 16) {
        Node->mMatrix = glm::make_mat4x4(inputNode.matrix.data());
    }

    /* Load node's children */
    if (inputNode.children.size() > 0) {
        for (size_t i = 0; i < inputNode.children.size(); i++) {
            LoadNode(input.nodes[inputNode.children[i]], input, Node.get(), indexBuffer, vertexBuffer);
        }
    }

    /* inGLTF accessing done by buffer */
    bool NodeContainsMesh = inputNode.mesh > -1;
    if (NodeContainsMesh) {
        const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
        for (size_t i = 0; i < mesh.primitives.size(); i++) {
            const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];
            uint32_t first_index = static_cast<uint32_t>(indexBuffer.size());
            uint32_t vertex_start = static_cast<uint32_t>(vertexBuffer.size());
            uint32_t index_count = 0;

            /* Vertices */
            {
                const float* positionBuffer = nullptr;
                const float* normalsBuffer = nullptr;
                const float* texCoordsBuffer = nullptr;
                size_t vertexCount = 0;

                /*Get Buffer data for vertex Position*/
                if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor& accessor
                        = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
                    const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                    positionBuffer = reinterpret_cast<const float*>(
                        &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    vertexCount = accessor.count;
                }

                /* Get Buffer Data From Vertex Normals */
                if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor& accessor
                        = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
                    const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                    normalsBuffer = reinterpret_cast<const float*>(
                        &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                }

                /*Get Buffer Data From TexCoords*/
                if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor& accessor
                        = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
                    const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                    texCoordsBuffer = reinterpret_cast<const float*>(
                        &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                }

                /* Append data to Model's VertexBuffer */
                for (size_t v = 0; v < vertexCount; v++) {
                    Vertex3D vert {};
                    vert.mPosition = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
                    vert.mNormal = glm::normalize(glm::vec3(
                        normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
                    vert.mUV = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2])
                                               : glm::vec2(0.0f);
                    vert.mColor = glm::vec3(1.0f);
                    vertexBuffer.push_back(vert);
                }
            }

            /* Indices */
            {
                const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.indices];
                const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];

                index_count += static_cast<uint32_t>(accessor.count);

                /* glTF supports different components of Indices */
                switch (accessor.componentType) {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                    const uint32_t* buf = reinterpret_cast<const uint32_t*>(
                        &buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                    for (size_t index = 0; index < accessor.count; index++) {
                        indexBuffer.push_back(buf[index] + vertex_start);
                    }
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                    const uint16_t* buf = reinterpret_cast<const uint16_t*>(
                        &buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                    for (size_t index = 0; index < accessor.count; index++) {
                        indexBuffer.push_back(buf[index] + vertex_start);
                    }
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                    const uint8_t* buf = reinterpret_cast<const uint8_t*>(
                        &buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                    for (size_t index = 0; index < accessor.count; index++) {
                        indexBuffer.push_back(buf[index] + vertex_start);
                    }
                    break;
                }
                default:
                    std::cerr << "Index component type " << accessor.componentType
                              << " not supported!" << std::endl;
                    return;
                }
            }
            Primitive primitive {};
            primitive.mFirstIndex = first_index;
            primitive.mIndexCount = index_count;
            primitive.mMaterialIndex = glTFPrimitive.material;
            Node->mMesh.mPrimitives.push_back(primitive);
        }
    }

    if (inParent) {
        inParent->mChildren.push_back(std::move(Node));
    } else {
        mNodes.push_back(std::move(Node));
    }
}

void glTF_Model::DrawNode(VulkanCommandBuffer& inCommandBuffer,
    VulkanPipelineLayoutBase& inPipelineLayout,
    Node* inNode)
{
    if (inNode->mMesh.mPrimitives.size() > 0) {
        glm::mat4 NodeMatrix = inNode->mMatrix;
        glTF_Model::Node* CurrentParent = inNode->mParent;
        while (CurrentParent) {
            NodeMatrix = CurrentParent->mMatrix * NodeMatrix;
            CurrentParent = CurrentParent->mParent;
        }

        auto value = PushConstant { .mModelMatrix = NodeMatrix };
        inCommandBuffer.PushConstants<PushConstant>(inPipelineLayout, value);
        for (auto& primitive : inNode->mMesh.mPrimitives) {
            if (primitive.mIndexCount > 0) {
                glTF_Model::Texture Texture
                    = mTextures[mMaterials[primitive.mMaterialIndex].mBaseColorTextureIndex];
                /* Bind DescriptorSets Here */
                /* Draw Indexed Here */
            }
        }
    }
}
