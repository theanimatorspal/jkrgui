#include "BestText_base.hpp"
#include <Vendor/stbi/stb_image_write.h>

Jkr::Renderer::BestText_base::BestText_base()
{
    if (FT_Init_FreeType(&mFtLibrary)) {
        std::cout << "Cnnot initialize freetype\n";
        exit(1);
    }
}

Jkr::Renderer::BestText_base::~BestText_base()
{
    for (int i = 0; i < mFontFaceCount; ++i) {
        hb_font_destroy(mHbFonts[i]);
        FT_Done_Face(mFaces[i]);
    }
    FT_Done_FreeType(mFtLibrary);
}

void Jkr::Renderer::BestText_base::AddFontFace(const std::string_view inFontFilePathName, size_t inFontSize, uint32_t& outFontId)
{
    uint32_t FaceIndex = mFontFaceCount++;
    if (FT_New_Face(mFtLibrary, inFontFilePathName.data(), 0, &mFaces[FaceIndex])) {
        std::cout << "Font Face Load Failed" << '\n';
    }
    if (FT_Set_Char_Size(mFaces[FaceIndex], ToFontUnits(inFontSize), ToFontUnits(inFontSize), 300, 300)) {
        std::cout << "Font Char Size Set Failed" << '\n';
    }
    FT_Set_Transform(mFaces[FaceIndex], 0, 0);
    mHbFonts[FaceIndex] = hb_ft_font_create(mFaces[FaceIndex], 0);
    outFontId = FaceIndex;
}

Jkr::Renderer::BestText_base::TextDimensions Jkr::Renderer::BestText_base::AddText(uint32_t inX, uint32_t inY, const std::string_view inString, uint32_t inFontShapeId, uint32_t inDepthValue, std::vector<uint32_t>& outCodePoints, uint32_t& outId)
{
    hb_buffer_t* hbBuffer = hb_buffer_create();
    hb_buffer_add_utf8(hbBuffer, reinterpret_cast<const char*>(inString.data()), -1, 0, -1);
    hb_buffer_guess_segment_properties(hbBuffer);
    hb_shape(mHbFonts[inFontShapeId], hbBuffer, 0, 0);
    unsigned int len = hb_buffer_get_length(hbBuffer);
    hb_glyph_info_t* info = hb_buffer_get_glyph_infos(hbBuffer, 0);
    hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(hbBuffer, 0);

    LoadTextToKeyMap(len, inFontShapeId, info, pos);
    auto TextDims = GetTextDimensions(inString, inFontShapeId, info, pos, len);

    for (int i = 0; i < len; i++)
        outCodePoints.push_back(info[i].codepoint);

    uint32_t x, y;
    switch (mCurrentTextProp.H) {
    case AlignH::Left:
        x = inX;
        break;
    case AlignH::Right:
        x = inX - TextDims.mWidth;
        break;
    case AlignH::Center:
        x = inX - TextDims.mWidth / 2.0f;
        break;
    }

    switch (mCurrentTextProp.V) {
    case AlignV::Bottom:
        y = inY;
        break;
    case AlignV::Top:
        y = inY + TextDims.mHeight;
        break;
    case AlignV::Center:
        y = inY + TextDims.mHeight / 2.0f;
        break;
    }

    /*
            In Accordance with TextProperty modify this TODO
    */
    AddRespectiveVerticesAndIndicesAt(len, mCharQuadGlyphCount, inDepthValue, inFontShapeId, info, x, y);
    outId = mCharQuadGlyphCount;
    mCharQuadGlyphCount += len;
    hb_buffer_destroy(hbBuffer);
    return TextDims;
}

Jkr::Renderer::BestText_base::TextDimensions Jkr::Renderer::BestText_base::UpdateText(uint32_t inX, uint32_t inY, uint32_t inId, const std::string_view inString, uint32_t inFontShapeId, uint32_t inDepthValue, std::vector<uint32_t>& outCodePoints)
{
    hb_buffer_t* hbBuffer = hb_buffer_create();
    hb_buffer_add_utf8(hbBuffer, reinterpret_cast<const char*>(inString.data()), -1, 0, -1);
    hb_buffer_guess_segment_properties(hbBuffer);
    hb_shape(mHbFonts[inFontShapeId], hbBuffer, 0, 0);
    unsigned int len = hb_buffer_get_length(hbBuffer);
    hb_glyph_info_t* info = hb_buffer_get_glyph_infos(hbBuffer, 0);
    hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(hbBuffer, 0);

    LoadTextToKeyMap(len, inFontShapeId, info, pos);
    auto TextDims = GetTextDimensions(inString, inFontShapeId, info, pos, len);

    for (int i = 0; i < len; i++)
        outCodePoints.push_back(info[i].codepoint);

    uint32_t x, y;
    switch (mCurrentTextProp.H) {
    case AlignH::Left:
        x = inX;
        break;
    case AlignH::Right:
        x = inX - TextDims.mWidth;
        break;
    case AlignH::Center:
        x = inX - TextDims.mWidth / 2.0f;
        break;
    }

    switch (mCurrentTextProp.V) {
    case AlignV::Bottom:
        y = inY + TextDims.mHeight;
        break;
    case AlignV::Top:
        y = inY;
        break;
    case AlignV::Center:
        y = inY + TextDims.mHeight / 2.0f;
        break;
    }
    /*
            In Accordance with TextProperty modify this TODO
    */
    AddRespectiveVerticesAndIndicesAt(len, inId, inDepthValue, inFontShapeId, info, x, y);

    hb_buffer_destroy(hbBuffer);
    return TextDims;
}

