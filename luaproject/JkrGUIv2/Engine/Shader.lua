Engine.Shader = function()
    local o              = {}

    o.uImage2D           = function()
        o.NewLine()
        o.Append([[

layout(set = 0, binding = 0, rgba8) uniform image2D storageImage;
    ]])
        return o.NewLine()
    end

    o.CInvocationLayout  = function(inX, inY, inZ)
        o.NewLine()
        o.Append(
            string.format("layout(local_size_x = %d, local_size_y = %d, local_size_z = %d) in;", inX,
                inY, inZ)
        )
        return o.NewLine()
    end

    o.ImagePainterPush   = function()
        o.NewLine()
        o.Append(
            [[
layout(std430, push_constant) uniform pc {
        vec4 mPosDimen;
        vec4  mColor;
        vec4 mParam;
} push;
    ]]
        )
        return o.NewLine()
    end

    o.ImagePainterAssist = function()
        o.NewLine()
        o.Append(
            [[
uvec3 gID = gl_GlobalInvocationID;
ivec2 image_size = ivec2(imageSize(storageImage));
ivec2 to_draw_at = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

float x_cart = (float(gl_GlobalInvocationID.x) - float(image_size.x) / float(2)) / (float((image_size.x) / float(2)));
float y_cart = (float(image_size.y) / float(2) - float(gl_GlobalInvocationID.y)) / (float(image_size.y) / float(2));

float ColorValueX = x_cart;
float ColorValueY = y_cart;
vec2 xy = vec2(x_cart, y_cart);
      ]]
        )
        return o.NewLine()
    end


    local vLayout               = [[

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
                    ]]

    local push                  = [[

layout(push_constant, std430) uniform pc {
	mat4 model;
    mat4 m2;
} Push;
]]
    local Ubo                   = [[

layout(set = 0, binding = 0) uniform UBO {
   mat4 view;
   mat4 proj;
   vec3 campos;
   vec4 nearFar;
   vec4 lights[8];
   mat4 shadowMatrix;
} Ubo;
]]

    local LinearizeDepth        = [[

float LinearizeDepth(float depth, float near, float far)
{
  float n = near; // TODO Add an entry on uniform
  float f = far;
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));	
}
          ]]

    local ShadowTextureProject  = [[

float ShadowTextureProject(vec4 shadowCoord, vec2 off)
{
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 )
	{
		float dist = LinearizeDepth(texture(%s, shadowCoord.st + off ).r);
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z )
		{
			shadow = ambient;
		}
	}
	return shadow;
}
]]

    local inJointInfluence      = [[

struct JointInfluence {
    vec4 mJointIndices;
    vec4 mJointWeights;
};

layout(std140, set = 1, binding = 2) readonly buffer JointInfluenceSSBOIn {
   JointInfluence inJointInfluence[ ];
};

]]

    local inJointMatrices       = [[

layout(std140, set = 1, binding = 1) readonly buffer JointMatrixSSBOIn {
    mat4 inJointMatrices[ ];
};
          ]]
    local BiasMatrix            = [[

    const mat4 BiasMatrix = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0 );
    ]]

    local D_GGX                 = [[
// Normal Distribution Function
float D_GGX(float dotNH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2)/ (PI * denom * denom);
}

    ]]

    local G_SchlicksmithGGX     = [[
// Geometric Shadowing Function
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float GL = dotNL / (dotNL * (1.0 - k) + k);
    float GV = dotNV / (dotNV * (1.0 - k) + k);
    return GL * GV;
}
    ]]

    local F_Schlick             = [[
// Fresnel Function
vec3 F_Schlick(float cosTheta, float metallic)
{
    vec3 F0 = mix(vec3(0.04), MaterialColor(), metallic); // material.specular
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    return F;
}
    ]]

    local F_SchlickR            = [[
vec3 F_SchlickR(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
    ]]

    local PrefilteredReflection = [[
vec3 PrefilteredReflection(vec3 R, float roughness)
{
	const float MAX_REFLECTION_LOD = 9.0; // todo: param/const
	float lod = roughness * MAX_REFLECTION_LOD;
	float lodf = floor(lod);
	float lodc = ceil(lod);
	vec3 a = textureLod(prefilteredMap, R, lodf).rgb;
	vec3 b = textureLod(prefilteredMap, R, lodc).rgb;
	return mix(a, b, lod - lodf);
}
    ]]


    local BRDF                 = [[
// Specular BRDF composition
vec3 BRDF(vec3 L, vec3 V, vec3 N, float metallic, float roughness)
{
    vec3 H = normalize(V + L);
    float dotNV = clamp(dot(N, V), 0.0, 1.0);
    float dotNL = clamp(dot(N, L), 0.0, 1.0);
    float dotLH = clamp(dot(L, H), 0.0, 1.0);
    float dotNH = clamp(dot(N, H), 0.0, 1.0);

    // Light color fixed
    vec3 lightColor = vec3(1.0);

    vec3 color = vec3(0.0);

    if (dotNL > 0.0)
    {
        float rroughness = max(0.05, roughness); // D = Normal distribution (Distribution of the microfacets)
        float D = D_GGX(dotNH, roughness); // G = Geometric shadowing term (Microfacets shadowing)
        float G = G_SchlicksmithGGX(dotNL, dotNV, rroughness); // F = Fresnel factor (Reflectance depending on angle of incidence)
        vec3 F = F_Schlick(dotNV, metallic);

        vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

        color += spec * dotNL * lightColor;
    }

    return color;
}
    ]]

    local Uncharted2Tonemap    =
    [[
        // From http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 Uncharted2Tonemap(vec3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}
    ]]

    local Random               = [[
// Based omn http://byteblacksmith.com/improvements-to-the-canonical-one-liner-glsl-rand-for-opengl-es-2-0/
float Random(vec2 co)
{
	float a = 12.9898;
	float b = 78.233;
	float c = 43758.5453;
	float dt= dot(co.xy ,vec2(a,b));
	float sn= mod(dt,3.14);
	return fract(sin(sn) * c);
}

       ]]

    local Hammerslay2d         = [[
vec2 Hammersley2d(uint i, uint N)
{
	// Radical inverse based on http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
	uint bits = (i << 16u) | (i >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	float rdi = float(bits) * 2.3283064365386963e-10;
	return vec2(float(i) /float(N), rdi);
}
       ]]

    local ImportanceSample_GGX = [[
        // Based on http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_slides.pdf
vec3 ImportanceSample_GGX(vec2 Xi, float roughness, vec3 normal)
{
	// Maps a 2D point to a hemisphere with spread based on roughness
	float alpha = roughness * roughness;
	float phi = 2.0 * PI * Xi.x + random(normal.xz) * 0.1;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha*alpha - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	vec3 H = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

	// Tangent space
	vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangentX = normalize(cross(up, normal));
	vec3 tangentY = normalize(cross(normal, tangentX));

	// Convert to world Space
	return normalize(tangentX * H.x + tangentY * H.y + normal * H.z);
}
       ]]

    local PrefilterEnvMap      = [[

vec3 PrefilterEnvMap(vec3 R, float roughness)
{
	vec3 N = R;
	vec3 V = R;
	vec3 color = vec3(0.0);
	float totalWeight = 0.0;
	float envMapDim = float(textureSize(samplerEnv, 0).s);
	for(uint i = 0u; i < consts.numSamples; i++) {
		vec2 Xi = Hammersley2d(i, consts.numSamples);
		vec3 H = ImportanceSample_GGX(Xi, roughness, N);
		vec3 L = 2.0 * dot(V, H) * H - V;
		float dotNL = clamp(dot(N, L), 0.0, 1.0);
		if(dotNL > 0.0) {
			// Filtering based on https://placeholderart.wordpress.com/2015/07/28/implementation-notes-runtime-environment-map-filtering-for-image-based-lighting/

			float dotNH = clamp(dot(N, H), 0.0, 1.0);
			float dotVH = clamp(dot(V, H), 0.0, 1.0);

			// Probability Distribution Function
			float pdf = D_GGX(dotNH, roughness) * dotNH / (4.0 * dotVH) + 0.0001;
			// Slid angle of current smple
			float omegaS = 1.0 / (float(consts.numSamples) * pdf);
			// Solid angle of 1 pixel across all cube faces
			float omegaP = 4.0 * PI / (6.0 * envMapDim * envMapDim);
			// Biased (+1.0) mip level for better result
			float mipLevel = roughness == 0.0 ? 0.0 : max(0.5 * log2(omegaS / omegaP) + 1.0, 0.0f);
			color += textureLod(samplerEnv, L, mipLevel).rgb * dotNL;
			totalWeight += dotNL;

		}
	}
	return (color / totalWeight);
}

]]

    local SpecularContribution = [[
vec3 SpecularContribution(vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	// Precalculate vectors and dot products	
	vec3 H = normalize (V + L);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);

	// Light color fixed
	vec3 lightColor = vec3(1.0);

	vec3 color = vec3(0.0);

	if (dotNL > 0.0) {
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness);
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_Schlick(dotNV, F0);		
		vec3 spec = D * F * G / (4.0 * dotNL * dotNV + 0.001);		
		vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);			
		color += (kD * ALBEDO / PI + spec) * dotNL;
	}

	return color;
}
]]

    o.str                      = ""

    o.Append                   = function(inStr)
        o.str = o.str .. inStr
        return o.NewLine()
    end

    o.Clear                    = function()
        o.str = ""
        return o
    end

    o.NewLine                  = function()
        o.str = o.str .. "\n"
        return o
    end

    o.Indent                   = function()
        o.str = o.str .. "\t"
        return o
    end

    o.Header                   = function(inVersion)
        o.str = o.str .. string.format("#version %d", inVersion)
        o.NewLine()
        o.str = o.str .. "#extension GL_EXT_debug_printf : enable"
        return o.NewLine()
    end

    o.VLayout                  = function()
        o.str = o.str .. vLayout
        return o.NewLine()
    end

    o.Out                      = function(inLocation, inType, inName)
        o.str = o.str .. string.format("layout (location = %d) out %s %s;", inLocation, inType, inName)
        return o.NewLine()
    end

    o.In                       = function(inLocation, inType, inName)
        o.str = o.str .. string.format("layout (location = %d) in %s %s;", inLocation, inType, inName)
        return o.NewLine()
    end

    o.outFragColor             = function()
        o.str = o.str .. "layout( location = 0 ) out vec4 outFragColor;"
        return o.NewLine()
    end

    o.Push                     = function()
        o.str = o.str .. push
        return o.NewLine()
    end

    o.Ubo                      = function()
        o.str = o.str .. Ubo
        return o.NewLine()
    end

    o.GlslMainBegin            = function()
        o.str = o.str .. "void GlslMain()"
        o.NewLine()
        o.str = o.str .. "{"
        return o.NewLine()
    end

    o.GlslMainEnd              = function()
        o.str = o.str .. "}"
        return o.NewLine()
    end

    o.ShadowGLPosition         = function()
        o.Indent()
        o.str = o.str .. "gl_Position = Ubo.proj * Ubo.shadowMatrix * Push.model * vec4(inPosition, 1.0);"
        return o.NewLine()
    end

    o.InvertY                  = function()
        o.Indent()
        o.str = o.str .. "gl_Position.y = -gl_Position.y;"
        return o.NewLine()
    end

    o.uSampler2D               = function(inBinding, inName)
        o.str = o.str ..
            string.format("layout(set = 1, binding = %d) uniform sampler2D %s;", inBinding, inName)
        return o.NewLine()
    end

    o.uSamplerCubeMap          = function(inBinding, inName)
        o.str = o.str ..
            string.format("layout(set = 1, binding = %d) uniform samplerCube %s;", inBinding, inName)
        return o.NewLine()
    end

    o.LinearizeDepth           = function()
        o.NewLine()
        o.str = o.str .. LinearizeDepth
        return o.NewLine()
    end

    o.ShadowTextureProject     = function()
        o.NewLine()
        o.str = o.str .. ShadowTextureProject
        return o.NewLine()
    end

    o.inJointInfluence         = function()
        o.Append(inJointInfluence)
        return o.NewLine()
    end

    o.inJointMatrices          = function()
        o.Append(inJointMatrices)
        return o.NewLine()
    end

    o.BiasMatrix               = function()
        o.Append(BiasMatrix)
        return o.NewLine()
    end

    o.PI                       = function()
        o.Append("const float PI = 3.14159;")
        return o.NewLine()
    end

    o.D_GGX                    = function()
        o.NewLine()
        o.Append(D_GGX)
        return o.NewLine()
    end

    o.G_SchlicksmithGGX        = function()
        o.NewLine()
        o.Append(G_SchlicksmithGGX)
        return o.NewLine()
    end

    o.F_Schlick                = function()
        o.NewLine()
        o.Append(F_Schlick)
        return o.NewLine()
    end

    o.F_SchlickR               = function()
        o.NewLine()
        o.Append(F_SchlickR)
        return o.NewLine()
    end

    o.BRDF                     = function()
        o.NewLine()
        o.Append(BRDF)
        return o.NewLine()
    end

    o.Uncharted2Tonemap        = function()
        o.NewLine()
        o.Append(Uncharted2Tonemap)
        return o.NewLine()
    end

    o.Random                   = function()
        o.NewLine()
        o.Append(Random)
        return o.NewLine()
    end

    o.Hammerslay2d             = function()
        o.NewLine()
        o.Append(Hammerslay2d)
        return o.NewLine()
    end

    o.ImportanceSample_GGX     = function()
        o.NewLine()
        o.Append(ImportanceSample_GGX)
        return o.NewLine()
    end

    o.PrefilterEnvMap          = function()
        o.NewLine()
        o.Append(PrefilterEnvMap)
        return o.NewLine()
    end

    o.PrefilteredReflection    = function()
        o.NewLine()
        o.Append(PrefilteredReflection)
        return o.NewLine()
    end

    o.SpecularContribution     = function()
        o.NewLine()
        o.Append(SpecularContribution)
        return o.NewLine()
    end

    o.MaterialColorBegin       = function()
        o.str = o.str .. "vec3 MaterialColor()"
        o.NewLine()
        o.str = o.str .. "{"
        return o.NewLine()
    end

    o.MaterialColorEnd         = function()
        o.str = o.str .. "}"
        return o.NewLine()
    end

    o.Define                   = function(inName, inValue)
        o.str = o.str .. string.format("#define %s %s", inName, inValue)
        return o.NewLine().NewLine()
    end

    o.Print                    = function()
        local lineNumber = 1

        for line in o.str:gmatch("[^\n]+") do
            print(lineNumber .. ": " .. line)
            lineNumber = lineNumber + 1
        end
        return o
    end

    return o
