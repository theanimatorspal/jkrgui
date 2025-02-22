#include "Generator.hpp"
#include "Global/Standards.hpp"
#include "glm/fwd.hpp"

using namespace Jkr;

Generator::Generator(Shapes inShape, Arguments inArgs) : mArgs(inArgs), mShape(inShape) {
    try {
        switch (inShape) {
            case Shapes::EllipseWire:
                mVertexCount = std::get<glm::uvec2>(mArgs).y;
                mIndexCount  = std::get<glm::uvec2>(mArgs).y + 1;
                break;
            case Shapes::EllipseFill:
                // TODO Implement
                break;
            case Shapes::RectangleFill:
                mVertexCount = 4;
                mIndexCount  = 6;
                break;
            case Shapes::RectangleWire:
                // TODO Implement
                break;
            case Shapes::Bezier2_8Wire:
                mVertexCount = 8;
                mIndexCount  = 8;
                break;
            case Shapes::Zeros3D: {
                glm::vec2 vi_size = std::get<glm::vec2>(mArgs);
                mVertexCount      = vi_size.x;
                mIndexCount       = vi_size.y;
            } break;
            case Shapes::Triangles3D: {
                glm::vec2 vi_size = std::get<glm::vec2>(mArgs);
                mVertexCount      = vi_size.x;
                mIndexCount       = vi_size.y;
            } break;
            case Shapes::Cube3D:
                mVertexCount = 24;
                mIndexCount  = 36;
                break;
            case Shapes::Sphere3D: {
                const glm::vec4 r_s2d_s3d = std::get<glm::vec4>(mArgs);
                const float r             = r_s2d_s3d.x;
                const ui s2d              = static_cast<ui>(r_s2d_s3d.y);
                const ui s3d              = static_cast<ui>(r_s2d_s3d.z);
                mVertexCount              = 2 + s2d * s3d;
                mIndexCount               = 3 * s2d + s2d * s3d * 6;
            } break;
            case Shapes::Cylinder3D: {
                const glm::vec3 rh     = std::get<glm::vec3>(mArgs);
                const float r          = rh.x;
                const float h          = rh.y;
                const int SegmentCount = static_cast<ui>(rh.z);
                const float DelTheta   = 360.0f / (float)SegmentCount;
                mIndexCount            = SegmentCount * 12;
                mVertexCount           = SegmentCount * 10;
            } break;
            case Shapes::Cone3D:
                // TODO Implement
                break;
            case Shapes::Torus3D:
                // TODO Implement
                break;
            case Shapes::Icosphere3D:
                // TODO Implement
                break;
            case Shapes::CustomShape3D:
                auto &CustomShape = std::get<CustomShape3D>(mArgs);
                mVertexCount      = CustomShape.mVertices.size();
                mIndexCount       = CustomShape.mIndices.size();
                break;
        }
    } catch (const std::exception &e) {
        // throw std::exception("The parameters passed to generate the object is not compatible.");
        Log("The parameters passed to generate the object is not compatible.\n");
    }
}

