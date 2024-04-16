#include "glTF_Model.hpp"
#include "Global/Standards.hpp"

using namespace Jkr::Renderer::_3D;
using namespace ksai;

glm::mat4 glTF_Model::Node::GetLocalMatrix() { return glm::translate(glm::mat4(1.0f), mTranslation) * glm::mat4(mRotation) * glm::scale(glm::mat4(1.0f), mScale) * mMatrix; }

void glTF_Model::Load(ui inInitialVertexOffset) {
               s error, warning;
               bool file_loaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, s(mFileName));
               if (file_loaded) {
                              this->LoadImages(glTFInput);
                              this->LoadMaterials(glTFInput);
                              this->LoadTextures(glTFInput);
                              const tinygltf::Scene& scene = glTFInput.scenes[0];
                              for (size_t i = 0; i < scene.nodes.size(); i++) {
                                             const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
                                             this->LoadNode(
                                                       node,
                                                       glTFInput,
                                                       nullptr,
                                                       mIndexBuffer,
                                                       mVertexBuffer,
                                                       [](Vertex3DExt inVertex) {
                                                                      return Vertex3D{.mPosition = inVertex.mPosition, .mNormal = inVertex.mNormal, .mUV = inVertex.mUV, .mColor = inVertex.mColor};
                                                       },
                                                       [=](ui inIndex) { return (inIndex) + inInitialVertexOffset; });
                              }
               } else {
                              std::cout << "File Not Loaded, Not found with the name:" << mFileName << '\n';
               }
}

void glTF_Model::Load(FillVertexCallBack inVertexCallback, FillIndexCallBack inIndexCallback) {
               s error, warning;
               bool file_loaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, s(mFileName));
               if (file_loaded) {
                              this->LoadImages(glTFInput);
                              this->LoadMaterials(glTFInput);
                              this->LoadTextures(glTFInput);
                              const tinygltf::Scene& scene = glTFInput.scenes[0];
                              for (size_t i = 0; i < scene.nodes.size(); i++) {
                                             const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
                                             this->LoadNode(node, glTFInput, nullptr, mIndexBuffer, mVertexBuffer, inVertexCallback, inIndexCallback);
                              }
               } else {
                              std::cout << "File Not Loaded, Not found with the name:" << mFileName << '\n';
               }
}

void glTF_Model::LoadImages(tinygltf::Model& input) {
               // Images can be stored inside the glTF (which is the case for the sample model), so instead of directly
               // loading them from disk, we fetch them from the glTF loader and upload the buffers
               mImages.resize(input.images.size());
               for (size_t i = 0; i < input.images.size(); i++) {
                              tinygltf::Image& glTFImage = input.images[i];
                              // Get the image data from the glTF loader
                              v<uc> buffvec;
                              uc* buffer = nullptr;
                              size_t bufferSize = 0;
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
                              // TODO Make this local image
                              // Load texture from image buffer
                              // an_image.mTextureImage = MakeUp<ImageType>(mInstance);
                              // an_image.mTextureImage->Setup(reinterpret_cast<void**>(&buffer),
                              //     glTFImage.width,
                              //     glTFImage.height,
                              //     4);
                              Image Image;
                              Image.mTextureImage = buffvec;
                              Image.mWidth = glTFImage.width;
                              Image.mHeight = glTFImage.height;
                              mImages.push_back(std::move(Image));
               }
}

void glTF_Model::LoadTextures(tinygltf::Model& input) {
               mTextures.resize(input.textures.size());
               for (size_t i = 0; i < input.textures.size(); i++) {
                              mTextures[i].mImageIndex = input.textures[i].source;
               }
}

void glTF_Model::LoadMaterials(tinygltf::Model& input) {
               mMaterials.resize(input.materials.size());
               for (size_t i = 0; i < input.materials.size(); i++) {
                              // We only read the most basic properties required for our sample
                              tinygltf::Material glTFMaterial = input.materials[i];
                              // Get the base color factor
                              if (glTFMaterial.values.find("baseColorFactor") != glTFMaterial.values.end()) {
                                             mMaterials[i].mBaseColorFactor = glm::make_vec4(glTFMaterial.values["baseColorFactor"].ColorFactor().data());
                              }
                              // Get base color texture index
                              if (glTFMaterial.values.find("baseColorTexture") != glTFMaterial.values.end()) {
                                             mMaterials[i].mBaseColorTextureIndex = glTFMaterial.values["baseColorTexture"].TextureIndex();
                              }
               }
}

