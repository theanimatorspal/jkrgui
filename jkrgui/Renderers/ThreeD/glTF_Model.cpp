#include "glTF_Model.hpp"
#include "Global/Standards.hpp"
#include <optional>

using namespace Jkr::Renderer::_3D;
using namespace ksai;

constexpr float ErrorFactor = 0.05;

glm::mat4 glTF_Model::Node::GetLocalMatrix() {
    {};
    return glm::translate(glm::mat4(1.0f), mTranslation) * glm::mat4(mRotation) * glm::scale(glm::mat4(1.0f), mScale) * mMatrix;
}

glTF_Model::BoundingBox::BoundingBox(){};

glTF_Model::BoundingBox::BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max){};

glTF_Model::BoundingBox glTF_Model::BoundingBox::GetAABB(glm::mat4 m) {
    glm::vec3 min = glm::vec3(m[3]);
    glm::vec3 max = min;
    glm::vec3 v0, v1;

    glm::vec3 right = glm::vec3(m[0]);
    v0              = right * this->min.x;
    v1              = right * this->max.x;
    min += glm::min(v0, v1);
    max += glm::max(v0, v1);

    glm::vec3 up = glm::vec3(m[1]);
    v0           = up * this->min.y;
    v1           = up * this->max.y;
    min += glm::min(v0, v1);
    max += glm::max(v0, v1);

    glm::vec3 back = glm::vec3(m[2]);
    v0             = back * this->min.z;
    v1             = back * this->max.z;
    min += glm::min(v0, v1);
    max += glm::max(v0, v1);

    return BoundingBox(min, max);
}

bool glTF_Model::BoundingBox::IsPointInside(glm::vec3 inVec) {
    return inVec.x >= min.x and inVec.x <= max.x and inVec.y >= min.y and inVec.y <= max.y and inVec.z >= min.z and
           inVec.z <= max.z;
}

static void CalculateTangents(v<Vertex3DExt> &inVertices, v<ui> &inIndices, ui inInitialVertexOffset) {
    using namespace glm;
    std::vector<vec3> tan1(inVertices.size(), vec3(0.0f));
    std::vector<vec3> tan2(inVertices.size(), vec3(0.0f));
    for (size_t i = 0; i < inIndices.size(); i += 3) {
        const auto vertex0 = inVertices[inIndices[i] - inInitialVertexOffset];
        const auto vertex1 = inVertices[inIndices[i + 1] - inInitialVertexOffset];
        const auto vertex2 = inVertices[inIndices[i + 2] - inInitialVertexOffset];

        const vec3 dPos1   = vertex1.mPosition - vertex0.mPosition;
        const vec3 dPos2   = vertex2.mPosition - vertex0.mPosition;

        const vec2 dUV1    = vertex1.mUV - vertex0.mUV;
        const vec2 dUV2    = vertex2.mUV - vertex0.mUV;

        const float r      = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);

        const vec3 uDir    = (dPos1 * dUV2.y - dPos2 * dUV1.y) * r;
        const vec3 vDir    = (dPos2 * dUV1.x - dPos1 * dUV2.x) * r;

        tan1[inIndices[i] - inInitialVertexOffset] += uDir;
        tan1[inIndices[i + 1] - inInitialVertexOffset] += uDir;
        tan1[inIndices[i + 2] - inInitialVertexOffset] += uDir;

        tan2[inIndices[i] - inInitialVertexOffset] += vDir;
        tan2[inIndices[i + 1] - inInitialVertexOffset] += vDir;
        tan2[inIndices[i + 2] - inInitialVertexOffset] += vDir;
    }

    for (size_t i = 0; i < inVertices.size(); i++) {
        const vec3 n                  = inVertices[i].mNormal;
        const vec3 t                  = tan1[i];

        const vec3 tangent            = normalize(t - n * dot(n, t));
        const float tangentHandedness = (dot(cross(n, t), tan2[i]) < 0.0f) ? -1.0f : 1.0f;

        inVertices[i].mTangent        = {tangent.x, tangent.y, tangent.z, tangentHandedness};
    }
}

// TODO Merge these two functions
void glTF_Model::Load(ui inInitialVertexOffset) {
    std::scoped_lock<std::mutex> Lock(mUpdateMutex);
    tinygltf::Model glTFInput;
    tinygltf::TinyGLTF gltfContext;
    s error, warning;
    bool file_loaded = false;
    s fileName       = s(mFileName);
    if (s(mFileName).find(".glb") != std::string::npos) {
        file_loaded = gltfContext.LoadBinaryFromFile(&glTFInput, &error, &warning, s(mFileName));
    } else {
        file_loaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, s(mFileName));
    }
    if (file_loaded) {
        this->LoadImages(glTFInput);
        this->LoadMaterials(glTFInput);
        this->LoadTextures(glTFInput);
        const tinygltf::Scene &scene = glTFInput.scenes[0]; // TODO Fix This
        for (size_t i = 0; i < scene.nodes.size(); i++) {
            ui index                  = 0;
            const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
            this->LoadNode(
                 node,
                 glTFInput,
                 nullptr,
                 scene.nodes[i],
                 mIndexBuffer,
                 mVertexBuffer,
                 [&](Vertex3DExt inVertex) {
                     return Vertex3D{.mPosition = inVertex.mPosition,
                                     .mNormal   = inVertex.mNormal,
                                     .mUV       = inVertex.mUV,
                                     .mColor    = inVertex.mColor};
                 },
                 [=](ui inIndex) { return (inIndex) + inInitialVertexOffset; });
        }
        this->LoadSkins(glTFInput);
        this->LoadAnimations(glTFInput);
        for (auto &Node : mNodes) {
            UpdateJoints(Node.get());
        }
        CalculateTangents(GetVerticesExtRef(), GetIndicesRef(), inInitialVertexOffset);
    } else {
        std::cout << "File Not Loaded, Not found with the name:" << mFileName << '\n';
    }
}

