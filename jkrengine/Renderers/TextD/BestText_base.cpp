#include "BestText_base.hpp"
#include <Vendor/Tracy/tracy/Tracy.hpp>
#include <Vendor/stbi/stb_image_write.h>
#include <concepts>
#include <functional>
#include <type_traits>
#include "ksai_thread.hpp"
#include <Vendor/Tracy/tracy/Tracy.hpp>
#include <cassert>
#include <climits>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace ksai::image {

template <typename... T> using var        = std::variant<T...>;
using s                                   = std::string;
using sv                                  = std::string_view;

template <typename T> using v             = std::vector<T>;
template <typename T> using up            = std::unique_ptr<T>;
template <typename T> using sp            = std::shared_ptr<T>;
template <typename T> using opt           = std::optional<T>;
template <typename T> using optref        = std::optional<std::reference_wrapper<T>>;
template <typename T, typename U> using p = std::pair<T, U>;
using Number                              = double;
using Integer                             = int;
using std::make_unique;
using std::move;
using ui = uint32_t;
using uc = unsigned char;

template <typename T>
concept ImageConcept = requires(T &t) {
    typename T::value_type;
    typename T::size_type;
    typename T::iterator;
    typename T::const_iterator;
    { std::declval<T>().size() } -> std::same_as<typename T::size_type>;
    { std::declval<T>().begin() } -> std::same_as<typename T::iterator>;
    { std::declval<T>().end() } -> std::same_as<typename T::iterator>;
    { std::declval<T>().cbegin() } -> std::same_as<typename T::const_iterator>;
    { std::declval<T>().cend() } -> std::same_as<typename T::const_iterator>;
};

template <typename T, typename U>
concept ManipulatorConcept = requires(T &t, U &u, int x, int y, int w, int h, int c) {
    { t(u, u, x, y, w, h, c) } -> std::same_as<void>;
    { std::is_invocable_v<void, T, U &, U &, int, int, int, int, int> };
};

inline const auto flipvertically =
     []<typename T>(const T &from, T &to, int x, int y, int w, int h, int c) {
         ui fid  = x + y * w * c;
         ui oid  = x + (h - y - 1) * w * c;
         to[fid] = from[oid];
     };

inline const auto fliphorizontally =
     []<typename T>(const T &from, T &to, int x, int y, int w, int h, int c) {
         ui fid  = x + y * w * c;
         ui oid  = (w * c - x - 1) + y * w * c;
         to[fid] = from[oid];
     };

template <ImageConcept T, ManipulatorConcept<T> F>
inline void process(ui inw,
                    ui inh,
                    ui inComp,
                    T &inoutImage,
                    F &inOp,
                    optref<ksai::ThreadPool> inThreadPool = std::nullopt) {
    auto Pro = [=, &inoutImage]() {
        ZoneScoped;
        T flippedimage;
        flippedimage.resize(inoutImage.size());

        for (int y = 0; y < inh; ++y) {
            for (int x = 0; x < inw * inComp; ++x) {
                inOp(inoutImage, flippedimage, x, y, inw, inh, inComp);
            }
        }
        inoutImage = flippedimage;
    };

    if (inThreadPool.has_value()) {
        inThreadPool.value().get().Add_Job(Pro);
    } else {
        Pro();
    }
}

template <ImageConcept T, ManipulatorConcept<T> F>
inline void process(ui inw,
                    ui inh,
                    ui inComp,
                    const T &fromImage,
                    T &toImage,
                    F &inOp,
                    optref<ksai::ThreadPool> inThreadPool = std::nullopt) {
    ZoneScoped;
    auto Pro = [=, &fromImage, &toImage]() {
        for (int y = 0; y < inh; ++y) {
            for (int x = 0; x < inw * inComp; ++x) {
                inOp(fromImage, toImage, x, y, inw, inh, inComp);
            }
        }
    };
    if (inThreadPool.has_value()) {
        inThreadPool.value().get().Add_Job(Pro);
    } else {
        Pro();
    }
}

} // namespace ksai::image

using namespace Jkr::Renderer;
using bb = BestText_base;

bb::BestText_base() {
    if (FT_Init_FreeType(&mFtLibrary)) {
        std::cout << "Cnnot initialize freetype\n";
        exit(1);
    }
}

bb::~BestText_base() {
    for (int i = 0; i < mFontFaceCount; ++i) {
        hb_font_destroy(mHbFonts[i]);
        FT_Done_Face(mFaces[i]);
    }
    FT_Done_FreeType(mFtLibrary);
}

