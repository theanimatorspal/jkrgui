
/*
        ॥ मङ्गलाचरणम् ॥
        ब्रह्मा रुद्रः कुमारो हरिवरुणयमा वह्निरिन्द्रः कुबेर
        श्चन्द्रादित्यौ सरस्वत्युदधियुगनगा वायुरुर्वी भुजङ्गाः ।
        सिद्धा नद्योऽश्विनौ श्रीर्दितिरदितिसुता मातरश्चण्डिकाद्या
        वेदस्तीर्थानि यज्ञा गणवसुमुनयः पान्तु नित्यं ग्रहाश्च ॥
*/

#pragma once
#include <array>
#include <ft2build.h>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include FT_FREETYPE_H
#include <Global/Standards.hpp>
#include <glm/glm.hpp>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>

namespace Jkr::Renderer {
using namespace ksai;
class BestText_base {
public:
    struct TextDimensions {
        size_t mWidth;
        size_t mHeight;
    };
    enum class AlignH { Center,
        Left,
        Right };
    enum class AlignV { Center,
        Top,
        Bottom };
    struct TextProperty {
        AlignH H = AlignH::Left;
        AlignV V = AlignV::Bottom;
    };

public:
    BestText_base();
    ~BestText_base();
    void AddFontFace(const std::string_view inFontFilePathName, size_t inFontSize, uint32_t& outFontId);
    TextDimensions GetTextDimensions(const std::string_view inString, uint32_t inFontShapeId);
    void SetTextProperty(TextProperty inProp) { mCurrentTextProp = inProp; };
    GETTER GetTextProperty() const { return mCurrentTextProp; };

protected:
    TextDimensions GetTextDimensions(const std::string_view inString, uint32_t inFontShapeId, hb_glyph_info_t* info, hb_glyph_position_t* pos, uint32_t len);
    TextDimensions AddText(uint32_t inX, uint32_t inY, const std::string_view inString, uint32_t inFontShapeId, uint32_t inDepthValue, std::vector<uint32_t>& outCodePoints, uint32_t& outIdt);
    TextDimensions RenderTextToImage(std::string_view inString, uint32_t inFontShapeId, std::vector<uc>& outImage);

public:
    [[nodiscard]] v<uc> RenderTextToImage(ui inFontShapeId, sv inStringView, TextDimensions& outDimens)
    {
        v<uc> img;
        outDimens = RenderTextToImage(inStringView, inFontShapeId, img);
        return img;
    }

protected:
    TextDimensions UpdateText(uint32_t inX, uint32_t inY, uint32_t inId, const std::string_view inString, uint32_t inFontShapeId, uint32_t inDepthValue, std::vector<uint32_t>& outCodePoints);
    void FillTextureIndexDataInVertexBufferAt(uint32_t inTextureId, uint32_t inAtIndex)
    {
        mVertices[inAtIndex].mIvec3 = { inTextureId, inTextureId, inTextureId };
    }
    GETTER CharCountToVertexBytes(size_t inCharCount) { return 4 * sizeof(ksai::kstd::VertexEXT) * inCharCount; }
    GETTER CharCountToIndexBytes(size_t inCharCount) { return 6 * sizeof(uint32_t) * inCharCount; }
    GETTER GetCurrentCharOffsetAbsolute() const { return mCharQuadGlyphCount; }
    GETTER GetVertexBufferData() { return reinterpret_cast<void*>(mVertices.data()); }
    GETTER GetIndexBufferData() { return reinterpret_cast<void*>(mIndices.data()); }
    void Resize(uint32_t inNewSize)
    {
        mVertices.reserve(CharCountToVertexBytes(inNewSize));
        mIndices.reserve(CharCountToIndexBytes(inNewSize));
    }
    GETTER GetGlyphTextureData(uint32_t inFaceId, uint32_t inCodePoint, uint32_t& outWidth, uint32_t& outHeight, uint32_t& outChannelCount)
    {
        CharacterKey key { .mFontShapeId = inFaceId, .mGlyphCodePoint = inCodePoint };
        outWidth = mCharacterBitmapSet[key].first.mBitmapWidth;
        outHeight = mCharacterBitmapSet[key].first.mBitmapRows;
        outChannelCount = mImageChannelCount;
        return mCharacterBitmapSet[key].second.data();
    }

private:
    std::vector<ksai::kstd::VertexEXT> mVertices; // Todo Extra eight bytes needed to be removed out for ivec3
    std::vector<uint32_t> mIndices;
    TextProperty mCurrentTextProp = { .H = AlignH::Left, .V = AlignV::Bottom };

private:
    void AddRespectiveVerticesAndIndicesAt(unsigned int len, uint32_t inStartIndex, uint32_t inDepthValue, const uint32_t& inFontShapeId, hb_glyph_info_t* info, int inOffsetX, int inOffsetY);
    void LoadTextToKeyMap(unsigned int len, const uint32_t& inFontShapeId, hb_glyph_info_t* info, hb_glyph_position_t* pos);
    [[nodiscard]] constexpr size_t ToPixels(size_t inSize) { return inSize >> 6; }
    [[nodiscard]] constexpr size_t ToFontUnits(size_t inSize) { return inSize << 6; }
    FT_Library mFtLibrary;

private:
    std::vector<FT_Face> mFaces;
    std::vector<hb_font_t*> mHbFonts;
    int mFontFaceCount = 0;
    uint32_t mCharQuadGlyphCount = 0;

private:
    struct CharacterInfo {
        hb_glyph_position_t mGlyphPos;
        hb_glyph_info_t mGlyphInfo;
        FT_Glyph_Metrics mGlyphMetrics;
        uint32_t mBitmapWidth;
        uint32_t mBitmapRows;
        int mBitmapPitch;
    };
    struct CharacterKey {
        uint32_t mFontShapeId;
        uint32_t mGlyphCodePoint;
        bool operator==(const CharacterKey&) const = default;
    };
    struct CharacterKeyHash {
        std::size_t operator()(const CharacterKey& inS) const noexcept
        {
            std::size_t h1 = std::hash<uint32_t> {}(inS.mFontShapeId);
            std::size_t h2 = std::hash<uint32_t> {}(inS.mGlyphCodePoint);
            return h1 ^ (h2 << 1);
        }
    };
    std::unordered_map<
        CharacterKey,
        std::pair<CharacterInfo, std::vector<uint8_t>>,
        CharacterKeyHash>
        mCharacterBitmapSet;

private:
    const uint32_t mImageChannelCount = 4;
};
}