end


Jkrmt.CreateShaderByGLTFMaterial = function(inGLTF, inMaterialIndex)
    local Material = inGLTF:GetMaterialsRef()[inMaterialIndex]
    local vShader = Jkrmt.Shader()
        .Header(450)
        .NewLine()
        .VLayout()
        .Out(0, "vec2", "vUV")
        .Out(1, "vec3", "vNormal")
        .Out(2, "vec3", "vWorldPos")
        .Push()
        .Ubo()
        .GlslMainBegin()
        .Indent()
        .Append(
            [[
              vec4 Pos = Push.model * vec4(inPosition, 1.0);
              gl_Position = Ubo.proj * Ubo.view * Pos;
              vUV = inUV;
              vNormal = vec3(Push.model) * inNormal;
              vWorldPos = vec3(Pos);
        ]]
        )
        .NewLine()
        .InvertY()
        .GlslMainEnd()
        .NewLine()
    local fShader = Jkrmt.Shader()
        .Header(450)
        .NewLine()
        .In(0, "vec2", "vUV")
        .In(1, "vec3", "vNormal")
        .In(2, "vec3", "vWorldPos")
        .outFragColor()
        .Push()
        .Ubo()

    local binding = 3
    if (Material.mBaseColorTextureIndex ~= -1) then
        fShader.uSampler2D(binding, "uBaseColorTexture").NewLine()
        binding = binding + 1
    end
    if (Material.mMetallicRoughnessTextureIndex ~= -1) then
        fShader.uSampler2D(binding, "uMetallicRoughnessTexture").NewLine()
        binding = binding + 1
    end
    if (Material.mNormalTextureIndex ~= -1) then
        fShader.uSampler2D(binding, "uNormalTexture").NewLine()
        binding = binding + 1
    end
    if (Material.mOcclusionTextureIndex ~= -1) then
        fShader.uSampler2D(binding, "uOcclusionTexture").NewLine()
        binding = binding + 1
    end
    if (Material.mEmissiveTextureIndex ~= -1) then
        fShader.uSampler2D(binding, "uEmissiveTexture").NewLine()
        binding = binding + 1
    end

    fShader.GlslMainBegin()
    if (Material.mBaseColorTextureIndex ~= -1) then
        fShader.Append([[ vec4 BaseColor = texture(uBaseColorTexture, vUV); ]])
            .NewLine()
    else
        fShader.Append([[ vec4 BaseColor = vec4(1, 1, 0, 1); ]])
            .NewLine()
    end

    fShader.Append([[outFragColor = BaseColor;]])
    fShader.GlslMainEnd()

    return { vShader = vShader.str, fShader = fShader.str }