void bb::AddFontFace(const sv inFontFilePathName, size_t inFontSize, ui &outFontId) {
    ui FaceIndex = mFontFaceCount++;
    mFaces.resize(mFontFaceCount);
    mHbFonts.resize(mFontFaceCount);
    if (FT_New_Face(mFtLibrary, inFontFilePathName.data(), 0, &mFaces[FaceIndex])) {
        std::cout << "Font Face Load Failed" << '\n';
    }
    if (FT_Set_Char_Size(
             mFaces[FaceIndex], ToFontUnits(inFontSize), ToFontUnits(inFontSize), 96, 96)) {
        std::cout << "Font Char Size Set Failed" << '\n';
    }
    FT_Set_Transform(mFaces[FaceIndex], 0, 0);
    mHbFonts[FaceIndex] = hb_ft_font_create(mFaces[FaceIndex], 0);
    outFontId           = FaceIndex;
}

bb::TextDimensions bb::AddText(ui inX,
                               ui inY,
                               const sv inString,
                               ui inFontShapeId,
                               ui inDepthValue,
                               v<ui> &outCodePoints,
                               ui &outId) {
    hb_buffer_t *hbBuffer = hb_buffer_create();
    hb_buffer_add_utf8(hbBuffer, reinterpret_cast<const char *>(inString.data()), -1, 0, -1);
    hb_buffer_guess_segment_properties(hbBuffer);
    hb_shape(mHbFonts[inFontShapeId], hbBuffer, 0, 0);
    unsigned int len         = hb_buffer_get_length(hbBuffer);
    hb_glyph_info_t *info    = hb_buffer_get_glyph_infos(hbBuffer, 0);
    hb_glyph_position_t *pos = hb_buffer_get_glyph_positions(hbBuffer, 0);

    LoadTextToKeyMap(len, inFontShapeId, info, pos);
    auto TextDims = GetTextDimensions(inString, inFontShapeId, info, pos, len);

    for (int i = 0; i < len; i++)
        outCodePoints.push_back(info[i].codepoint);

    ui x, y;
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
    AddRespectiveVerticesAndIndicesAt(
         len, mCharQuadGlyphCount, inDepthValue, inFontShapeId, info, x, y);
    outId = mCharQuadGlyphCount;
    mCharQuadGlyphCount += len;
    hb_buffer_destroy(hbBuffer);
    return TextDims;
}

