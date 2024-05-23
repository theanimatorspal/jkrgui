require "JkrGUIv2.Basic"
require "JkrGUIv2.Widgets"
require "JkrGUIv2.Threed"
require "JkrGUIv2.Multit"
require "JkrGUIv2.ShaderFactory"

local lerp = Jmath.Lerp

Engine = {}
Engine.Load = function(self, inEnableValidation)
    self.i = Jkr.CreateInstance(inEnableValidation)
    self.e = Jkr.CreateEventManager()
    print("INSTANCE:: ", self.i)
    self.mt = Jkr.MultiThreading(self.i)
    print("SHIT")
end

Engine.GravitationalForce = 10

Engine.MakeRigidBody = function(inObject, inType)
    local o = {}
    local object = inObject
    o.object = inObject
    local scale = object.mScale
    local y = scale.y
    local x = scale.x
    local z = scale.z
    object.mIBody = mat3(
        vec3(1 / 12 * (y * y + z * z), 0, 0),
        vec3(0, 1 / 12 * (x * x + z * z), 0),
        vec3(0, 0, 1 / 12 * (x * x + y * y))
    )
    object.mMomentum = vec3(0)
    object.mVelocity = object.mMomentum / object.mMass
    object.mAngularVelocity = vec3(0)
    object.mAngularMomentum = object.mIBody * object.mAngularVelocity
    object.mForce = vec3(0.0, -object.mMass * Engine.GravitationalForce, 0.0)
    object.mTorque = vec3(0)

    local IWorld = mat3(0)
    local IWorld_inv = mat3(0)

    -- local ComputeAuxiliary = function()
    --           local Rmat = object.mRotation:GetMatrix3x3()
    --           IWorld = Rmat * object.mIBody * Jmath.Transpose(Rmat)
    --           IWorld_inv = Rmat * Jmath.Inverse(object.mIBody) * Jmath.Transpose(Rmat)
    --           object.mAngularVelocity = IWorld_inv * object.mAngularMomentum
    -- end

    -- local CalculateTorque = function()

    -- end

    o.ResetForces = function()
        object.mForce = vec3(0, -object.mMass * Engine.GravitationalForce, 0)
        object.mTorque = vec3(0)
    end

    if inType == "STATIC" then
        o.mStaticTranslation = vec3(object.mTranslation)
        o.mStatic = true
        object.mMass = 10
        o.Simulate = function(dt)
            object.mTranslation = object.mTranslation + object.mVelocity * dt
            object.mTranslation = o.mStaticTranslation
        end
    else
        o.Simulate = function(dt)
            object.mVelocity = object.mVelocity + object.mForce / object.mMass * dt
            object.mTranslation = object.mTranslation + object.mVelocity * dt
        end
    end
    return o
end

local GetVelocityAfterCollision = function(inObject1, inObject2, e, inStatic)
    if inStatic then
        return vec3(0)
    end
    local m1 = inObject1.mMass
    local m2 = inObject2.mMass
    local v1 = inObject1.mVelocity
    local v2 = inObject2.mVelocity
    return (m1 * v1 + m2 * v2 - m2 * (v1 - v2) * e) / (m1 + m2)
end

local abs = math.abs
Engine.SimulateRigidBody = function(inObjectsTable, dt, e, inCallback)
    local ObjectsSize = #inObjectsTable
    for i = 1, ObjectsSize, 1 do
        local O1 = inObjectsTable[i].object
        inObjectsTable[i].Simulate(dt)
        for j = i + 1, ObjectsSize, 1 do
            local O2 = inObjectsTable[j].object
            local CollisionThreasold = O1:GetCollisionThreashold(O2)
            if CollisionThreasold > 0.0 then
                inCallback(i, j)
                O1.mVelocity = GetVelocityAfterCollision(O1, O2, e,
                    inObjectsTable[i].mStatic)
                O2.mVelocity = GetVelocityAfterCollision(O2, O1, e,
                    inObjectsTable[j].mStatic)
                if CollisionThreasold > 0.000001 then
                    local ContactNormal = O1:GetContactNormal(O2)
                    local absX = math.abs(ContactNormal.x)
                    local absY = math.abs(ContactNormal.y)
                    local absZ = math.abs(ContactNormal.z)

                    if absX > absY and absX > absZ then
                        O1.mTranslation.x = O1.mTranslation.x +
                            ContactNormal.x * 0.01
                    elseif absY > absZ then
                        O1.mTranslation.y = O1.mTranslation.y +
                            ContactNormal.y * 0.01
                    else
                        O1.mTranslation.z = O1.mTranslation.z +
                            ContactNormal.z * 0.01
                    end
                end
            end
        end
    end