end

Engine.CreateObjectByGLTFPrimitiveAndUniform = function(inWorld3d,
                                                        inGLTFModelId,
                                                        inGLTFModelInWorld3DId,
                                                        inMaterialToSimple3DIndex,
                                                        inMeshIndex,
                                                        inPrimitive)
    local uniformIndex = inWorld3d:AddUniform3D(Engine.i)
    local uniform = inWorld3d:GetUniform3D(uniformIndex)
    local simple3dIndex = inMaterialToSimple3DIndex[inPrimitive.mMaterialIndex + 1]
    local simple3d = inWorld3d:GetSimple3D(simple3dIndex)
    local gltf = inWorld3d:GetGLTFModel(inGLTFModelId)
    uniform:Build(simple3d, gltf, inPrimitive)

    local Object3D = Jkr.Object3D()
    Object3D.mId = inGLTFModelInWorld3DId
    Object3D.mAssociatedUniform = uniformIndex
    Object3D.mAssociatedModel = inGLTFModelId
    Object3D.mAssociatedSimple3D = simple3dIndex
    Object3D.mIndexCount = inPrimitive.mIndexCount
    Object3D.mFirstIndex = inPrimitive.mFirstIndex
    local NodeIndices = gltf:GetNodeIndexByMeshIndex(inMeshIndex - 1)
    Object3D.mMatrix = gltf:GetNodeMatrixByIndex(NodeIndices[1])

    return Object3D
