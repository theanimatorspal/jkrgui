require "JkrGUIv2.Engine.Shader"

local lerp = Jmath.Lerp

Engine.Load = function(self, inEnableValidation)
    self.i = Jkr.CreateInstance(inEnableValidation)
    self.e = Jkr.CreateEventManager()
    print("INSTANCE:: ", self.i)
    self.mt = Jkr.MultiThreading(self.i)
    self.gate = {
        run = function(f, t)
            self.mt:AddJobFIndex(f, t)
        end
    }
    setmetatable(self.gate, {
        __index = function(_, key)
            return self.mt:Get(key, StateId)
        end,
        __newindex = function(_, k, v)
            if k == "_run" and type(v) == "function" then
                self.mt:AddJobF(v)
            elseif type(k) == "number" then
                self.mt:AddJobFIndex(v, k)
            else
                self.mt:Inject(k, v)
            end
        end,
    })

    self.port = 6345
    self.net = {}


    self.net = {
        Server = function()
            Jkr.StartServer(self.port)

            self.net.listenOnce = function(FileName)
                if not Jkr.IsMessagesBufferEmpty() then
                    local msg = Jkr.PopFrontMessagesBuffer()
                    local id = msg.mHeader.mId
                    if id == 1 then
                        local name = msg:GetString()
                        if string.sub(name, #name, #name) == ":" then -- in this format "name.txt:" then it is a file
                            self.net.listenOnce(string.sub(name, 1, #name - 1))
                        else
                            return name
                        end
                    elseif id == 2 then
                        return msg:GetFloat()
                    elseif id == 3 then
                        return load(msg:GetFunction())
                    elseif id == 4 then
                        msg:GetFile(FileName)
                    elseif id == 5 then
                        print("Message Error: Invalid Message ID in the header");
                    end
                end
            end

            self.net.BroadCast = function(inToSend)
                local msg = Jkr.Message()
                if type(inToSend) == "string" then
                    if string.sub(inToSend, 1, 1) == ":" and string.sub(inToSend, #inToSend, #inToSend) == ":" then -- represents a file :name.txt: in this way
                        local fileName = string.sub(inToSend, 2, #inToSend - 1)
                        local fileNameToBeSent = string.sub(inToSend, 2, #inToSend)
                        self.net.BroadCast(fileNameToBeSent) -- send the filename in name.txt: this way
                        msg.mHeader.mId = 4
                        msg:InsertFile(fileName)             -- and then send the file
                    else
                        msg.mHeader.mId = 1
                        msg:InsertString(inToSend)
                    end
                end
                if type(inToSend) == "number" then
                    msg.mHeader.mId = 2
                    msg:InsertFloat(inToSend)
                end
                if type(inToSend) == "function" then
                    msg.mHeader.mId = 3
                    msg:InsertFunction(inToSend)
                end
                -- TODO Improve this client ID
                Jkr.BroadcastServer(msg)
            end
        end,

        Client = function(inIp)
            Jkr.AddClient()
            Jkr.ConnectFromClient(0, inIp, self.port)
            self.net.SendToServer = function(inToSend)
                local msg = Jkr.Message()
                if type(inToSend) == "string" then
                    if string.sub(inToSend, 1, 1) == ":" and string.sub(inToSend, #inToSend, #inToSend) == ":" then -- represents a file :name.txt: in this way
                        local fileName = string.sub(inToSend, 2, #inToSend - 1)
                        local fileNameToBeSent = string.sub(inToSend, 2, #inToSend)
                        self.net.SendToServer(fileNameToBeSent) -- send the filename in name.txt: this way
                        msg.mHeader.mId = 4
                        msg:InsertFile(fileName)                -- and then send the file
                    else
                        msg.mHeader.mId = 1
                        msg:InsertString(inToSend)
                    end
                end
                if type(inToSend) == "number" then
                    msg.mHeader.mId = 2
                    msg:InsertFloat(inToSend)
                end
                if type(inToSend) == "function" then
                    msg.mHeader.mId = 3
                    msg:InsertFunction(inToSend)
                end
                -- TODO Improve this client ID
                Jkr.SendMessageFromClient(0, msg)
            end

            self.net.listenOnce = function(inFileName)
                if not Jkr.IsIncomingMessagesEmptyClient(0) then
                    local msg = Jkr.PopFrontIncomingMessagesClient(0)
                    local id = msg.mHeader.mId
                    if id == 1 then
                        local name = msg:GetString()
                        if string.sub(name, #name, #name) == ":" then -- in this format "name.txt:" then it is a file
                            self.net.listenOnce(string.sub(name, 1, #name - 1))
                        else
                            return name
                        end
                    elseif id == 2 then
                        return msg:GetFloat()
                    elseif id == 3 then
                        return load(msg:GetFunction())
                    elseif id == 4 then
                        return msg:GetFile(inFileName)
                    elseif id == 5 then
                        print("Message Error: Invalid Message ID in the header");
                    end
                end
            end
        end,
    }
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


Engine.PrintGLTFInfo = function(inLoadedGLTF, inShouldPrint)
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
Meshes = %d
                ]],
            inLoadedGLTF:GetVerticesSize(),
            inLoadedGLTF:GetIndicesSize(),
            inLoadedGLTF:GetImagesSize(),
            inLoadedGLTF:GetTexturesSize(),
            inLoadedGLTF:GetMaterialsSize(),
            inLoadedGLTF:GetNodesSize(),
            inLoadedGLTF:GetSkinsSize(),
            inLoadedGLTF:GetAnimationsSize(),
            inLoadedGLTF:GetMeshesSize()
        ))
    end
end

Engine.GetGLTFInfo = Engine.PrintGLTFInfo

Engine.CreatePBRShaderByGLTFMaterial = function(inGLTF, inMaterialIndex)
    inMaterialIndex = inMaterialIndex + 1
    local Material = inGLTF:GetMaterialsRef()[inMaterialIndex]
    local vShader = Engine.Shader()
        .Header(450)
        .NewLine()
        .VLayout()
        .Out(0, "vec2", "vUV")
        .Out(1, "vec3", "vNormal")
        .Out(2, "vec3", "vWorldPos")
        .Out(3, "vec4", "vTangent")
        .Out(4, "int", "vVertexIndex")
        .Push()
        .Ubo()
        .inTangent()
        .GlslMainBegin()
        .Indent()
        .Append(
            [[
              vec4 Pos = Push.model * vec4(inPosition, 1.0);
              gl_Position = Ubo.proj * Ubo.view * Pos;
              vUV = inUV;
              vNormal = vec3(Push.model) * inNormal;
              vWorldPos = vec3(Pos);
              vVertexIndex = gl_VertexIndex;
              vec4 tang = inTangent[gl_VertexIndex].mTangent;
              vTangent = vec4(mat3(Push.model) * tang.xyz, tang.w);
        ]]
        )
        .NewLine()
        .InvertY()
        .GlslMainEnd()
        .NewLine()

    local fShader = Engine.Shader()
        .Header(450)
        .NewLine()
        .In(0, "vec2", "vUV")
        .In(1, "vec3", "vNormal")
        .In(2, "vec3", "vWorldPos")
        .In(3, "vec4", "vTangent")
        .In(4, "flat int", "vVertexIndex")
        .uSamplerCubeMap(21, "samplerIrradiance", 0)
        .uSamplerCubeMap(22, "prefilteredMap", 0)
        .Ubo()
        .uSampler2D(10, "samplerBRDFLUT", 1)
        .outFragColor()
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
        .PI()


    local binding = 3
    if (Material.mBaseColorTextureIndex ~= -1) then
        fShader.uSampler2D(binding, "uBaseColorTexture")
        binding = binding + 1
    end
    if (Material.mMetallicRoughnessTextureIndex ~= -1) then
        fShader.uSampler2D(binding, "uMetallicRoughnessTexture")
        binding = binding + 1
    end
    if (Material.mNormalTextureIndex ~= -1) then
        fShader.uSampler2D(binding, "uNormalTexture")
        binding = binding + 1
    end
    if (Material.mOcclusionTextureIndex ~= -1) then
        fShader.uSampler2D(binding, "uOcclusionTexture")
        binding = binding + 1
    end
    if (Material.mEmissiveTextureIndex ~= -1) then
        fShader.uSampler2D(binding, "uEmissiveTexture")
        binding = binding + 1
    end

    if (Material.mMetallicRoughnessTextureIndex ~= -1) then
        fShader.Append "#define ALBEDO pow(texture(uBaseColorTexture, vUV).rgb, vec3(2.2))"
            .NewLine()
    else
        fShader
            .Append "#define ALBEDO vec3(material.r, material.g, material.b)"
            .NewLine()
    end

    fShader
        .Append "vec3 MaterialColor() {return vec3(material.r, material.g, material.b);}"
        .NewLine()
        .Uncharted2Tonemap()
        .D_GGX()
        .G_SchlicksmithGGX()
        .F_Schlick()
        .F_SchlickR()
        .PrefilteredReflection()
        .SpecularContribution()
        .SRGBtoLINEAR()
        .Append [[

vec3 calculateNormal()
{
    vec3 tangentNormal = texture(uNormalTexture, vUV).xyz * 2.0 - 1.0;

    vec3 N = normalize(vNormal);
    vec3 T = normalize(vTangent.xyz);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

        ]]

    fShader.GlslMainBegin()
    fShader.Append [[

    vec3 N = calculateNormal();
    //vec3 N = vNormal;
    vec3 V = normalize(Ubo.campos - vWorldPos);
	vec3 R = reflect(-V, N);

    ]]

    if (Material.mMetallicRoughnessTextureIndex ~= -1) then
        fShader.Append [[

            float metallic = texture(uMetallicRoughnessTexture, vUV).b;
	        float roughness = texture(uMetallicRoughnessTexture, vUV).g;
            // metallic = 0;
            // roughness = 0.5;
         ]]
    else
        fShader.Append [[

            float metallic = material.metallic;
	        float roughness = material.roughness;

         ]]
    end

    if (Material.mEmissiveTextureIndex ~= -1) then
        fShader.Append [[

    vec3 Emissive = texture(uEmissiveTexture, vUV).rgb;

        ]]
    else
        fShader.Append [[

    vec3 Emissive = vec3(1);
        ]]
    end

    fShader.Append [[


	vec3 F0 = vec3(0.04);
	F0 = mix(F0, SRGBtoLINEAR(vec4(ALBEDO, 1.0)).xyz, metallic);
	vec3 Lo = vec3(0.0);
	for(int i = 0; i < Ubo.lights[i].length(); i++) {
		vec3 L = normalize(Ubo.lights[i].xyz - vWorldPos) * Ubo.lights[i].w;
		Lo += SpecularContribution(L, V, N, F0, metallic, roughness);
	}
	
	vec2 brdf = texture(samplerBRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 reflection = SRGBtoLINEAR(PrefilteredReflection(R, roughness)).rgb;	
	vec3 irradiance = SRGBtoLINEAR(texture(samplerIrradiance, N)).rgb;

	// Diffuse based on irradiance
	vec3 diffuse = irradiance * ALBEDO;	

	vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

	// Specular reflectance
	vec3 specular = reflection * (F * brdf.x + brdf.y);

	// Ambient part
	vec3 kD = 1.0 - F;
	kD *= 1.0 - metallic;	

    ]]

    if (Material.mOcclusionTextureIndex ~= -1) then
        fShader.Append [[

            	vec3 ambient = (kD * diffuse + specular) * texture(uOcclusionTexture, vUV).rrr;
        ]]
    else
        fShader.Append [[

            	vec3 ambient = (kD * diffuse + specular);
        ]]
    end

    fShader.Append [[

    vec3 color = ambient + Lo;

    // Tone mapping exposure = 1.5
	color = Uncharted2Tonemap(color * 2);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction gamma = 0.3
	color = pow(color, vec3(1.0f / 2));

	outFragColor = vec4(color + Emissive, 1.0);
    ]]

    fShader.GlslMainEnd()

    return { vShader = vShader, fShader = fShader }
end

Engine.CreateObjectByGLTFPrimitiveAndUniform = function(inWorld3d,
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
    if inAssociatedModel then Object.mAssociatedModel = math.floor(inAssociatedModel) end
    if inUniformIndex then Object.mAssociatedUniform = math.floor(inUniformIndex) end
    if inSimple3dIndex then Object.mAssociatedSimple3D = math.floor(inSimple3dIndex) end
    if (inGLTFHandle) then
        local NodeIndices = inGLTFHandle:GetNodeIndexByMeshIndex(inMeshIndex)
        Object.mMatrix = inGLTFHandle:GetNodeMatrixByIndex(NodeIndices[1])
    end
    modObjectsVector:add(Object)
    return #modObjectsVector
end

Engine.AddAndConfigureGLTFToWorld = function(w, inworld3d, inshape3d, ingltfmodelname, inshadertype, inprimitive,
                                             inmeshindex)
    local materialindex = inprimitive.mMaterialIndex
    local meshindex = 0
    local shouldload = false
    if inmeshindex then meshindex = inmeshindex end

    local gltfmodelindex = inworld3d:AddGLTFModel(ingltfmodelname)
    local gltfmodel = inworld3d:GetGLTFModel(gltfmodelindex)
    local uniform3dindex = inworld3d:AddUniform3D(Engine.i)
    local uniform = inworld3d:GetUniform3D(uniform3dindex)

    local shaderindex = inworld3d:AddSimple3D(Engine.i, w)
    local shader = inworld3d:GetSimple3D(shaderindex)

    local shapeindex = inshape3d:Add(gltfmodel) -- this ACUTALLY loads the GLTF Model
    if inshadertype == "CONSTANT_COLOR" then
        local vshader = Basics.GetVertexWithTangent()

        local fshader = Basics.GetConstantFragmentHeader()
            .gltfPutMaterialTextures(gltfmodel, materialindex)
            .GlslMainBegin()
        if fshader.gltfMaterialTextures.mBaseColorTexture == true then
            fshader.Append [[
                    outFragColor = texture(uBaseColorTexture, vUV);
                    ]]
        else
            fshader.Append [[
                outFragColor = vec4(1, 0, 0, 1)
            ]]
        end
        fshader.GlslMainEnd()

        shader:CompileEXT(
            Engine.i,
            w,
            "cache/constant_color.glsl",
            vshader.str,
            fshader.str,
            "",
            shouldload,
            Jkr.CompileContext.Default
        )
    end
    uniform:Build(shader, gltfmodel, inprimitive)

    local object = Jkr.Object3D()
    object.mId = shapeindex;
    object.mAssociatedModel = gltfmodelindex;
    object.mAssociatedUniform = uniform3dindex;
    object.mAssociatedSimple3D = shaderindex;
    object.mFirstIndex = inprimitive.mFirstIndex
    object.mIndexCount = inprimitive.mIndexCount
    local NodeIndices = gltfmodel:GetNodeIndexByMeshIndex(meshindex - 1) --@lua indexes from one
    object.mMatrix = gltfmodel:GetNodeMatrixByIndex(NodeIndices[1])
    return object
end
