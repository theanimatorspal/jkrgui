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

Jkr.GetDefaultResource = function(inRenderer, inShaderType, inX, inY, inZ)
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
    --[============================================================[
        CustomImagePainter Resources
    ]============================================================]
    if inRenderer == "CustomImagePainter" then
        local Header = [[

#version 450
layout(set = 0, binding = 0, rgba8) uniform image2D storageImage;

        ]]
        local InvocationLayout =
        [[

layout(local_size_x = 1, local_size_y = 1, local_size_z =1) in;

]]
        if (inX) then
            InvocationLayout = string.format([[

layout(local_size_x = %d, local_size_y = %d, local_size_z = %d) in;

        ]], inX, inY, inZ)
        end

        local Assist = [[

uvec3 gID = gl_GlobalInvocationID;
ivec2 image_size = ivec2(imageSize(storageImage));
ivec2 to_draw_at = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

float x_cart = (float(gl_GlobalInvocationID.x) - float(image_size.x) / float(2)) / (float((image_size.x) / float(2)));
float y_cart = (float(image_size.y) / float(2) - float(gl_GlobalInvocationID.y)) / (float(image_size.y) / float(2));

float ColorValueX = x_cart;
float ColorValueY = y_cart;
vec2 xy = vec2(x_cart, y_cart);

        ]]

        local PushConstant = [[

layout(std430, push_constant) uniform pc {
        vec4 mPosDimen;
        vec4  mColor;
        vec4 mParam;
} push;

        ]]

        if inShaderType == "Header" then return Header end
        if inShaderType == "Push" then return PushConstant end
        if inShaderType == "Assist" then return Assist end
        if inShaderType == "InvocationLayout" then return InvocationLayout end

        if inShaderType == "RoundedRectangle" then
            return Header .. InvocationLayout .. PushConstant .. [[

void GlslMain()
{

]] .. Assist .. [[

vec2 center = vec2(push.mPosDimen.x, push.mPosDimen.y);
vec2 hw = vec2(push.mPosDimen.z, push.mPosDimen.w);
float radius = push.mParam.x;
vec2 Q = abs(xy - center) - hw;

float color = distance(max(Q, vec2(0.0)), vec2(0.0)) + min(max(Q.x, Q.y), 0.0) - radius;
color = smoothstep(-0.05, 0.05, -color);

vec4 old_color = imageLoad(storageImage, to_draw_at);
vec4 final_color = vec4(push.mColor.x * color, push.mColor.y * color, push.mColor.z * color, push.mColor.w * color);
final_color = mix(final_color, old_color, push.mParam.w);

imageStore(storageImage, to_draw_at, final_color);
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
                Jkr.GetDefaultResource("Line", "Vertex"),
                Jkr.GetDefaultResource("Line", "Fragment"),
                Jkr.GetDefaultResource(nil, "Compute")
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
            Jkr.GetDefaultResource("ShapeFill", "Vertex"),
            Jkr.GetDefaultResource("ShapeFill", "Fragment"),
            Jkr.GetDefaultResource(nil, "Compute"),
            ShouldLoadCaches_b
        )
        DefaultCaches["Shape"]:Add(
            inInstance,
            Jkr.FillType.Image,
            Jkr.PipelineProperties.Default,
            "cache2/ShapeImageCache.glsl",
            Jkr.GetDefaultResource("ShapeImage", "Vertex"),
            Jkr.GetDefaultResource("ShapeImage", "Fragment"),
            Jkr.GetDefaultResource(nil, "Compute"),
            ShouldLoadCaches_b
        )
        DefaultCaches["Shape"]:Add(
            inInstance,
            Jkr.FillType.ContinousLine,
            Jkr.PipelineProperties.Line,
            "cache2/ShapeFillCache.glsl",
            Jkr.GetDefaultResource("ShapeFill", "Vertex"),
            Jkr.GetDefaultResource("ShapeFill", "Fragment"),
            Jkr.GetDefaultResource(nil, "Compute"),
            ShouldLoadCaches_b
        )
        return DefaultCaches["Shape"]
    end
end

Jkr.GetDefaultShapeRendererResources = function(inInstance)
    local Resource = Jkr.ShapeRendererResources()
    Resource:Add(
        inInstance,
        Jkr.FillType.Fill,
        Jkr.PipelineProperties.Default,
        "cache2/ShapeFillCache.glsl",
        Jkr.GetDefaultResource("ShapeFill", "Vertex"),
        Jkr.GetDefaultResource("ShapeFill", "Fragment"),
        Jkr.GetDefaultResource(nil, "Compute"),
        ShouldLoadCaches_b
    )
    Resource:Add(
        inInstance,
        Jkr.FillType.Image,
        Jkr.PipelineProperties.Default,
        "cache2/ShapeImageCache.glsl",
        Jkr.GetDefaultResource("ShapeImage", "Vertex"),
        Jkr.GetDefaultResource("ShapeImage", "Fragment"),
        Jkr.GetDefaultResource(nil, "Compute"),
        ShouldLoadCaches_b
    )
    Resource:Add(
        inInstance,
        Jkr.FillType.ContinousLine,
        Jkr.PipelineProperties.Line,
        "cache2/ShapeFillCache.glsl",
        Jkr.GetDefaultResource("ShapeFill", "Vertex"),
        Jkr.GetDefaultResource("ShapeFill", "Fragment"),
        Jkr.GetDefaultResource(nil, "Compute"),
        ShouldLoadCaches_b
    )
    return Resource
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

    o.AddImage = function(self, inWidth, inHeight)
        return sr:AddImage(math.int(inWidth), math.int(inHeight))
    end

    o.AddImageByFileName = function(self, inFileName)
        return sr:AddImage(inFileName)
    end

    o.Remove = function(self, inId)
        recycleBin:Add(inId)
    end

    o.Update = function(self, inId, inGenerator, inPosition_3f)
        sr:Update(inId, inGenerator, inPosition_3f.x, inPosition_3f.y, inPosition_3f.z) -- TODO Improve this
    end
    o.BindShapes = function(self, w)
        sr:BindShapes(w)
    end
    o.BindFillMode = function(self, inFillMode, inWindow, inCmdParam)
        sr:BindFillMode(inFillMode, inWindow, inCmdParam)
    end
    o.BindImage = function(self, inWindow, inImageId, inCmdParam)
        sr:BindImage(inWindow, inImageId, inCmdParam)
    end
    o.Draw = function(self, w, inColor_4f, inStartShapeId, inEndShapeId, inMatrix, inCmdParam)
        sr:Draw(w, inColor_4f, inStartShapeId, inEndShapeId, inMatrix, inCmdParam)
    end
    o.Dispatch = function(self, w, inParam)
        sr:Dispatch(w, inParam)
    end
    o.CopyToImage = function(self, inId, inCustomPainterImage) -- TODO Wrap This
        sr:CopyToImage(inId, inCustomPainterImage)
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
    o.Draw = function(self, inTextImageId, w, inColor, inMatrix, inCmdParam)
        tr:Draw(inTextImageId, w, inColor, inMatrix, inCmdParam)
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
    o.handle = CreateCustomImagePainter(inCacheFileName, inComputeShader)

    o.Load = function(self, i, w)
        o.handle:Load(i, w)
    end

    o.Store = function(self, i, w)
        o.handle:Store(i, w)
    end

    o.Bind = function(self, w, inCmdParam)
        o.handle:Bind(w, inCmdParam)
    end

    o.BindImageFromImage = function(self, w, inCustomImagePainter, inCmdParam)
        o.handle:BindImageFromImage(w, inCustomImagePainter, inCmdParam)
    end

    o.Draw = function(self, w, inPushConstant, inX, inY, inZ, inCmdParam)
        o.handle:Draw(w, inPushConstant, inX, inY, inZ, inCmdParam)
    end

    return o
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

        w:BeginDispatches()
        if (inDispatch) then inDispatch() end
        w:EndDispatches()

        if (inMTDraw) then inMTDraw() end
        -- /* All UI Renders are Recordeed here*/
        w:BeginUIs()
        if (inDraw) then inDraw() end
        w:EndUIs()

        -- /* All ComputeShader Invocations are Done here*/

        -- /* All Draws (Main CmdBuffer Recording) is done here*/
        if inColor_4f then
            w:BeginDraws(inColor_4f.x, inColor_4f.y, inColor_4f.z, inColor_4f.w, 1)
        else
            w:BeginDraws(0.8, 0.8, 0.8, 1, 1)
        end
        if (inMT) then inMT:Wait() end
        if (inExecute) then inExecute() end
        w:ExecuteUIs() -- The UI CmdBuffer is executed onto the main CmdBuffer
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
