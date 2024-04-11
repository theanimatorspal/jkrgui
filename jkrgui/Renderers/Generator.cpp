#include "Generator.hpp"
#include "glm/fwd.hpp"

using namespace Jkr;

Generator::Generator(Shapes inShape, Arguments inArgs) : mArgs(inArgs), mShape(inShape) {
          switch (inShape) {
                    case Shapes::EllipseWire:
                              mVertexCount = std::get<glm::uvec2>(mArgs).y;
                              mIndexCount = std::get<glm::uvec2>(mArgs).y + 1;
                              break;
                    case Shapes::EllipseFill:
                              // TODO Implement
                              break;
                    case Shapes::RectangleFill:
                              mVertexCount = 4;
                              mIndexCount = 6;
                              break;
                    case Shapes::RectangleWire:
                              // TODO Implement
                              break;
                    case Shapes::Bezier2_8Wire:
                              mVertexCount = 8;
                              mIndexCount = 8;
                    case Shapes::Cube3D:
                              mVertexCount = 24;
                              mIndexCount = 36;
                              break;
                    case Shapes::Sphere3D:
                              // TODO Implement
                              break;
                    case Shapes::Cylinder3D:
                              // TODO Implement
                              break;
                    case Shapes::Cone3D:
                              // TODO Implement
                              break;
                    case Shapes::Torus3D:
                              // TODO Implement
                              break;
                    case Shapes::Icosphere3D:
                              // TODO Implement
                              break;
          }
}

void Jkr::Generator::operator()(int inX,
                                int inY,
                                uint32_t inZ,
                                uint32_t inStartVertexIndex,
                                uint32_t inStartIndexIndex,
                                std::vector<kstd::Vertex>& modVertices,
                                std::vector<uint32_t>& modIndices) {
          switch (mShape) {
                    // TODO Implement Ellipse (This is a Circle Implementation)
                    case Shapes::EllipseWire: {
                              float x = inX;
                              float y = inY;
                              float R = std::get<glm::uvec2>(mArgs).x;
                              modVertices.resize(mVertexCount + modVertices.size());
                              modIndices.resize(mIndexCount + modIndices.size());
                              float TH = 0;
                              float dTH = 2 * 3.14159 / mVertexCount;

                              {
                                        int i = 0;
                                        for (i = 0; i < mVertexCount; i++) {
                                                  const float ix = R * cos(TH);
                                                  const float iy = R * sin(TH);
                                                  modVertices[inStartVertexIndex + i] =
                                                            kstd::Vertex{.mPosition = {ix + y, iy + x, inZ}, .mTextureCoordinates = {ix, iy}

                                                            };
                                                  modIndices[inStartIndexIndex + i] = inStartVertexIndex + i;
                                                  TH += dTH;
                                        }
                                        modIndices[inStartIndexIndex + i] = inStartVertexIndex;
                              }
                              break;
                    }
                    case Shapes::EllipseFill: {
                              // TODO Implement
                    } break;
                    case Shapes::RectangleWire: {
                              // TODO Implement
                    } break;
                    case Shapes::RectangleFill: {
                              int x = inX;
                              int dx = std::get<glm::uvec2>(mArgs).x;
                              int y = inY;
                              int dy = std::get<glm::uvec2>(mArgs).y;
                              modVertices.resize(4 + modVertices.size());
                              modIndices.resize(6 + modIndices.size());
                              const int v_index = inStartVertexIndex;
                              const int i_index = inStartIndexIndex;
                              modVertices[v_index + 0] = kstd::Vertex{.mPosition = {x, y + dy, inZ}, .mTextureCoordinates = {0, 1}};

                              modVertices[v_index + 1] = kstd::Vertex{.mPosition = {x, y, inZ}, .mTextureCoordinates = {0, 0}};

                              modVertices[v_index + 2] = kstd::Vertex{.mPosition = {x + dx, y, inZ}, .mTextureCoordinates = {1, 0}};

                              modVertices[v_index + 3] = kstd::Vertex{.mPosition = {x + dx, y + dy, inZ}, .mTextureCoordinates = {1, 1}};
                              modIndices[i_index + 0] = v_index + 0;
                              modIndices[i_index + 1] = v_index + 1;
                              modIndices[i_index + 2] = v_index + 2;
                              modIndices[i_index + 3] = v_index + 0;
                              modIndices[i_index + 4] = v_index + 2;
                              modIndices[i_index + 5] = v_index + 3;
                              break;
                    }
                    case Shapes::Bezier2_8Wire: {
                              modVertices.resize(8 + modVertices.size());
                              modIndices.resize(8 + modIndices.size());
                              const auto v_index = inStartVertexIndex;
                              const auto i_index = inStartIndexIndex;
                              float del_t = 1.0f / 8.0f;
                              float t = 0.0f;
                              auto& p = std::get<std::vector<glm::uvec2>>(mArgs);
                              auto p_o = glm::vec2(p[0].x, p[0].y);
                              auto p_1 = glm::vec2(p[1].x, p[1].y);
                              auto p_2 = glm::vec2(p[2].x, p[2].y);
                              auto p_3 = glm::vec2(p[3].x, p[3].y);
                              for (int i = 0; i < 8; i++) {
                                        const glm::vec2 pt = (1 - t) * (1 - t) * (1 - t) * p_o + 3 * (1 - t) * (1 - t) * t * p_1 +
                                                             3 * (1 - t) * t * t * p_2 + t * t * t * p_3;

                                        modVertices[v_index + i] = kstd::Vertex{.mPosition = glm::vec3(pt.x + inX, pt.y + inY, inZ),
                                                                                .mTextureCoordinates = glm::vec2(0, 0)};
                                        modIndices[i + i_index] = i + v_index;
                                        t += del_t;
                              }
                    } break;
                    default:
                              break;
          }
}