bb::TextDimensions bb::RenderTextToImage(sv inString,
                                         ui inFontShapeId,
                                         v<uc> &outImage,
                                         ThreadPool &inThreadPool,
                                         optref<int> outYoff) {
    hb_buffer_t *hbBuffer = hb_buffer_create();
    hb_buffer_add_utf8(hbBuffer, reinterpret_cast<const char *>(inString.data()), -1, 0, -1);
    hb_buffer_guess_segment_properties(hbBuffer);
    hb_shape(mHbFonts[inFontShapeId], hbBuffer, 0, 0);
    unsigned int len         = hb_buffer_get_length(hbBuffer);
    hb_glyph_info_t *info    = hb_buffer_get_glyph_infos(hbBuffer, 0);
    hb_glyph_position_t *pos = hb_buffer_get_glyph_positions(hbBuffer, 0);

    LoadTextToKeyMap(len, inFontShapeId, info, pos);

    v<ui> codepoints;
    for (int i = 0; i < len; i++)
        codepoints.push_back(info[i].codepoint);

    ui outbmp_h = 0;
    ui outbmp_w = 0;

    int originX = 0, originY = 0;
    int minX = 0, minY = 0, maxX = 0, maxY = 0;
    for (int img_index = 0; img_index < len; img_index++) {
        CharacterKey key           = {.mFontShapeId    = inFontShapeId,
                                      .mGlyphCodePoint = info[img_index].codepoint};
        const auto &CharacterInMap = mCharacterBitmapSet[key];
        const auto &bmp            = CharacterInMap.second;
        const auto &info           = CharacterInMap.first.mGlyphInfo;
        const auto &metrics        = CharacterInMap.first.mGlyphMetrics;
        const auto &pos            = CharacterInMap.first.mGlyphPos;
        const auto bitmap_width    = CharacterInMap.first.mBitmapWidth;
        const auto bitmap_rows     = CharacterInMap.first.mBitmapRows;
        int advance                = ToPixels(pos.x_advance);
        int offsetX                = ToPixels(pos.x_offset + metrics.horiBearingX);
        int offsetY                = ToPixels(pos.y_offset + metrics.horiBearingY);
        int glyphMinX              = originX + offsetX;
        int glyphMaxX              = originX + bitmap_width + offsetX;
        int glyphMinY              = originY - bitmap_rows + offsetY;
        int glyphMaxY              = originY + offsetY;

        if (glyphMinX < minX) minX = glyphMinX;
        if (glyphMaxX > maxX) maxX = glyphMaxX;
        if (glyphMinY < minY) minY = glyphMinY;
        if (glyphMaxY > maxY) maxY = glyphMaxY;
        originX += advance;
    }
    outbmp_w = maxX - minX + 1;
    outbmp_h = maxY - minY + 1;

    outImage.resize(outbmp_h * outbmp_w * mImageChannelCount, 0);

    originX         = -minX;
    originY         = -minY;

    int maxYBearing = 0;
    for (int img_index = 0; img_index < len; img_index++) {
        CharacterKey key           = {.mFontShapeId    = inFontShapeId,
                                      .mGlyphCodePoint = info[img_index].codepoint};
        const auto &CharacterInMap = mCharacterBitmapSet[key];
        const auto &bmp            = CharacterInMap.second;
        const auto &info           = CharacterInMap.first.mGlyphInfo;
        const auto &metrics        = CharacterInMap.first.mGlyphMetrics;
        const auto &pos            = CharacterInMap.first.mGlyphPos;
        const auto bitmap_width    = CharacterInMap.first.mBitmapWidth;
        const auto bitmap_rows     = CharacterInMap.first.mBitmapRows;
        int advance                = CharacterInMap.first.mGlyphPos.x_advance;
        int drawX                  = originX + ToPixels(pos.x_offset + metrics.horiBearingX);
        int drawY                  = originY + ToPixels(pos.y_offset + metrics.horiBearingY);
        if (abs(maxYBearing) < ToPixels(metrics.horiBearingY)) {
            maxYBearing = ToPixels(metrics.horiBearingY);
        }

        const auto join_img =
             [=]<typename T>(const T &from, T &to, int x, int y, int w, int h, int c) {
                 ui maini = drawX * c + x + (outbmp_h - (drawY - y - 1) - 1) * outbmp_w * c;
                 ui biti  = x + y * w * c;
                 to[maini] += from[biti];
             };

        ksai::image::process(bitmap_width,
                             bitmap_rows,
                             mImageChannelCount,
                             mCharacterBitmapSet[key].second,
                             outImage,
                             join_img,
                             inThreadPool);
        int PixelAdvance = ToPixels(advance);
        originX += PixelAdvance;
    }

    inThreadPool.Wait();
    if (outYoff.has_value()) {
        outYoff.value().get() = (maxY - minY) - maxYBearing;
    }
    hb_buffer_destroy(hbBuffer);
    return TextDimensions{.mWidth = outbmp_w, .mHeight = outbmp_h};
}