glTF_Model::Node* glTF_Model::FindNode(Node* inParent, ui inIndex) {
               Node* NodeFound = nullptr;
               if (inParent->mIndex == inIndex) {
                              return inParent;
               }
               for (auto& child : inParent->mChildren) {
                              NodeFound = FindNode(child.get(), inIndex);
                              if (NodeFound) {
                                             break;
                              }
               }
               return NodeFound;
}

glTF_Model::Node* glTF_Model::NodeFromIndex(ui inIndex) {
               Node* NodeFound = nullptr;
               for (auto& node : mNodes) {
                              NodeFound = FindNode(node.get(), inIndex);
                              if (NodeFound) {
                                             break;
                              }
               }
               return NodeFound;
}

void glTF_Model::LoadSkins(tinygltf::Model& input) {
               mSkins.resize(input.skins.size());
               for (size_t i = 0; i < input.skins.size(); i++) {
                              tinygltf::Skin gltfSkin = input.skins[i];
                              mSkins[i].mName = input.skins[i].name;
                              mSkins[i].mSkeletonRoot = NodeFromIndex(gltfSkin.skeleton);

                              for (int jointIndex : gltfSkin.joints) {
                                             Node* node = NodeFromIndex(jointIndex);
                                             if (node) {
                                                            mSkins[i].mJoints.push_back(node);
                                             }
                              }

                              if (gltfSkin.inverseBindMatrices > -1) {
                                             const tinygltf::Accessor& accessor = input.accessors[gltfSkin.inverseBindMatrices];
                                             const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
                                             const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];
                                             mSkins[i].mInverseBindMatrices.resize(accessor.count);
                                             memcpy(mSkins[i].mInverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::mat4));
                                             // TODO Store Inverse bind matrices in Shader Storage buffer Objects
                              }
               }
}

void glTF_Model::LoadAnimations(tinygltf::Model& input) {
               mAnimations.resize(input.animations.size());
               for (size_t i = 0; i < input.animations.size(); i++) {
                              tinygltf::Animation glTFAnimation = input.animations[i];
                              mAnimations[i].mName = glTFAnimation.name;
                              mAnimations[i].mSamplers.resize(glTFAnimation.samplers.size());
                              for (size_t j = 0; j < glTFAnimation.samplers.size(); j++) {
                                             tinygltf::AnimationSampler glTFSampler = glTFAnimation.samplers[j];
                                             AnimationSampler& dstSampler = mAnimations[i].mSamplers[j];
                                             dstSampler.mInterpolation = glTFSampler.interpolation;
                                             // Read sampler keyframe input time values
                                             {
                                                            const tinygltf::Accessor& accessor = input.accessors[glTFSampler.input];
                                                            const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
                                                            const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];
                                                            const void* dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
                                                            const float* buf = static_cast<const float*>(dataPtr);
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
                                                            const tinygltf::Accessor& accessor = input.accessors[glTFSampler.output];
                                                            const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
                                                            const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];
                                                            const void* dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
                                                            switch (accessor.type) {
                                                                           case TINYGLTF_TYPE_VEC3: {
                                                                                          const glm::vec3* buf = static_cast<const glm::vec3*>(dataPtr);
                                                                                          for (size_t index = 0; index < accessor.count; index++) {
                                                                                                         dstSampler.mOutputsVec4.push_back(glm::vec4(buf[index], 0.0f));
                                                                                          }
                                                                                          break;
                                                                           }
                                                                           case TINYGLTF_TYPE_VEC4: {
                                                                                          const glm::vec4* buf = static_cast<const glm::vec4*>(dataPtr);
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
                                                            AnimationChannel& dstChannel = mAnimations[i].mChannels[j];
                                                            dstChannel.mPath = glTFChannel.target_path;
                                                            dstChannel.mSamplerIndex = glTFChannel.sampler;
                                                            dstChannel.mNode = NodeFromIndex(glTFChannel.target_node);
                                             }
                              }
               }
}