void Jkr::Generator::operator()(float inX,
                                float inY,
                                float inZ,
                                uint32_t inStartVertexIndex,
                                uint32_t inStartIndexIndex,
                                std::vector<kstd::Vertex> &modVertices,
                                std::vector<uint32_t> &modIndices) {
    switch (mShape) {
        // TODO Implement Ellipse (This is a Circle Implementation)
        case Shapes::EllipseWire: {
            float x = inX;
            float y = inY;
            float R = std::get<glm::uvec2>(mArgs).x;
            modVertices.resize(mVertexCount + modVertices.size());
            modIndices.resize(mIndexCount + modIndices.size());
            float TH  = 0;
            float dTH = 2 * 3.14159 / mVertexCount;

            {
                int i = 0;
                for (i = 0; i < mVertexCount; i++) {
                    const float ix                      = R * cos(TH);
                    const float iy                      = R * sin(TH);
                    modVertices[inStartVertexIndex + i] = kstd::Vertex{
                         .mPosition = {ix + y, iy + x, inZ}, .mTextureCoordinates = {ix, iy}

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
            int x  = inX;
            int dx = std::get<glm::uvec2>(mArgs).x;
            int y  = inY;
            int dy = std::get<glm::uvec2>(mArgs).y;
            modVertices.resize(4 + modVertices.size());
            modIndices.resize(6 + modIndices.size());
            const int v_index = inStartVertexIndex;
            const int i_index = inStartIndexIndex;
            modVertices[v_index + 0] =
                 kstd::Vertex{.mPosition = {x, y + dy, inZ}, .mTextureCoordinates = {0, 1}};

            modVertices[v_index + 1] =
                 kstd::Vertex{.mPosition = {x, y, inZ}, .mTextureCoordinates = {0, 0}};

            modVertices[v_index + 2] =
                 kstd::Vertex{.mPosition = {x + dx, y, inZ}, .mTextureCoordinates = {1, 0}};

            modVertices[v_index + 3] =
                 kstd::Vertex{.mPosition = {x + dx, y + dy, inZ}, .mTextureCoordinates = {1, 1}};
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
            float del_t        = 1.0f / 8.0f;
            float t            = 0.0f;
            auto &p            = std::get<std::vector<glm::uvec2>>(mArgs);
            auto p_o           = glm::vec2(p[0].x, p[0].y);
            auto p_1           = glm::vec2(p[1].x, p[1].y);
            auto p_2           = glm::vec2(p[2].x, p[2].y);
            auto p_3           = glm::vec2(p[3].x, p[3].y);
            for (int i = 0; i < 8; i++) {
                const glm::vec2 pt = (1 - t) * (1 - t) * (1 - t) * p_o +
                                     3 * (1 - t) * (1 - t) * t * p_1 + 3 * (1 - t) * t * t * p_2 +
                                     t * t * t * p_3;

                modVertices[v_index + i] =
                     kstd::Vertex{.mPosition           = glm::vec3(pt.x + inX, pt.y + inY, inZ),
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
                                ksai::v<kstd::Vertex3D> &modVertices,
                                ksai::v<ksai::ui> &modIndices) {
    switch (mShape) {
        case Shapes::Zeros3D: {
            modVertices.resize(mVertexCount + modVertices.size(), {});
            modIndices.resize(mIndexCount + modIndices.size(), {});
        } break;
        case Shapes::CustomShape3D: {
            modVertices.resize(mVertexCount + modVertices.size(), {});
            modIndices.resize(mIndexCount + modIndices.size(), {});
            auto &CustomShape = std::get<CustomShape3D>(mArgs);
            {
                int i = 0;
                for (auto &Vertex : CustomShape.mVertices) {
                    modVertices[i] = Vertex;
                    ++i;
                }
            }
            {
                int i = 0;
                for (auto index : CustomShape.mIndices) {
                    modIndices[inStartIndexIndex + i] = index + inStartVertexIndex;
                    ++i;
                }
            }
        } break;
        case Shapes::Triangles3D: {
            modVertices.resize(mVertexCount * 3 + modVertices.size(), {});
            modIndices.resize(mIndexCount * 3 + modIndices.size(), {});
            int start = inStartIndexIndex;
            for (int i = 0; i < mIndexCount; ++i) {
                modIndices[start++] = inStartVertexIndex + (i * 3);
                modIndices[start++] = inStartVertexIndex + (i * 3 + 1);
                modIndices[start++] = inStartVertexIndex + (i * 3 + 2);
            }
        } break;
        case Shapes::Cube3D: {
            glm::vec3 Size = std::get<decltype(Size)>(mArgs);
            modVertices.resize(mVertexCount + modVertices.size());
            modIndices.resize(mIndexCount + modIndices.size());
            float x                = Size.x;
            float y                = Size.y;
            float z                = Size.z;
            glm::vec3 positions[8] = {
                 // Bottom
                 {-x, -y, z},
                 {x, -y, z},
                 {x, -y, -z},
                 {-x, -y, -z},
                 // Top
                 {-x, y, z},
                 {x, y, z},
                 {x, y, -z},
                 {-x, y, -z},
            };

            glm::vec3 normals[6] = {
                 {0, -1, 0}, // Bottom
                 {0, 1, 0},  // Top
                 {-1, 0, 0}, // Left
                 {0, 0, 1},  // Right
                 {0, 0, -1}, // Back
                 {0, 0, 1},  // Front
            };

            glm::vec2 uvs[4]               = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};

            unsigned int faceIndices[6][4] = {
                 {0, 1, 2, 3}, // Bottom
                 {4, 5, 6, 7}, // Top
                 {0, 1, 5, 4}, // Left
                 {3, 2, 6, 7}, // Right
                 {0, 3, 7, 4}, // Back
                 {1, 2, 6, 5}  // Front
            };

            ///@warning here the modVertices that are actually the vertices with 0 size
            /// so start = 0
            int start = 0;
            for (int i = 0; i < 6; ++i) {
                for (int j = 0; j < 4; ++j) {
                    int FaceIndex        = faceIndices[i][j];
                    modVertices[start++] = {
                         .mPosition = positions[FaceIndex],
                         .mNormal   = normals[i],
                         .mUV       = uvs[j],
                         .mColor    = glm::vec4(1),
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
            modVertices.resize(mVertexCount + modVertices.size());
            modIndices.resize(mIndexCount + modIndices.size());
            const glm::vec4 r_s2d_s3d  = std::get<glm::vec4>(mArgs);
            const float r              = r_s2d_s3d.x;
            const ui s2d               = static_cast<ui>(r_s2d_s3d.y);
            const ui s3d               = static_cast<ui>(r_s2d_s3d.z);
            const int topPlaneY        = r;
            const int bottomPlaneY     = -r;
            constexpr float pi         = 3.14159; // TODO increase precision
            const float DelTheta       = 2 * pi / static_cast<float>(s2d);
            const float DelPhi         = pi / static_cast<float>(s3d);
            ui vIndex                  = 0;
            ui iIndex                  = inStartIndexIndex;
            kstd::Vertex3D UpperCenter = {.mPosition = glm::vec3(0, topPlaneY, 0),
                                          .mNormal   = glm::vec3(0, 0, 0),
                                          .mUV       = glm::vec2(0, 0),
                                          .mColor    = glm::vec3(0, 0, 0)};
            kstd::Vertex3D LowerCenter = {.mPosition = glm::vec3(0, bottomPlaneY, 0),
                                          .mNormal   = glm::vec3(0, 0, 0),
                                          .mUV       = glm::vec2(0, 0),
                                          .mColor    = glm::vec3(0, 0, 0)};
            float Theta                = 0.0f;
            float Phi                  = 0.0f;
            using namespace kstd;
            // TODO Complete this
        } break;
        case Shapes::Cylinder3D: {
            modVertices.resize(mVertexCount + modVertices.size());
            modIndices.resize(mIndexCount + modIndices.size());
            const glm::vec3 rh     = std::get<glm::vec3>(mArgs);
            const float r          = rh.x;
            const float h          = rh.y;
            const int SegmentCount = static_cast<ui>(rh.z);
            constexpr float pi     = 3.14159; // TODO increase precision
            const float DelTheta   = 2 * pi / static_cast<float>(SegmentCount);
            ui vIndex              = inStartVertexIndex;
            ui iIndex              = inStartIndexIndex;

            int topPlaneY          = -1 * h;
            int bottomPlaneY       = 1 * h;
            float Theta            = 0.0f;
            for (int i = 0; i < SegmentCount; i++) {
                kstd::Vertex3D UpperCenter;
                kstd::Vertex3D UpperLeft;
                kstd::Vertex3D UpperRight;
                kstd::Vertex3D LowerCenter;
                kstd::Vertex3D LowerLeft;
                kstd::Vertex3D LowerRight;

                float xLeft           = r * sin(Theta);
                float zLeft           = r * cos(Theta);
                float xRight          = r * sin(Theta + DelTheta);
                float zRight          = r * cos(Theta + DelTheta);

                UpperCenter           = {.mPosition = glm::vec3{0, topPlaneY, 0},
                                         .mNormal   = glm::vec3{0, 1, 0},
                                         .mUV       = glm::vec2{0},
                                         .mColor    = glm::vec4{1, 1, 1, 1}};
                UpperLeft             = {.mPosition = glm::vec3{xLeft, topPlaneY, zLeft},
                                         .mNormal   = glm::vec3{0, 1, 0},
                                         .mUV       = glm::vec2{0},
                                         .mColor    = glm::vec4{1, 1, 1, 1}};
                UpperRight            = {.mPosition = glm::vec3{xRight, topPlaneY, zRight},
                                         .mNormal   = glm::vec3{0, 1, 0},
                                         .mUV       = glm::vec2{0},
                                         .mColor    = glm::vec4{1, 1, 1, 1}};

                LowerCenter           = {.mPosition = glm::vec3{0, bottomPlaneY, 0},
                                         .mNormal   = glm::vec3{0, -1, 0},
                                         .mUV       = glm::vec2{0},
                                         .mColor    = glm::vec4{1, 1, 1, 1}};
                LowerLeft             = {.mPosition = glm::vec3{xLeft, bottomPlaneY, zLeft},
                                         .mNormal   = glm::vec3{0, -1, 0},
                                         .mUV       = glm::vec2{0},
                                         .mColor    = glm::vec4{1, 1, 1, 1}};
                LowerRight            = {.mPosition = glm::vec3{xRight, bottomPlaneY, zRight},
                                         .mNormal   = glm::vec3{0, -1, 0},
                                         .mUV       = glm::vec2{0},
                                         .mColor    = glm::vec4{1, 1, 1, 1}};

                modVertices[vIndex++] = UpperCenter;
                modVertices[vIndex++] = UpperLeft;
                modVertices[vIndex++] = UpperRight;

                {
                    using namespace glm;
                    vec3 A                = UpperLeft.mPosition;
                    vec3 B                = LowerRight.mPosition;
                    vec3 C                = UpperRight.mPosition;
                    vec3 AB               = B - A;
                    vec3 AC               = C - A;
                    vec3 Normal           = cross(AB, AC);

                    auto UpperLeft_       = UpperLeft;
                    auto UpperRight_      = UpperRight;
                    auto LowerRight_      = LowerRight;
                    auto LowerLeft_       = LowerLeft;
                    UpperLeft_.mNormal    = Normal;
                    LowerLeft_.mNormal    = Normal;
                    LowerRight_.mNormal   = Normal;
                    UpperRight_.mNormal   = Normal;

                    modVertices[vIndex++] = UpperLeft_;
                    modVertices[vIndex++] = LowerLeft_;
                    modVertices[vIndex++] = LowerRight_;
                    modVertices[vIndex++] = UpperRight_;
                }

                modVertices[vIndex++] = LowerCenter;
                modVertices[vIndex++] = LowerLeft;
                modVertices[vIndex++] = LowerRight;

                modIndices[iIndex++]  = inStartVertexIndex + i * 10 + 0;
                modIndices[iIndex++]  = inStartVertexIndex + i * 10 + 1;
                modIndices[iIndex++]  = inStartVertexIndex + i * 10 + 2;

                modIndices[iIndex++]  = inStartVertexIndex + i * 10 + 3;
                modIndices[iIndex++]  = inStartVertexIndex + i * 10 + 4;
                modIndices[iIndex++]  = inStartVertexIndex + i * 10 + 5;
                modIndices[iIndex++]  = inStartVertexIndex + i * 10 + 3;
                modIndices[iIndex++]  = inStartVertexIndex + i * 10 + 5;
                modIndices[iIndex++]  = inStartVertexIndex + i * 10 + 6;

                modIndices[iIndex++]  = inStartVertexIndex + i * 10 + 7;
                modIndices[iIndex++]  = inStartVertexIndex + i * 10 + 8;
                modIndices[iIndex++]  = inStartVertexIndex + i * 10 + 9;
                Theta += DelTheta;
            }
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