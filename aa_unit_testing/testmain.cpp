#include <Renderers/TextD/BestText_base.hpp>
#include <iostream>
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H


using namespace Jkr::Renderer;
auto BestText_base_utest ( ) -> int
{
	BestText_base base;
	uint32_t fid;
	base.AddFontFace ( "font.ttf", 16, fid );
	base.AddText ( "नमस्कारोऽस्तु", fid );
	return 0;
}

auto Text ( )
{
	// Input texts in UTF-8 (without Byte Order Mask)
	static const char* texts[] = {
		 "\xd7\xa9\xd7\x9c\xd7\x95\xd7\x9d \xd7\xa2\xd7\x95\xd7\x9c\xd7\x9d",
		 "\xd9\x85\xd8\xb1\xd8\xad\xd8\xa8\xd8\xa7 \xd8\xa8\xd8\xa7\xd9\x84\xd8\xb9\xd8\xa7\xd9\x84\xd9\x85",
		 "\xe0\xa4\xa8\xe0\xa4\xae\xe0\xa4\xb8\xe0\xa5\x8d\xe0\xa4\xa4\xe0\xa5\x87 \xe0\xa4\xa6\xe0\xa5\x81\xe0\xa4\xa8\xe0\xa4\xbf\xe0\xa4\xaf\xe0\xa4\xbe"
	};

	// Font sizes in points
	static const size_t fontSizes[] = {
		 16,
		 16,
		 16
	};

	// Font filenames
	static const char* fontFiles[] = {
		 "font.ttf",
		 "font.ttf",
		 "font.ttf"
	};

	// Freetype library handle
	FT_Library library;

	// Initialize FreeType
	if (FT_Init_FreeType ( &library )) {
		std::cerr << "Can't initialize FreeType" << std::endl;
		return 1;
	}

	// Go through all input texts
	for (size_t textIndex = 0; textIndex < sizeof ( texts ) / sizeof ( texts[0] ); ++textIndex) {
		// Current input text
		const char* text = texts[textIndex];
		const size_t fontSize = fontSizes[textIndex];
		const char* fontFile = fontFiles[textIndex];

		// FreeType font face handle
		FT_Face face;

		// Load font
		if (FT_New_Face ( library, fontFile, 0, &face )) {
			std::cerr << "Can't load font " << fontFile << std::endl;
			return 1;
		}

		// Set character size
		if (FT_Set_Char_Size ( face, fontSize << 6, fontSize << 6, 0, 0 )) {
			std::cerr << "Can't set character size" << std::endl;
			return 1;
		}

		// Set no transform (identity)
		FT_Set_Transform ( face, 0, 0 );

		// Load font into HarfBuzz
		hb_font_t* hbFont = hb_ft_font_create ( face, 0 );

		// Create buffer for our text
		hb_buffer_t* hbBuffer = hb_buffer_create ( );

		// Add our text to buffer
		hb_buffer_add_utf8 ( hbBuffer, text, -1, 0, -1 );

		// Detect direction etc
		hb_buffer_guess_segment_properties ( hbBuffer );

		// Shape our text
		hb_shape ( hbFont, hbBuffer, 0, 0 );

		// Shaped text info
		unsigned int len = hb_buffer_get_length ( hbBuffer );
		hb_glyph_info_t* info = hb_buffer_get_glyph_infos ( hbBuffer, 0 );
		hb_glyph_position_t* pos = hb_buffer_get_glyph_positions ( hbBuffer, 0 );

		// Compute text width and origin (from min and max X and Y drawing coordinate)
		int originX = 0, originY = 0;
		int minX = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;

		// Go through all glyphs and find minimum and maximum X and Y coordinate
		for (size_t i = 0; i < len; ++i) {
			// Load glyph
			if (FT_Load_Glyph ( face, info[i].codepoint, FT_LOAD_RENDER )) {
				std::cerr << "Can't load glyph " << info[i].codepoint << std::endl;
				return 1;
			}

			// Glyph data
			FT_GlyphSlot slot = face->glyph;

			// Get X and Y offset
			int offsetX = ((pos[i].x_offset + slot->metrics.horiBearingX) >> 6);
			int offsetY = ((pos[i].y_offset + slot->metrics.horiBearingY) >> 6);

			// Compute minimum and maximum X and Y for this glyph
			int glyphMinX = originX + offsetX;
			int glyphMaxX = originX + slot->bitmap.width + offsetX;
			int glyphMinY = originY - slot->bitmap.rows + offsetY;
			int glyphMaxY = originY + offsetY;

			// Update minimum and maximum X and Y for text
			if (glyphMinX < minX) minX = glyphMinX;
			if (glyphMaxX > maxX) maxX = glyphMaxX;
			if (glyphMinY < minY) minY = glyphMinY;
			if (glyphMaxY > maxY) maxY = glyphMaxY;

			// Advance
			originX += pos[i].x_advance >> 6;
		}

		// Text will start on 0
		originX = -minX;
		originY = -minY;

		// Compute width and height
		size_t width = maxX - minX + 1;
		size_t height = maxY - minY + 1;

		// Allocate buffer for image
		unsigned char* image = new unsigned char[width * height];

		// Clear image bufer
		memset ( image, 0, width * height );

		// Go through glyphs and draw them
		for (size_t i = 0; i < len; ++i) {
			// Load glyph
			if (FT_Load_Glyph ( face, info[i].codepoint, FT_LOAD_RENDER )) {
				std::cerr << "Can't load glyph " << info[i].codepoint << std::endl;
				return 1;
			}

			// Glyph data
			FT_GlyphSlot slot = face->glyph;

			// Pointer to bitmap data
			unsigned char* ptr = slot->bitmap.buffer;

			// Get real offset
			int drawX = originX + ((pos[i].x_offset + slot->metrics.horiBearingX) >> 6);
			int drawY = originY + ((pos[i].y_offset + slot->metrics.horiBearingY) >> 6);

			// Copy bitmap
			for (size_t y = 0; y < slot->bitmap.rows; ++y) {
				// Copy row
				for (size_t x = 0; x < slot->bitmap.width; ++x) {
					if (drawX + x < 0) {
						std::cerr << "drawX (" << drawX << ") + x (" << x << ") < 0" << std::endl;
						abort ( );
					}
					if (drawX + x >= width) {
						std::cerr << "drawX (" << drawX << ") + x (" << x << ") > width (" << width << ")" << std::endl;
						abort ( );
					}
					if (drawY - y < 0) {
						std::cerr << "drawY (" << drawY << ") - y (" << y << ") < 0" << std::endl;
						abort ( );
					}
					if (drawY - y >= height) {
						std::cerr << "drawY (" << drawY << ") - y (" << y << ") > height (" << height << ")" << std::endl;
						abort ( );
					}
					image[(drawY - y) * width + drawX + x] = ptr[x];
				}
				// Advance pointer
				ptr += slot->bitmap.pitch;
			}

			// Advance
			originX += pos[i].x_advance >> 6;
		}

		// Output rendered text
		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {
				unsigned char value = image[(height - y - 1) * width + x];
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
			std::cout << std::endl;
		}
		std::cout << std::endl;

		// Delete image buffer
		delete[] image;

		// Destroy buffer for text
		hb_buffer_destroy ( hbBuffer );

		// Destroy HarfBuzz font
		hb_font_destroy ( hbFont );

		// Destroy FreeType font
		FT_Done_Face ( face );
	}

	// Destroy FreeType
	FT_Done_FreeType ( library );

	return 0;
}

auto main ( ) -> int
{
	BestText_base_utest ( );
	return 0;
}