
/*
	॥ मङ्गलाचरणम् ॥
	ब्रह्मा रुद्रः कुमारो हरिवरुणयमा वह्निरिन्द्रः कुबेर
	श्चन्द्रादित्यौ सरस्वत्युदधियुगनगा वायुरुर्वी भुजङ्गाः ।
	सिद्धा नद्योऽश्विनौ श्रीर्दितिरदितिसुता मातरश्चण्डिकाद्या
	वेदस्तीर्थानि यज्ञा गणवसुमुनयः पान्तु नित्यं ग्रहाश्च ॥
*/

#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <glm/glm.hpp>

namespace Jkr::Renderer
{
	class BestText_base
	{
	public:
		BestText_base ( );
		~BestText_base ( );
		void AddFontFace ( const std::string_view inFontFilePathName, size_t inFontSize, uint32_t& outFontId );
		void AddText ( const std::string_view inString, uint32_t inFontShapeId );
	private:
		void AddRespectiveVerticesAndIndices ( unsigned int len, const uint32_t& inFontShapeId, hb_glyph_info_t* info );
		void LoadTextToKeyMap ( unsigned int len, const uint32_t& inFontShapeId, hb_glyph_info_t* info, hb_glyph_position_t* pos );
		[[nodiscard]] constexpr size_t ToPixels ( size_t inSize ) { return inSize >> 6; }
		[[nodiscard]] constexpr size_t ToFontUnits ( size_t inSize ) { return inSize << 6; }
		FT_Library mFtLibrary;
	private:
		std::array<FT_Face, 10> mFaces;
		std::array<hb_font_t*, 10> mHbFonts;
		int mFontFaceCount = 0;
	private:
		struct CharacterInfo
		{
			hb_glyph_position_t mGlyphPos;
			hb_glyph_info_t mGlyphInfo;
			FT_Glyph_Metrics mGlyphMetrics;
			uint32_t mBitmapWidth;
			uint32_t mBitmapRows;
			int mBitmapPitch;
		};
		struct CharacterKey
		{
			uint32_t mFontShapeId;
			uint32_t mGlyphCodePoint;
			bool operator==( const CharacterKey& ) const = default;
		};
		struct CharacterKeyHash {
			std::size_t operator()( const CharacterKey& inS ) const noexcept
			{
				std::size_t h1 = std::hash<uint32_t>{}(inS.mFontShapeId);
				std::size_t h2 = std::hash<uint32_t>{}(inS.mGlyphCodePoint);
				return h1 ^ (h2 << 1);
			}
		};
		std::unordered_map<
			CharacterKey,
			std::pair<CharacterInfo, std::vector<uint8_t>>,
			CharacterKeyHash
		> mCharacterBitmapSet;
	};
}