void Jkr::Generator::operator()(float inX,
                                float inY,
                                float inZ,
                                uint32_t inStartVertexIndex,
                                uint32_t inStartIndexIndex,
                                ksai::v<kstd::Vertex3D>& modVertices,
                                ksai::v<ksai::ui>& modIndices) {
          switch (mShape) {
                    case Shapes::Cube3D: {
                              glm::vec3 Size = std::get<decltype(Size)>(mArgs);
                              modVertices.resize(mVertexCount + modVertices.size());
                              modIndices.resize(mIndexCount + modIndices.size());
                              float x = Size.x;
                              float y = Size.y;
                              float z = Size.z;
                              glm::vec3 positions[8] = {
                                        // Bottom
                                        {-x, -y, z},
                                        {x, -y, z},
                                        {x, -y, -z},
                                        {-x, -y, -z},
                                        // Top
                                        {-x, -y, z},
                                        {x, -y, z},
                                        {x, -y, -z},
                                        {-x, -y, -z},
                              };

                              glm::vec3 normals[6] = {
                                        {0, -1, 0}, // Bottom
                                        {0, 1, 0},  // Top
                                        {-1, 0, 0}, // Left
                                        {0, 0, 1},  // Right
                                        {0, 0, -1}, // Back
                                        {0, 0, 1},  // Front
                              };

                              glm::vec2 uvs[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};

                              unsigned int faceIndices[6][4] = {
                                        {0, 1, 2, 3}, // Bottom
                                        {4, 5, 6, 7}, // Top
                                        {0, 1, 5, 4}, // Left
                                        {3, 2, 6, 7}, // Right
                                        {0, 3, 7, 4}, // Back
                                        {0, 2, 6, 5}  // Front
                              };

                              int start = inStartVertexIndex;
                              for (int i = 0; i < 6; ++i) {
                                        for (int j = 0; j < 4; ++j) {
                                                  modVertices[start++] = {
                                                            .mPosition = positions[faceIndices[i][j]],
                                                            .mNormal = normals[i],
                                                            .mUV = uvs[j],
                                                            .mColor = glm::vec4(1, 1, 1, 1),
                                                  };
                                        }
                              }

                              start = inStartIndexIndex;
                              for (int i = 0; i < 6; ++i) {
                                        modIndices[start++] = inStartVertexIndex + (i * 4);
                                        modIndices[start++] = inStartVertexIndex + (i * 4 + 1);
                                        modIndices[start++] = inStartVertexIndex + (i * 4 + 2);
                                        modIndices[start++] = inStartVertexIndex + (i * 4);
                                        modIndices[start++] = inStartVertexIndex + (i * 4 + 2);
                                        modIndices[start++] = inStartVertexIndex + (i * 4 + 3);
                              }
                    } break;
                    case Shapes::Sphere3D: {
                    } break;
                    case Shapes::Cylinder3D: {
                    } break;
                    case Shapes::Cone3D: {
                    } break;
                    case Shapes::Torus3D: {
                    } break;
                    case Shapes::Icosphere3D: {
                    } break;
                    default:
                              break;
          }
}