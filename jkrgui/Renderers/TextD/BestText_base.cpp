#include "BestText_base.hpp"

Jkr::Renderer::BestText_base::BestText_base ( )
{
	if (FT_Init_FreeType ( &mFtLibrary ))
	{
		std::cout << "Cnnot initialize freetype\n";
		exit ( 1 );
	}
}

Jkr::Renderer::BestText_base::~BestText_base ( )
{
	for (int i = 0; i < mFontFaceCount; ++i)
	{
		hb_font_destroy ( mHbFonts[i] );
		FT_Done_Face ( mFaces[i] );
	}
	FT_Done_FreeType ( mFtLibrary );

}

void Jkr::Renderer::BestText_base::AddFontFace ( const std::string_view inString, size_t inFontSize, uint32_t& outFontId )
{
	uint32_t FaceIndex = mFontFaceCount++;
	if (FT_New_Face ( mFtLibrary, inString.data ( ), 0, &mFaces[FaceIndex] ))
	{
		std::cout << "Font Face Load Failed" << '\n';
	}
	if (FT_Set_Char_Size ( mFaces[FaceIndex], ToFontUnits ( inFontSize ), ToFontUnits ( inFontSize ), 0, 0 ))
	{
		std::cout << "Font Char Size Set Failed" << '\n';
	}
	FT_Set_Transform ( mFaces[FaceIndex], 0, 0 );
	mHbFonts[FaceIndex] = hb_ft_font_create ( mFaces[FaceIndex], 0 );
	outFontId = FaceIndex;
}

//void Jkr::Renderer::BestText_base::AddText ( const std::string_view inString, uint32_t inFontShapeId )
//{
//	hb_buffer_t* hbBuffer = hb_buffer_create ( );
//	hb_buffer_add_utf8 ( hbBuffer, inString.data ( ), -1, 0, -1 );
//	hb_buffer_guess_segment_properties ( hbBuffer );
//	hb_shape ( mHbFonts[inFontShapeId], hbBuffer, 0, 0 );
//	unsigned int len = hb_buffer_get_length ( hbBuffer );
//	hb_glyph_info_t* info = hb_buffer_get_glyph_infos ( hbBuffer, 0 );
//	hb_glyph_position_t* pos = hb_buffer_get_glyph_positions ( hbBuffer, 0 );
//
//	int originX = 0, originY = 0;
//	int minX = INT_MAX;
//	int maxX = INT_MIN;
//	int minY = INT_MAX;
//	int maxY = INT_MIN;
//
//	for (size_t i = 0; i < len; i++)
//	{
//		if (FT_Load_Glyph ( mFaces[inFontShapeId], info[i].codepoint, FT_LOAD_RENDER ))
//		{
//			std::cout << "Error Font Glyph loading" << '\n';
//		}
//
//		/*
//			यो pos[i] लाई हाल्नु पर्छ ।
//		*/
//
//		FT_GlyphSlot slot = mFaces[inFontShapeId]->glyph;
//		int offsetX = ToPixels ( pos[i].x_offset + slot->metrics.horiBearingX );
//		int offsetY = ToPixels ( pos[i].y_offset + slot->metrics.horiBearingY );
//		int glyphMinX = originX + offsetX;
//		int glyphMaxX = originX + slot->bitmap.width + offsetX;
//		int glyphMinY = originY - slot->bitmap.rows + offsetY;
//		int glyphMaxY = originY + offsetY;
//
//		if (glyphMinX < minX) minX = glyphMinX;
//		if (glyphMaxX > maxX) maxX = glyphMaxX;
//		if (glyphMinY < minY) minY = glyphMinY;
//		if (glyphMaxY > maxY) maxY = glyphMaxY;
//
//		originX += ToPixels ( pos[i].x_advance );
//	}
//	originX = -minX;
//	originY = -minY;
//
//	size_t width = maxX - minX + 1;
//	size_t height = maxY - minY + 1;
//
//	std::vector<uint8_t> Image;
//	Image.resize ( width * height, 0 );
//
//	for (size_t i = 0; i < len; ++i)
//	{
//		if (FT_Load_Glyph ( mFaces[inFontShapeId], info[i].codepoint, FT_LOAD_RENDER ))
//		{
//			std::cout << "Error Faces" << '\n';
//		}
//		FT_GlyphSlot slot = mFaces[inFontShapeId]->glyph;
//		uint8_t* ptr = slot->bitmap.buffer;
//		int drawX = originX + ToPixels ( pos[i].x_offset + slot->metrics.horiBearingX );
//		int drawY = originY + ToPixels ( pos[i].y_offset + slot->metrics.horiBearingY );
//
//		for (size_t y = 0; y < slot->bitmap.rows; ++y)
//		{
//			for (size_t x = 0; x < slot->bitmap.width; ++x)
//			{
//				Image[(drawY - y) * width + drawX + x] = ptr[x];
//			}
//			ptr += slot->bitmap.pitch;
//		}
//		originX += ToPixels ( pos[i].x_advance );
//	}
//	hb_buffer_destroy ( hbBuffer );
//}

