-- True : Will compile and store caches
-- False: Will load caches instead of compiling the shaders
local ShouldLoadCaches_b = false

--[============================================================[
        DEFAULT RESOURCES

 These are all the shaders (vertex, fragment, compute) that
 are default but can be given by the application developer.
 The Compute shader is not used anywhere in renderers, but
 *can* be used, so we have kept it as is.

 Currently there is a Line Renderer and a Shape Renderer
]============================================================]

-- For no error squiggles in VSCode
Jkr = Jkr
Jmath = Jmath
vec3 = vec3
vec4 = vec4
vec2 = vec2
uvec2 = uvec2

--[============================================================[
    UTILITY FUNCTIONS
]============================================================]

math.int = math.floor
ImportShared = function(inLibName)
    local libName = inLibName
    if ANDROID then
        libName = "lib" .. inLibName .. ".so"
    elseif _WIN32 then
        libName = inLibName .. ".dll"
    elseif APPLE then
        libName = "lib" .. inLibName .. ".so"
    end
    local f = package.loadlib(libName, "luaopen_" .. inLibName)
    f()
end


GetDefaultResource = function(inRenderer, inShaderType)
    --[============================================================[
            DEFAULT COMPUTE SHADER
    ]============================================================]
    local is3D = inRenderer == "Simple3D"
    if inShaderType == "Compute" and not is3D then
        return [[
#version 450
#extension GL_EXT_debug_printf : enable
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
} push;

layout(set = 0, binding = 0) uniform sampler2D u_atlas8;

void GlslMain()
{
	uint gID = gl_GlobalInvocationID.x;
}

       ]]
    end

    --[============================================================[
            SHAPE RENDERER RESOURCES
    ]============================================================]


    if inRenderer == "ShapeFill" or inRenderer == "ShapeImage" then
        if inShaderType == "Vertex" then
            return [[
#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 0) out vec2 outTexCoord;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
	vec4 mParams;
} push;


void GlslMain() {
	vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
	gl_Position = push.Matrix * dx;
	outTexCoord = inTexCoord;
}
           ]]
        else
            if inRenderer == "ShapeFill" then
                return [[
#version 450
#extension GL_EXT_debug_printf : enable
layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 inTexCoord;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
	vec4 mParams;
} push;

void GlslMain()
{
	outColor = push.Color;
}
                ]]
            elseif inRenderer == "ShapeImage" then
                return [[
#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 0) uniform sampler2D image;
layout(location = 0) in vec2 inTexCoord;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
	vec4 mParams;
} push;

void GlslMain()
{
	vec4 color = texture(image, inTexCoord);
	outColor = vec4(color.r * push.Color.r, color.g * push.Color.g, color.b * push.Color.b, color.a * push.Color.a);
}
                ]]
            elseif inRenderer == "ShapeImageVarDes" then
                return [[
#version 450
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 0) uniform sampler2D u_image[];
layout(location = 0) in vec2 inTextCoord;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
	vec4 mParams;
} push;

void GlslMain()
{
	vec4 color = texture(u_image[int(push.mParams.x)], inTextCoord);
	outColor = vec4(color.r * push.Color.r, color.g * push.Color.g, color.b * push.Color.b, color.a * push.Color.a);
}
                ]]
            end
        end
    end

    --[============================================================[
         LINE RENDERER RESOURCES
    ]============================================================]


    if inRenderer == "Line" then
        if inShaderType == "Vertex" then
            return [[
#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inPosition;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
} push;


void GlslMain() {
	vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
	gl_Position = push.Matrix * dx;
}
        ]]
        elseif inShaderType == "Fragment" then
            return [[
#version 450
#extension GL_EXT_debug_printf : enable
layout(location = 0) out vec4 outColor;

layout(push_constant, std430) uniform pc {
	mat4 Matrix;
	vec4 Color;
} push;

void GlslMain()
{
	outColor = push.Color;
}
            ]]
        end
    end

    --[============================================================[
         3D RENDERER RESOURCES
    ]============================================================]
    if inRenderer == "Simple3D" then
        if inShaderType == "Vertex" then
            return [[
                // Vertex Shader
                #version 450
                #pragma shader_stage(vertex)
                #extension GL_EXT_debug_printf : enable
                layout(location = 0) in vec3 inPosition;
                layout(location = 1) in vec3 inNormal;
                layout(location = 2) in vec2 inUV;
                layout(location = 3) in vec3 inColor;
                layout(location = 0) out vec3 outColor;

                layout(push_constant, std430) uniform pc {
                    mat4 m1;
                    mat4 m2;
                } push;

                void GlslMain()
                {
                        gl_Position = push.m1 * vec4(inPosition, 1.0);
                        gl_Position.y = - gl_Position.y;
                        outColor = inColor;
                }
            ]]
        elseif inShaderType == "Fragment" then
            return [[
                #version 450
                #pragma shader_stage(fragment)
                #extension GL_EXT_debug_printf : enable

                layout(location = 0) in vec3 inColor;
                layout(location = 0) out vec4 outColor;

                layout(push_constant, std430) uniform pc {
                    mat4 m1;
                    mat4 m2;
                } push;

                void GlslMain()
                {
                    outColor = vec4(inColor, 1);
                }
            ]]
        elseif inShaderType == "Compute" then
            return [[
                #version 450
                #pragma shader_stage(compute)

                void GlslMain()
                {

                }
            ]]
        end
    end