void glTF_Model::Load(FillVertexCallBack inVertexCallback, FillIndexCallBack inIndexCallback) {
    std::scoped_lock<std::mutex> Lock(mUpdateMutex);
    tinygltf::Model glTFInput;
    tinygltf::TinyGLTF gltfContext;
    s error, warning;
    bool file_loaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, s(mFileName));
    if (file_loaded) {
        this->LoadImages(glTFInput);
        this->LoadMaterials(glTFInput);
        this->LoadTextures(glTFInput);
        const tinygltf::Scene &scene = glTFInput.scenes[0]; // TODO Fix this
        for (size_t i = 0; i < scene.nodes.size(); i++) {
            const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
            this->LoadNode(
                 node, glTFInput, nullptr, scene.nodes[i], mIndexBuffer, mVertexBuffer, inVertexCallback, inIndexCallback);
        }
        this->LoadSkins(glTFInput);
        this->LoadAnimations(glTFInput);
        for (auto &Node : mNodes) {
            UpdateJoints(Node.get());
        }
    } else {
        std::cout << "File Not Loaded, Not found with the name:" << mFileName << '\n';
    }
}

void glTF_Model::LoadImages(tinygltf::Model &input) {
    // Images can be stored inside the glTF (which is the case for the sample model), so instead of
    // directly loading them from disk, we fetch them from the glTF loader and upload the buffers
    mImages.resize(input.images.size());
    for (size_t i = 0; i < input.images.size(); i++) {
        tinygltf::Image &glTFImage = input.images[i];
        // Get the image data from the glTF loader
        v<uc> buffvec;
        uc *buffer        = nullptr;
        size_t bufferSize = 0;
        // We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
        if (glTFImage.component == 3) {
            bufferSize = glTFImage.width * glTFImage.height * 4;
            buffvec.resize(bufferSize);
            buffer              = buffvec.data();
            unsigned char *rgba = buffer;
            unsigned char *rgb  = &glTFImage.image[0];
            for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i) {
                memcpy(rgba, rgb, sizeof(unsigned char) * 3);
                rgba += 4;
                rgb += 3;
            }
        } else {
            bufferSize = glTFImage.width * glTFImage.height * 4;
            buffvec.resize(bufferSize);
            buffer = buffvec.data();
            buffer = &glTFImage.image[0];
            memcpy(buffvec.data(), &glTFImage.image[0], bufferSize);
        }
        Image Image;
        Image.mTextureImage = std::move(buffvec);
        Image.mWidth        = glTFImage.width;
        Image.mHeight       = glTFImage.height;
        mImages[i]          = std::move(Image);
    }
}

void glTF_Model::LoadTextures(tinygltf::Model &input) {
    mTextures.resize(input.textures.size());
    for (size_t i = 0; i < input.textures.size(); i++) {
        mTextures[i].mImageIndex = input.textures[i].source;
    }
}

