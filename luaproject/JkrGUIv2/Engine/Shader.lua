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


          local vLayout              = [[

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
                    ]]

          local push                 = [[

layout(push_constant, std430) uniform pc {
	mat4 model;
          mat4 m2;
} Push;
]]
          local Ubo                  = [[

layout(set = 0, binding = 0) uniform UBO {
   mat4 view;
   mat4 proj;
   vec3 campos;
   vec4 nearFar;
   vec4 lights[8];
   mat4 shadowMatrix;
} Ubo;
]]

          local LinearizeDepth       = [[

float LinearizeDepth(float depth, float near, float far)
{
  float n = near; // TODO Add an entry on uniform
  float f = far;
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));	
}
          ]]

          local ShadowTextureProject = [[

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

          local inJointInfluence     = [[

struct JointInfluence {
    vec4 mJointIndices;
    vec4 mJointWeights;
};

layout(std140, set = 1, binding = 2) readonly buffer JointInfluenceSSBOIn {
   JointInfluence inJointInfluence[ ];
};

]]

          local inJointMatrices      = [[

layout(std140, set = 1, binding = 1) readonly buffer JointMatrixSSBOIn {
    mat4 inJointMatrices[ ];
};
          ]]
          local BiasMatrix           = [[

    const mat4 BiasMatrix = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0 );
    ]]

          local D_GGX                = [[
// Normal Distribution Function
float D_GGX(float dotNH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2)/ (PI * denom * denom);
}

    ]]

          local G_SchlicksmithGGX    = [[
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

          local F_Schlick            = [[
// Fresnel Function
vec3 F_Schlick(float cosTheta, float metallic)
{
    vec3 F0 = mix(vec3(0.04), MaterialColor(), metallic); // material.specular
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    return F;
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

          o.BRDF                     = function()
                    o.NewLine()
                    o.Append(BRDF)
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
