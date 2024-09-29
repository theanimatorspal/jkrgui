require "JkrGUIv2.Engine.Shader"

local lerp = Jmath.Lerp

Engine.Load = function(self, inEnableValidation)
    self.i = Jkr.CreateInstance(inEnableValidation)
    self.e = Jkr.CreateEventManager()
    self.mt = Jkr.MultiThreading(self.i)

    self.PrepareMultiThreadingAndNetworking = function(self)
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
            Server = function(inport)
                self.net.Start = function()
                    if not inport then inport = self.port else self.port = inport end
                    return Jkr.StartServer(self.port)
                end

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

            Client = function()
                self.net.Start = function(inIp, inId)
                    if not inId then inId = 0 end
                    Jkr.AddClient()
                    Jkr.ConnectFromClient(inId, inIp, self.port)
                end
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

            --@warning you have to use TCP connection as above to set the UDP's Buffer,
            --using Jkr.SetBufferSizeUDP() call before receiving or sending anything
            UDP = function()
                self.net.StartUDP = function(inPort)
                    Jkr.StartUDP(inPort)
                end
                self.net.SendUDP = function(inMessage, inDestination, inPort)
                    local msg = Jkr.ConvertToVChar(inMessage)
                    Jkr.SendUDP(msg, inDestination, inPort)
                end
                self.net.listenOnce = function()
                    if not Jkr.IsMessagesBufferEmptyUDP() then
                        local msg = Jkr.PopFrontMessagesBufferUDP()
                        return Jkr.ConvertFromVChar(msg)
                    end
                end
            end
        }
    end

    self:PrepareMultiThreadingAndNetworking()
    self.MakeGlobals = function()
        _G.mt = self.mt
        _G.gate = self.gate
        _G.net = self.net
        _G.i = self.i
        _G.e = self.e
    end

    -- in multithreading, in threads other than main thread,
    -- Globalify the main handles for convenience
    self.mt:Inject("Engine", self)
    self.mt:Inject("mt", self.mt)
    self.mt:Inject("gate", self.gate)
end


Engine.PrintGLTFInfo = function(inLoadedGLTF)
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
              vWorldPos = vec3(Pos);
              vVertexIndex = gl_VertexIndex;

              mat3 mNormal = transpose(inverse(mat3(Push.model)));
              vNormal = mNormal * normalize(inNormal.xyz);
              vec4 IN_Tangent = inTangent[gl_VertexIndex].mTangent;
              vec3 tangentttt = mNormal * normalize(IN_Tangent.w > 0 ? IN_Tangent.xyz : vec3(-IN_Tangent.x, IN_Tangent.y, IN_Tangent.z));
              vTangent = vec4(tangentttt, 1.0f);
        ]]
        )
        .NewLine()
        .InvertY()
        .GlslMainEnd()
        .NewLine().Print()

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
	color = Uncharted2Tonemap(color * 10);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction gamma = 0.3
	color = pow(color, vec3(1.0f / 2.2));

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

Engine.AddAndConfigureGLTFToWorld = function(w, inworld3d, inshape3d, ingltfmodelname, inshadertype, incompilecontext,
                                             inskinning)
    if not incompilecontext then incompilecontext = Jkr.CompileContext.Default end
    local gltfmodelindex = inworld3d:AddGLTFModel(ingltfmodelname)
    local gltfmodel = inworld3d:GetGLTFModel(gltfmodelindex)
    local shapeindex = inshape3d:Add(gltfmodel) -- this ACUTALLY loads the GLTF Model
    local Nodes = gltfmodel:GetNodesRef()
    local Meshes = gltfmodel:GetMeshesRef()
    Engine.GetGLTFInfo(gltfmodel)
    local Objects = {}

    for NodeIndex = 1, #Nodes, 1 do
        local shouldload = false
        local primitives = Nodes[NodeIndex].mMesh.mPrimitives

        for PrimitiveIndex = 1, #primitives, 1 do
            local inprimitive = primitives[PrimitiveIndex]
            local materialindex = inprimitive.mMaterialIndex

            -- [[[[[[[[[[[[[[[[[[[[[[[[[[THIS IS NOT OPTIMAL]]]]]]]]]]]]]]]]]]]]]]]]]]
            local uniform3dindex = inworld3d:AddUniform3D(Engine.i)
            local uniform = inworld3d:GetUniform3D(uniform3dindex)
            local shaderindex = inworld3d:AddSimple3D(Engine.i, w)
            local shader = inworld3d:GetSimple3D(shaderindex)
            local vshader, fshader = Engine.GetAppropriateShader(inshadertype, incompilecontext, gltfmodel, materialindex,
                inskinning)
            shader:CompileEXT(
                Engine.i,
                w,
                "cache/constant_color.glsl",
                vshader.str,
                fshader.str,
                "",
                shouldload,
                incompilecontext
            )
            local allocate_for_skinning = false
            local allocate_for_tangent = true
            if inskinning then allocate_for_skinning = true end
            uniform:Build(shader, gltfmodel, 0, allocate_for_skinning, allocate_for_tangent)
            uniform:Build(shader, gltfmodel, primitives[PrimitiveIndex])
            -- [[[[[[[[[[[[[[[[[[[[[[[[[[CHANGE THIS LATER]]]]]]]]]]]]]]]]]]]]]]]]]]

            local object = Jkr.Object3D()
            object.mId = shapeindex;
            object.mAssociatedModel = gltfmodelindex;
            object.mAssociatedUniform = uniform3dindex;
            object.mAssociatedSimple3D = shaderindex;
            object.mFirstIndex = inprimitive.mFirstIndex
            object.mIndexCount = inprimitive.mIndexCount
            object.mMatrix = Nodes[NodeIndex]:GetLocalMatrix()
            object.mP1 = NodeIndex

            Objects[#Objects + 1] = object
        end

        if #primitives == 0 and #Nodes[NodeIndex].mChildren ~= 0 then
            local object = Jkr.Object3D()
            object.mId = shapeindex;
            object.mAssociatedModel = gltfmodelindex;
            object.mAssociatedUniform = -1;
            object.mAssociatedSimple3D = -1;
            object.mFirstIndex = -1
            object.mIndexCount = -1
            object.mDrawable = 0
            object.mMatrix = Nodes[NodeIndex]:GetLocalMatrix()
            object.mP1 = NodeIndex
        end
    end

    -- @warning You've to store this Objects {} table somewhere
    for i = 1, #Objects, 1 do
        for j = 1, #Objects, 1 do
            if i ~= j then
                if gltfmodel:IsNodeParentOfByIndex(Objects[i].mP1, Objects[j].mP1) then
                    print("PARENT")
                    Objects[i]:SetParent(Objects[j])
                end
            end
        end
    end
    return Objects
end