void glTF_Model::LoadMaterials(tinygltf::Model &gltfModel) {
    for (tinygltf::Material &mat : gltfModel.materials) {
        glTF_Model::Material material{};
        material.mDoubleSided = mat.doubleSided;
        if (mat.values.find("baseColorTexture") != mat.values.end()) {
            material.mBaseColorTextureIndex            = mat.values["baseColorTexture"].TextureIndex();
            material.mTextureCoordinateSets.mBaseColor = mat.values["baseColorTexture"].TextureTexCoord();
        }
        if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
            material.mMetallicRoughnessTextureIndex            = mat.values["metallicRoughnessTexture"].TextureIndex();
            material.mTextureCoordinateSets.mMetallicRoughness = mat.values["metallicRoughnessTexture"].TextureTexCoord();
        }
        if (mat.values.find("roughnessFactor") != mat.values.end()) {
            material.mRoughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
        }
        if (mat.values.find("metallicFactor") != mat.values.end()) {
            material.mMetallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
        }
        if (mat.values.find("baseColorFactor") != mat.values.end()) {
            material.mBaseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
        }
        if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
            material.mNormalTextureIndex            = mat.additionalValues["normalTexture"].TextureIndex();
            material.mTextureCoordinateSets.mNormal = mat.additionalValues["normalTexture"].TextureTexCoord();
        }
        if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
            material.mEmissiveTextureIndex            = mat.additionalValues["emissiveTexture"].TextureIndex();
            material.mTextureCoordinateSets.mEmissive = mat.additionalValues["emissiveTexture"].TextureTexCoord();
        }
        if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
            material.mOcclusionTextureIndex            = mat.additionalValues["occlusionTexture"].TextureIndex();
            material.mTextureCoordinateSets.mOcclusion = mat.additionalValues["occlusionTexture"].TextureTexCoord();
        }
        if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
            tinygltf::Parameter param = mat.additionalValues["alphaMode"];
            if (param.string_value == "BLEND") {
                material.mAlphaMode = Material::AlphaMode::Blend;
            }
            if (param.string_value == "MASK") {
                material.mAlphaCutOff = 0.5f;
                material.mAlphaMode   = Material::AlphaMode::Mask;
            }
        }
        if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
            material.mAlphaCutOff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
        }
        if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end()) {
            material.mEmissiveFactor =
                 glm::vec4(glm::make_vec3(mat.additionalValues["emissiveFactor"].ColorFactor().data()), 1.0);
        }

        if (mat.extensions.find("KHR_materials_pbrSpecularGlossiness") != mat.extensions.end()) {
            auto ext = mat.extensions.find("KHR_materials_pbrSpecularGlossiness");
            if (ext->second.Has("specularGlossinessTexture")) {
                auto index                                          = ext->second.Get("specularGlossinessTexture").Get("index");
                material.mExtension.mSpecularGlossinessTextureIndex = index.Get<int>();
                auto texCoordSet = ext->second.Get("specularGlossinessTexture").Get("texCoord");
                material.mTextureCoordinateSets.mSpecularGlossiness = texCoordSet.Get<int>();
                material.mPbrWorkflows.mSpecularGlossiness          = true;
            }
            if (ext->second.Has("diffuseTexture")) {
                auto index                               = ext->second.Get("diffuseTexture").Get("index");
                material.mExtension.mDiffuseTextureIndex = index.Get<int>();
            }
            if (ext->second.Has("diffuseFactor")) {
                auto factor = ext->second.Get("diffuseFactor");
                for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
                    auto val                              = factor.Get(i);
                    material.mExtension.mDiffuseFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
                }
            }
            if (ext->second.Has("specularFactor")) {
                auto factor = ext->second.Get("specularFactor");
                for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
                    auto val                               = factor.Get(i);
                    material.mExtension.mSpecularFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
                }
            }
        }

        if (mat.extensions.find("KHR_materials_unlit") != mat.extensions.end()) {
            material.unlit = true;
        }

        if (mat.extensions.find("KHR_materials_emissive_strength") != mat.extensions.end()) {
            auto ext = mat.extensions.find("KHR_materials_emissive_strength");
            if (ext->second.Has("emissiveStrength")) {
                auto value                = ext->second.Get("emissiveStrength");
                material.emissiveStrength = (float)value.Get<double>();
            }
        }

        material.mIndex = static_cast<uint32_t>(mMaterials.size());
        mMaterials.push_back(material);
    }
    // Push a default material at the end of the list for meshes with no material assigned
    mMaterials.push_back(Material());
}

glTF_Model::Node *glTF_Model::FindNode(Node *inParent, ui inIndex) {
    Node *NodeFound = nullptr;
    if (inParent->mIndex == inIndex) {
        return inParent;
    }
    for (auto &child : inParent->mChildren) {
        NodeFound = FindNode(child.get(), inIndex);
        if (NodeFound) {
            break;
        }
    }
    return NodeFound;
}

glTF_Model::Node *glTF_Model::NodeFromIndex(ui inIndex) {
    Node *NodeFound = nullptr;
    for (auto &node : mNodes) {
        NodeFound = FindNode(node.get(), inIndex);
        if (NodeFound) {
            break;
        }
    }
    return NodeFound;
}

void glTF_Model::LoadSkins(tinygltf::Model &input) {
    mSkins.resize(input.skins.size());
    for (size_t i = 0; i < input.skins.size(); i++) {
        tinygltf::Skin gltfSkin = input.skins[i];
        mSkins[i].mName         = input.skins[i].name;
        mSkins[i].mSkeletonRoot = NodeFromIndex(gltfSkin.skeleton);

        for (int jointIndex : gltfSkin.joints) {
            Node *node = NodeFromIndex(jointIndex);
            if (node) {
                mSkins[i].mJoints.push_back(node);
            }
        }

        if (gltfSkin.inverseBindMatrices > -1) {
            const tinygltf::Accessor &accessor     = input.accessors[gltfSkin.inverseBindMatrices];
            const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
            const tinygltf::Buffer &buffer         = input.buffers[bufferView.buffer];
            mSkins[i].mInverseBindMatrices.resize(accessor.count);
            memcpy(mSkins[i].mInverseBindMatrices.data(),
                   &buffer.data[accessor.byteOffset + bufferView.byteOffset],
                   accessor.count * sizeof(glm::mat4));
            // TODO Store Inverse bind matrices in Shader Storage buffer Objects
        }
    }
}

