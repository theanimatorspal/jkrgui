#include "FastText_base.hpp"

constexpr float FontSizeInverseFactor = 2.0f;

using namespace ksai;
using namespace Jkr::Renderer;

FastText_base::TextDimensions Jkr::Renderer::FastText_base::GenerateQuadsAt(const std::string_view inString, std::vector<kstd::Vertex>& outVertices, std::vector<kstd::ui32>& outIndices, kstd::ui32 inX, kstd::ui32 inY, kstd::f32 inFontSize, kstd::ui32 inStartIndex, kstd::ui32 inDepthValue)
{
	auto inTextSize = inString.size();
	if (outVertices.size() < 4 * inTextSize + inStartIndex * 4)
	{
		outVertices.resize(4 * inTextSize + inStartIndex * 4);
		outIndices.resize(6 * inTextSize + inStartIndex * 6);
	}
	float w = mFontDescription.features.scale_w; // texture fnt sdf width

	float posx = inX;
	float posy = inY;
	const auto ResizeFactor = FontSizeInverseFactor;

	float maxH = 0.0f;

	for (uint32_t i = 0; i < inTextSize; i++)
	{
		BmChar char_info = mFontDescription.fchars[(int)inString[i]];

		for (int j = 0; j < 255; j++)
		{
			if (mFontDescription.fchars[j].id == inString[i])
			{
				char_info = mFontDescription.fchars[j];
				break;
			}
		}

		if (char_info.width == 0)
		{
			char_info.width = ResizeFactor;
		}

		float charw = ((char_info.width)) / ResizeFactor;
		float dimx = 1.0f * charw;
		float charh = ((char_info.height)) / ResizeFactor;
		float dimy = 1.0f * charh;

		float us = char_info.x / w;
		float ue = (char_info.x + char_info.width) / w;
		float ts = char_info.y / w;
		float te = (char_info.y + char_info.height) / w;

		float xo = char_info.xoffset / ResizeFactor;
		float yo = char_info.yoffset / ResizeFactor;
		posy = yo + inY;

		const auto v_index = i * 4 + inStartIndex * 4;
		outVertices[v_index + 0] = kstd::Vertex{
			.mPosition = { posx + xo,  (posy + dimy), inDepthValue },
			.mTextureCoordinates = { us, te },
		};

		outVertices[v_index + 1] = kstd::Vertex{
			.mPosition = { posx + xo,  posy, inDepthValue },
			.mTextureCoordinates = { us, ts },
		};

		outVertices[v_index + 2] = kstd::Vertex{
			.mPosition = { posx + dimx + xo,  posy, inDepthValue },
			.mTextureCoordinates = { ue, ts },
		};

		outVertices[v_index + 3] = kstd::Vertex{
			.mPosition = { posx + dimx + xo, (posy + dimy), inDepthValue},
			.mTextureCoordinates = { ue, te },
		};

		const auto i_index = i * 6 + inStartIndex * 6;

		outIndices[i_index + 0] = v_index + 0;
		outIndices[i_index + 1] = v_index + 1;
		outIndices[i_index + 2] = v_index + 2;
		outIndices[i_index + 3] = v_index + 0;
		outIndices[i_index + 4] = v_index + 2;
		outIndices[i_index + 5] = v_index + 3;

		if (charh > maxH)
		{
			maxH = charh + yo;
		}

		float advance = ((float)(char_info.xadvance) / ResizeFactor);
		posx += advance;
	}

	return TextDimensions{ .mWidth = posx, .mHeight = maxH };
}


int Jkr::Renderer::FastText_base::GetNthPair(int inN, char* inString)
{
	int count_eq = 0;
	for (int i = 0; inString[i] != '\n' && inString[i] != '\0'; i++)
	{
		if (inString[i] == '=')
		{
			count_eq++;
			if (count_eq == inN)
			{
				int j = i + 1;
				char dummy[1 << 5];
				int x;
				for (x = 0; inString[j] != ' ' && inString[j] != '\t'; x++)
				{
					dummy[x] = inString[j];
					j++;
				}
				dummy[x] = '\0';
				int out;
                sscanf(dummy, "%d", &out);
                return out;
			}
		}
	}
	return 0;
}