end

--[============================================================[
        GET DEFAULT CACHES
    ]============================================================]

local DefaultCaches = {}
Jkr.GetDefaultCache = function(inInstance, inRend)
    if inRend == "Line" then
        DefaultCaches["Line"] = Jkr.PainterCache(inInstance, Jkr.PainterType.Line)
        if ShouldLoadCaches_b then
            DefaultCaches["Line"]:Load("cache2/LineRendererCache.glsl")
        else
            DefaultCaches["Line"]:Store("cache2/LineRendererCache.glsl",
                GetDefaultResource("Line", "Vertex"),
                GetDefaultResource("Line", "Fragment"),
                GetDefaultResource(nil, "Compute")
            )
        end
        return DefaultCaches["Line"]
    elseif inRend == "Shape" then
        DefaultCaches["Shape"] = Jkr.ShapeRendererResources()
        DefaultCaches["Shape"]:Add(
            inInstance,
            Jkr.FillType.Fill,
            Jkr.PipelineProperties.Default,
            "cache2/ShapeFillCache.glsl",
            GetDefaultResource("ShapeFill", "Vertex"),
            GetDefaultResource("ShapeFill", "Fragment"),
            GetDefaultResource(nil, "Compute"),
            ShouldLoadCaches_b
        )
        DefaultCaches["Shape"]:Add(
            inInstance,
            Jkr.FillType.Image,
            Jkr.PipelineProperties.Default,
            "cache2/ShapeImageCache.glsl",
            GetDefaultResource("ShapeImage", "Vertex"),
            GetDefaultResource("ShapeImage", "Fragment"),
            GetDefaultResource(nil, "Compute"),
            ShouldLoadCaches_b
        )
        DefaultCaches["Shape"]:Add(
            inInstance,
            Jkr.FillType.ContinousLine,
            Jkr.PipelineProperties.Line,
            "cache2/ShapeFillCache.glsl",
            GetDefaultResource("ShapeFill", "Vertex"),
            GetDefaultResource("ShapeFill", "Fragment"),
            GetDefaultResource(nil, "Compute"),
            ShouldLoadCaches_b
        )
        return DefaultCaches["Shape"]
    end
end


--[============================================================[
    CREATE JKR INSTANCE
]============================================================]


Jkr.CreateInstance = function(inVarDesSet, inPoolSize, inThreadsCount)
    if not inVarDesSet then inVarDesSet = 1000 end
    if not inPoolSize then inPoolSize = 1000 end
    if not inThreadsCount then inThreadsCount = 7 end
    return Jkr.Instance(inVarDesSet, inPoolSize)
end

--[============================================================[
    CREATE JKR WINDOW
]============================================================]

Jkr.CreateWindow = function(inJkrInstance, inTitle, inDimension_2f, inThreadCount)
    if not inTitle then inTitle = "JkrGUIv2" end
    if not inDimension_2f then inDimension_2f = uvec2(900, 700) end
    if not inThreadCount then inThreadCount = 4 end
    return Jkr.Window(inJkrInstance, inTitle, math.int(inDimension_2f.x), math.int(inDimension_2f.y), inThreadCount)
end

--[============================================================[
    CREATE JKR EVENT MANAGER
]============================================================]

Jkr.CreateEventManager = function()
    return Jkr.EventManager()
end


--[============================================================[
    CREATE LINE RENDERER
]============================================================]