void glTF_Model::LoadAnimations(tinygltf::Model &input) {
    mAnimations.resize(input.animations.size());
    for (size_t i = 0; i < input.animations.size(); i++) {
        tinygltf::Animation glTFAnimation = input.animations[i];
        mAnimations[i].mName              = glTFAnimation.name;
        mAnimations[i].mSamplers.resize(glTFAnimation.samplers.size());
        for (size_t j = 0; j < glTFAnimation.samplers.size(); j++) {
            tinygltf::AnimationSampler glTFSampler = glTFAnimation.samplers[j];
            AnimationSampler &dstSampler           = mAnimations[i].mSamplers[j];
            dstSampler.mInterpolation              = glTFSampler.interpolation;
            // Read sampler keyframe input time values
            {
                const tinygltf::Accessor &accessor     = input.accessors[glTFSampler.input];
                const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer         = input.buffers[bufferView.buffer];
                const void *dataPtr                    = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
                const float *buf                       = static_cast<const float *>(dataPtr);
                for (size_t index = 0; index < accessor.count; index++) {
                    dstSampler.mInputs.push_back(buf[index]);
                }
                // Adjust animation's start and end times
                for (auto input : mAnimations[i].mSamplers[j].mInputs) {
                    if (input < mAnimations[i].mStart) {
                        mAnimations[i].mStart = input;
                    };
                    if (input > mAnimations[i].mEnd) {
                        mAnimations[i].mEnd = input;
                    }
                }
            }

            // Read sampler keyframe output translate/rotate/scale values
            {
                const tinygltf::Accessor &accessor     = input.accessors[glTFSampler.output];
                const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer         = input.buffers[bufferView.buffer];
                const void *dataPtr                    = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
                switch (accessor.type) {
                    case TINYGLTF_TYPE_VEC3: {
                        const glm::vec3 *buf = static_cast<const glm::vec3 *>(dataPtr);
                        for (size_t index = 0; index < accessor.count; index++) {
                            dstSampler.mOutputsVec4.push_back(glm::vec4(buf[index], 0.0f));
                        }
                        break;
                    }
                    case TINYGLTF_TYPE_VEC4: {
                        const glm::vec4 *buf = static_cast<const glm::vec4 *>(dataPtr);
                        for (size_t index = 0; index < accessor.count; index++) {
                            dstSampler.mOutputsVec4.push_back(buf[index]);
                        }
                        break;
                    }
                    default: {
                        std::cout << "unknown type" << std::endl;
                        break;
                    }
                }
            }
            // Channels
            mAnimations[i].mChannels.resize(glTFAnimation.channels.size());
            for (size_t j = 0; j < glTFAnimation.channels.size(); j++) {
                tinygltf::AnimationChannel glTFChannel = glTFAnimation.channels[j];
                AnimationChannel &dstChannel           = mAnimations[i].mChannels[j];
                dstChannel.mPath                       = glTFChannel.target_path;
                dstChannel.mSamplerIndex               = glTFChannel.sampler;
                dstChannel.mNode                       = NodeFromIndex(glTFChannel.target_node);
            }
        }
    }
}