end

PBR = {}

PBR.Skybox3dV = Engine.Shader()
    .Header(450)
    .NewLine()
    .VLayout()
    .Out(0, "vec3", "vVertUV")
    .Push()
    .Ubo()
    .GlslMainBegin()
    .Indent()
    .Append([[
                vec3 position = mat3(Ubo.view * Push.model) * inPosition.xyz;
                gl_Position = (Ubo.proj * vec4(position, 0.0)).xyzz;
                vVertUV = inPosition.xyz;
        ]]).InvertY()
    .NewLine()
    .GlslMainEnd()
    .NewLine().str

PBR.Skybox3dF = Engine.Shader()
    .Header(450)
    .NewLine()
    .In(0, "vec3", "vVertUV")
    .outFragColor()
    .uSamplerCubeMap(20, "samplerCubeMap")
    .Push()
    .Ubo()
    .Uncharted2Tonemap()
    .GlslMainBegin()
    .Indent()
    .Append([[
        vec3 Color = texture(samplerCubeMap, vVertUV).rgb;
          // Tone mapping
          // 0.5 = exposure
	Color = Uncharted2Tonemap(Color * 0.5);
	Color = Color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
          // 2.2 = gamma
	Color = pow(Color, vec3(1.0f / 0.3));
	outFragColor = vec4(Color, 1.0);
        ]])
    .GlslMainEnd()
    .NewLine().str