local CreateLineRenderer = function(inInstance, inCompatibleWindow, inCache)
    local DefaultCache = inCache
    if not inCache then
        DefaultCache = Jkr.GetDefaultCache(inInstance, "Line")
    end
    return Jkr.LineRenderer(inInstance, inCompatibleWindow, DefaultCache)
end

Jkr.CreateLineRenderer = function(inInstance, inCompatibleWindow, inCache)
    local o = {}
    o.handle = CreateLineRenderer(inInstance, inCompatibleWindow, inCache)
    o.recycleBin = Jkr.RecycleBin()

    o.CreateMethods = function(self)
        self.Add = function(self, inP1_3f, inP2_3f)
            if not self.recycleBin:IsEmpty() then
                local i = self.recycleBin:Get()
                self.handle:Update(i, inP1_3f, inP2_3f)
                return i
            else
                return self.handle:Add(inP1_3f, inP2_3f)
            end
        end
        self.Remove = function(self, inIndex)
            self.recycleBin:Add(inIndex)
        end
        self.Draw = function(self, w, inColor, startId, endId, inMatrix)
            self.handle:Draw(w, inColor, startId, endId, inMatrix)
        end
        self.Bind = function(self, w)
            self.handle:Bind(w)
        end
        self.Dispatch = function(self, w)
            self.handle:Dispatch(w)
        end
    end

    o:CreateMethods()
    return o
end

function Fetch(inObject)
    local f = string.dump(inObject.CreateMethods)
    local func = load(f)
    func(inObject)
end

--[============================================================[
    CREATE SHAPE RENDERER
]============================================================]

local CreateShapeRenderer = function(inInstance, inCompatibleWindow, inShapeRendererResouce)
    if inShapeRendererResouce then
        return Jkr.ShapeRenderer(inInstance, inCompatibleWindow, inShapeRendererResouce)
    else
        return Jkr.ShapeRenderer(inInstance, inCompatibleWindow, Jkr.GetDefaultCache(inInstance, "Shape"))
    end
end

Jkr.CreateShapeRenderer = function(inInstance, inCompatibleWindow, inShapeRendererResouce)
    local o = {}
    local sr = CreateShapeRenderer(inInstance, inCompatibleWindow, inShapeRendererResouce)
    o.handle = sr

    local recycleBin = Jkr.RecycleBin()
    o.Add = function(self, inGenerator, inPosition_3f)
        if not recycleBin:IsEmpty() then
            local i = recycleBin:Get()
            sr:Update(inGenerator, inPosition_3f)
            return i
        else
            return sr:Add(inGenerator, inPosition_3f)
        end
    end

    o.Remove = function(self, inId)
        recycleBin:Add(inId)
    end

    o.Update = function(self, inId, inGenerator, inPosition_3f)
        sr:Update(inId, inGenerator, inPosition_3f)
    end
    o.BindShapes = function(self, w)
        sr:BindShapes(w)
    end
    o.BindFillMode = function(self, inFillMode, inWindow)
        sr:BindFillMode(inFillMode, inWindow, Jkr.CmdParam.UI)
    end
    o.Draw = function(self, w, inColor_4f, inWindowW, inWindowH, inStartShapeId, inEndShapeId, inMatrix)
        sr:Draw(w, inColor_4f, inWindowW, inWindowH, inStartShapeId, inEndShapeId, inMatrix)
    end
    o.Dispatch = function(self, w)
        sr:Dispatch(w, Jkr.CmdParam.UI)
    end
    return o
end

local CreateShapeRendererEXT = function(inInstance, inCompatibleWindow, inCaches)
    -- TODO
end

Jkr.CreateShaperRendererEXT = function(inInstance, inCompatibleWindow, inCache)
    -- TODO
end

--[============================================================[
    CREATE TEXT RENDERERS
]============================================================]

local CreateTextRendererBestTextAlt = function(inInstance, inShapeHandle, inBestTextBase)
    return Jkr.BestText_Alt(inInstance, inShapeHandle, inBestTextBase)
end