void glTF_Model::LoadNode(const tinygltf::Node &inputNode,
                          const tinygltf::Model &input,
                          Node *inParent,
                          ui inNodeIndex,
                          std::vector<uint32_t> &inIndexBuffer,
                          v<Vertex3D> &inVertexBuffer,
                          FillVertexCallBack inVertexCallBack,
                          FillIndexCallBack inIndexCallBack) {
    Up<glTF_Model::Node> Node = MakeUp<glTF_Model::Node>();
    Node->mParent             = inParent;
    Node->mMatrix             = glm::mat4(1.0f);
    Node->mIndex              = inNodeIndex;
    Node->mSkin               = inputNode.skin;

    /* Node Matrix is either made from translation, rotation, scale or a 4x4 matrix */
    if (inputNode.translation.size() == 3) {
        Node->mTranslation = glm::make_vec3(inputNode.translation.data());
    }

    if (inputNode.rotation.size() == 4) {
        glm::quat q     = glm::make_quat(inputNode.rotation.data());
        Node->mRotation = glm::mat4(q);
    }
    if (inputNode.scale.size() == 3) {
        Node->mScale = glm::make_vec3(inputNode.scale.data());
    }
    if (inputNode.matrix.size() == 16) {
        Node->mMatrix = glm::make_mat4x4(inputNode.matrix.data());
    }

    /* Load node's children */
    if (inputNode.children.size() > 0) {
        for (size_t i = 0; i < inputNode.children.size(); i++) {
            LoadNode(input.nodes[inputNode.children[i]],
                     input,
                     Node.get(),
                     inputNode.children[i],
                     inIndexBuffer,
                     inVertexBuffer,
                     inVertexCallBack,
                     inIndexCallBack);
        }
    }

    /* inGLTF accessing done by buffer */
    bool NodeContainsMesh = inputNode.mesh > -1;
    if (NodeContainsMesh) {
        const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
        for (size_t i = 0; i < mesh.primitives.size(); i++) {
            const tinygltf::Primitive &glTFPrimitive = mesh.primitives[i];
            uint32_t first_index                     = static_cast<uint32_t>(inIndexBuffer.size());
            uint32_t vertex_start                    = static_cast<uint32_t>(inVertexBuffer.size());
            uint32_t index_count                     = 0;
            bool hasSkin                             = false;
            glm::vec3 PosMin                         = glm::vec3{0};
            glm::vec3 PosMax                         = glm::vec3{0};
            bool hasIndices                          = glTFPrimitive.indices > -1;

            /* Vertices */
            {
                const float *positionBuffer     = nullptr;
                const float *normalsBuffer      = nullptr;
                const float *texCoordsBuffer    = nullptr;
                const float *texCoordsBuffer1   = nullptr;
                const float *colorBuffer0       = nullptr;
                const void *jointIndicesBuffer  = nullptr;
                const float *jointWeightsBuffer = nullptr;
                const float *tangentsBuffer     = nullptr;
                size_t vertexCount              = 0;

                int posByteStride;
                int normByteStride;
                int uv0ByteStride;
                int uv1ByteStride;
                int color0ByteStride;
                int jointByteStride;
                int weightByteStride;
                int tangesByteStride;

                /*Get Buffer data for vertex Position*/
                if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
                    const tinygltf::BufferView &view   = input.bufferViews[accessor.bufferView];
                    positionBuffer                     = reinterpret_cast<const float *>(
                         &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    vertexCount   = accessor.count;
                    posByteStride = accessor.ByteStride(view) ? (accessor.ByteStride(view) / sizeof(float))
                                                              : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
                    PosMin        = glm::vec3(accessor.minValues[0], accessor.minValues[1], accessor.minValues[2]);
                    PosMax        = glm::vec3(accessor.maxValues[0], accessor.maxValues[1], accessor.maxValues[2]);
                } else {
                    std::cout << "No POSITION in GLTF File \n";
                }

                /* Get Buffer Data From Vertex Normals */
                if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
                    const tinygltf::BufferView &view   = input.bufferViews[accessor.bufferView];
                    normalsBuffer                      = reinterpret_cast<const float *>(
                         &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    normByteStride = accessor.ByteStride(view) ? (accessor.ByteStride(view) / sizeof(float))
                                                               : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
                }

                /*Get Buffer Data From TexCoords*/
                if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
                    const tinygltf::BufferView &view   = input.bufferViews[accessor.bufferView];
                    texCoordsBuffer                    = reinterpret_cast<const float *>(
                         &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    uv0ByteStride = accessor.ByteStride(view) ? (accessor.ByteStride(view) / sizeof(float))
                                                              : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
                }

                if (glTFPrimitive.attributes.find("TEXCOORD_1") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &uvAccessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_1")->second];
                    const tinygltf::BufferView &uvView   = input.bufferViews[uvAccessor.bufferView];
                    texCoordsBuffer1                     = reinterpret_cast<const float *>(
                         &(input.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                    uv1ByteStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float))
                                                                  : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
                }

                // Vertex colors
                if (glTFPrimitive.attributes.find("COLOR_0") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &accessor = input.accessors[glTFPrimitive.attributes.find("COLOR_0")->second];
                    const tinygltf::BufferView &view   = input.bufferViews[accessor.bufferView];
                    colorBuffer0                       = reinterpret_cast<const float *>(
                         &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    color0ByteStride = accessor.ByteStride(view) ? (accessor.ByteStride(view) / sizeof(float))
                                                                 : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
                }

                if (glTFPrimitive.attributes.find("TANGENT") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &tangentAccessor = input.accessors[glTFPrimitive.attributes.find("TANGENT")->second];
                    const tinygltf::BufferView &tangentView   = input.bufferViews[tangentAccessor.bufferView];
                    tangentsBuffer                            = reinterpret_cast<const float *>(
                         &(input.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
                    tangesByteStride = tangentAccessor.ByteStride(tangentView)
                                            ? (tangentAccessor.ByteStride(tangentView) / sizeof(float))
                                            : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
                }

                // POI: Get buffer data required for vertex skinning
                // Get vertex joint indices

                int JointComponentType;
                if (glTFPrimitive.attributes.find("JOINTS_0") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &accessor = input.accessors[glTFPrimitive.attributes.find("JOINTS_0")->second];
                    const tinygltf::BufferView &view   = input.bufferViews[accessor.bufferView];
                    jointIndicesBuffer = &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
                    JointComponentType = accessor.componentType;
                    jointByteStride    = accessor.ByteStride(view)
                                              ? (accessor.ByteStride(view) / tinygltf::GetComponentSizeInBytes(JointComponentType))
                                              : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
                }
                // Get vertex joint weights
                if (glTFPrimitive.attributes.find("WEIGHTS_0") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &accessor = input.accessors[glTFPrimitive.attributes.find("WEIGHTS_0")->second];
                    const tinygltf::BufferView &view   = input.bufferViews[accessor.bufferView];
                    jointWeightsBuffer                 = reinterpret_cast<const float *>(
                         &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    weightByteStride = accessor.ByteStride(view) ? (accessor.ByteStride(view) / sizeof(float))
                                                                 : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
                }

                hasSkin = (jointIndicesBuffer && jointWeightsBuffer);

                /* Append data to Model's VertexBuffer */
                for (size_t v = 0; v < vertexCount; v++) {
                    Vertex3DExt vert{};
                    vert.mPosition = glm::vec4(glm::make_vec3(&positionBuffer[v * posByteStride]), 1.0f);
                    vert.mNormal   = glm::normalize(
                         glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * normByteStride]) : glm::vec3(0.0f)));
                    vert.mUV      = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * uv0ByteStride]) : glm::vec2(0.0f);
                    vert.mColor   = colorBuffer0 ? glm::make_vec4(&colorBuffer0[v * color0ByteStride]) : glm::vec4(1.0f);
                    vert.mTangent = tangentsBuffer ? glm::vec4(glm::make_vec4(&tangentsBuffer[v * 4])) : glm::vec4(0.0f);

                    if (hasSkin) {
                        switch (JointComponentType) {
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                                const uint16_t *buf = static_cast<const uint16_t *>(jointIndicesBuffer);
                                vert.mJointIndices  = glm::vec4(glm::make_vec4(&buf[v * jointByteStride]));
                                break;
                            }
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                                const uint8_t *buf = static_cast<const uint8_t *>(jointIndicesBuffer);
                                vert.mJointIndices = glm::vec4(glm::make_vec4(&buf[v * jointByteStride]));
                                break;
                            }
                            default:
                                // Not supported by spec
                                std::cerr << "Joint component type " << JointComponentType << " not supported!" << std::endl;
                                break;
                        }
                    } else {
                        vert.mJointIndices = glm::vec4(0.0f);
                    }

                    vert.mJointWeights = hasSkin ? glm::make_vec4(&jointWeightsBuffer[v * weightByteStride]) : glm::vec4(0.0f);

                    // Fix for all zero weights
                    if (glm::length(vert.mJointWeights) == 0.0f) {
                        vert.mJointWeights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
                    }

                    inVertexBuffer.push_back(inVertexCallBack(vert));
                    mVertexBufferExt.push_back(vert);
                }
            }

            /* Indices */
            {
                const tinygltf::Accessor &accessor     = input.accessors[glTFPrimitive.indices];
                const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer         = input.buffers[bufferView.buffer];

                index_count += static_cast<uint32_t>(accessor.count);

                /* glTF supports different components of Indices */
                switch (accessor.componentType) {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                        const uint32_t *buf =
                             reinterpret_cast<const uint32_t *>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        for (size_t index = 0; index < accessor.count; index++) {
                            inIndexBuffer.push_back(inIndexCallBack(buf[index] + vertex_start));
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                        const uint16_t *buf =
                             reinterpret_cast<const uint16_t *>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        for (size_t index = 0; index < accessor.count; index++) {
                            inIndexBuffer.push_back(inIndexCallBack(buf[index] + vertex_start));
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                        const uint8_t *buf =
                             reinterpret_cast<const uint8_t *>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        for (size_t index = 0; index < accessor.count; index++) {
                            inIndexBuffer.push_back(inIndexCallBack(buf[index] + vertex_start));
                        }
                        break;
                    }
                    default:
                        std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
                        return;
                }
            }
            Primitive primitive{};
            primitive.mFirstIndex    = first_index;
            primitive.mIndexCount    = index_count;
            primitive.mMaterialIndex = glTFPrimitive.material;
            primitive.mBB.min        = PosMin;
            primitive.mBB.max        = PosMax;
            primitive.mBB.valid      = true;
            Node->mMesh.mPrimitives.push_back(primitive);
            Node->mMesh.mNodeIndex.push_back(Node->mIndex); // TODO Might need a cleanup
        }
        for (auto &p : Node->mMesh.mPrimitives) {
            if (p.mBB.valid and not Node->mMesh.mBB.valid) {
                Node->mMesh.mBB       = p.mBB;
                Node->mMesh.mBB.valid = true;
            }
            Node->mMesh.mBB.min = glm::min(Node->mMesh.mBB.min, p.mBB.min);
            Node->mMesh.mBB.max = glm::max(Node->mMesh.mBB.max, p.mBB.max);
        }
        mMeshes.push_back(Node->mMesh);
    }

    if (inParent) {
        inParent->mChildren.push_back(std::move(Node));
    } else {
        mNodes.push_back(std::move(Node));
    }
}