PBR.PreFilterEnvMapF = Engine.Shader()
    .Header(450)
    .NewLine()
    .In(0, "vec3", "inPos")
    .Push()
    .Append [[
    struct {
          float roughness;
          uint numSamples;
    } consts;
    consts.roughness = Push.m2[0][0];
    consts.numSamples = uint(Push.m2[1][0]);
    ]]
    .outFragColor()
    .uSamplerCubeMap(20, "samplerEnv")
    .Random()
    .Hammerslay2d()
    .ImportanceSample_GGX()
    .D_GGX()
    .PrefilterEnvMap()
    .GlslMainBegin()
    .Append [[
         	vec3 N = normalize(inPos);
	outColor = vec4(PrefilterEnvMap(N, consts.roughness), 1.0);
    ]]
    .GlslMainEnd()
    .str

PBR.IBLV = Engine.Shader()
    .Header(450)
    .NewLine()
    .VLayout()
    .Out(0, "vec2", "vUV")
    .Out(1, "vec3", "vNormal")
    .Out(2, "vec3", "vWorldPos")
    .Ubo()
    .Push()
    .GlslMainBegin()
    .Append [[
    vec3 locPos = vec3(Push.model * vec4(inPosition, 1.0));
	vWorldPos = locPos;// + pushConsts.objPos;
	vNormal = mat3(Push.model) * inNormal;
	vUV = inUV;
	vUV.t = 1.0 - inUV.t;
	gl_Position =  ubo.projection * ubo.view * vec4(vWorldPos, 1.0);
    ]]
    .GlslMainEnd()
    .str