Jkr::Renderer::BestText_base::TextDimensions Jkr::Renderer::BestText_base::GetTextDimensions(const std::string_view inString, uint32_t inFontShapeId, hb_glyph_info_t* info, hb_glyph_position_t* pos, uint32_t len)
{
    int originX = 0, originY = 0;
    int minX = INT_MAX;
    int maxX = INT_MIN;
    int minY = INT_MAX;
    int maxY = INT_MIN;
    for (int i = 0; i < len; ++i) {
        CharacterKey key = { .mFontShapeId = inFontShapeId, .mGlyphCodePoint = info[i].codepoint };
        const auto& CharacterInMap = mCharacterBitmapSet[key];
        const auto& info = CharacterInMap.first.mGlyphInfo;
        const auto& metrics = CharacterInMap.first.mGlyphMetrics;
        const auto& pos = CharacterInMap.first.mGlyphPos;
        const auto bitmap_width = CharacterInMap.first.mBitmapWidth;
        const auto bitmap_rows = CharacterInMap.first.mBitmapRows;

        int offsetX = ToPixels(pos.x_offset + metrics.horiBearingX);
        int offsetY = ToPixels(pos.y_offset + metrics.horiBearingY);
        int glyphMinX = originX + offsetX;
        int glyphMaxX = originX + bitmap_width + offsetX;
        int glyphMinY = originY - bitmap_rows + offsetY;
        int glyphMaxY = originY + offsetY;

        if (glyphMinX < minX)
            minX = glyphMinX;
        if (glyphMaxX > maxX)
            maxX = glyphMaxX;
        if (glyphMinY < minY)
            minY = glyphMinY;
        if (glyphMaxY > maxY)
            maxY = glyphMaxY;
        originX += ToPixels(pos.x_advance);
    }

    originX = -minX;
    originY = -minY;
    size_t width = maxX - minX + 1;
    size_t height = maxY - minY + 1;

    return TextDimensions { .mWidth = width, .mHeight = height };
}

void Jkr::Renderer::BestText_base::AddRespectiveVerticesAndIndicesAt(unsigned int len, uint32_t inStartIndex, uint32_t inDepthValue, const uint32_t& inFontShapeId, hb_glyph_info_t* info, int inOriginX, int inOriginY)
{

    int originX = inOriginX, originY = inOriginY;

    for (int i = 0; i < len; ++i) {
        CharacterKey key = { .mFontShapeId = inFontShapeId, .mGlyphCodePoint = info[i].codepoint };
        const auto& CharacterInMap = mCharacterBitmapSet[key];
        const auto& bmp = CharacterInMap.second;
        const auto& info = CharacterInMap.first.mGlyphInfo;
        const auto& metrics = CharacterInMap.first.mGlyphMetrics;
        const auto& pos = CharacterInMap.first.mGlyphPos;
        const auto bitmap_width = CharacterInMap.first.mBitmapWidth;
        const auto bitmap_rows = CharacterInMap.first.mBitmapRows;
        int offsetX = ToPixels(pos.x_offset + metrics.horiBearingX);
        int offsetY = ToPixels(pos.y_offset + metrics.horiBearingY);
        int glyphMinX = originX + offsetX;
        int glyphMaxX = originX + bitmap_width + offsetX;
        int glyphMinY = originY + (bitmap_rows - offsetY);
        int glyphMaxY = originY - offsetY;

        int drawX = originX + ToPixels(pos.x_offset + metrics.horiBearingX);
        int drawY = originY + ToPixels(pos.y_offset + metrics.horiBearingY);
        [[maybe_unused]] size_t width = glyphMaxX - glyphMinX + 1;
        [[maybe_unused]] size_t height = glyphMaxY - glyphMinY + 1;

        /* Add a Single Quad */
        {
            auto inTextSize = len;
            if (mVertices.size() < 4 * inTextSize + inStartIndex * 4) {
                mVertices.resize(4 * inTextSize + inStartIndex * 4);
                mIndices.resize(6 * inTextSize + inStartIndex * 6);
            }

            using namespace ksai;

            const auto v_index = i * 4 + inStartIndex * 4;

            mVertices[v_index + 0] = kstd::VertexEXT {
                .mPosition = { glyphMinX, glyphMaxY, inDepthValue },
                .mTextureCoordinates = { 0, 0 },
                .mIvec3 = { 0, 0, 0 }
            };

            mVertices[v_index + 1] = kstd::VertexEXT {
                .mPosition = { glyphMinX, glyphMinY, inDepthValue },
                .mTextureCoordinates = { 0, 1 },
                .mIvec3 = { 0, 0, 0 }
            };

            mVertices[v_index + 2] = kstd::VertexEXT {
                .mPosition = { glyphMaxX, glyphMinY, inDepthValue },
                .mTextureCoordinates = { 1, 1 },
                .mIvec3 = { 0, 0, 0 }
            };

            mVertices[v_index + 3] = kstd::VertexEXT {
                .mPosition = { glyphMaxX, glyphMaxY, inDepthValue },
                .mTextureCoordinates = { 1, 0 },
                .mIvec3 = { 0, 0, 0 }
            };

            const auto i_index = i * 6 + inStartIndex * 6;

            mIndices[i_index + 0] = v_index + 0;
            mIndices[i_index + 1] = v_index + 1;
            mIndices[i_index + 2] = v_index + 2;
            mIndices[i_index + 3] = v_index + 0;
            mIndices[i_index + 4] = v_index + 2;
            mIndices[i_index + 5] = v_index + 3;

            originX += ToPixels(pos.x_advance);
        }
    }
}

