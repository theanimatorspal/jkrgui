require "JkrGUIv2.require"

local ImagePainterPush          = [[
layout(std430, push_constant) uniform pc {
        vec4 mPosDimen;
        vec4 mColor;
        vec4 mParam;
} push;
    ]]

local ImagePainterPushMatrix2   = [[
layout(std430, push_constant) uniform pc {
    mat4 a;
    mat4 b;
} push;
       ]]

local ImagePainterAssistMatrix2 = [[
uvec3 gID = gl_GlobalInvocationID;
ivec2 image_size = ivec2(imageSize(storageImage));
ivec2 to_draw_at = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);
float x_cart = (float(gl_GlobalInvocationID.x) - float(image_size.x) / float(2)) / (float((image_size.x) / float(2)));
float y_cart = (float(image_size.y) / float(2) - float(gl_GlobalInvocationID.y)) / (float(image_size.y) / float(2));
vec2 xy = vec2(x_cart, y_cart);
ivec2 xy_is = ivec2(to_draw_at.x + image_size.x / 2, to_draw_at.y + image_size.y / 2);
vec4 p1 = push.a[0];
vec4 p2 = push.a[1];
vec4 p3 = push.a[2];
vec4 p4 = push.a[3];
       ]]

local ImagePainterAssist        = [[
uvec3 gID = gl_GlobalInvocationID;
ivec2 image_size = ivec2(imageSize(storageImage));
ivec2 to_draw_at = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

float x_cart = (float(gl_GlobalInvocationID.x) - float(image_size.x) / float(2)) / (float((image_size.x) / float(2)));
float y_cart = (float(image_size.y) / float(2) - float(gl_GlobalInvocationID.y)) / (float(image_size.y) / float(2));

float ColorValueX = x_cart;
float ColorValueY = y_cart;
vec2 xy = vec2(x_cart, y_cart);
ivec2 xy_is = ivec2(to_draw_at.x + image_size.x / 2, to_draw_at.y + image_size.y / 2);
vec4 p1 = push.mPosDimen;
vec4 p2 = push.mColor;
vec4 p3 = push.mParam;
      ]]


local storageVIBufferLayoutCustomImagePainter = [[
        struct Vertex {
            vec3 mPosition;
            vec3 mNormal;
            vec2 mUV;
            vec3 mColor;

        };

        layout(std430, set = 0, binding = 14) buffer VerticesSSBOIn {
            Vertex inVertices[];
        };

        struct Index {
            uint mId;
        };

        layout(std430, set = 0, binding = 15) buffer IndicesSSBOIn {
            Index inIndices[];
        };
    ]]


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
   vec4 lightDirections[8];
   mat4 shadowMatrixCascades[4];
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

float ShadowTextureProject(vec4 shadowCoord, vec2 off, float ambient)
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