void Jkr::Renderer::FastText_base::ParseBMFont(const char* inPath, FontDescription* inFontDesp)
{
	static char file_buffer[1 << 25];

	FILE* file = fopen(inPath, "r");
	fgets(file_buffer, 1 << 25, file);
	fgets(file_buffer, 1 << 25, file);
	/* fnt features extract */
	{
		inFontDesp->features.c_lineheight = GetNthPair(1, file_buffer);
		inFontDesp->features.base = GetNthPair(2, file_buffer);
		inFontDesp->features.scale_w = GetNthPair(3, file_buffer);
		inFontDesp->features.scale_h = GetNthPair(4, file_buffer);
		inFontDesp->features.pages = GetNthPair(5, file_buffer);
		inFontDesp->features.packed = GetNthPair(6, file_buffer);
		fgets(file_buffer, 1 << 25, file);
		inFontDesp->features.page_id = GetNthPair(1, file_buffer);
	}
	fgets(file_buffer, 1 << 25, file);
	int chars_count = GetNthPair(1, file_buffer);

	for (int i = 0; i < chars_count; i++)
	{
		fgets(file_buffer, 1 << 25, file);
		inFontDesp->fchars[i].id = GetNthPair(1, file_buffer);
		inFontDesp->fchars[i].x = GetNthPair(2, file_buffer);
		inFontDesp->fchars[i].y = GetNthPair(3, file_buffer);
		inFontDesp->fchars[i].width = GetNthPair(4, file_buffer);
		inFontDesp->fchars[i].height = GetNthPair(5, file_buffer);
		inFontDesp->fchars[i].xoffset = GetNthPair(6, file_buffer);
		inFontDesp->fchars[i].yoffset = GetNthPair(7, file_buffer);
		inFontDesp->fchars[i].xadvance = GetNthPair(8, file_buffer);
		inFontDesp->fchars[i].page = GetNthPair(9, file_buffer);
	}
	fclose(file);
}

Jkr::Renderer::FastText_base::TextDimensions Jkr::Renderer::FastText_base::AddText(const std::string_view inText, uint32_t inX, uint32_t inY, uint32_t inDepthValue, uint32_t& outId)
{
	auto dim = GetTextDimensions(inText);
	uint32_t x = 0, y = 0;
	switch (mCurrentTextProp.H)
	{
	case AlignH::Left:
		x = inX;
		break;
	case AlignH::Right:
		x = inX - dim.mWidth;
		break;
	case AlignH::Center:
		x = inX - dim.mWidth / 2.0f;
		break;
	}

	switch (mCurrentTextProp.V)
	{
	case AlignV::Bottom:
		y = inY;
		break;
	case AlignV::Top:
		y = inY + dim.mHeight;
		break;
	case AlignV::Center:
		y = inY + dim.mHeight / 2.0f;
		break;
	}

	GenerateQuadsAt(inText, mCharVertices, mCharIndices, x, y, FontSizeInverseFactor, mCharCount, inDepthValue);

	outId = mCharCount;
	mCharCount += inText.size();
	return dim;
}

FastText_base::TextDimensions Jkr::Renderer::FastText_base::GetTextDimensions(const std::string_view inString, kstd::f32 inFontSizeInverseFactor)
{
	auto inTextSize = inString.size();

	const auto ResizeFactor = 2.0f;
	float posx = 0;
	float maxH = 0.0f;
	for (uint32_t i = 0; i < inTextSize; i++)
	{
		BmChar char_info = mFontDescription.fchars[(int)inString[i]];

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
	return TextDimensions{ .mWidth = posx, .mHeight = maxH };
}

FastText_base::TextDimensions Jkr::Renderer::FastText_base::UpdateText(uint32_t inId, const std::string_view inText, uint32_t inX, uint32_t inY, uint32_t inDepthValue)
{
	auto prop = GetTextProperty();
	auto dim = GetTextDimensions(inText);
	uint32_t x = 0, y = 0;
	switch (mCurrentTextProp.H)
	{
	case AlignH::Left:
		x = inX;
		break;
	case AlignH::Right:
		x = inX - dim.mWidth;
		break;
	case AlignH::Center:
		x = inX - dim.mWidth / 2.0f;
		break;
	}

	switch (mCurrentTextProp.V)
	{
	case AlignV::Bottom:
		y = inY;
		break;
	case AlignV::Top:
		y = inY + dim.mHeight;
		break;
	case AlignV::Center:
		y = inY + dim.mHeight / 2.0f;
		break;
	}

	GenerateQuadsAt(inText, mCharVertices, mCharIndices, x, y, 1.0f, inId, inDepthValue);
	return dim;
}


