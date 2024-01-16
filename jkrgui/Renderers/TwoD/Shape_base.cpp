#include "Shape_base.hpp"

using namespace Jkr;
using namespace Jkr::Renderer;

Generator::Generator(Shapes inShape, Arguments inArgs)
    : mArgs(inArgs)
    , mShape(inShape)
{
    switch (inShape) {
    case Shapes::Circle:
        mVertexCount = std::get<glm::uvec2>(mArgs).y;
        mIndexCount = std::get<glm::uvec2>(mArgs).y + 1;
        break;
    case Shapes::Rectangle_Fill:
        mVertexCount = 4;
        mIndexCount = 6;
        break;
    case Shapes::Bezier2_8:
        mVertexCount = 8;
        mIndexCount = 8;
    }
}

void Jkr::Generator::operator()(int inX, int inY, uint32_t inZ, uint32_t inStartVertexIndex, uint32_t inStartIndexIndex, std::vector<kstd::Vertex>& modVertices, std::vector<uint32_t>& modIndices)
{
    switch (mShape) {
    case Shapes::Circle: {
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
                modVertices[inStartVertexIndex + i] = kstd::Vertex{.mPosition = {ix + y,
                                                                                 iy + x,
                                                                                 inZ},
                                                                   .mTextureCoordinates = {ix, iy}

                };
                modIndices[inStartIndexIndex + i] = inStartVertexIndex + i;
                TH += dTH;
            }
            modIndices[inStartIndexIndex + i] = inStartVertexIndex;
        }
        break;
    }
    case Shapes::Rectangle_Fill: {
        int x = inX;
        int dx = std::get<glm::uvec2>(mArgs).x;
        int y = inY;
        int dy = std::get<glm::uvec2>(mArgs).y;
        modVertices.resize(4 + modVertices.size());
        modIndices.resize(6 + modIndices.size());
        const int v_index = inStartVertexIndex;
        const int i_index = inStartIndexIndex;
        modVertices[v_index + 0] = kstd::Vertex {
            .mPosition = { x, y + dy, inZ },
            .mTextureCoordinates = { 0, 1 }
        };

        modVertices[v_index + 1] = kstd::Vertex {
            .mPosition = { x, y, inZ },
            .mTextureCoordinates = { 0, 0 }
        };

        modVertices[v_index + 2] = kstd::Vertex {
            .mPosition = { x + dx, y, inZ },
            .mTextureCoordinates = { 1, 0 }
        };

        modVertices[v_index + 3] = kstd::Vertex {
            .mPosition = { x + dx, y + dy, inZ },
            .mTextureCoordinates = { 1, 1 }
        };
        modIndices[i_index + 0] = v_index + 0;
        modIndices[i_index + 1] = v_index + 1;
        modIndices[i_index + 2] = v_index + 2;
        modIndices[i_index + 3] = v_index + 0;
        modIndices[i_index + 4] = v_index + 2;
        modIndices[i_index + 5] = v_index + 3;
        break;
    }
    case Shapes::Bezier2_8: {
        modVertices.resize(8 + modVertices.size());
        modIndices.resize(8 + modIndices.size());
        const auto v_index = inStartVertexIndex;
        const auto i_index = inStartIndexIndex;
        float del_t = 1.0f / 8.0f;
        float t = 0.0f;
        auto& p = std::get<std::span<glm::uvec2>>(mArgs);
        auto p_o = glm::vec2(p[0].x, p[0].y);
        auto p_1 = glm::vec2(p[1].x, p[1].y);
        auto p_2 = glm::vec2(p[2].x, p[2].y);
        auto p_3 = glm::vec2(p[3].x, p[3].y);
        for (int i = 0; i < 8; i++) {
            const glm::vec2 pt = (1 - t) * (1 - t) * (1 - t) * p_o + 3 * (1 - t) * (1 - t) * t * p_1 + 3 * (1 - t) * t * t * p_2 + t * t * t * p_3;

            modVertices[v_index + i] = kstd::Vertex {
                .mPosition = glm::vec3(pt.x + inX, pt.y + inY, inZ),
                .mTextureCoordinates = glm::vec2(0, 0)
            };
            modIndices[i + i_index] = i + v_index;
            t += del_t;
        }
    } break;
    }
}

void Jkr::Renderer::Shape_base::Add(Jkr::Generator& inShape, int inX, int inY, uint32_t inZ, uint32_t& outId)
{
    inShape(inX, inY, inZ, mOffsetVertex, mOffsetIndices, mVertices, mIndices);
    mOffsetVertexOfShape.push_back(mOffsetVertex);
    mOffsetIndexOfShape.push_back(mOffsetIndices);
    mGenerators.push_back(inShape);
    mOffsetVertex += inShape.GetVertexCount();
    mOffsetIndices += inShape.GetIndexCount();
    mEndOffsetVertexOfShape.push_back(mOffsetVertex);
    mEndOffsetIndexOfShape.push_back(mOffsetIndices);
    outId = mNoOfShapes;
    mNoOfShapes++;
}

/*
        WARNING: The Shape Generator Here SHOULD NOT modify the number of Vertices / Indices
*/
void Jkr::Renderer::Shape_base::Update(Jkr::Generator& inShape, uint32_t inId, int inX, int inY, uint32_t inZ)
{
    uint32_t vcount = 0;
    uint32_t icount = 0;
    inShape(inX, inY, inZ, mOffsetVertexOfShape[inId], mOffsetIndexOfShape[inId], mVertices, mIndices);
}