bb::TextDimensions bb::UpdateText(ui inX,
                                  ui inY,
                                  ui inId,
                                  const sv inString,
                                  ui inFontShapeId,
                                  ui inDepthValue,
                                  v<ui> &outCodePoints) {

    hb_buffer_t *hbBuffer = hb_buffer_create();
    hb_buffer_add_utf8(hbBuffer, inString.data(), -1, 0, -1);
    hb_buffer_guess_segment_properties(hbBuffer);
    hb_shape(mHbFonts[inFontShapeId], hbBuffer, 0, 0);
    unsigned int len         = hb_buffer_get_length(hbBuffer);
    hb_glyph_info_t *info    = hb_buffer_get_glyph_infos(hbBuffer, 0);
    hb_glyph_position_t *pos = hb_buffer_get_glyph_positions(hbBuffer, 0);

    LoadTextToKeyMap(len, inFontShapeId, info, pos);
    auto TextDims = GetTextDimensions(inString, inFontShapeId, info, pos, len);

    for (int i = 0; i < len; i++)
        outCodePoints.push_back(info[i].codepoint);

    ui x, y;
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

bb::TextDimensions bb::GetTextDimensions(
     const sv inString, ui inFontShapeId, hb_glyph_info_t *info, hb_glyph_position_t *pos, ui len) {
    int originX = 0, originY = 0;
    int minX        = INT_MAX;
    int maxX        = INT_MIN;
    int minY        = INT_MAX;
    int maxY        = INT_MIN;
    int maxYBearing = 0;
    for (int i = 0; i < len; ++i) {
        CharacterKey key = {.mFontShapeId = inFontShapeId, .mGlyphCodePoint = info[i].codepoint};
        const auto &CharacterInMap = mCharacterBitmapSet[key];
        const auto &info           = CharacterInMap.first.mGlyphInfo;
        const auto &metrics        = CharacterInMap.first.mGlyphMetrics;
        const auto &pos            = CharacterInMap.first.mGlyphPos;
        const auto bitmap_width    = CharacterInMap.first.mBitmapWidth;
        const auto bitmap_rows     = CharacterInMap.first.mBitmapRows;

        int offsetX                = ToPixels(pos.x_offset + metrics.horiBearingX);
        int offsetY                = ToPixels(pos.y_offset + metrics.horiBearingY);
        int glyphMinX              = originX + offsetX;
        int glyphMaxX              = originX + bitmap_width + offsetX;
        int glyphMinY              = originY - bitmap_rows + offsetY;
        int glyphMaxY              = originY + offsetY;
        if (abs(maxYBearing) < ToPixels(abs(metrics.horiBearingY))) {
            maxYBearing = ToPixels(metrics.horiBearingY);
        }
        if (glyphMinX < minX) minX = glyphMinX;
        if (glyphMaxX > maxX) maxX = glyphMaxX;
        if (glyphMinY < minY) minY = glyphMinY;
        if (glyphMaxY > maxY) maxY = glyphMaxY;

        originX += ToPixels(pos.x_advance);

        if (i == len - 1) {
            maxX = originX;
        }
    }

    // originX = -minX;
    // originY = -minY;
    size_t width  = maxX - minX + 1;
    size_t height = maxY - minY + 1;

    // if (outYOff.has_value()) {
    //     outYOff.value().get() = (maxY - minY) - maxYBearing;
    // }

    return TextDimensions{
         .mWidth = width, .mHeight = height, .mYBearing = maxYBearing, .mDelY = maxY - minY};
}

void bb::AddRespectiveVerticesAndIndicesAt(unsigned int len,
                                           ui inStartIndex,
                                           ui inDepthValue,
                                           const ui &inFontShapeId,
                                           hb_glyph_info_t *info,
                                           int inOriginX,
                                           int inOriginY) {

    int originX = inOriginX, originY = inOriginY;

    for (int i = 0; i < len; ++i) {
        CharacterKey key = {.mFontShapeId = inFontShapeId, .mGlyphCodePoint = info[i].codepoint};
        const auto &CharacterInMap     = mCharacterBitmapSet[key];
        const auto &bmp                = CharacterInMap.second;
        const auto &info               = CharacterInMap.first.mGlyphInfo;
        const auto &metrics            = CharacterInMap.first.mGlyphMetrics;
        const auto &pos                = CharacterInMap.first.mGlyphPos;
        const auto bitmap_width        = CharacterInMap.first.mBitmapWidth;
        const auto bitmap_rows         = CharacterInMap.first.mBitmapRows;
        int offsetX                    = ToPixels(pos.x_offset + metrics.horiBearingX);
        int offsetY                    = ToPixels(pos.y_offset + metrics.horiBearingY);
        int glyphMinX                  = originX + offsetX;
        int glyphMaxX                  = originX + bitmap_width + offsetX;
        int glyphMinY                  = originY + (bitmap_rows - offsetY);
        int glyphMaxY                  = originY - offsetY;

        [[maybe_unused]] size_t width  = glyphMaxX - glyphMinX + 1;
        [[maybe_unused]] size_t height = glyphMaxY - glyphMinY + 1;

        {
            auto inTextSize = len;
            if (mVertices.size() < 4 * inTextSize + inStartIndex * 4) {
                mVertices.resize(4 * inTextSize + inStartIndex * 4);
                mIndices.resize(6 * inTextSize + inStartIndex * 6);
            }

            using namespace ksai;

            const auto v_index                         = i * 4 + inStartIndex * 4;

            mVertices[v_index + 0].mPosition           = {glyphMinX, glyphMaxY, inDepthValue};
            mVertices[v_index + 0].mTextureCoordinates = {0, 0};
            mVertices[v_index + 1].mPosition           = {glyphMinX, glyphMinY, inDepthValue};
            mVertices[v_index + 1].mTextureCoordinates = {0, 1};
            mVertices[v_index + 2].mPosition           = {glyphMaxX, glyphMinY, inDepthValue};
            mVertices[v_index + 2].mTextureCoordinates = {1, 1};
            mVertices[v_index + 3].mPosition           = {glyphMaxX, glyphMaxY, inDepthValue};
            mVertices[v_index + 3].mTextureCoordinates = {1, 0};

            const auto i_index                         = i * 6 + inStartIndex * 6;

            mIndices[i_index + 0]                      = v_index + 0;
            mIndices[i_index + 1]                      = v_index + 1;
            mIndices[i_index + 2]                      = v_index + 2;
            mIndices[i_index + 3]                      = v_index + 0;
            mIndices[i_index + 4]                      = v_index + 2;
            mIndices[i_index + 5]                      = v_index + 3;

            originX += ToPixels(pos.x_advance);
        }
    }
}

void bb::LoadTextToKeyMap(unsigned int len,
                          const ui &inFontShapeId,
                          hb_glyph_info_t *info,
                          hb_glyph_position_t *pos) {
    /* Load Characters To Keymap */
    {
        for (int i = 0; i < len; ++i) {

            CharacterKey key = {.mFontShapeId    = inFontShapeId,
                                .mGlyphCodePoint = info[i].codepoint};

            if (not mCharacterBitmapSet.contains(key)) {
                if (FT_Load_Glyph(mFaces[inFontShapeId], info[i].codepoint, FT_LOAD_RENDER))
                    std::cout << "Error Font Glyph loading" << '\n';
                FT_GlyphSlot slot = mFaces[inFontShapeId]->glyph;
                CharacterInfo character_info{.mGlyphPos     = pos[i],
                                             .mGlyphInfo    = info[i],
                                             .mGlyphMetrics = slot->metrics,
                                             .mBitmapWidth  = slot->bitmap.width,
                                             .mBitmapRows   = slot->bitmap.rows,
                                             .mBitmapPitch  = slot->bitmap.pitch};
                v<uint8_t> BitmapImage;
                uint8_t *ptr = slot->bitmap.buffer;
                int originX = 0, originY = 0;
                int offsetX   = ToPixels(pos[i].x_offset + slot->metrics.horiBearingX);
                int offsetY   = ToPixels(pos[i].y_offset + slot->metrics.horiBearingY);
                int glyphMinX = originX + offsetX;
                int glyphMaxX = originX + slot->bitmap.width + offsetX;
                int glyphMinY = originY - slot->bitmap.rows + offsetY;
                int glyphMaxY = originY + offsetY;
                originX       = -glyphMinX;
                originY       = -glyphMinY;
                int drawX     = originX + ToPixels(pos[i].x_offset + slot->metrics.horiBearingX);
                int drawY     = originY + ToPixels(pos[i].y_offset + slot->metrics.horiBearingY);
                [[maybe_unused]] size_t width  = glyphMaxX - glyphMinX + 1;
                [[maybe_unused]] size_t height = glyphMaxY - glyphMinY + 1;
                BitmapImage.resize(mImageChannelCount * character_info.mBitmapWidth *
                                   character_info.mBitmapRows);
                auto *dst = BitmapImage.data();

                if (mImageChannelCount == 4) {
                    ui i = 0;
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

bb::TextDimensions bb::GetTextDimensions(const sv inString, ui inFontShapeId) {
    hb_buffer_t *hbBuffer = hb_buffer_create();
    hb_buffer_add_utf8(hbBuffer, reinterpret_cast<const char *>(inString.data()), -1, 0, -1);
    hb_buffer_guess_segment_properties(hbBuffer);
    hb_shape(mHbFonts[inFontShapeId], hbBuffer, 0, 0);
    unsigned int len         = hb_buffer_get_length(hbBuffer);
    hb_glyph_info_t *info    = hb_buffer_get_glyph_infos(hbBuffer, 0);
    hb_glyph_position_t *pos = hb_buffer_get_glyph_positions(hbBuffer, 0);
    LoadTextToKeyMap(len, inFontShapeId, info, pos);
    auto td = GetTextDimensions(inString, inFontShapeId, info, pos, len);
    hb_buffer_destroy(hbBuffer);
    return td;
}

glm::vec4 Jkr::Renderer::BestText_base::GetTextDimensionsEXT(const std::string_view inString,
                                                             uint32_t inFontShapeId) {
    TextDimensions dimen = GetTextDimensions(inString, inFontShapeId);
    return glm::vec4(dimen.mWidth, dimen.mHeight, dimen.mYBearing, dimen.mDelY);
}

ksai::ui Jkr::Renderer::BestText_base::AddFontFaceEXT(const std::string_view inFontFilePathName,
                                                      size_t inFontSize) {
    ui i;
    AddFontFace(inFontFilePathName, inFontSize, i);
    return i;
}