void Jkr::Renderer::BestText_base::AddText ( const std::string_view inString, uint32_t inFontShapeId )
{
	hb_buffer_t* hbBuffer = hb_buffer_create ( );
	hb_buffer_add_utf8 ( hbBuffer, reinterpret_cast<const char*>(inString.data ( )), -1, 0, -1 );
	hb_buffer_guess_segment_properties ( hbBuffer );
	hb_shape ( mHbFonts[inFontShapeId], hbBuffer, 0, 0 );
	unsigned int len = hb_buffer_get_length ( hbBuffer );
	hb_glyph_info_t* info = hb_buffer_get_glyph_infos ( hbBuffer, 0 );
	hb_glyph_position_t* pos = hb_buffer_get_glyph_positions ( hbBuffer, 0 );

	/* Load Characters To Keymap */
	{
		for (int i = 0; i < len; ++i)
		{

			CharacterKey key = { .mFontShapeId = inFontShapeId, .mGlyphCodePoint = info[i].codepoint };

			if (not mCharacterBitmapSet.contains ( key ))
			{
				if (FT_Load_Glyph ( mFaces[inFontShapeId], info[i].codepoint, FT_LOAD_RENDER )) std::cout << "Error Font Glyph loading" << '\n';
				FT_GlyphSlot slot = mFaces[inFontShapeId]->glyph;
				CharacterInfo character_info{
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
				int offsetX = ToPixels ( pos[i].x_offset + slot->metrics.horiBearingX );
				int offsetY = ToPixels ( pos[i].y_offset + slot->metrics.horiBearingY );
				int glyphMinX = originX + offsetX;
				int glyphMaxX = originX + slot->bitmap.width + offsetX;
				int glyphMinY = originY - slot->bitmap.rows + offsetY;
				int glyphMaxY = originY + offsetY;
				originX = -glyphMinX;
				originY = -glyphMinY;
				int drawX = originX + ToPixels ( pos[i].x_offset + slot->metrics.horiBearingX );
				int drawY = originY + ToPixels ( pos[i].y_offset + slot->metrics.horiBearingY );
				[[maybe_unused]] size_t width = glyphMaxX - glyphMinX + 1;
				[[maybe_unused]] size_t height = glyphMaxY - glyphMinY + 1;
				BitmapImage.resize ( width * height );
				for (size_t y = 0; y < slot->bitmap.rows; ++y)
				{
					for (size_t x = 0; x < slot->bitmap.width; ++x)
					{
						BitmapImage[(drawY - y) * width + drawX + x] = ptr[x];
					}
					ptr += slot->bitmap.pitch;
				}
				mCharacterBitmapSet[key] = std::make_pair ( character_info, std::move ( BitmapImage ) );
			}
		}
	}


	/* Add Text*/
	{
		int originX = 0, originY = 0;
		int minX = INT_MAX;
		int maxX = INT_MIN;
		int minY = INT_MAX;
		int maxY = INT_MIN;
		for (int i = 0; i < len; ++i)
		{
			CharacterKey key = { .mFontShapeId = inFontShapeId, .mGlyphCodePoint = info[i].codepoint };
			const auto& CharacterInMap = mCharacterBitmapSet[key];
			const auto& info = CharacterInMap.first.mGlyphInfo;
			const auto& metrics = CharacterInMap.first.mGlyphMetrics;
			const auto& pos = CharacterInMap.first.mGlyphPos;
			const auto bitmap_width = CharacterInMap.first.mBitmapWidth;
			const auto bitmap_rows = CharacterInMap.first.mBitmapRows;

			int offsetX = ToPixels ( pos.x_offset + metrics.horiBearingX );
			int offsetY = ToPixels ( pos.y_offset + metrics.horiBearingY );
			int glyphMinX = originX + offsetX;
			int glyphMaxX = originX + bitmap_width + offsetX;
			int glyphMinY = originY - bitmap_rows + offsetY;
			int glyphMaxY = originY + offsetY;

			if (glyphMinX < minX) minX = glyphMinX;
			if (glyphMaxX > maxX) maxX = glyphMaxX;
			if (glyphMinY < minY) minY = glyphMinY;
			if (glyphMaxY > maxY) maxY = glyphMaxY;
			originX += ToPixels ( pos.x_advance );
		}

		originX = -minX;
		originY = -minY;
		size_t width = maxX - minX + 1;
		size_t height = maxY - minY + 1;

		for (int i = 0; i < inString.size ( ); ++i)
		{
			CharacterKey key = { .mFontShapeId = inFontShapeId, .mGlyphCodePoint = info[i].codepoint };
			const auto& CharacterInMap = mCharacterBitmapSet[key];
			const auto& bmp = CharacterInMap.second;
			const auto& info = CharacterInMap.first.mGlyphInfo;
			const auto& metrics = CharacterInMap.first.mGlyphMetrics;
			const auto& pos = CharacterInMap.first.mGlyphPos;
			const auto bitmap_width = CharacterInMap.first.mBitmapWidth;
			const auto bitmap_rows = CharacterInMap.first.mBitmapRows;

			for (size_t y = 0; y < bitmap_rows; ++y) {
				for (size_t x = 0; x < bitmap_width; ++x) {
					unsigned char value = bmp[(bitmap_rows - y - 1) * bitmap_width + x];
					if (value >= 0x80) {
						std::cout << "XX"; // if it's 128+
					}
					else if (value >= 0x40) {
						std::cout << ".."; // if it's 64+
					}
					else {
						std::cout << "  "; // if its under 64
					}
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
	}

	hb_buffer_destroy ( hbBuffer );
}