PBR.IBLF = Engine.Shader()
    .Header(450)
    .NewLine()
    .In(0, "vec2", "vUV")
    .In(1, "vec3", "vNormal")
    .In(2, "vec3", "vWorldPos")
    .Ubo()
    .Push()
    .Append [[
struct PushConsts {
          float roughness;
          float metallic;
          float specular;
          float r;
          float g;
          float b;
} material;

material.roughness = Push.m2[0][0];
material.metallic = Push.m2[0][1];
material.specular = Push.m2[0][2];
material.r = Push.m2[1][0];
material.g = Push.m2[1][1];
material.b = Push.m2[1][2];
    ]]
    .uSamplerCubeMap(20, "samplerIrradiance")
    .uSampler2D(3, "samplerBRDFLUT")
    .uSamplerCubeMap(21, "prefilteredMap")
    .outFragColor()
    .PI()
    .Append "#define ALBEDO vec3(material.r, material.g, material.b);"
    .NewLine()
    .Uncharted2Tonemap()
    .D_GGX()
    .G_SchlicksmithGGX()
    .F_SchlickR()
    .PrefilteredReflection()
    .SpecularContribution()
    .GlslMainBegin()
    .Append [[
	vec3 N = normalize(inNormal);
	vec3 V = normalize(ubo.camPos - vWorldPos);
	vec3 R = reflect(-V, N);

	float metallic = material.metallic;
	float roughness = material.roughness;

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, ALBEDO, metallic);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < Ubo.lights.length(); i++) {
		vec3 L = normalize(uboParams.lights[i].xyz - vWorldPos);
		Lo += specularContribution(L, V, N, F0, metallic, roughness);
	}
	
	vec2 brdf = texture(samplerBRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 reflection = prefilteredReflection(R, roughness).rgb;	
	vec3 irradiance = texture(samplerIrradiance, N).rgb;

	// Diffuse based on irradiance
	vec3 diffuse = irradiance * ALBEDO;	

	vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

	// Specular reflectance
	vec3 specular = reflection * (F * brdf.x + brdf.y);

	// Ambient part
	vec3 kD = 1.0 - F;
	kD *= 1.0 - metallic;	
	vec3 ambient = (kD * diffuse + specular);
	
	vec3 color = ambient + Lo;

	// Tone mapping -- exposure = 0.5
	color = Uncharted2Tonemap(color * 0.5);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	

	// Gamma correction gamma = 3.3
	color = pow(color, vec3(1.0f / 3.3));

	outFragColor = vec4(color, 1.0);
    ]]
    .GlslMainEnd()
    .str