void Jkr::Renderer::BestText_base::LoadTextToKeyMap(unsigned int len, const uint32_t& inFontShapeId, hb_glyph_info_t* info, hb_glyph_position_t* pos)
{
    /* Load Characters To Keymap */
    {
        for (int i = 0; i < len; ++i) {

            CharacterKey key = { .mFontShapeId = inFontShapeId, .mGlyphCodePoint = info[i].codepoint };

            if (not mCharacterBitmapSet.contains(key)) {
                if (FT_Load_Glyph(mFaces[inFontShapeId], info[i].codepoint, FT_LOAD_RENDER))
                    std::cout << "Error Font Glyph loading" << '\n';
                FT_GlyphSlot slot = mFaces[inFontShapeId]->glyph;
                CharacterInfo character_info {
                    .mGlyphPos = pos[i],
                    .mGlyphInfo = info[i],
                    .mGlyphMetrics = slot->metrics,
                    .mBitmapWidth = slot->bitmap.width,
                    .mBitmapRows = slot->bitmap.rows,
                    .mBitmapPitch = slot->bitmap.pitch
                };
                std::vector<uint8_t> BitmapImage;
                uint8_t* ptr = slot->bitmap.buffer;
                int originX = 0, originY = 0;
                int offsetX = ToPixels(pos[i].x_offset + slot->metrics.horiBearingX);
                int offsetY = ToPixels(pos[i].y_offset + slot->metrics.horiBearingY);
                int glyphMinX = originX + offsetX;
                int glyphMaxX = originX + slot->bitmap.width + offsetX;
                int glyphMinY = originY - slot->bitmap.rows + offsetY;
                int glyphMaxY = originY + offsetY;
                originX = -glyphMinX;
                originY = -glyphMinY;
                int drawX = originX + ToPixels(pos[i].x_offset + slot->metrics.horiBearingX);
                int drawY = originY + ToPixels(pos[i].y_offset + slot->metrics.horiBearingY);
                [[maybe_unused]] size_t width = glyphMaxX - glyphMinX + 1;
                [[maybe_unused]] size_t height = glyphMaxY - glyphMinY + 1;
                BitmapImage.resize(mImageChannelCount * width * height);
                auto* dst = BitmapImage.data();

                // for (size_t y = 0; y < slot->bitmap.rows; ++y)
                //{
                //	for (size_t x = 0; x < slot->bitmap.width; ++x)
                //	{
                //		BitmapImage[(drawY - y) * width + drawX + x] = ptr[x];
                //	}
                //	ptr += slot->bitmap.pitch;
                // }
                if (mImageChannelCount == 4) {
                    uint32_t i = 0;
                    for (size_t y = 0; y < slot->bitmap.rows; ++y) {
                        for (size_t x = 0; x < slot->bitmap.width; ++x) {

                            dst[i++] = ptr[x];
                            dst[i++] = ptr[x];
                            dst[i++] = ptr[x];
                            dst[i++] = ptr[x];
                        }
                        ptr += slot->bitmap.pitch;
                    }
                }

                mCharacterBitmapSet[key] = std::make_pair(character_info, std::move(BitmapImage));
            }
        }
    }
}

Jkr::Renderer::BestText_base::TextDimensions Jkr::Renderer::BestText_base::GetTextDimensions(const std::string_view inString, uint32_t inFontShapeId)
{
    hb_buffer_t* hbBuffer = hb_buffer_create();
    hb_buffer_add_utf8(hbBuffer, reinterpret_cast<const char*>(inString.data()), -1, 0, -1);
    hb_buffer_guess_segment_properties(hbBuffer);
    hb_shape(mHbFonts[inFontShapeId], hbBuffer, 0, 0);
    unsigned int len = hb_buffer_get_length(hbBuffer);
    hb_glyph_info_t* info = hb_buffer_get_glyph_infos(hbBuffer, 0);
    hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(hbBuffer, 0);
    auto td = GetTextDimensions(inString, inFontShapeId, info, pos, len);
    hb_buffer_destroy(hbBuffer);
    return td;
}
