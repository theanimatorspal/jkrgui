#pragma once
#include <Instance.hpp>
#include <glm/glm.hpp>
#include "../Global/Standards.hpp"
/*
	My legacy Code from KSAI ANIM
*/

namespace ksai::Font {

	class FastText
	{
	private:
		struct FontContainer
		{
			glm::vec3		ps;
			glm::vec3		scl;
			glm::vec3		rt;
		};

		struct bmchar {
			uint32_t	id;
			uint32_t	x, y;
			uint32_t	width;
			uint32_t	height;
			int32_t	xoffset;
			int32_t	yoffset;
			int32_t	xadvance;
			uint32_t	page;
		};

		struct font_features
		{
			uint32_t		c_lineheight;
			uint32_t		base;
			uint32_t		scale_w;
			uint32_t		scale_h;
			uint32_t		pages;
			uint32_t		packed;
			uint32_t		page_id;
			char			file[1 << 7];
		};

		struct font_description
		{
			char					fnt_img_pth[1 << 8];
			bmchar					fchars[1 << 8];
			font_features				features;
		};

	public:
		struct TextFeatures
		{
			float mWidth;
			float mHeight;
		};
	public:
		FastText(std::string inFontPath)
		{
			ParseBMFont(inFontPath.c_str(), &mFontDescription);
		}
		void GenerateQuadsAt(const char* inString,
			std::vector<kstd::Vertex>& outVertices,
			std::vector<kstd::ui32>& outIndices,
			kstd::ui32 inX,
			kstd::ui32 inY,
			kstd::f32 inFontSizeInverseFactor,
			kstd::ui32 offset_NoOfCharactersDrawn,
			kstd::ui32 inDepthValue = 0
		);
		TextFeatures GetTextFeatures(const char* inString, kstd::f32 inFontSizeInverseFactor)
		{
			auto inTextSize = strlen(inString);

			const auto ResizeFactor = inFontSizeInverseFactor;
			float posx = 0;
			float maxH = 0.0f;
			for (uint32_t i = 0; i < inTextSize; i++)
			{
				bmchar char_info = mFontDescription.fchars[(int)inString[i]];

				for (int j = 0; j < 255; j++)
				{
					if (mFontDescription.fchars[j].id == inString[i])
					{
						char_info = mFontDescription.fchars[j];
						break;
					}
				}
				float charh = ((char_info.height)) / ResizeFactor;
				float yo = char_info.yoffset / ResizeFactor;
				if (charh > maxH)
				{
					maxH = charh + yo;
				}

				float advance = ((float)(char_info.xadvance) / ResizeFactor);
				posx += advance;
			}
			return TextFeatures{.mWidth = posx, .mHeight = maxH};

		}
	private:
		int GetNthPair(int n, char* string_in);
		void ParseBMFont(const char* path, font_description* font_desp);
		font_description mFontDescription;
	};
}