void glTF_Model::LoadNode(const tinygltf::Node& inputNode,
                          const tinygltf::Model& input,
                          Node* inParent,
                          std::vector<uint32_t>& inIndexBuffer,
                          v<Vertex3D>& inVertexBuffer,
                          FillVertexCallBack inVertexCallBack,
                          FillIndexCallBack inIndexCallBack) {
               Up<glTF_Model::Node> Node = MakeUp<glTF_Model::Node>();
               Node->mMatrix = glm::mat4(1.0f);
               Node->mParent = inParent;

               /* Node Matrix is either made from translation, rotation, scale or a 4x4 matrix */
               if (inputNode.translation.size() == 3) {
                              Node->mMatrix = glm::translate(Node->mMatrix, glm::vec3(glm::make_vec3(inputNode.translation.data())));
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
                                             LoadNode(input.nodes[inputNode.children[i]], input, Node.get(), inIndexBuffer, inVertexBuffer, inVertexCallBack, inIndexCallBack);
                              }
               }

               /* inGLTF accessing done by buffer */
               bool NodeContainsMesh = inputNode.mesh > -1;
               if (NodeContainsMesh) {
                              const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
                              for (size_t i = 0; i < mesh.primitives.size(); i++) {
                                             const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];
                                             uint32_t first_index = static_cast<uint32_t>(inIndexBuffer.size());
                                             uint32_t vertex_start = static_cast<uint32_t>(inVertexBuffer.size());
                                             uint32_t index_count = 0;
                                             bool hasSkin = false;

                                             /* Vertices */
                                             {
                                                            const float* positionBuffer = nullptr;
                                                            const float* normalsBuffer = nullptr;
                                                            const float* texCoordsBuffer = nullptr;
                                                            const uint16_t* jointIndicesBuffer = nullptr;
                                                            const float* jointWeightsBuffer = nullptr;
                                                            size_t vertexCount = 0;

                                                            /*Get Buffer data for vertex Position*/
                                                            if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) {
                                                                           const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
                                                                           const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                                                                           positionBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                                                                           vertexCount = accessor.count;
                                                            }

                                                            /* Get Buffer Data From Vertex Normals */
                                                            if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
                                                                           const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
                                                                           const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                                                                           normalsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                                                            }

                                                            /*Get Buffer Data From TexCoords*/
                                                            if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
                                                                           const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
                                                                           const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                                                                           texCoordsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                                                            }
                                                            // POI: Get buffer data required for vertex skinning
                                                            // Get vertex joint indices
                                                            if (glTFPrimitive.attributes.find("JOINTS_0") != glTFPrimitive.attributes.end()) {
                                                                           const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("JOINTS_0")->second];
                                                                           const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                                                                           jointIndicesBuffer =
                                                                                     reinterpret_cast<const uint16_t*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                                                            }
                                                            // Get vertex joint weights
                                                            if (glTFPrimitive.attributes.find("WEIGHTS_0") != glTFPrimitive.attributes.end()) {
                                                                           const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("WEIGHTS_0")->second];
                                                                           const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                                                                           jointWeightsBuffer =
                                                                                     reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                                                            }

                                                            hasSkin = (jointIndicesBuffer && jointWeightsBuffer);

                                                            /* Append data to Model's VertexBuffer */
                                                            for (size_t v = 0; v < vertexCount; v++) {
                                                                           Vertex3DExt vert{};
                                                                           vert.mPosition = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
                                                                           vert.mNormal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
                                                                           vert.mUV = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec2(0.0f);
                                                                           vert.mColor = glm::vec3(1.0f);
                                                                           vert.mJointIndices = hasSkin ? glm::vec4(glm::make_vec4(&jointIndicesBuffer[v * 4])) : glm::vec4(0.0f);
                                                                           vert.mJointWeights = hasSkin ? glm::make_vec4(&jointWeightsBuffer[v * 4]) : glm::vec4(0.0f);
                                                                           inVertexBuffer.push_back(inVertexCallBack(vert));
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
                                                                                          const uint32_t* buf =
                                                                                                    reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                                                                                          for (size_t index = 0; index < accessor.count; index++) {
                                                                                                         inIndexBuffer.push_back(inIndexCallBack(buf[index] + vertex_start));
                                                                                          }
                                                                                          break;
                                                                           }
                                                                           case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                                                                                          const uint16_t* buf =
                                                                                                    reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                                                                                          for (size_t index = 0; index < accessor.count; index++) {
                                                                                                         inIndexBuffer.push_back(inIndexCallBack(buf[index] + vertex_start));
                                                                                          }
                                                                                          break;
                                                                           }
                                                                           case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                                                                                          const uint8_t* buf =
                                                                                                    reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
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