PBR.IrradianceCubeF = Engine.Shader()
    .Header(450)
    .VLayout()
    .Ubo()
    .Push()
    .Append [[
    struct {
          float deltaPhi;
          float deltaTheta;
    } consts;
    consts.deltaPhi = Push.m2[0].x;
    consts.deltaTheta = Push.m2[0].y;
    ]]
    .uSamplerCubeMap(20, "samplerEnv")
    .PI()
    .GlslMainBegin()
    .outFragColor()
    .Append [[
	vec3 N = normalize(inPosition);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up = cross(N, right);

	const float TWO_PI = PI * 2.0;
	const float HALF_PI = PI * 0.5;

	vec3 color = vec3(0.0);
	uint sampleCount = 0u;
	for (float phi = 0.0; phi < TWO_PI; phi += consts.deltaPhi) {
		for (float theta = 0.0; theta < HALF_PI; theta += consts.deltaTheta) {
			vec3 tempVec = cos(phi) * right + sin(phi) * up;
			vec3 sampleVector = cos(theta) * N + sin(theta) * tempVec;
			color += texture(samplerEnv, sampleVector).rgb * cos(theta) * sin(theta);
			sampleCount++;
		}
	}
	outFragColor = vec4(PI * color / float(sampleCount), 1.0);
    ]]
    .GlslMainEnd()
    .str

PBR.GenBrdfLutV = Engine.Shader()
    .Header(450)
    .Out(0, "vec2", "vUV")
    .GlslMainBegin()
    .Append [[
    	outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
    ]]
    .GlslMainEnd()
    .str

PBR.GenBrdfLutF = Engine.Shader()
    .Header(450)
    .VLayout()
    .In(0, "vec2", "vUV")
    .Append "const uint NUM_SAMPLES = 1024u;"
    .NewLine()
    .PI()
    .Random()
    .Hammerslay2d()
    .ImportanceSample_GGX()
    .G_SchlicksmithGGX()
    .BRDF()
    .GlslMainBegin()
    .Append [[
        vUV = inUV;
        outFragColor = vec4(BRDF(vUV.s, vUV.t), 0.0, 1.0);
    ]]
    .GlslMainEnd()


PBR.FilterCubeV = Engine.Shader()
    .Header(450)
    .VLayout()
    .Push()
    .Out(0, "vec2", "vUV")
    .GlslMainBegin()
    .Append [[

    ]]
    .GlslMainEnd()

PBR.BasicCompute = Engine.Shader()
    .Header(450)
    .GlslMainBegin()
    .GlslMainEnd().str