end

Engine.SimulateRigidBodySubSteps = function(inObjectsTable, dt, inSubsteps, e, inCallback)
    local SubSteps = 10
    if inSubsteps then
        SubSteps = inSubsteps
    end
    local newdt = dt / SubSteps
    for i = 1, SubSteps, 1 do
        Engine.SimulateRigidBody(inObjectsTable, newdt, e, inCallback)
    end
end


Engine.AnimateObject = function(inCallBuffer, inO1, inO2, inO, inStepValue, inStartingFrame)
    local Frame = 1
    local Value = 0.0
    local StepValue = 0.1
    if inStartingFrame then Frame = inStartingFrame end
    if inStepValue then StepValue = inStepValue end

    local c = inCallBuffer
    while Value <= 1.0 do
        local Value_ = Value
        c.PushOneTime(
            Jkr.CreateUpdatable(
                function()
                    if (inO1.mPosition_3f) then
                        local NewTranslation = lerp(inO1.mPosition_3f, inO2.mPosition_3f, Value_)
                        inO.mTranslation = NewTranslation
                    end
                    if (inO1.mRotation_Qf) then
                        local NewRotation = lerp(inO1.mRotation_Qf, inO2.mRotation_Qf, Value_)
                        inO.mRotation = NewRotation
                    end
                    if (inO1.mScale_3f) then
                        local NewScale = lerp(inO1.mScale_3f, inO2.mScale_3f, Value_)
                        inO.mScale = NewScale
                    end
                end
            ), Frame
        )
        Value = Value + StepValue
        Frame = Frame + 1
    end
    return Frame
end

Engine.Animate_4f = function(inCallBuffer, inV1, inV2, inV, inStepValue, inStartingFrame)
    local Frame = 1
    local Value = 0.0
    local StepValue = 0.1
    if inStartingFrame then Frame = inStartingFrame end
    if inStepValue then StepValue = inStepValue end

    local c = inCallBuffer
    while Value <= 1.0 do
        local Value_ = Value
        c.PushOneTime(
            Jkr.CreateUpdatable(
                function()
                    local vv = lerp(inV1, inV2, Value_)
                    inV.x = vv.x
                    inV.y = vv.y
                    inV.z = vv.z
                    inV.w = vv.w
                    --print(string.format("from %f, to: %f, value: %f, value_X: %f", inV1.x, inV2.x, vv.x, Value_))
                end
            ), Frame
        )
        Value = Value + StepValue
        Frame = Frame + 1
    end
    return Frame
end

Engine.GetGLTFInfo = function(inLoadedGLTF, inShouldPrint)
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
                ]],
            inLoadedGLTF:GetVerticesSize(),
            inLoadedGLTF:GetIndicesSize(),
            inLoadedGLTF:GetImagesSize(),
            inLoadedGLTF:GetTexturesSize(),
            inLoadedGLTF:GetMaterialsSize(),
            inLoadedGLTF:GetNodesSize(),
            inLoadedGLTF:GetSkinsSize(),
            inLoadedGLTF:GetAnimationsSize()
        ))
    end
end

Engine.CreateShaderByGLTFMaterial = function(inGLTF, inMaterialIndex)
    local Material = inGLTF:GetMaterialsRef()[inMaterialIndex]
    local vShader = Engine.Shader()
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

Jkrmt.CreateObjectByGLTFPrimitiveAndUniform = function(inWorld3d,
                                                       inGLTFModelId,
                                                       inGLTFModelInWorld3DId,
                                                       inMaterialToSimple3DIndex,
                                                       inMeshIndex,
                                                       inPrimitive)
    local uniformIndex = inWorld3d:AddUniform3D(i)
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


Engine.AddObject = function(modObjectsVector, inId, inAssociatedModel, inUniformIndex, inSimple3dIndex, inGLTFHandle,
                            inMeshIndex)
    local Object = Jkr.Object3D()
    if inId then Object.mId = inId end
    if inAssociatedModel then Object.mAssociatedModel = inAssociatedModel end
    if inUniformIndex then Object.mAssociatedUniform = inUniformIndex end
    if inSimple3dIndex then Object.mAssociatedSimple3D = inSimple3dIndex end
    if (inGLTFHandle) then
        local NodeIndices = inGLTFHandle:GetNodeIndexByMeshIndex(inMeshIndex)
        Object.mMatrix = inGLTFHandle:GetNodeMatrixByIndex(NodeIndices[1])
    end
    modObjectsVector:add(Object)
    return #modObjectsVector
end
