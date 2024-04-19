--[============================================================[
    CAMERA 3D
]============================================================]

Jkr.CreateCamera3D = function(inCameraType)
    local o = {}
    o.type = inCameraType
    o.mTarget = vec3(0)
    o.mRight = vec3(0)
    o.mEye = vec3(0)
    o.mUp = vec3(0, 1, 0)
    o.mCameraUp = vec3(0)
    o.mDirection = vec3(0)
    function CreateMethods(o)
        o.SetAttributes = function(inTarget_3f, inEye_3f)
            if inTarget_3f then o.mTarget = inTarget_3f end
            if inEye_3f then o.mEye = inEye_3f end
            o.mDirection = Jmath.Normalize(o.mTarget - o.mEye)
            o.mRight = Jmath.Normalize(Jmath.Cross(o.mUp, o.mDirection))
            o.mCameraUp = Jmath.Cross(o.mDirection, o.mRight)
        end
        o.MoveForward = function(inFactor)
            o.mEye = o.mEye + o.mDirection * inFactor
        end
        o.MoveBackward = function(inFactor)
            o.mEye = o.mEye - o.mDirection * inFactor
        end
        o.MoveLeft = function(inFactor)
            o.mEye = o.mEye - Jmath.Normalize(Jmath.Cross(o.mCameraUp, o.mDirection)) * inFactor
        end
        o.MoveRight = function(inFactor)
            o.mEye = o.mEye + Jmath.Normalize(Jmath.Cross(o.mCameraUp, o.mDirection)) * inFactor
        end
        o.GetPerspective = function(inFov, inAspect, inNearZ, inFarZ)
            local lookat = Jmath.LookAt(o.mEye, o.mTarget, o.mUp)
            return Jmath.Perspective(inFov, inAspect, inNearZ, inFarZ) * lookat
        end
    end

    CreateMethods()
    return o
end

--[============================================================[
    GLTF Utils 3D
]============================================================]

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
    Simple 3D Renderer
]============================================================]

function Jkr.CreateSimple3DRenderer(i, w)
    return Jkr.Simple3D(i, w)
end
