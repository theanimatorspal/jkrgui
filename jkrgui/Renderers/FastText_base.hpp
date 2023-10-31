#pragma once
#include <glm/glm.hpp>
#include <stdint.h>
#include <vector>
#include <string>
#include <Global/Standards.hpp>

#define GETTER inline auto

namespace Jkr::Renderer
{
	using namespace ksai;
	class FastText_base
	{
	public:
		struct TextDimensions
		{
			float mWidth;
			float mHeight;
		};
	public:
		FastText_base(std::string inFontPath)
		{
			ParseBMFont(inFontPath.c_str(), &mFontDescription);
		}

		TextDimensions GenerateQuadsAt(const char* inString,
			std::vector<kstd::Vertex>& outVertices,
			std::vector<kstd::ui32>& outIndices,
			kstd::ui32 inX,
			kstd::ui32 inY,
			kstd::f32 inFontSizeInverseFactor,
			kstd::ui32 offset_NoOfCharactersDrawn,
			kstd::ui32 inDepthValue = 0
		);
	public:
		GETTER CharCountToVertexBytes(size_t inCharCount) { return 4 * sizeof(kstd::LineVertex) * inCharCount; }
		GETTER CharCountToIndexBytes(size_t inCharCount) { return 6 * sizeof(uint32_t) * inCharCount; }
		GETTER GetCurrentCharOffset() const { return mCharCount - 1; }
		GETTER GetVertexBufferData() { return reinterpret_cast<void*>(mCharVertices.data()); }
		GETTER GetIndexBufferData() { return  reinterpret_cast<void*>(mCharIndices.data()); }
	private:
		std::vector<ksai::kstd::Vertex>  mCharVertices;
		std::vector<ksai::kstd::ui32> mCharIndices;
		uint32_t mCharCount = 0;
	private:
		struct FontContainer
		{
			glm::vec3		ps;
			glm::vec3		scl;
			glm::vec3		rt;
		};

		struct BmChar {
			uint32_t	id;
			uint32_t	x, y;
			uint32_t	width;
			uint32_t	height;
			int32_t	xoffset;
			int32_t	yoffset;
			int32_t	xadvance;
			uint32_t	page;
		};

		struct FontFeatures
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

		struct FontDescription
		{
			char					fnt_img_pth[1 << 8];
			BmChar					fchars[1 << 8];
			FontFeatures				features;
		};

		int GetNthPair(int n, char* string_in);
		void ParseBMFont(const char* path, FontDescription* font_desp);
		FontDescription mFontDescription;
	};
};