glm::mat4 glTF_Model::GetNodeMatrixByIndex(int inIndex) { return GetNodeMatrix(NodeFromIndex(inIndex)); }

// POI: Traverse the node hierarchy to the top-most parent to get the local matrix of the given node
glm::mat4 glTF_Model::GetNodeMatrix(glTF_Model::Node *inNode) {
    glm::mat4 NodeMatrix            = inNode->GetLocalMatrix();
    glTF_Model::Node *CurrentParent = inNode->mParent;
    while (CurrentParent) {
        NodeMatrix    = CurrentParent->GetLocalMatrix() * NodeMatrix;
        CurrentParent = CurrentParent->mParent;
    }
    return NodeMatrix;
}

// POI: Update the joint matrices from the current animation frame and pass them to the GPU
void glTF_Model::UpdateJoints(glTF_Model::Node *inNode, UpdateJointsCallBack inCallBack) {
    if (inNode->mSkin > -1) {
        // Update the joint matrices
        glm::mat4 inverseTransform = glm::inverse(GetNodeMatrix(inNode));
        Skin skin                  = mSkins[inNode->mSkin];
        size_t numJoints           = (uint32_t)skin.mJoints.size();
        std::vector<glm::mat4> jointMatrices(numJoints);
        for (size_t i = 0; i < numJoints; i++) {
            jointMatrices[i] = GetNodeMatrix(skin.mJoints[i]) * skin.mInverseBindMatrices[i];
            jointMatrices[i] = inverseTransform * jointMatrices[i];
        }
        inCallBack(jointMatrices);
    }

    for (auto &child : inNode->mChildren) {
        UpdateJoints(child.get(), inCallBack);
    }
}