local inTangent             = [[
        struct Tangent {
            vec4 mTangent;
        };

        layout(std140, set = 1, binding = 14) readonly buffer TangentSSBOIn {
            Tangent inTangent[];
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

vec3 F_Schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
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

local BRDF_LUT             = [[
vec2 BRDF(float NoV, float roughness)
{
    // Normal always points along z-axis for the 2D lookup
    const vec3 N = vec3(0.0, 0.0, 1.0);
    vec3 V = vec3(sqrt(1.0 - NoV*NoV), 0.0, NoV);

    vec2 LUT = vec2(0.0);
    for(uint i = 0u; i < NUM_SAMPLES; i++) {
        vec2 Xi = Hammersley2d(i, NUM_SAMPLES);
        vec3 H = ImportanceSample_GGX(Xi, roughness, N);
        vec3 L = 2.0 * dot(V, H) * H - V;

        float dotNL = max(dot(N, L), 0.0);
        float dotNV = max(dot(N, V), 0.0);
        float dotVH = max(dot(V, H), 0.0);
        float dotNH = max(dot(H, N), 0.0);

        if (dotNL > 0.0) {
            float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
            float G_Vis = (G * dotVH) / (dotNH * dotNV);
            float Fc = pow(1.0 - dotVH, 5.0);
            LUT += vec2((1.0 - Fc) * G_Vis, Fc * G_Vis);
        }
    }
    return LUT / float(NUM_SAMPLES);
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
    float phi = 2.0 * PI * Xi.x + Random(normal.xz) * 0.1;
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

local SRGBtoLINEAR         = [[

vec4 SRGBtoLINEAR(vec4 srgbIn)
{
    #define MANUAL_SRGB 1
    #ifdef MANUAL_SRGB
    #ifdef SRGB_FAST_APPROXIMATION
    vec3 linOut = pow(srgbIn.xyz,vec3(2.2));
    #else //SRGB_FAST_APPROXIMATION
    vec3 bLess = step(vec3(0.04045),srgbIn.xyz);
    vec3 linOut = mix( srgbIn.xyz/vec3(12.92), pow((srgbIn.xyz+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
    #endif //SRGB_FAST_APPROXIMATION
    return vec4(linOut,srgbIn.w);;
    #else //MANUAL_SRGB
    return srgbIn;
    #endif //MANUAL_SRGB
}

vec3 SRGBtoLINEAR(vec3 srgbIn)
{
    #define MANUAL_SRGB 1
    #ifdef MANUAL_SRGB
    #ifdef SRGB_FAST_APPROXIMATION
    vec3 linOut = pow(srgbIn.xyz,vec3(2.2));
    #else //SRGB_FAST_APPROXIMATION
    vec3 bLess = step(vec3(0.04045),srgbIn.xyz);
    vec3 linOut = mix( srgbIn.xyz/vec3(12.92), pow((srgbIn.xyz+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
    #endif //SRGB_FAST_APPROXIMATION
    return linOut;
    #else //MANUAL_SRGB
    return srgbIn;
    #endif //MANUAL_SRGB
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


local shadow_textureProj = [[
float textureProj(vec4 shadowCoord, vec2 offset, uint cascadeIndex, float ambient)
{
    float shadow = 1.0;
    float bias = 0.005;

    if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) {
        float dist = texture(shadowMap, vec3(shadowCoord.st + offset, cascadeIndex)).r;
        if (shadowCoord.w > 0 && dist < shadowCoord.z - bias) {
            shadow = ambient;
        }
    }
    return shadow;

}
]]


local filterPCF = [[
float filterPCF(vec4 sc, uint cascadeIndex, float ambient)
{
    ivec2 texDim = textureSize(shadowMap, 0).xy;
    float scale = 0.75;
    float dx = scale * 1.0 / float(texDim.x);
    float dy = scale * 1.0 / float(texDim.y);

    float shadowFactor = 0.0;
    int count = 0;
    int range = 1;

    for (int x = -range; x <= range; x++) {
        for (int y = -range; y <= range; y++) {
            shadowFactor += textureProj(sc, vec2(dx*x, dy*y), cascadeIndex, ambient);
            count++;
        }
    }
    return shadowFactor / count;
}
]]

-- STRING_STRING


-- @warning this eats up a lot of memory, @todo keep the locals, locally in this file
-- but before that, ensure that the multithreaded shader compilation works fine
Engine.Shader = function()
    local o                         = {}

    ---
    ---
    ---
    --- FOR IMAGE PAINTER COMPUTE SHADER
    ---
    ---
    ---

    o.uImage2D                      = function(inBinding, inImageName)
        if not inBinding then inBinding = 0 end
        if not inImageName then inImageName = "storageImage" end
        o.NewLine()
        o.Append(string.format([[
layout(set = 0, binding = %d, rgba8) uniform image2D %s;
    ]], inBinding, inImageName))
        return o.NewLine()
    end

    o.CInvocationLayout             = function(inX, inY, inZ)
        o.NewLine()
        o.Append(
            string.format("layout(local_size_x = %d, local_size_y = %d, local_size_z = %d) in;", inX,
                inY, inZ)
        )
        return o.NewLine()
    end

    o.ImagePainterPush              = function()
        o.NewLine()
        o.Append(ImagePainterPush)
        return o.NewLine()
    end

    o.ImagePainterPushMatrix2       = function()
        o.NewLine()
        o.Append(ImagePainterPushMatrix2)
        return o
    end

    o.ImagePainterAssistMatrix2     = function()
        o.NewLine()
        o.Append(ImagePainterAssistMatrix2)
        return o;
    end

    o.ImagePainterAssist            = function()
        o.NewLine()
        o.Append(ImagePainterAssist)
        return o.NewLine()
    end

    o.ImagePainterAssistConvolution = function(inImageNameFrom, inImageNameTo)
        o.NewLine()
        o.Append(string.format(
            [[

            ]],
            inImageNameFrom, inImageNameTo
        ))
        o.NewLine()
        o.Append [[

        ]]
        return o.NewLine()
    end

    ---
    ---
    --- CUSTOM IMAGE PAINTER FOR 3D
    ---
    --- binding = 14 for storageVertex, and 15 for storageIndex


    o.vertexStorageBufferIndex    = 14
    o.indexStorageBufferIndex     = 15

    o.ImagePainterVIStorageLayout = function()
        o.NewLine()
        o.Append(storageVIBufferLayoutCustomImagePainter)
        return o.NewLine()
    end

    ---
    ---
    --- MOSTLY FOR 3D
    ---
    ---

    o.str                         = ""

    o.Append                      = function(inStr)
        o.str = o.str .. inStr
        return o.NewLine()
    end

    o.DontAppend                  = function(inStr)
        return o -- do nothing
    end

    o.Clear                       = function()
        o.str = ""
        return o
    end

    o.NewLine                     = function()
        o.str = o.str .. "\n"
        return o
    end

    o.Indent                      = function()
        o.str = o.str .. "\t"
        return o
    end

    o.Header                      = function(inVersion)
        o.str = o.str .. string.format("#version %d", inVersion)
        o.NewLine()
        o.str = o.str .. "#extension GL_EXT_debug_printf : enable"
        o.NewLine()
        return o.NewLine()
    end

    o.VLayout                     = function()
        o.str = o.str .. vLayout
        return o.NewLine()
    end

    o.Out                         = function(inLocation, inType, inName)
        o.str = o.str .. string.format("layout (location = %d) out %s %s;", inLocation, inType, inName)
        return o.NewLine()
    end

    o.In                          = function(inLocation, inType, inName)
        o.str = o.str .. string.format("layout (location = %d) in %s %s;", inLocation, inType, inName)
        return o.NewLine()
    end

    o.outFragColor                = function()
        o.str = o.str .. "layout( location = 0 ) out vec4 outFragColor;"
        return o.NewLine()
    end

    o.Push                        = function()
        o.str = o.str .. push
        return o.NewLine()
    end

    o.Ubo                         = function()
        o.str = o.str .. Ubo
        return o.NewLine()
    end

    o.GlslMainBegin               = function()
        o.str = o.str .. "void GlslMain()"
        o.NewLine()
        o.str = o.str .. "{"
        return o.NewLine()
    end

    o.GlslMainEnd                 = function()
        o.str = o.str .. "}"
        return o.NewLine()
    end

    o.ShadowGLPosition            = function()
        o.Indent()
        o.str = o.str .. "gl_Position = Ubo.proj * Ubo.shadowMatrix * Push.model * vec4(inPosition, 1.0);"
        return o.NewLine()
    end

    o.InvertY                     = function()
        o.Indent()
        o.str = o.str .. "gl_Position.y = -gl_Position.y;"
        return o.NewLine()
    end

    o.uSampler2D                  = function(inBinding, inName, inSet)
        if inSet then
            o.str = o.str ..
                string.format("layout(set = %d, binding = %d) uniform sampler2D %s;", inSet, inBinding, inName)
        else
            o.str = o.str ..
                string.format("layout(set = 1, binding = %d) uniform sampler2D %s;", inBinding, inName)
        end
        return o.NewLine()
    end

    o.uSamplerCubeMap             = function(inBinding, inName, inSet)
        if (inSet) then
            o.str = o.str ..
                string.format("layout(set = %d, binding = %d) uniform samplerCube %s;", inSet, inBinding, inName)
        else
            o.str = o.str ..
                string.format("layout(set = 1, binding = %d) uniform samplerCube %s;", inBinding, inName)
        end
        return o.NewLine()
    end

    o.LinearizeDepth              = function()
        o.NewLine()
        o.str = o.str .. LinearizeDepth
        return o.NewLine()
    end

    o.ShadowTextureProject        = function()
        o.NewLine()
        o.str = o.str .. ShadowTextureProject
        return o.NewLine()
    end

    o.inJointInfluence            = function()
        o.Append(inJointInfluence)
        return o.NewLine()
    end

    o.inJointMatrices             = function()
        o.Append(inJointMatrices)
        return o.NewLine()
    end

    o.inTangent                   = function()
        o.Append(inTangent)
        return o.NewLine()
    end

    o.BiasMatrix                  = function()
        o.Append(BiasMatrix)
        return o.NewLine()
    end

    o.PI                          = function()
        o.Append("const float PI = 3.14159;")
        return o.NewLine()
    end

    o.D_GGX                       = function()
        o.NewLine()
        o.Append(D_GGX)
        return o.NewLine()
    end

    o.G_SchlicksmithGGX           = function()
        o.NewLine()
        o.Append(G_SchlicksmithGGX)
        return o.NewLine()
    end

    o.F_Schlick                   = function()
        o.NewLine()
        o.Append(F_Schlick)
        return o.NewLine()
    end

    o.F_SchlickR                  = function()
        o.NewLine()
        o.Append(F_SchlickR)
        return o.NewLine()
    end

    o.BRDF                        = function()
        o.NewLine()
        o.Append(BRDF)
        return o.NewLine()
    end

    o.Uncharted2Tonemap           = function()
        o.NewLine()
        o.Append(Uncharted2Tonemap)
        return o.NewLine()
    end

    o.SRGBtoLINEAR                = function()
        o.NewLine()
        o.Append(SRGBtoLINEAR)
        return o.NewLine()
    end

    o.Random                      = function()
        o.NewLine()
        o.Append(Random)
        return o.NewLine()
    end

    o.Hammerslay2d                = function()
        o.NewLine()
        o.Append(Hammerslay2d)
        return o.NewLine()
    end

    o.ImportanceSample_GGX        = function()
        o.NewLine()
        o.Append(ImportanceSample_GGX)
        return o.NewLine()
    end

    o.PrefilterEnvMap             = function()
        o.NewLine()
        o.Append(PrefilterEnvMap)
        return o.NewLine()
    end

    o.PrefilteredReflection       = function()
        o.NewLine()
        o.Append(PrefilteredReflection)
        return o.NewLine()
    end

    o.SpecularContribution        = function()
        o.NewLine()
        o.Append(SpecularContribution)
        return o.NewLine()
    end

    o.BRDF_LUT                    = function()
        o.NewLine()
        o.Append(BRDF_LUT)
        return o.NewLine()
    end

    o.MaterialColorBegin          = function()
        o.str = o.str .. "vec3 MaterialColor()"
        o.NewLine()
        o.str = o.str .. "{"
        return o.NewLine()
    end

    o.MaterialColorEnd            = function()
        o.str = o.str .. "}"
        return o.NewLine()
    end

    o.Define                      = function(inName, inValue)
        o.str = o.str .. string.format("#define %s %s", inName, inValue)
        return o.NewLine().NewLine()
    end

    o.Shadow_textureProj          = function()
        return o.Append(shadow_textureProj).NewLine()
    end

    o.filterPCF                   = function()
        return o.Append(filterPCF).NewLine()
    end

    o.Print                       = function()
        local lineNumber = 13

        for line in o.str:gmatch("[^\n]+") do
            print(lineNumber .. ": " .. line)
            lineNumber = lineNumber + 1
        end
        return o
    end
    o.gltfMaterialTextures        = {}
    o.gltfPutMaterialTextures     = function(inGLTF, inMaterialIndex)
        if inGLTF then
            local Materials = inGLTF:GetMaterials()
            local Material = Materials[inMaterialIndex + 1]
            if Material then
                local binding = 3
                if (Material.mBaseColorTextureIndex ~= -1) then
                    o.uSampler2D(binding, "uBaseColorTexture").NewLine()
                    o.gltfMaterialTextures.mBaseColorTexture = true
                    binding = binding + 1
                end
                if (Material.mMetallicRoughnessTextureIndex ~= -1) then
                    o.uSampler2D(binding, "uMetallicRoughnessTexture").NewLine()
                    o.gltfMaterialTextures.mMetallicRoughnessTexture = true
                    binding = binding + 1
                end
                if (Material.mNormalTextureIndex ~= -1) then
                    o.uSampler2D(binding, "uNormalTexture").NewLine()
                    o.gltfMaterialTextures.mNormalTexture = true
                    binding = binding + 1
                end
                if (Material.mOcclusionTextureIndex ~= -1) then
                    o.uSampler2D(binding, "uOcclusionTexture").NewLine()
                    o.gltfMaterialTextures.mOcclusionTexture = true
                    binding = binding + 1
                end
                if (Material.mEmissiveTextureIndex ~= -1) then
                    o.uSampler2D(binding, "uEmissiveTexture").NewLine()
                    o.gltfMaterialTextures.mEmissiveTextureIndex = true
                    binding = binding + 1
                end
            end
        end
        return o
    end

    return o
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
    .NewLine()

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
    .NewLine()


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
    gl_Position =  Ubo.proj * Ubo.view * vec4(vWorldPos, 1.0);
    ]]
    .GlslMainEnd()


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

        ]]
    .uSamplerCubeMap(20, "samplerIrradiance")
    .uSampler2D(3, "samplerBRDFLUT")
    .uSamplerCubeMap(21, "prefilteredMap")
    .outFragColor()
    .PI()
    .Append "#define ALBEDO vec3(material.r, material.g, material.b)"
    .NewLine()
    .Append "vec3 MaterialColor() {return vec3(material.r, material.g, material.b);}"
    .NewLine()
    .Uncharted2Tonemap()
    .D_GGX()
    .G_SchlicksmithGGX()
    .F_SchlickR()
    .F_Schlick()
    .PrefilteredReflection()
    .SpecularContribution()
    .GlslMainBegin()
    .Append [[

    material.roughness = Push.m2[0].x;
    material.metallic = Push.m2[0].y;
    material.specular = Push.m2[0].z;
    material.r = Push.m2[1].x;
    material.g = Push.m2[1].y;
    material.b = Push.m2[1].z;

    vec3 N = normalize(vNormal);
    vec3 V = normalize(Ubo.campos - vWorldPos);
    vec3 R = reflect(-V, N);

    float metallic = material.metallic;
    float roughness = material.roughness;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, ALBEDO, metallic);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < Ubo.lights.length(); i++) {
        vec3 L = normalize(Ubo.lights[i].xyz - vWorldPos);
        Lo += SpecularContribution(L, V, N, F0, metallic, roughness) * Ubo.lights[i].w;
    }

    vec2 brdf = texture(samplerBRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 reflection = PrefilteredReflection(R, roughness).rgb;	
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

    //GL_EXT_debug_printf("color: (%f, %f, %f)", color.x, color.y, color.z);
    outFragColor = vec4(color, 1.0);
    ]]
    .GlslMainEnd()


PBR.IBLF_texture = Engine.Shader()
    .Header(450)
    .NewLine()
    .In(0, "vec2", "vUV")
    .In(1, "vec3", "vNormal")
    .In(2, "vec3", "vWorldPos")
    .Ubo()
    .Push()
    .outFragColor()
    .Append [[

    struct PushConsts {
              float roughness;
              float metallic;
              float specular;
              float r;
              float g;
              float b;
    } material;

        ]]
    .uSamplerCubeMap(20, "samplerIrradiance")
    .uSampler2D(3, "samplerBRDFLUT")
    .uSamplerCubeMap(21, "prefilteredMap")
    .uSampler2D(4, "albedoMap")
    .uSampler2D(5, "normalMap")
    .uSampler2D(6, "aoMap")
    .uSampler2D(7, "metallicMap")
    .uSampler2D(8, "roughnessMap")
    .PI()
    .Append "#define ALBEDO vec3(material.r, material.g, material.b)"
    .NewLine()
    .Append "vec3 MaterialColor() {return vec3(material.r, material.g, material.b);}"
    .NewLine()
    .Uncharted2Tonemap()
    .D_GGX()
    .G_SchlicksmithGGX()
    .F_Schlick()
    .F_SchlickR()
    .PrefilteredReflection()
    .SpecularContribution()
    .Append [[

vec3 calculateNormal()
{
    vec3 tangentNormal = texture(normalMap, inUV).xyz * 2.0 - 1.0;

    vec3 N = normalize(inNormal);
    vec3 T = normalize(inTangent.xyz);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

    ]]
    .GlslMainBegin()
    .Append [[
    vec3 N = calculateNormal();
    vec3 V = normalize(ubo.campos - vWorldPos);
    vec3 R = reflect(-V, N);

    float metallic = texture(metallicMap, vUV).r;
    float roughness = texture(roughnessMap, vUV).r;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, ALBEDO, metallic);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < uboParams.lights[i].length(); i++) {
        vec3 L = normalize(uboParams.lights[i].xyz - inWorldPos);
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
    vec3 ambient = (kD * diffuse + specular) * texture(aoMap, vUV).rrr;

    vec3 color = ambient + Lo;

    // Tone mapping exposure = 1.5
    color = Uncharted2Tonemap(color * 1.5);
    color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
    // Gamma correction gamma = 0.3
    color = pow(color, vec3(1.0f / 0.3));

    outFragColor = vec4(color, 1.0);
    ]]
    .GlslMainEnd()




PBR.GenBrdfLutV = Engine.Shader()
    .Header(450)
    .Out(0, "vec2", "vUV")
    .GlslMainBegin()
    .Append [[
        vUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
        gl_Position = vec4(vUV * 2.0f - 1.0f, 0.0f, 1.0f);
    ]].InvertY()
    .GlslMainEnd()


PBR.GenBrdfLutF = Engine.Shader()
    .Header(450)
    .outFragColor()
    .In(0, "vec2", "vUV")
    .Append "const uint NUM_SAMPLES = 1024u;"
    .NewLine()
    .PI()
    .Random()
    .Hammerslay2d()
    .ImportanceSample_GGX()
    .G_SchlicksmithGGX()
    .BRDF_LUT()
    .GlslMainBegin()
    .Append [[
        outFragColor = vec4(BRDF(vUV.s, vUV.t), 0.0, 1.0);
    ]]
    .GlslMainEnd()


PBR.FilterCubeV = Engine.Shader()
    .Header(450)
    .VLayout()
    .Ubo()
    .Push()
    .Out(0, "vec3", "vUVW")
    .GlslMainBegin()
    .Append [[
    vUVW = inPosition;
    gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1.0);
    ]]
    .GlslMainEnd()

PBR.PreFilterEnvMapF = Engine.Shader()
    .Header(450)
    .PI()
    .In(0, "vec3", "inPos")
    .Push()
    .Append [[

    struct ConstantStruct {
          float roughness;
          uint numSamples;
    } consts;

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

    consts.roughness = Push.m2[0].x;
    consts.numSamples = uint(Push.m2[0].y);
    vec3 N = normalize(inPos);
    vec3 color = PrefilterEnvMap(N, consts.roughness);
    //debugPrintfEXT("PrefilterCubeMap:: Roughness: %f, Samples: %d, color: (%f, %f, %f)\n", consts.roughness, consts.numSamples, color.x, color.y, color.z);
    outFragColor = vec4(color, 1.0);

    ]]
    .GlslMainEnd()


PBR.IrradianceCubeF = Engine.Shader()
    .Header(450)
    .outFragColor()
    .Push()
    .In(0, "vec3", "vUVW")
    .Append [[
    struct {
          float deltaPhi;
          float deltaTheta;
    } consts;
    ]]
    .uSamplerCubeMap(20, "samplerEnv")
    .PI()
    .GlslMainBegin()
    .Append [[
    consts.deltaPhi = Push.m2[0].x;
    consts.deltaTheta = Push.m2[0].y;

    vec3 N = normalize(vUVW);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = cross(N, right);

    const float TWO_PI = PI * 2.0;
    const float HALF_PI = PI * 0.5;

    uint sampleCount = 0u;
    vec3 color = vec3(0.0);

    for (float phi = 0.0; phi < TWO_PI; phi += consts.deltaPhi) {
        for (float theta = 0.0; theta < HALF_PI; theta += consts.deltaTheta) {
            float sinTheta = sin(theta);
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            float cosTheta = cos(theta);
            vec3 tangentSample = vec3(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
            vec3 sampleVector = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
            color += texture(samplerEnv, sampleVector).rgb * cosTheta * sinTheta;
            sampleCount++;
        }
    }

    color  = PI * color / float(sampleCount);
    //debugPrintfEXT("deltaPhi = %f, deltaTheta = %f, color: (%f, %f, %f)\n", consts.deltaPhi, consts.deltaTheta, color.x, color.y, color.z);
    outFragColor = vec4(color, 1.0);
    ]]
    .GlslMainEnd()


PBR.BasicCompute = Engine.Shader()
    .Header(450)
    .GlslMainBegin()
    .GlslMainEnd()

PBR.EquirectangularMapToMultiVShader = Engine.Shader()
    .Header(450)
    .VLayout()
    .Push()
    .Out(0, "vec3", "localPos")
    .GlslMainBegin()
    .Append [[
    localPos = inPosition;
    gl_Position = Push.m2 * vec4(localPos, 1.0);
    ]]
    .GlslMainEnd()

PBR.EquirectangularMapToMultiFShader = Engine.Shader()
    .Header(450)
    .outFragColor()
    .In(0, "vec3", "localPos")
    .uSampler2D(0, "equirectangularMap", 0)
    .Append [[
    const vec2 invATan = vec2(0.1591, 0.3183);
    vec2 SampleSphericalMap(vec3 v)
    {
        vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
        uv *= invATan;
        uv += 0.5;
        return uv;
    }
    ]]
    .GlslMainBegin()
    .Append [[
    vec2 uv = SampleSphericalMap(normalize(localPos));
    vec3 color = texture(equirectangularMap, uv).rgb;
    outFragColor = vec4(color, 1.0);
    ]]
    .GlslMainEnd()

Deferred = {}

Deferred.ScreenQuadCompositionVertex = Engine.Shader()
    .Header(450)
    .Push()
    .Out(0, "vec2", "vUV")
    .GlslMainBegin()
    .Append [[
        vUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
        gl_Position = vec4(vUV * 2.0 - 1.0, 0.0, 1.0);
    ]]
    .GlslMainEnd()

Deferred.ScreenQuadCompositionFragment = Engine.Shader()
    .Header(450)
    .Ubo()
    .In(0, "vec2", "vUV")
    .uSampler2D(3, "inPositionImage", 1)
    .uSampler2D(4, "inNormalImage", 1)
    .uSampler2D(5, "inAlbedoImage", 1)
    .Push()
    .outFragColor()
    .GlslMainBegin()
    .Append [[
        vec3 fragPos = texture(inPositionImage, vUV).rgb;
        vec3 normal = texture(inNormalImage, vUV).rgb;
        vec4 albedo = texture(inAlbedoImage, vUV);
        #define lightCount 8
        #define ambient 0.1
        vec3 fragColor = albedo.rgb * ambient;

        vec3 N = normalize(normal);

        for(int i = 0; i < 8; ++i)
        {
            vec3 L = Ubo.lights[i].xyz - fragPos;
            float dist = length(L);
            vec3 V = Ubo.campos - fragPos;
            V = normalize(V);
            L = normalize(L);
            float attenuation = Ubo.lights[i].w / (dist * dist + 1.0f);

            // diffuse
            float NdotL = max(0.0f, dot(N, L));
            vec3 lightColor = vec3(1, 1, 1);
            vec3 diff = lightColor * albedo.rgb * NdotL * attenuation;
            // specular
            vec3 R = reflect(-L, N);
            float NdotR = max(0.0f, dot(R, V));
            vec3 spec = lightColor * albedo.a * pow(NdotR, 16.0f) * attenuation;
            fragColor += diff + spec;
        }
        outFragColor = vec4(fragColor, albedo.a);
    ]]
    .GlslMainEnd()

---@diagnostic disable-next-line: duplicate-set-field
function Deferred.GetBasicVertexHeader()
    return Engine.Shader()
        .Header(450)
        .VLayout()
        .Ubo()
        .Push()
        .Out(0, "vec3", "vNormal")
        .Out(1, "vec2", "vUV")
        .Out(2, "vec3", "vColor")
        .Out(3, "vec3", "vTangent")
        .Out(4, "vec3", "vWorldPos")
end

---@diagnostic disable-next-line: duplicate-set-field
function Deferred.GetBasicFragmentHeader()
    return Engine.Shader()
        .Header(450)
        .Ubo()
        .Push()
        .In(0, "vec3", "vNormal")
        .In(1, "vec2", "vUV")
        .In(2, "vec3", "vColor")
        .In(3, "vec3", "vTangent")
        .In(4, "vec3", "vWorldPos")
        .Out(0, "vec4", "outPosition")
        .Out(1, "vec4", "outNormal")
        .Out(2, "vec4", "outAlbedo")
end

Deferred.BasicVertex = Deferred.GetBasicVertexHeader()
    .Append [[
        struct Tangent {
            vec4 mTangent;
        };

        layout(std140, set = 1, binding = 14) readonly buffer TangentSSBOIn {
            Tangent inTangent[];
        };
    ]]
    .GlslMainBegin()
    .Append
    [[
        gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1);
        vWorldPos = vec3(Push.model * vec4(inPosition, 1));
        mat3 mNormal = transpose(inverse(mat3(Push.model)));
        vNormal = mNormal * normalize(inNormal.xyz);
        vTangent = mNormal * normalize(inTangent[gl_VertexIndex].mTangent.xyz);
        vUV = inUV;
        vColor = inColor;
    ]]
    .GlslMainEnd()

Deferred.GetBasicVertex = function()
    return Deferred.GetBasicVertexHeader()
        .Append [[
        struct Tangent {
            vec4 mTangent;
        };

        layout(std140, set = 1, binding = 14) readonly buffer TangentSSBOIn {
            Tangent inTangent[];
        };
    ]]
        .GlslMainBegin()
        .Append
        [[
        gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1);
        vWorldPos = vec3(Push.model * vec4(inPosition, 1));
        mat3 mNormal = transpose(inverse(mat3(Push.model)));
        vNormal = mNormal * normalize(inNormal.xyz);
        vec4 IN_Tangent = inTangent[gl_VertexIndex].mTangent;
        vTangent = mNormal * normalize(IN_Tangent.w > 0 ? IN_Tangent.xyz : vec3(-IN_Tangent.x, IN_Tangent.y, IN_Tangent.z));
        vUV = inUV;
        vColor = inColor;
    ]]
        .InvertY()
        .GlslMainEnd()
end

Deferred.BasicFragment = Deferred.GetBasicFragmentHeader()
    .uSampler2D(3, "samplerColor", 1)
    .uSampler2D(4, "samplerNormal", 1)
    .GlslMainBegin()
    .Append
    [[
        outPosition = vec4(vWorldPos, 1.0);
        // Calculate normal in tangent space
        vec3 N = normalize(vNormal);
        vec3 T = normalize(vTangent);
        vec3 B = cross(N, T);
        mat3 TBN = mat3(T, B, N);
        vec3 tnorm = TBN * normalize(texture(samplerNormal, vUV).xyz * 2.0 - vec3(1.0));
        outNormal = vec4(tnorm, 1.0);
        outAlbedo = texture(samplerColor, vUV);
    ]]
    .GlslMainEnd()

Shape2DShaders = {}
Shape2DShaders.GeneralVShader = Engine.Shader()
    .Header(450)
    .In(0, "vec3", "inPosition")
    .In(1, "vec2", "inTexCoord")
    .Out(0, "vec2", "outTexCoord")
    .ImagePainterPushMatrix2()
    .GlslMainBegin()
    .Append [[
    vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
    gl_Position = push.b * dx;
    outTexCoord = inTexCoord;
]]
    .GlslMainEnd()

Shape2DShaders.RoundedRectangleFShader = Engine.Shader()
    .Header(450)
    .outFragColor()
    .In(0, "vec2", "inTexCoord")
    .ImagePainterPushMatrix2()
    .GlslMainBegin()
    .Append [[
    vec4 p1 = push.a[0];
    vec4 p2 = push.a[1]; // Color
    vec4 p3 = push.a[3]; // Has Radius
    vec2 center = vec2(p1.x, p1.y);
    vec2 hw = vec2(p1.z, p1.w);
    float radius = p3.x;
    vec2 xy = inTexCoord * 2.0f - vec2(1.0f);
    vec2 Q = abs(xy - center) - hw;

    float color = distance(max(Q, vec2(0.0)), vec2(0.0)) + min(max(Q.x, Q.y), 0.0) - radius;
    color = smoothstep(-0.05, 0.05, -color);
    outFragColor = p2 * color;
    ]].GlslMainEnd()

Shape2DShaders.ShowImageFShader = Engine.Shader()
    .Header(450)
    .outFragColor()
    .In(0, "vec2", "inTexCoord")
    .Append [[
        layout(set = 0, binding = 0) uniform sampler2D image;
    ]]
    .ImagePainterPushMatrix2()
    .GlslMainBegin()
    .Append [[
    vec4 p1 = push.a[0];
    vec4 p2 = push.a[1]; // Color
    vec4 p3 = push.a[3]; // Has Radius
    outFragColor = p2 * texture(image, inTexCoord);
    ]].GlslMainEnd()


TwoDimensionalIPs = {}
TwoDimensionalIPs.Header = function()
    return Engine.Shader()
        .Header(450)
        .CInvocationLayout(16, 16, 1)
        .uImage2D()
        ---@warning for uniform pipelinelayout for descriptors
        .ImagePainterVIStorageLayout()
        .ImagePainterPushMatrix2()
        .GlslMainBegin()
        .ImagePainterAssistMatrix2()
end

TwoDimensionalIPs.HeaderWithoutBegin = function()
    return Engine.Shader()
        .Header(450)
        .CInvocationLayout(16, 16, 1)
        .uImage2D()
        ---@warning for uniform pipelinelayout for descriptors (spirv reflect)
        .ImagePainterVIStorageLayout()
        .ImagePainterPushMatrix2()
        .Append [[

vec2 NormalizeToImage(vec2 inVec, ivec2 image_size)
{
    return vec2(
        (inVec.x - image_size.x / 2.0f) / (image_size.x / 2.0f),
        (image_size.y / 2.0f - inVec.y) / (image_size.y / 2.0f)
    );
}

        ]]
end

TwoDimensionalIPs.ConstantColor =
    TwoDimensionalIPs.Header()
    .Append [[
        imageStore(storageImage, to_draw_at, p4);
    ]]
    .GlslMainEnd()

TwoDimensionalIPs.RoundedRectangle =
    TwoDimensionalIPs.Header()
    .Append [[
        vec2 center = vec2(p1.x, p1.y);
        vec2 hw = vec2(p1.z, p1.w);
        float radius = p3.x;
        vec2 Q = abs(xy - center) - hw;

        float color = distance(max(Q, vec2(0.0)), vec2(0.0)) + min(max(Q.x, Q.y), 0.0) - radius;
        color = smoothstep(-0.05, 0.05, -color);

        vec4 old_color = imageLoad(storageImage, to_draw_at);
        vec4 final_color = vec4(p2.x * color, p2.y * color, p2.z * color, p2.w * color);
        final_color = mix(final_color, old_color, p3.w);

        imageStore(storageImage, to_draw_at, final_color);
          ]]
    .GlslMainEnd()

---@note @todo Later make this ellipse
TwoDimensionalIPs.Circle =
    TwoDimensionalIPs.Header()
    .Append [[
        vec2 center = vec2(p1.x, p1.y);
        float radius = p1.z;
        vec2 Q = abs(xy - center);
        float color = length(Q) - radius;
        //color = smoothstep(-0.05, 0.05, -color);
        color = -color;

        vec4 old_color = imageLoad(storageImage, to_draw_at);
        vec4 final_color = vec4(p2.x * color, p2.y * color, p2.z * color, p2.w * color);
        final_color = mix(final_color, old_color, p3.w);
        //debugPrintfEXT("final_color %f %f %f", final_color.x, final_color.y, final_color.z);

        imageStore(storageImage, to_draw_at, final_color);
          ]]
    .GlslMainEnd()

TwoDimensionalIPs.Line =
    TwoDimensionalIPs.HeaderWithoutBegin()
    .GlslMainBegin()
    .ImagePainterAssistMatrix2()
    .Append [[
            vec2 point1 = vec2(push.b * vec4(p1.x, p1.y, 0, 1));
            vec2 point2 = vec2(push.b * vec4(p1.z, p1.w, 0, 1));
            float inv_thickness = p3.x;
            float radius = p3.y;
            float draw_thickness = p3.z;
            vec2 np_1 = NormalizeToImage(point1, image_size);
            vec2 np_2 = NormalizeToImage(point2, image_size);
            vec2 pa = xy - np_1;
            vec2 ba = np_2 - np_1;
            float h = min(1.0,
                    max(0.0, dot(pa, ba) / dot(ba, ba))
            );
            float k = 1 - (length(pa - ba * h) * inv_thickness * 10 - radius);
            if (k > draw_thickness)
            {
                imageStore(storageImage, to_draw_at, p2 * k);
            }
    ]]
    .GlslMainEnd()

TwoDimensionalIPs.Clear = TwoDimensionalIPs.Header()
    .Append [[
    imageStore(storageImage, to_draw_at, p1);
    ]]
    .GlslMainEnd()




Basics = {}

Basics.GetVertexWithTangent = function()
    return Deferred.GetBasicVertexHeader()
        .Append [[
        struct Tangent {
            vec4 mTangent;
        };

        layout(std140, set = 1, binding = 14) readonly buffer TangentSSBOIn {
            Tangent inTangent[];
        };
        ]]
        .GlslMainBegin()
        .Append [[
        gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1);
        vWorldPos = vec3(Push.model * vec4(inPosition, 1));
        mat3 mNormal = transpose(inverse(mat3(Push.model)));
        vNormal = mNormal * normalize(inNormal.xyz);
        vTangent = mNormal * normalize(inTangent[gl_VertexIndex].mTangent.xyz);
        vUV = inUV;
        vColor = inColor;
        ]]
        .GlslMainEnd()
end

Basics.GetConstantFragmentHeader = function()
    return Engine.Shader()
        .Header(450)
        .Ubo()
        .Push()
        .In(0, "vec3", "vNormal")
        .In(1, "vec2", "vUV")
        .In(2, "vec3", "vColor")
        .In(3, "vec3", "vTangent")
        .In(4, "vec3", "vWorldPos")
        .outFragColor()
end

Basics.GetBasicVertexHeaderWithTangent = function()
    return Deferred.GetBasicVertexHeader()
        .Append [[
        struct Tangent {
            vec4 mTangent;
        };

        layout(std140, set = 1, binding = 14) readonly buffer TangentSSBOIn {
            Tangent inTangent[];
        };
        ]]
end

Basics.GetBasicVertexHeaderWithoutTangent = function()
    return Deferred.GetBasicVertexHeader()
end

PBR.PreCalcImages = function(inShader)
    return inShader.uSamplerCubeMap(20, "samplerCubeMap", 0)
        .uSampler2D(23, "samplerBRDFLUT", 0)
        .uSamplerCubeMap(24, "samplerIrradiance", 0)
        .uSamplerCubeMap(25, "prefilteredMap", 0)
end

Engine.GetAppropriateShader = function(inShaderType, incompilecontext, gltfmodel, materialindex, inskinning, intangent,
                                       inextraInfo)
    if intangent == nil then if gltfmodel then intangent = true else intangent = false end end
    if inShaderType == "NORMAL" and incompilecontext == Jkr.CompileContext.Default then
        local vshader
        if intangent then
            vshader = Basics.GetBasicVertexHeaderWithTangent()
        else
            vshader = Basics.GetBasicVertexHeaderWithoutTangent()
        end
        vshader.GlslMainBegin()

        if not inskinning then
            vshader.Append [[
                    gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1);
                    vWorldPos = vec3(Push.model * vec4(inPosition, 1));
                    mat3 mNormal = transpose(inverse(mat3(Push.model)));
                    vNormal = mNormal * normalize(inNormal.xyz);
                    vUV = inUV;
                    vColor = inColor;
                    ]]
        else
            vshader.inJointInfluence()
                .inJointMatrices()
                .Append([[
                        vec4 jweight = inJointInfluence[gl_VertexIndex].mJointWeights;
                        vec4 jindex = inJointInfluence[gl_VertexIndex].mJointIndices;
                        mat4 skinMat  =
                                jweight.x * inJointMatrices[int(jindex.x)] +
                                jweight.y * inJointMatrices[int(jindex.y)] +
                                jweight.z * inJointMatrices[int(jindex.z)] +
                                jweight.w * inJointMatrices[int(jindex.w)];
                        ]])
                .Append [[
                        vec4 Pos = Push.model * skinMat * vec4(inPosition, 1.0f);
                        gl_Position = Ubo.proj * Ubo.view * Pos;
                        vWorldPos = vec3(Push.model * vec4(inPosition, 1));
                        mat3 mNormal = transpose(inverse(mat3(Push.model)));
                        vNormal = mNormal * normalize(inNormal.xyz);
                        vUV = inUV;
                        vColor = inColor;
                ]]
        end
        if intangent then
            vshader.Append
            [[
                vTangent = mNormal * normalize(inTangent[gl_VertexIndex].mTangent.xyz);
            ]]
        end

        vshader.InvertY()
        vshader.GlslMainEnd()

        local fshader = Basics.GetConstantFragmentHeader()
            .gltfPutMaterialTextures(gltfmodel, materialindex)
            .Append [[
            layout(set = 0, binding = 32) uniform sampler2DArray shadowMap;
            ]]

        if inextraInfo and inextraInfo.baseColorTexture == true then
            fshader.uSampler2D(3, "uBaseColorTexture")
        end

        PBR.PreCalcImages(fshader)

        fshader.GlslMainBegin()
        if fshader.gltfMaterialTextures.mBaseColorTexture == true then
            if fshader.gltfMaterialTextures.mEmissiveTextureIndex == true then
                fshader.Append [[
                    outFragColor = texture(uBaseColorTexture, vUV) + texture(uEmissiveTexture, vUV);
                    ]]
            else
                fshader.Append [[
                    vec4 color = Push.m2[0];
                    vec4 emission_color = Push.m2[1];
                    vec4 outC = texture(uBaseColorTexture, vUV);
                    outFragColor = vec4(outC.x * color.x, outC.y * color.y, outC.z * color.z, outC.w * color.w) + emission_color;
                    ]]
            end
        elseif inextraInfo and inextraInfo.baseColorTexture == true then
            fshader.Append [[
                    vec4 color = Push.m2[0];
                    vec4 emission_color = Push.m2[1];
                    vec4 outC = texture(uBaseColorTexture, vUV);
                    outFragColor = vec4(outC.x * color.x, outC.y * color.y, outC.z * color.z, outC.w * color.w) + emission_color;
                    ]]
        else
            fshader.Append [[
                vec4 color = Push.m2[0];
                vec4 emission_color = Push.m2[1];
                outFragColor = vec4(vColor.x * color.x, vColor.y * color.y, vColor.z * color.z, 1 * color.w) + emission_color;
            ]]
        end
        fshader.GlslMainEnd()
        return vshader, fshader
    end
    if inShaderType == "CONSTANT_COLOR" and incompilecontext == Jkr.CompileContext.Default then
        local vshader
        if intangent then
            vshader = Basics.GetBasicVertexHeaderWithTangent()
        else
            vshader = Basics.GetBasicVertexHeaderWithoutTangent()
        end
        vshader.GlslMainBegin()
        vshader.Append [[
                    gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1);
                    vWorldPos = vec3(Push.model * vec4(inPosition, 1));
                    mat3 mNormal = transpose(inverse(mat3(Push.model)));
                    vNormal = mNormal * normalize(inNormal.xyz);
                    vUV = inUV;
                    vColor = inColor;
                    ]]
        vshader.InvertY()
        vshader.GlslMainEnd()

        local fshader = Basics.GetConstantFragmentHeader()
        PBR.PreCalcImages(fshader)
        fshader.GlslMainBegin()
        fshader.Append [[
            vec4 param = Push.m2[0];
            outFragColor = param;
        ]]
        fshader.GlslMainEnd()
        return vshader, fshader
    end
    if inShaderType == "NORMAL" and incompilecontext == Jkr.CompileContext.Deferred then
        local vshader = Deferred.GetBasicVertex()
        local fshader = Deferred.GetBasicFragmentHeader()
            .gltfPutMaterialTextures(gltfmodel, materialindex)
            .GlslMainBegin()
            .Append [[
        outPosition = vec4(vWorldPos, 1.0);
        // Calculate normal in tangent space
        vec3 N = normalize(vNormal);
        vec3 T = normalize(vTangent);
        vec3 B = cross(N, T);
        mat3 TBN = mat3(T, B, N);
            ]]

        if fshader.gltfMaterialTextures.mBaseColorTexture == true then
            fshader.Append [[
                    outAlbedo = texture(uBaseColorTexture, vUV);
                    ]]
        else
            fshader.Append [[
                outAlbedo = vec4(1, 0, 0, 1);
            ]]
        end

        if fshader.gltfMaterialTextures.mNormalTexture == true then
            fshader.Append [[
                vec3 tnorm = TBN * normalize(texture(uNormalTexture, vUV).xyz * 2.0 - vec3(1.0));
                outNormal = vec4(tnorm, 1.0);
            ]]
        else
            fshader.Append [[
                outNormal = vec4(0, 0, 0, 0);
            ]]
        end

        if fshader.gltfMaterialTextures.mEmissiveTextureIndex == true then
            fshader.Append [[
                outAlbedo = outAlbedo * texture(uEmissiveTexture, vUV);
            ]]
        end

        fshader.GlslMainEnd()

        return vshader, fshader
    end

    if inShaderType == "PBR" then
        local out = Engine.CreatePBRShaderByGLTFMaterial(gltfmodel, materialindex)
        return out.vShader, out.fShader
    end
    if inShaderType == "PBR_SHADOW" then
        local out = Engine.CreatePBRShaderByGLTFMaterial(gltfmodel, materialindex, true)
        return out.vShader, out.fShader
    end
    if inShaderType == "GENERAL_UNIFORM" then
        local vShader = Engine.Shader()
            .Header(450)
            .NewLine()
            .VLayout()
            .Push()
            .Ubo()
            .GlslMainBegin()
            .Append [[
                    gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1);
            ]]
            .GlslMainEnd()

        local fShader = Engine.Shader()
            .Header(450)
            .NewLine()
            .Ubo()
            .Append [[
            layout(set = 0, binding = 32) uniform sampler2DArray shadowMap;
            ]]

        PBR.PreCalcImages(fShader)

        fShader.outFragColor()
            .GlslMainBegin()
            .Append
            [[
                outFragColor = vec4(0.0f);
            ]]
            .GlslMainEnd()
        return vShader, fShader
    end
    if inShaderType == "SKYBOX" then
        local vShader = Engine.Shader()
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
        ]])
            .NewLine()
            .GlslMainEnd()
            .NewLine()

        local fShader = Engine.Shader()
            .Header(450)
            .NewLine()
            .In(0, "vec3", "vVertUV")
            .outFragColor()
            .Ubo()

        PBR.PreCalcImages(fShader)
        fShader
            .Push()
            .Append [[
            layout(set = 0, binding = 32) uniform sampler2DArray shadowMap;
            ]]
            .GlslMainBegin()
            .Indent()
            .Append([[
            outFragColor = texture(prefilteredMap, vVertUV) * 0.5;
            outFragColor = texture(samplerIrradiance, vVertUV) * 0.5;
            outFragColor = texture(samplerCubeMap, vVertUV) * 1;
        ]])
            .GlslMainEnd()
            .NewLine()
        return vShader, fShader
    end

    if inShaderType == "CASCADED_SHADOW_DEPTH_PASS" then
        local vShader = Engine.Shader()
            .Header(450)
            .VLayout()
            .Define("SHADOW_MAP_CASCADE_COUNT", "4")
            .Push()
            .Ubo()
            .Out(0, "vec2", "outUV")
            .GlslMainBegin()
            .Append [[
                vec4 Index = Push.m2[0];
                outUV = inUV;
                vec3 pos = inPosition + vec3(Ubo.lights[int(Index.y)]);
                gl_Position = Ubo.shadowMatrixCascades[int(Index.x)] * vec4(pos, 1.0);
            ]]
            .GlslMainEnd()

        local fShader = Engine.Shader()
            .Header(450)
            .Ubo()
            .Push()
            .Append [[
            layout(set = 0, binding = 32) uniform sampler2DArray shadowMap;
            ]]

        PBR.PreCalcImages(fShader)
        fShader
            .outFragColor()
            .GlslMainBegin()
            .Append [[ outFragColor = vec4(1.0); ]]
            .GlslMainEnd()
        return vShader, fShader
    end

    if inShaderType == "CASCADE_SHADOWED" then
        local vShader = Engine.Shader()
            .Header(450)
            .VLayout()
            .Ubo()
            .Push()
            .Out(0, "vec3", "outNormal")
            .Out(1, "vec3", "outColor")
            .Out(2, "vec3", "outViewPos")
            .Out(3, "vec3", "outPos")
            .Out(4, "vec2", "outUV")
            .Append [[
            out gl_PerVertex {
                vec4 gl_Position;
            };
            ]]
            .GlslMainBegin()
            .Append [[
                vec4 Index = Push.m2[0];
                outColor = inColor;
                outNormal = inNormal;
                outUV = inUV;
                vec3 pos = inPosition + Ubo.lights[int(Index.x)].xyz;
                outPos = pos;
                outViewPos = (Ubo.view * vec4(pos.xyz, 1.0)).xyz;
                gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(pos.xyz, 1.0);
            ]]
            .GlslMainEnd()

        local fShader = Engine.Shader()
            .Header(450)
            .Define("SHADOW_MAP_CASCADE_COUNT", "4")
            .Append [[
            layout(set = 0, binding = 1) uniform sampler2DArray shadowMap;
            ]]
            .In(0, "vec3", "inNormal")
            .In(1, "vec3", "inColor")
            .In(2, "vec3", "inViewPos")
            .In(3, "vec3", "inPos")
            .In(4, "vec2", "inUV")
            .outFragColor()
            .Define("ambient", "0.3")
            .Ubo()
            .Push()
            .Append [[
            const mat4 biasMat = mat4(
                0.5, 0.0, 0.0, 0.0,
                0.0, 0.5, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.5, 0.5, 0.0, 1.0
            );
            ]]
            .Append(shadow_textureProj)
            .Append(filterPCF)
            .GlslMainBegin()
            .Append [[
            vec4 cascadeSplits = Push.m2[1];
            int enablePCF = 1;
            uint cascadeIndex = 0;
            for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
            {
                if(inViewPos.z < cascadeSplits[i])
                {
                    cascadeIndex = i + 1;
                }
            }
            vec4 shadowCoord = (biasMat * Ubo.shadowMatrix[cascadeIndex]) * vec4 (inPos, 1.0);

            float shadow = 0;
            if (enablePCF == 1) {
                shadow = filterPCF(shadowCoord / shadowCoord.w, cascadeIndex);
            } else {
                shadow = textureProj(shadowCoord / shadowCoord.w, vec2(0.0), cascadeIndex);
            }

            vec3 N = normalize(inNormal);
            vec4 lightDir = -(Ubo.lightDirections[int(Index.x)]);
            vec3 L = normalize(vec3(lightDir.x, lightDir.y, lightDir.z));
            vec3 H = normalize(L + inViewPos);
            float diffuse = max(dot(N, L), ambient);
            vec3 lightColor = vec3(1.0);
            outFragColor.rgb = max(lightColor * (diffuse), vec3(0.0));
            outFragColor.rgba *= shadow;
            ]]
            .GlslMainEnd()
        return vShader, fShader
    end
end


PBR.Setup = function(w, world3d, buffer3d, inSkyboxModelId, inWorldUniformHandle, cacheprefix, hdr_path)
    Jkr.SetupPBR(Engine.i,
        w,
        inWorldUniformHandle,
        world3d,
        buffer3d, -- shape 3d
        math.floor(inSkyboxModelId),
        cacheprefix,
        PBR.GenBrdfLutV.str,
        PBR.GenBrdfLutF.str,
        PBR.FilterCubeV.str,
        PBR.IrradianceCubeF.str,
        PBR.FilterCubeV.str,
        PBR.PreFilterEnvMapF.str,
        PBR.EquirectangularMapToMultiVShader.str,
        PBR.EquirectangularMapToMultiFShader.str,
        hdr_path);
end
