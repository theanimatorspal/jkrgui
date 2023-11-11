#define vertex ksai::Shader::Current::VertexShader
#define fragment ksai::Shader::Current::FragmentShader
#define geometry ksai::Shader::Current::GeometryShader
#define compute ksai::Shader::Current::ComputeShader
#define uniform struct
//#define location
#define GlslBindPushConstant(name, size, T, ...) Jkr::Container<size, T, __VA_ARGS__> name(); inShader.AddStringToShader(convertToString<size, T, __VA_ARGS__>(name)) 
#define GlslBindSampler(sampler)
#define layout(...)
#define GlslBindIn(type, idx, member) type member; inShader.AddStringToShader(std::string("layout(").append("location = ").append(#idx).append(") in ").append(#type).append(" ").append(#member).append(";\n"))
#define GlslBindOut(type, idx, member) type member; inShader.AddStringToShader(std::string("layout(").append("location = ").append(#idx).append(") out ").append(#type).append(" ").append(#member).append(";\n"))
#define GlslPushConstant(x) x;
#define std430
#define push_constant
#define GlslShader(shader) inShader.SetCurrentShader(shader); inShader.AddStringToShader(std::string("#pragma shader_stage(").append(#shader).append(")\n"));
#define GlslShaderStart(shader) inShader.SetCurrentShader(shader);


#define GlslVersion(version) inShader.AddStringToShader(std::string("#version ").append(#version).append( "\n"));
#define GlslExtension(extension) inShader.AddStringToShader(std::string("#extension ").append(#extension).append(" : enable").append("\n"))
#define GL_EXT_debug_printf
#define nonuniformEXT(x) 

#define in
#define out
#define location

#define GlslMain() ();;
#define GlslCodeStart() inShader.SetStartLine(__LINE__)
#define GlslCodeFinish() inShader.SetEndLine(__LINE__); inShader.CopySourceToGlsl(__FILE__)
#define buffer struct
#define image2D
#define texture2D
#define debugPrintfEXT(...) 
#define imageStore(outImage, inIvec2, inPixelVec3)
#define imageLoad(inputImage, inIvec2) vec4(0,0, 0, 0)
#define sampler2D
#define texture(inSampler2D, inTexCoord) vec4(0, 0, 0, 0)
#define imageSize(Image) ivec2(0, 0)
#define readonly
#define writeonly
#define smoothstep(...) 0.0f
#define ShaderRawAdd(x) inShader.AddStringToShader(x)
#define sampler
#define flat
