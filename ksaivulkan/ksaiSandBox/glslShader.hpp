#pragma once
#include <glm/glm.hpp>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>
#include <shaderc/shaderc.hpp>
#include <glslang/SPIRV/GlslangToSpv.h>



namespace ksai
{
	class Shader
	{
	public:
		enum class Current
		{
			NoneShader,
			VertexShader,
			FragmentShader,
			GeometryShader,
			ComputeShader
		};
		enum class UType
		{
			Uniform,
			Sampler
		};
		using ShaderStream = std::function<void(Shader& inShader)>;
		Shader(ShaderStream vertexShader, ShaderStream fragmentShader) {
			vertexShader(*this);
			fragmentShader(*this);
		}
		Shader(ShaderStream vertexShader, ShaderStream fragmentShader, ShaderStream geometryShader) {
			vertexShader(*this);
			fragmentShader(*this);
			geometryShader(*this);
		}
		Shader(uint32_t Threads, ShaderStream computeShader)
		{
			computeShader(*this);
		}
		void constexpr AddStringToShader(std::string inString) {
			switch (mCurrentOperatingShader)
			{
			case Current::VertexShader:
				mVertexShader << inString;
				break;
			case Current::FragmentShader:
				mFragmentShader << inString;
				break;
			case Current::GeometryShader:
				mGeometryShader << inString;
				break;
			case Current::ComputeShader:
				mComputeShader << inString;
				break;
			case Current::NoneShader:
				mVertexShader << inString;
				mFragmentShader << inString;
				break;
			}
		}
		inline void constexpr SetCurrentShader(Current inShader) {
			mCurrentOperatingShader = inShader;
		}
		inline void Print()
		{
			std::cout << mVertexShader.str();
			std::cout << mFragmentShader.str();
		}
		void CopySourceToGlsl(std::string ShaderFile)
		{
			int startLine = mStartLine;
			int endLine = mEndLine;
			int currentLine = 0;
			std::ifstream file(ShaderFile);
			std::string line;

			while (currentLine < startLine && std::getline(file, line))
			{
				currentLine++;
			}
			while (currentLine < (endLine - 1) && std::getline(file, line))
			{
				size_t st = line.find_first_not_of("\t");
				if (st != std::string::npos) line = line.substr(st);
				AddStringToShader(line + std::string("\n"));
				currentLine++;
			}
			file.close();
		}
		inline void constexpr SetStartLine(uint32_t inLine) { mStartLine = inLine; }
		inline void constexpr SetEndLine(uint32_t inLine) { mEndLine = inLine; }

		inline constexpr const std::ostringstream& GetVertexShader() const { return mVertexShader; }
		inline constexpr const std::ostringstream& GetFragmentShader() const { return mFragmentShader; }
		inline constexpr const std::ostringstream& GetGeometryShader() const { return mGeometryShader; }
		inline constexpr const std::ostringstream& GetComputeShader() const { return mComputeShader; }

	private:
		Current mCurrentOperatingShader = Current::NoneShader;
		std::ostringstream mVertexShader;
		std::ostringstream mFragmentShader;
		std::ostringstream mGeometryShader;
		std::ostringstream mComputeShader;
		uint32_t mStartLine = 0, mEndLine = 0;
	};
}

