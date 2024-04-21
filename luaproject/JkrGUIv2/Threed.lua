require "JkrGUIv2.Basic"
--[============================================================[
    CAMERA 3D
]============================================================]
Jkrmt = {}

Jkrmt.Camera3D = {
    New = function(self)
        local o = {}
        setmetatable(o, self)
        self.__index = self
        o.mTarget = vec3(0)
        o.mRight = vec3(0)
        o.mEye = vec3(0)
        o.mUp = vec3(0, 1, 0)
        o.mCameraUp = vec3(0)
        o.mDirection = vec3(0)
        o.mFov = 0.45
        o.mAspect = 16 / 9
        o.mNearZ = 0.1
        o.mFarZ = 100
        o.mMatrix = Jmath.GetIdentityMatrix4x4()
        return o
    end,
    SetAttributes = function(self, inTarget_3f, inEye_3f)
        if inTarget_3f then self.mTarget = inTarget_3f end
        if inEye_3f then self.mEye = inEye_3f end
        self.mDirection = Jmath.Normalize(self.mTarget - self.mEye)
        self.mRight = Jmath.Normalize(Jmath.Cross(self.mUp, self.mDirection))
        self.mCameraUp = Jmath.Cross(self.mDirection, self.mRight)
    end,
    MoveForward = function(self, inFactor)
        self.mEye = self.mEye + self.mDirection * inFactor
    end,
    MoveBackward = function(self, inFactor)
        self.mEye = self.mEye - self.mDirection * inFactor
    end,
    MoveLeft = function(self, inFactor)
        self.mEye = self.mEye - Jmath.Normalize(Jmath.Cross(self.mCameraUp, self.mDirection)) * inFactor
    end,
    MoveRight = function(self, inFactor)
        self.mEye = self.mEye + Jmath.Normalize(Jmath.Cross(self.mCameraUp, self.mDirection)) * inFactor
    end,
    SetPerspective = function(self, inFov, inAspect, inNearZ, inFarZ)
        if (inFov) then self.mFov = inFov end
        if (inAspect) then self.mAspect = inAspect end
        if (inNearZ) then self.mNearZ = inNearZ end
        if (inFarZ) then self.mFarZ = inFarZ end

        local lookat = Jmath.LookAt(self.mEye, self.mTarget, self.mCameraUp)
        self.mMatrix = Jmath.Perspective(self.mFov, self.mAspect, self.mNearZ, self.mFarZ) * lookat
    end
}

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