void glTF_Model::UpdateAnimation(ui inActiveAnimation, float inDeltaTime, bool inShouldLoop) {
    Animation &Animation = mAnimations[inActiveAnimation];
    Animation.mCurrentTime += inDeltaTime;

    if (inShouldLoop) {
        if (Animation.mCurrentTime > Animation.mEnd) {
            Animation.mCurrentTime -= Animation.mEnd;
        }

        if (Animation.mCurrentTime < Animation.mStart) {
            Animation.mCurrentTime += Animation.mEnd;
        }
    } else {
        if (Animation.mCurrentTime > Animation.mEnd) {
            Animation.mCurrentTime = Animation.mEnd;
        }
        if (Animation.mCurrentTime < Animation.mStart) {
            Animation.mCurrentTime = Animation.mStart;
        }
    }

    if (Animation.mCurrentTime < ErrorFactor) {
        Animation.mCurrentTime = ErrorFactor;
    }

    for (auto &channel : Animation.mChannels) {
        AnimationSampler &sampler = Animation.mSamplers[channel.mSamplerIndex];
        for (size_t i = 0; i < sampler.mInputs.size() - 1; i++) {
            if (sampler.mInterpolation != "LINEAR" and sampler.mInterpolation != "STEP") {
                std::cout << "Only Linear is Supported Yet"; // TODO Support Others
                continue;
            }

            if ((Animation.mCurrentTime >= sampler.mInputs[i]) && (Animation.mCurrentTime <= sampler.mInputs[i + 1])) {
                float a = (Animation.mCurrentTime - sampler.mInputs[i]) / (sampler.mInputs[i + 1] - sampler.mInputs[i]);
                if (sampler.mInterpolation == "STEP") {
                    a = 1.0f;
                }
                if (channel.mPath == "translation") {
                    channel.mNode->mTranslation = glm::mix(sampler.mOutputsVec4[i], sampler.mOutputsVec4[i + 1], a);
                }
                if (channel.mPath == "rotation") {
                    glm::quat q1;
                    q1.x = sampler.mOutputsVec4[i].x;
                    q1.y = sampler.mOutputsVec4[i].y;
                    q1.z = sampler.mOutputsVec4[i].z;
                    q1.w = sampler.mOutputsVec4[i].w;

                    glm::quat q2;
                    q2.x                     = sampler.mOutputsVec4[i + 1].x;
                    q2.y                     = sampler.mOutputsVec4[i + 1].y;
                    q2.z                     = sampler.mOutputsVec4[i + 1].z;
                    q2.w                     = sampler.mOutputsVec4[i + 1].w;

                    channel.mNode->mRotation = glm::normalize(glm::slerp(q1, q2, a));
                }
                if (channel.mPath == "scale") {
                    channel.mNode->mScale = glm::mix(sampler.mOutputsVec4[i], sampler.mOutputsVec4[i + 1], a);
                }
            }
        }
    }
}

void glTF_Model::UpdateBlendCombineAnimation(
     float inDelTime, ui inDestinationAnimationIndex, float inBlendFactor, bool inShouldLoop, bool inShouldResetByBlendFactor) {
    Animation &Animation = mAnimations[inDestinationAnimationIndex];
    Animation.mCurrentTime += inDelTime;

    if (inShouldResetByBlendFactor) {
        if (inBlendFactor == 0.0f) Animation.mCurrentTime = Animation.mStart;
    }

    if (inShouldLoop) {
        if (Animation.mCurrentTime > Animation.mEnd) {
            Animation.mCurrentTime -= Animation.mEnd;
        }

        if (Animation.mCurrentTime < Animation.mStart) {
            Animation.mCurrentTime += Animation.mEnd;
        }
    } else {
        if (Animation.mCurrentTime > Animation.mEnd) {
            Animation.mCurrentTime = Animation.mEnd;
        }
        if (Animation.mCurrentTime < Animation.mStart) {
            Animation.mCurrentTime = Animation.mStart;
        }
    }

    if (Animation.mCurrentTime < ErrorFactor) {
        Animation.mCurrentTime = ErrorFactor;
    }

    for (auto &channel : Animation.mChannels) {
        AnimationSampler &sampler = Animation.mSamplers[channel.mSamplerIndex];
        for (size_t i = 0; i < sampler.mInputs.size() - 1; i++) {
            if (sampler.mInterpolation != "LINEAR" and sampler.mInterpolation != "STEP") {
                std::cout << "Only Linear is Supported Yet"; // TODO Support Others
                continue;
            }

            if ((Animation.mCurrentTime >= sampler.mInputs[i]) && (Animation.mCurrentTime <= sampler.mInputs[i + 1])) {
                float a = (Animation.mCurrentTime - sampler.mInputs[i]) / (sampler.mInputs[i + 1] - sampler.mInputs[i]);
                if (sampler.mInterpolation == "STEP") {
                    a = 1.0f;
                }
                if (channel.mPath == "translation") {
                    glm::vec3 t                 = glm::mix(sampler.mOutputsVec4[i], sampler.mOutputsVec4[i + 1], a);
                    channel.mNode->mTranslation = glm::mix(channel.mNode->mTranslation, t, inBlendFactor);
                }
                if (channel.mPath == "rotation") {
                    glm::quat q1;
                    q1.x = sampler.mOutputsVec4[i].x;
                    q1.y = sampler.mOutputsVec4[i].y;
                    q1.z = sampler.mOutputsVec4[i].z;
                    q1.w = sampler.mOutputsVec4[i].w;

                    glm::quat q2;
                    q2.x                     = sampler.mOutputsVec4[i + 1].x;
                    q2.y                     = sampler.mOutputsVec4[i + 1].y;
                    q2.z                     = sampler.mOutputsVec4[i + 1].z;
                    q2.w                     = sampler.mOutputsVec4[i + 1].w;

                    glm::quat r              = glm::normalize(glm::slerp(q1, q2, a));
                    channel.mNode->mRotation = glm::normalize(glm::slerp(channel.mNode->mRotation, r, inBlendFactor));
                }
                if (channel.mPath == "scale") {
                    glm::vec3 s           = glm::mix(sampler.mOutputsVec4[i], sampler.mOutputsVec4[i + 1], a);
                    channel.mNode->mScale = glm::mix(channel.mNode->mScale, s, inBlendFactor);
                }
            }
        }
    }
}

