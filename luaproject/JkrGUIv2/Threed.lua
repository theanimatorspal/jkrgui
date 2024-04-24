require "JkrGUIv2.Basic"
--[============================================================[
    CAMERA 3D
]============================================================]
Jkrmt = {}

--[============================================================[
    GLTF Utils 3D
]============================================================]

Jkrmt.GetPBRBasic_VertexShaderLayout = function()
    return [[

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;

layout (location = 0) out vec2 vUV;
layout (location = 1) out vec3 vNormal;
layout(push_constant, std430) uniform pc {
	mat4 model;
    mat4 m2;
} push;

layout(set = 0, binding = 0) uniform UBO {
   mat4 view;
   mat4 proj;
   vec3 campos;
   vec4 lights[8];
} ubo;

]]
end

Jkrmt.GetPBRBasic_VertexShaderMain = function()
    return [[

void GlslMain()
{
	gl_Position = ubo.proj * ubo.view * push.model * vec4(inPosition, 1.0f);	
	vUV = inUV;
	vNormal = inNormal;
}
   ]]
end

Jkrmt.GetPBRBasic_VertexShaderWithSkinningMain = function(inJointCounts)
    return [[
void GlslMain()
{
    vec4 jweight = inJointInfluence[gl_VertexIndex].mJointWeights;
    vec4 jindex = inJointInfluence[gl_VertexIndex].mJointIndices;
    mat4 skinMat  =
            jweight.x * jointMatrixUBO.mJointMatrix[int(jindex.x)] +
            jweight.y * jointMatrixUBO.mJointMatrix[int(jindex.y)] +
            jweight.z * jointMatrixUBO.mJointMatrix[int(jindex.z)] +
            jweight.w * jointMatrixUBO.mJointMatrix[int(jindex.w)];

	gl_Position = ubo.proj * ubo.view * push.model * skinMat * vec4(inPosition, 1.0f);	
    gl_Position.y = -gl_Position.y;
	vUV = inUV;
	vNormal = inNormal;
}
   ]]
end

Jkrmt.GetPBRBasic_FragmentShader = function()
    return [[
layout(location = 0) in vec2 vUV;
layout(location = 1) in vec3 vNormal;
layout(location = 0) out vec4 out_color;
layout(set = 0, binding = 3) uniform sampler2D image;

layout(set = 0, binding = 0) uniform UBO {
    mat4 view;
    mat4 proj;
    vec3 campos;
    vec4 lights[8];
} ubo;

layout(push_constant, std430) uniform pc {
    mat4 mvp;
    mat4 m2;
} push;

const float PI = 3.14159;
vec3 materialcolor()
{
    return vec3(texture(image, vUV));
}

// Normal Distribution Function

float D_GGX(float dotNH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2)/ (PI * denom * denom);
}

// Geometric Shadowing Function
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float GL = dotNL / (dotNL * (1.0 - k) + k);
    float GV = dotNV / (dotNV * (1.0 - k) + k);
    return GL * GV;
}

// Fresnel Function
vec3 F_Schlick(float cosTheta, float metallic)
{
    vec3 F0 = mix(vec3(0.04), materialcolor(), metallic); // material.specular
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    return F;
}

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

void GlslMain()
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(ubo.campos - vec3(0, 0, 0)); // TODO ubo.campos - PositioninWorld
    float roughness = 0.8; // TODO Use that shit
    // Specular Contribution
    vec3 Lo = vec3(0.0);
    vec3 L = normalize(vec3(ubo.lights[0]) - vec3(0, 0, 0)); // in World ko position
    Lo += BRDF(L, V, N, 1, roughness);

    // Combination With Ambient
    vec3 color = materialcolor() * 0.02;
    color += Lo;
    color = pow(color, vec3(0.4545));
    out_color = vec4(color, 1.0);
}

    ]]
end

Jkrmt.GetSkinningLayout = function(inLoadedGLTF, inIndex)
    return string.format([[
struct JointInfluence {
    vec4 mJointIndices;
    vec4 mJointWeights;
};

layout(std140, set = 0, binding = 2) readonly buffer JointInfluenceSSBOIn {
   JointInfluence inJointInfluence[ ];
};

layout(std140, set = 0, binding = 1) uniform JointMatrix {
    mat4 mJointMatrix[%d];
} jointMatrixUBO;
   ]], inLoadedGLTF:GetJointsCount(inIndex))
end