Jkr.CreateTextRendererBestTextAlt = function(inInstance, inShapeRenderer)
    local o = {}
    local bt = Jkr.BestText_base()
    local tr = CreateTextRendererBestTextAlt(inInstance, inShapeRenderer.handle, bt)
    o.AddFontFace = function(self, inFontFileName, inSize)
        return bt:AddFontFace(inFontFileName, inSize)
    end
    o.Add = function(self, inFontId, inPosition_3f, inText)
        return tr:Add(inFontId, inPosition_3f, inText)
    end
    o.Update = function(self, inTextImageId, inFontId, inPosition_3f, inText)
        tr:Update(inTextImageId, inFontId, inPosition_3f, inText)
    end
    o.UpdatePosOnly = function(self, inTextImageId, inFontId, inPosition_3f, inText)
        tr:UpdatePosOnly(inTextImageId, inFontId, inPosition_3f, inText)
    end
    o.Draw = function(self, inTextImageId, w, inColor, inMatrix)
        tr:Draw(inTextImageId, w, inColor, inMatrix)
    end
    return o
end


--[============================================================[
    CREATE CUSTOM PAINTER
]============================================================]

Jkr.CreateCustomPainterImage = function(i, w, inWidth, inHeight)
    return Jkr.CustomPainterImage(i, w, inWidth, inHeight)
end

local CreateCustomImagePainter = function(inCacheFileName, inComputeShader)
    return Jkr.CustomImagePainter(inCacheFileName, inComputeShader)
end

Jkr.CreateCustomImagePainter = function(inCacheFileName, inComputeShader)
    local o = {}
    local cp = CreateCustomImagePainter(inCacheFileName, inComputeShader)

    o.Load = function(self, i, w)
        cp:Load(i, w)
    end

    o.Store = function(self, i, w)
        cp:Store(i, w)
    end

    o.Bind = function(self, w, inCmdParam)
        cp:Bind(w, inCmdParam)
    end

    o.BindImageFromImage = function(self, w, inCustomImagePainter, inCmdParam)
        cp:BindImageFromImage(w, inCustomImagePainter, inCmdParam)
    end

    o.Draw = function(self, w, inPushConstant, inX, inY, inZ, inCmdParam)
        cp:Draw(w, inPushConstant, inX, inY, inZ, inCmdParam)
    end

    return o
end

--[============================================================[
    MultiThreading
]============================================================]

function Jkr.MultiThreadingInject(inMultiThreading, inTable)
    if inTable then
        for i = 1, #inTable, 1 do
            inMultiThreading:Inject(inTable[i][1], inTable[i][2])
        end
    end
end

function InjectMultiThreading()
end

function ConfigureMultiThreading(inMultiThreading)
    inMultiThreading:Inject("__MultiThreadingFetchInjected__", Jkr.MultiThreadingFetchInjected)
    inMultiThreading:Inject("__GetDefaultResource__", GetDefaultResource)
    inMultiThreading:Inject("ImportShared", ImportShared)
end

--[============================================================[
    MAIN LOOPS
]============================================================]

Jkr.DebugMainLoop = function(w, e, inUpdate, inDispatch, inDraw, inPostProcess, inColor_4f, inMT, inMTDraw, inExecute)
    local oldTime = 0.0
    local i = 0
    while not e:ShouldQuit() do
        oldTime = w:GetWindowCurrentTime()
        e:ProcessEvents()

        -- /* All Updates are done here*/
        w:BeginUpdates()
        if (inUpdate) then inUpdate() end
        WindowDimension = w:GetWindowDimension()
        w:EndUpdates()

        if (inMTDraw) then inMTDraw() end
        -- /* All UI Renders are Recordeed here*/
        w:BeginUIs()
        if (inDraw) then inDraw() end
        w:EndUIs()

        -- /* All ComputeShader Invocations are Done here*/
        w:BeginDispatches()
        if (inDispatch) then inDispatch() end
        w:EndDispatches()

        -- /* All Draws (Main CmdBuffer Recording) is done here*/
        if inColor_4f then
            w:BeginDraws(inColor_4f.x, inColor_4f.y, inColor_4f.z, inColor_4f.a, 1)
        else
            w:BeginDraws(0.8, 0.8, 0.8, 1, 1)
        end
        if (inMT) then inMT:Wait() end
        if (inExecute) then inExecute() end
        --w:ExecuteUIs() -- The UI CmdBuffer is executed onto the main CmdBuffer
        w:EndDraws()

        if (inPostProcess) then inPostProcess() end

        -- /* Finally is presented onto the screen */
        w:Present()
        local delta = w:GetWindowCurrentTime() - oldTime
        if (i % 100 == 0) then
            w:SetTitle("FrameRate: " .. 1000 / delta)
        end
        i = i + 1
    end
end