void glTF_Model::BlendCombineAnimationToArbritaryTime(float inDestinationTime,
                                                      ui inDestinationAnimationIndex,
                                                      float inBlendFactor,
                                                      bool inShouldLoop) {
    Animation &Animation  = mAnimations[inDestinationAnimationIndex];
    float DestinationTime = inDestinationTime;
    if (inShouldLoop) {
        if (inDestinationTime > Animation.mEnd) {
            DestinationTime -= Animation.mEnd;
        }

        if (inDestinationTime < Animation.mStart) {
            DestinationTime += Animation.mEnd;
        }
    } else {
        if (Animation.mCurrentTime > Animation.mEnd) {
            Animation.mCurrentTime = Animation.mEnd;
        }
        if (Animation.mCurrentTime < Animation.mStart) {
            Animation.mCurrentTime = Animation.mStart;
        }
    }

    if (DestinationTime < ErrorFactor) {
        DestinationTime = ErrorFactor;
    }

    for (auto &channel : Animation.mChannels) {
        AnimationSampler &sampler = Animation.mSamplers[channel.mSamplerIndex];
        for (size_t i = 0; i < sampler.mInputs.size() - 1; i++) {
            if (sampler.mInterpolation != "LINEAR" and sampler.mInterpolation != "STEP") {
                std::cout << "Only Linear is Supported Yet"; // TODO Support Others
                continue;
            }

            if ((DestinationTime >= sampler.mInputs[i]) && (DestinationTime <= sampler.mInputs[i + 1])) {
                float a = (DestinationTime - sampler.mInputs[i]) / (sampler.mInputs[i + 1] - sampler.mInputs[i]);
                if (sampler.mInterpolation == "STEP") {
                    a = 1.0f;
                }
                if (channel.mPath == "translation") {
                    glm::vec3 t                 = glm::mix(sampler.mOutputsVec4[i], sampler.mOutputsVec4[i + 1], a);
                    channel.mNode->mTranslation = glm::mix(channel.mNode->mTranslation, t, inBlendFactor);
                }
                if (channel.mPath == "rotation") {
                    glm::quat q1;
                    q1.x = sampler.mOutputsVec4[i].x;
                    q1.y = sampler.mOutputsVec4[i].y;
                    q1.z = sampler.mOutputsVec4[i].z;
                    q1.w = sampler.mOutputsVec4[i].w;

                    glm::quat q2;
                    q2.x                     = sampler.mOutputsVec4[i + 1].x;
                    q2.y                     = sampler.mOutputsVec4[i + 1].y;
                    q2.z                     = sampler.mOutputsVec4[i + 1].z;
                    q2.w                     = sampler.mOutputsVec4[i + 1].w;

                    glm::quat r              = glm::normalize(glm::slerp(q1, q2, a));
                    channel.mNode->mRotation = glm::normalize(glm::slerp(channel.mNode->mRotation, r, inBlendFactor));
                }
                if (channel.mPath == "scale") {
                    glm::vec3 s           = glm::mix(sampler.mOutputsVec4[i], sampler.mOutputsVec4[i + 1], a);
                    channel.mNode->mScale = glm::mix(channel.mNode->mScale, s, inBlendFactor);
                }
            }
        }
    }
}

void glTF_Model::UpdateAllJoints(UpdateJointsCallBack inCallBack) {
    std::scoped_lock<std::mutex> Lock(mUpdateMutex);
    for (auto &node : mNodes) {
        UpdateJoints(node.get(), inCallBack);
    }
}

void glTF_Model::Draw(glTF_Model::Node &inNode, PushCallBack inPushCallBack, DrawCallBack inDrawCallBack) {
    if (inNode.mMesh.mPrimitives.size() > 0) {
        glm::mat4 NodeMatrix            = inNode.mMatrix;
        glTF_Model::Node *CurrentParent = inNode.mParent;
        while (CurrentParent) {
            NodeMatrix    = CurrentParent->mMatrix * NodeMatrix;
            CurrentParent = CurrentParent->mParent;
        }
        inPushCallBack(NodeMatrix);
        for (glTF_Model::Primitive &primitive : inNode.mMesh.mPrimitives) {
            if (primitive.mIndexCount > 0) {
                if (mMaterials.size() > 0) {
                    glTF_Model::Texture texture = mTextures[mMaterials[primitive.mMaterialIndex].mBaseColorTextureIndex];
                    inDrawCallBack(primitive.mIndexCount, texture);
                } else {
                    inDrawCallBack(primitive.mIndexCount, std::nullopt);
                }
            }
        }
    }
}