Jkrmt.GetSkyboxVertexShader = function()
    return [[
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;

layout (location = 0) out vec2 vUV;
layout (location = 1) out vec3 vNormal;
layout (location = 2) out vec3 vVertUV;
layout(push_constant, std430) uniform pc {
	mat4 model;
    mat4 m2;
} push;

layout(set = 0, binding = 0) uniform UBO {
   mat4 view;
   mat4 proj;
   vec3 campos;
   vec4 lights[8];
} ubo;

void GlslMain() {
    vec3 position = mat3(ubo.view * push.model) * inPosition.xyz;
    gl_Position = (ubo.proj * vec4(position, 0.0)).xyzz;
    vVertUV = inPosition.xyz;
}
   ]]
end

Jkrmt.GetSkyboxFragmentShader = function()
    return [[
layout (location = 2) in vec3 vVertUV;
layout(set = 0, binding = 8) uniform samplerCube Cubemap;
layout(location = 0) out vec4 frag_color;

void GlslMain()
{
    frag_color = texture(Cubemap, vVertUV);
}
]]
end

Jkr.GetGLTFInfo = function(inLoadedGLTF, inShouldPrint)
    if (inShouldPrint) then
        io.write(string.format(
            [[
GLTF:-
Vertices = %d,
Indices = %d,
Images = %d,
Textures = %d,
Materials = %d,
Nodes = %d,
Skins = %d,
Animations = %d,
ActiveAnimation = %d,
                ]],
            inLoadedGLTF:GetVerticesSize(),
            inLoadedGLTF:GetIndicesSize(),
            inLoadedGLTF:GetImagesSize(),
            inLoadedGLTF:GetTexturesSize(),
            inLoadedGLTF:GetMaterialsSize(),
            inLoadedGLTF:GetNodesSize(),
            inLoadedGLTF:GetSkinsSize(),
            inLoadedGLTF:GetAnimationsSize(),
            inLoadedGLTF:GetActiveAnimation()
        ))
    end
end

Jkr.GetGLTFShaderLayoutString = function(inLoadedGLTF)
    local BindingIndex = 0
    local Layout = [[

    ]]
    if inLoadedGLTF:GetSkinsSize() ~= 0 then
        Layout =
            string.format(
                [[

struct JointInfluence {
    vec4 mJointIndices;
    vec4 mJointWeights;
};

layout(std140, set = 0, binding = %d) readonly buffer JointInfluenceSSBOIn {
   JointInfluence inJointInfluence[ ];
};

        ]], BindingIndex
            )
        BindingIndex = BindingIndex + 1

        Layout = Layout ..
            string.format(
                [[

layout(std140, set = 0, binding = %d) uniform JointMatrix {
    mat4 mJointMatrix[%d];
} jointMatrixUBO;

            ]], BindingIndex, inLoadedGLTF:GetJointsCount(0)
            )
        BindingIndex = BindingIndex + 1;
    end
    return Layout
end

Jkr.GetGLTFVertexShader = function(inGLTF)
    return [[

#version 450
#pragma shader_stage(vertex)
#extension GL_EXT_debug_printf : enable
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
layout(location = 0) out vec3 outColor;

      ]]
        .. Jkr.GetGLTFShaderLayoutString(inGLTF) ..
        [[

layout(push_constant, std430) uniform pc {
   mat4 m1;
   mat4 m2;
} push;

void GlslMain()
{
    vec4 jweight = inJointInfluence[gl_VertexIndex].mJointWeights;
    vec4 jindex = inJointInfluence[gl_VertexIndex].mJointIndices;
    mat4 jointMatrices[2] = jointMatrixUBO.mJointMatrix;
    mat4 skinMat = jweight.x * jointMatrices[int(jindex.x)]
                                + jweight.y * jointMatrices[int(jindex.y)]
                                + jweight.z * jointMatrices[int(jindex.z)]
                                + jweight.w * jointMatrices[int(jindex.w)] ;
    gl_Position = push.m1 * skinMat * vec4(inPosition, 1.0);
    gl_Position.y = - gl_Position.y;
    outColor = inColor;
}

    ]]
end

--[============================================================[
    SHAPE Renderer 3D
]============================================================]

function Jkr.CreateShapeRenderer3D(i, w)
    return Jkr.Shape3D(i, w)
end

--[============================================================[
    Simple 3D Pipeline
]============================================================]

function Jkr.CreateSimple3DPipeline(i, w)
    return Jkr.Simple3D(i, w)
end

function Jkr.CreateShapesHelper(inS)
    local o = {}
    o.AddDemoPlane = function()
        local Plane = Jkr.Generator(Jkr.Shapes.Cube3D, vec3(10, 5, 10))
        o.demoplane = inS:Add(Plane, vec3(0, 0, 0))
    end
    return o
end