// POI: Traverse the node hierarchy to the top-most parent to get the local matrix of the given node
glm::mat4 glTF_Model::GetNodeMatrix(glTF_Model::Node* inNode) {
               glm::mat4 NodeMatrix = inNode->GetLocalMatrix();
               glTF_Model::Node* CurrentParent = inNode->mParent;
               while (CurrentParent) {
                              NodeMatrix = CurrentParent->GetLocalMatrix() * NodeMatrix;
                              CurrentParent = CurrentParent->mParent;
               }
               return NodeMatrix;
}

// POI: Update the joint matrices from the current animation frame and pass them to the GPU
void glTF_Model::UpdateJoints(glTF_Model::Node* inNode) {
               if (inNode->mSkin > -1) {
                              // Update the joint matrices
                              glm::mat4 inverseTransform = glm::inverse(GetNodeMatrix(inNode));
                              Skin skin = mSkins[inNode->mSkin];
                              size_t numJoints = (uint32_t)skin.mJoints.size();
                              std::vector<glm::mat4> jointMatrices(numJoints);
                              for (size_t i = 0; i < numJoints; i++) {
                                             jointMatrices[i] = GetNodeMatrix(skin.mJoints[i]) * skin.mInverseBindMatrices[i];
                                             jointMatrices[i] = inverseTransform * jointMatrices[i];
                              }
                              // Update ssbo
                              // skin.ssbo.copyTo(jointMatrices.data(), jointMatrices.size() * sizeof(glm::mat4));
               }

               for (auto& child : inNode->mChildren) {
                              UpdateJoints(child.get());
               }
}

void glTF_Model::UpdateAnimation(float inDeltaTime) {
               if (mActiveAnimation > static_cast<ui>(mAnimations.size() - 1)) {
                              std::cout << "No animation with index " << mActiveAnimation << "\n";
                              return;
               }
               Animation& Animation = mAnimations[mActiveAnimation];
               Animation.mCurrentTime += inDeltaTime;
               if (Animation.mCurrentTime > Animation.mEnd) {
                              Animation.mCurrentTime -= Animation.mEnd;
               }

               for (auto& channel : Animation.mChannels) {
                              AnimationSampler& sampler = Animation.mSamplers[channel.mSamplerIndex];
                              for (size_t i = 0; i < sampler.mInputs.size() - 1; i++) {
                                             if (sampler.mInterpolation != "LINEAR") {
                                                            std::cout << "This sample only supports linear interpolations\n";
                                                            continue;
                                             }

                                             if ((Animation.mCurrentTime >= sampler.mInputs[i]) && (Animation.mCurrentTime <= sampler.mInputs[i + 1])) {
                                                            float a = (Animation.mCurrentTime - sampler.mInputs[i]) / (sampler.mInputs[i + 1] - sampler.mInputs[i]);
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
                                                                           q2.x = sampler.mOutputsVec4[i + 1].x;
                                                                           q2.y = sampler.mOutputsVec4[i + 1].y;
                                                                           q2.z = sampler.mOutputsVec4[i + 1].z;
                                                                           q2.w = sampler.mOutputsVec4[i + 1].w;

                                                                           channel.mNode->mRotation = glm::normalize(glm::slerp(q1, q2, a));
                                                            }
                                                            if (channel.mPath == "scale") {
                                                                           channel.mNode->mScale = glm::mix(sampler.mOutputsVec4[i], sampler.mOutputsVec4[i + 1], a);
                                                            }
                                             }
                              }
               }
               for (auto& node : mNodes) {
                              UpdateJoints(node.get());
               }
}

void glTF_Model::Draw(glTF_Model::Node& inNode, PushCallBack inPushCallBack, DrawCallBack inDrawCallBack) {
               if (inNode.mMesh.mPrimitives.size() > 0) {
                              glm::mat4 NodeMatrix = inNode.mMatrix;
                              glTF_Model::Node* CurrentParent = inNode.mParent;
                              while (CurrentParent) {
                                             NodeMatrix = CurrentParent->mMatrix * NodeMatrix;
                                             CurrentParent = CurrentParent->mParent;
                              }
                              inPushCallBack(NodeMatrix);
                              for (glTF_Model::Primitive& primitive : inNode.mMesh.mPrimitives) {
                                             if (primitive.mIndexCount > 0) {
                                                            glTF_Model::Texture texture = mTextures[mMaterials[primitive.mMaterialIndex].mBaseColorTextureIndex];
                                                            inDrawCallBack(0, texture);
                                             }
                              }
               }
}