Jkrmt.Shader = function()
  local o                    = {}
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
   vec4 lights[8];
   mat4 shadowMatrix;
} Ubo;
]]

  local LinearizeDepth       = [[

float LinearizeDepth(float depth, float near, float far)
{
  float n = 0.1; // TODO Add an entry on uniform
  float f = 10000;
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

  local JointInfluence       = [[

struct JointInfluence {
    vec4 mJointIndices;
    vec4 mJointWeights;
};

layout(std140, set = 0, binding = 2) readonly buffer JointInfluenceSSBOIn {
   JointInfluence inJointInfluence[ ];
};

]]

  local JointMatrices        = [[

layout(std140, set = 0, binding = 1) readonly buffer JointMatrixSSBOIn {
    mat4 JointMatrices[];
};
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
        string.format("layout(set = 0, binding = %d) uniform sampler2D %s;", inBinding, inName)
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

  o.JointInfluence           = function()
    o.Append(JointInfluence)
    return o.NewLine()
  end

  o.JointMatrices            = function()
    o.Append(JointMatrices)
    return o.NewLine()
  end

  return o
end

print(Jkrmt
  .Shader()
  .Header(450)
  .NewLine()
  .VLayout()
  .Push()
  .Ubo()
  .GlslMainBegin()
  .Indent()
  .Append("gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1.0);")
  .InvertY()
  .GlslMainEnd()
  .NewLine().str
)
