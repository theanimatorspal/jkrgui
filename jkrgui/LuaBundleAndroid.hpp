
#pragma once
#include <string_view>
std::string_view LuaBundleScript = R"ijum(
        Engine = {}

-- True : Will compile and store cachesC-- False: Will load caches instead of compiling the shaders
local ShouldLoadCaches_b = false

--[============================================================[
        JKRGUI v2 - ALL RIGHT RESERVED (c)

*userdata - means table in which you cannot insert elements.
    This is similar to class in C++

*factory function - These are the functions that are prefixed
    CreateXXXX, that means you are creating an object.
    *A table is returned by the factory function,

Notes:
1. All the factory functions that is not in the namespace Jkr,
    (is local to this file), will return userdata, that is
    you cannot extend the table. And those which are in the
    Jkr namespace will return a table which can be extended
    with your functionality.


CODING STANDARDS
    -- always  for member functions that are not meant to be used
            use "m" prefix, like mNumber, mComplex
    -- if the argument type is a table make it plural
            like inNumbers, inKeyframes etc


CREATING A FACTORY (CLASS PRODUCER)

Namespace.CreateCLASSNAME = function(inArgument1, inArgument2)
    local o = {}
    o.mArgument1 = inArgument1
    o.Argument2 = inArgument2

    o.AFunction = function()
        -- What the function does
    end

    return o
end

For Jkr Specific,
If Supported for Multithreaded use Jkrmt
If not use Jkr

]============================================================]

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
mat3 = mat3
mat4 = mat4
vec3 = vec3
vec4 = vec4
vec2 = vec2
uvec2 = uvec2
quat = quat

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


Jkr.CreateInstance = function(inEnableValidation, inVarDesSet, inPoolSize, inThreadsCount)
    if not inVarDesSet then inVarDesSet = 1000 end
    if not inPoolSize then inPoolSize = 1000 end
    if not inThreadsCount then inThreadsCount = 7 end
    return Jkr.Instance(inVarDesSet, inPoolSize, inEnableValidation)
end

--[============================================================[
    CREATE JKR WINDOW
]============================================================]

Jkr.CreateWindow = function(inJkrInstance, inTitle, inDimension_2f, inThreadCount, inFrameDimension_2f)
    if not inTitle then inTitle = "JkrGUIv2" end
    if not inDimension_2f then inDimension_2f = uvec2(900, 700) end
    if not inThreadCount then inThreadCount = 4 end
    if not inFrameDimension_2f then inFrameDimension_2f = uvec2(1920, 1080) end
    return Jkr.Window(inJkrInstance, inTitle, math.int(inDimension_2f.x), math.int(inDimension_2f.y),
        math.int(inFrameDimension_2f.x), math.int(inFrameDimension_2f.y), inThreadCount)
end

Jkr.CreateWindowNoWindow = function(inJkrInstance, inFrameDimension_2f, inThreadCount)
    if not inThreadCount then inThreadCount = 4 end
    if not inFrameDimension_2f then inFrameDimension_2f = uvec2(1920, 1080) end
    return Jkr.Window(inJkrInstance, math.int(inFrameDimension_2f.x), math.int(inFrameDimension_2f.y), inThreadCount)
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
    o.sr = CreateShapeRenderer(inInstance, inCompatibleWindow, inShapeRendererResouce)
    o.handle = o.sr

    o.recycleBin = Jkr.RecycleBin()
    o.Add = function(self, inGenerator, inPosition_3f)
        if not self.recycleBin:IsEmpty() then
            local i = self.recycleBin:Get()
            self.sr:Update(inGenerator, inPosition_3f)
            return i
        else
            return self.sr:Add(inGenerator, inPosition_3f)
        end
    end

    o.AddImage = function(self, inWidth, inHeight)
        return self.sr:AddImage(math.int(inWidth), math.int(inHeight))
    end

    o.AddImageByFileName = function(self, inFileName)
        return o.sr:AddImage(inFileName)
    end

    o.Remove = function(self, inId)
        self.recycleBin:Add(inId)
    end

    o.Update = function(self, inId, inGenerator, inPosition_3f)
        self.sr:Update(inId, inGenerator, inPosition_3f.x, inPosition_3f.y, inPosition_3f.z) -- TODO Improve this
    end
    o.BindShapes = function(self, w, inCmdParam)
        self.sr:BindShapes(w, inCmdParam)
    end
    o.BindFillMode = function(self, inFillMode, inWindow, inCmdParam)
        self.sr:BindFillMode(inFillMode, inWindow, inCmdParam)
    end
    o.BindImage = function(self, inWindow, inImageId, inCmdParam)
        self.sr:BindImage(inWindow, inImageId, inCmdParam)
    end
    o.Draw = function(self, w, inColor_4f, inStartShapeId, inEndShapeId, inMatrix, inCmdParam)
        self.sr:Draw(w, inColor_4f, inStartShapeId, inEndShapeId, inMatrix, inCmdParam)
    end
    o.Dispatch = function(self, w, inParam)
        self.sr:Dispatch(w, inParam)
    end
    o.CopyToImage = function(self, inId, inCustomPainterImage) -- TODO Wrap This
        self.sr:CopyToImage(inId, inCustomPainterImage)
    end
    o.CopyFromImage = function(self, inId, inCustomImagePainter)
        self.sr:CopyFromImage(inId, inCustomImagePainter)
    end
    o.GetImageSize = function(self, inFileName)
        return self.sr:GetImageSize(inFileName)
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
    o.bt = Jkr.BestText_base()
    o.tr = CreateTextRendererBestTextAlt(inInstance, inShapeRenderer.handle, o.bt)

    o.AddFontFace = function(self, inFontFileName, inSize)
        return self.bt:AddFontFace(inFontFileName, inSize)
    end
    o.Add = function(self, inFontId, inPosition_3f, inText)
        return self.tr:Add(inFontId, inPosition_3f, inText)
    end
    o.Update = function(self, inTextImageId, inFontId, inPosition_3f, inText)
        self.tr:Update(inTextImageId, inFontId, inPosition_3f, inText)
    end
    o.UpdatePosOnly = function(self, inTextImageId, inFontId, inPosition_3f, inText)
        self.tr:UpdatePosOnly(inTextImageId, inFontId, inPosition_3f, inText)
    end
    o.Draw = function(self, inTextImageId, w, inColor, inMatrix, inCmdParam)
        self.tr:Draw(inTextImageId, w, inColor, inMatrix, inCmdParam)
    end
    o.GetTextDimensions = function(self, inText, inFontShapeId)
        return self.bt:GetTextDimensions(inText, inFontShapeId)
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

    -- @warning this inComputeImage.handle breaks dependency principle,
    --- please fix this on refactor, since this doesn't know anything about
    --- underlying abstracted type, "inComputeImage" from Basics.lua,
    ---  this should only accept the type that this actually knows
    o.BindImageFromImage = function(self, w, inComputeImage, inCmdParam)
        o.handle:BindImageFromImage(w, inComputeImage.handle, inCmdParam)
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

local index = 1
function mark(inin)
    if not inin then inin = "" end
    print("==" .. inin .. "=++" .. index .. "++===")
    index = index + 1
end

function Copy(inElement)
    if type(inElement) == "table" then
        local t = {}
        for key, value in pairs(inElement) do
            t[key] = Copy(value) -- copy all
        end
        return t
    elseif type(inElement) == "string" then
        return string.sub(inElement, 1, #inElement)
    else
        return inElement
    end
end

function IterateEachElementRecursively(inElement, inFunc_val)
    if type(inElement) == "table" then
        for key, value in pairs(inElement) do
            inFunc_val(value)
            IterateEachElementRecursively(value, inFunc_val)
        end
    end
end


-- @warning this eats up a lot of memory, @todo keep the locals, locally in this file
-- but before that, ensure that the multithreaded shader compilation works fine
Engine.Shader = function()
    local o                                       = {}

    ---
    ---
    ---
    --- FOR IMAGE PAINTER COMPUTE SHADER
    ---
    ---
    ---
    local ImagePainterPush                        = [[
layout(std430, push_constant) uniform pc {
        vec4 mPosDimen;
        vec4 mColor;
        vec4 mParam;
} push;
    ]]

    local ImagePainterPushMatrix2                 = [[
layout(std430, push_constant) uniform pc {
    mat4 a;
    mat4 b;
} push;
       ]]

    local ImagePainterAssistMatrix2               = [[
uvec3 gID = gl_GlobalInvocationID;
ivec2 image_size = ivec2(imageSize(storageImage));
ivec2 to_draw_at = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);
float x_cart = (float(gl_GlobalInvocationID.x) - float(image_size.x) / float(2)) / (float((image_size.x) / float(2)));
float y_cart = (float(image_size.y) / float(2) - float(gl_GlobalInvocationID.y)) / (float(image_size.y) / float(2));
vec2 xy = vec2(x_cart, y_cart);
ivec2 xy_is = ivec2(to_draw_at.x + image_size.x / 2, to_draw_at.y + image_size.y / 2);
vec4 p1 = push.a[0];
vec4 p2 = push.a[1];
vec4 p3 = push.a[2];
vec4 p4 = push.a[3];
       ]]

    local ImagePainterAssist                      = [[
uvec3 gID = gl_GlobalInvocationID;
ivec2 image_size = ivec2(imageSize(storageImage));
ivec2 to_draw_at = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

float x_cart = (float(gl_GlobalInvocationID.x) - float(image_size.x) / float(2)) / (float((image_size.x) / float(2)));
float y_cart = (float(image_size.y) / float(2) - float(gl_GlobalInvocationID.y)) / (float(image_size.y) / float(2));

float ColorValueX = x_cart;
float ColorValueY = y_cart;
vec2 xy = vec2(x_cart, y_cart);
ivec2 xy_is = ivec2(to_draw_at.x + image_size.x / 2, to_draw_at.y + image_size.y / 2);
vec4 p1 = push.mPosDimen;
vec4 p2 = push.mColor;
vec4 p3 = push.mParam;
      ]]

    o.uImage2D                                    = function(inBinding, inImageName)
        if not inBinding then inBinding = 0 end
        if not inImageName then inImageName = "storageImage" end
        o.NewLine()
        o.Append(string.format([[
layout(set = 0, binding = %d, rgba8) uniform image2D %s;
    ]], inBinding, inImageName))
        return o.NewLine()
    end

    o.CInvocationLayout                           = function(inX, inY, inZ)
        o.NewLine()
        o.Append(
            string.format("layout(local_size_x = %d, local_size_y = %d, local_size_z = %d) in;", inX,
                inY, inZ)
        )
        return o.NewLine()
    end

    o.ImagePainterPush                            = function()
        o.NewLine()
        o.Append(ImagePainterPush)
        return o.NewLine()
    end

    o.ImagePainterPushMatrix2                     = function()
        o.NewLine()
        o.Append(ImagePainterPushMatrix2)
        return o
    end

    o.ImagePainterAssistMatrix2                   = function()
        o.NewLine()
        o.Append(ImagePainterAssistMatrix2)
        return o;
    end

    o.ImagePainterAssist                          = function()
        o.NewLine()
        o.Append(ImagePainterAssist)
        return o.NewLine()
    end

    o.ImagePainterAssistConvolution               = function(inImageNameFrom, inImageNameTo)
        o.NewLine()
        o.Append(string.format(
            [[

            ]],
            inImageNameFrom, inImageNameTo
        ))
        o.NewLine()
        o.Append [[

        ]]
        return o.NewLine()
    end

    ---
    ---
    --- CUSTOM IMAGE PAINTER FOR 3D
    ---
    --- binding = 14 for storageVertex, and 15 for storageIndex

    local storageVIBufferLayoutCustomImagePainter = [[
        struct Vertex {
            vec3 mPosition;
            vec3 mNormal;
            vec2 mUV;
            vec3 mColor;

        };

        layout(std430, set = 0, binding = 14) buffer VerticesSSBOIn {
            Vertex inVertices[];
        };

        struct Index {
            uint mId;
        };

        layout(std430, set = 0, binding = 15) buffer IndicesSSBOIn {
            Index inIndices[];
        };
    ]]

    o.vertexStorageBufferIndex                    = 14
    o.indexStorageBufferIndex                     = 15

    o.ImagePainterVIStorageLayout                 = function()
        o.NewLine()
        o.Append(storageVIBufferLayoutCustomImagePainter)
        return o.NewLine()
    end

    ---
    ---
    --- MOSTLY FOR 3D
    ---
    ---
    local vLayout                                 = [[

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
                    ]]

    local push                                    = [[

layout(push_constant, std430) uniform pc {
	mat4 model;
    mat4 m2;
} Push;
]]
    local Ubo                                     = [[

layout(set = 0, binding = 0) uniform UBO {
   mat4 view;
   mat4 proj;
   vec3 campos;
   vec4 nearFar;
   vec4 lights[8];
   mat4 shadowMatrix;
   vec4 lightDirections[8];
} Ubo;
]]

    local LinearizeDepth                          = [[

float LinearizeDepth(float depth, float near, float far)
{
  float n = near; // TODO Add an entry on uniform
  float f = far;
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));	
}
          ]]

    local ShadowTextureProject                    = [[

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

    local inJointInfluence                        = [[

struct JointInfluence {
    vec4 mJointIndices;
    vec4 mJointWeights;
};

layout(std140, set = 1, binding = 2) readonly buffer JointInfluenceSSBOIn {
   JointInfluence inJointInfluence[ ];
};

]]

    local inTangent                               = [[
        struct Tangent {
            vec4 mTangent;
        };

        layout(std140, set = 1, binding = 14) readonly buffer TangentSSBOIn {
            Tangent inTangent[];
        };
    ]]

    local inJointMatrices                         = [[

layout(std140, set = 1, binding = 1) readonly buffer JointMatrixSSBOIn {
    mat4 inJointMatrices[ ];
};
          ]]
    local BiasMatrix                              = [[

    const mat4 BiasMatrix = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0 );
    ]]

    local D_GGX                                   = [[
// Normal Distribution Function
float D_GGX(float dotNH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2)/ (PI * denom * denom);
}

    ]]

    local G_SchlicksmithGGX                       = [[
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

    local F_Schlick                               = [[
// Fresnel Function
vec3 F_Schlick(float cosTheta, float metallic)
{
    vec3 F0 = mix(vec3(0.04), MaterialColor(), metallic); // material.specular
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    return F;
}

vec3 F_Schlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

    ]]

    local F_SchlickR                              = [[
vec3 F_SchlickR(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
    ]]

    local PrefilteredReflection                   = [[
vec3 PrefilteredReflection(vec3 R, float roughness)
{
	const float MAX_REFLECTION_LOD = 9.0; // todo: param/const
	float lod = roughness * MAX_REFLECTION_LOD;
	float lodf = floor(lod);
	float lodc = ceil(lod);
	vec3 a = textureLod(prefilteredMap, R, lodf).rgb;
	vec3 b = textureLod(prefilteredMap, R, lodc).rgb;
	return mix(a, b, lod - lodf);
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

    local BRDF_LUT             = [[
vec2 BRDF(float NoV, float roughness)
{
	// Normal always points along z-axis for the 2D lookup
	const vec3 N = vec3(0.0, 0.0, 1.0);
	vec3 V = vec3(sqrt(1.0 - NoV*NoV), 0.0, NoV);

	vec2 LUT = vec2(0.0);
	for(uint i = 0u; i < NUM_SAMPLES; i++) {
		vec2 Xi = Hammersley2d(i, NUM_SAMPLES);
		vec3 H = ImportanceSample_GGX(Xi, roughness, N);
		vec3 L = 2.0 * dot(V, H) * H - V;

		float dotNL = max(dot(N, L), 0.0);
		float dotNV = max(dot(N, V), 0.0);
		float dotVH = max(dot(V, H), 0.0);
		float dotNH = max(dot(H, N), 0.0);

		if (dotNL > 0.0) {
			float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
			float G_Vis = (G * dotVH) / (dotNH * dotNV);
			float Fc = pow(1.0 - dotVH, 5.0);
			LUT += vec2((1.0 - Fc) * G_Vis, Fc * G_Vis);
		}
	}
	return LUT / float(NUM_SAMPLES);
}

    ]]

    local Uncharted2Tonemap    =
    [[
        // From http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 Uncharted2Tonemap(vec3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}
    ]]

    local Random               = [[
// Based omn http://byteblacksmith.com/improvements-to-the-canonical-one-liner-glsl-rand-for-opengl-es-2-0/
float Random(vec2 co)
{
	float a = 12.9898;
	float b = 78.233;
	float c = 43758.5453;
	float dt= dot(co.xy ,vec2(a,b));
	float sn= mod(dt,3.14);
	return fract(sin(sn) * c);
}

       ]]

    local Hammerslay2d         = [[
vec2 Hammersley2d(uint i, uint N)
{
	// Radical inverse based on http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
	uint bits = (i << 16u) | (i >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	float rdi = float(bits) * 2.3283064365386963e-10;
	return vec2(float(i) /float(N), rdi);
}
       ]]

    local ImportanceSample_GGX = [[
        // Based on http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_slides.pdf
vec3 ImportanceSample_GGX(vec2 Xi, float roughness, vec3 normal)
{
	// Maps a 2D point to a hemisphere with spread based on roughness
	float alpha = roughness * roughness;
	float phi = 2.0 * PI * Xi.x + Random(normal.xz) * 0.1;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha*alpha - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	vec3 H = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

	// Tangent space
	vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangentX = normalize(cross(up, normal));
	vec3 tangentY = normalize(cross(normal, tangentX));

	// Convert to world Space
	return normalize(tangentX * H.x + tangentY * H.y + normal * H.z);
}
       ]]

    local SRGBtoLINEAR         = [[

vec4 SRGBtoLINEAR(vec4 srgbIn)
{
	#define MANUAL_SRGB 1
	#ifdef MANUAL_SRGB
	#ifdef SRGB_FAST_APPROXIMATION
	vec3 linOut = pow(srgbIn.xyz,vec3(2.2));
	#else //SRGB_FAST_APPROXIMATION
	vec3 bLess = step(vec3(0.04045),srgbIn.xyz);
	vec3 linOut = mix( srgbIn.xyz/vec3(12.92), pow((srgbIn.xyz+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
	#endif //SRGB_FAST_APPROXIMATION
	return vec4(linOut,srgbIn.w);;
	#else //MANUAL_SRGB
	return srgbIn;
	#endif //MANUAL_SRGB
}

vec3 SRGBtoLINEAR(vec3 srgbIn)
{
	#define MANUAL_SRGB 1
	#ifdef MANUAL_SRGB
	#ifdef SRGB_FAST_APPROXIMATION
	vec3 linOut = pow(srgbIn.xyz,vec3(2.2));
	#else //SRGB_FAST_APPROXIMATION
	vec3 bLess = step(vec3(0.04045),srgbIn.xyz);
	vec3 linOut = mix( srgbIn.xyz/vec3(12.92), pow((srgbIn.xyz+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
	#endif //SRGB_FAST_APPROXIMATION
	return linOut;
	#else //MANUAL_SRGB
	return srgbIn;
	#endif //MANUAL_SRGB
}
]]



    local PrefilterEnvMap      = [[

vec3 PrefilterEnvMap(vec3 R, float roughness)
{
	vec3 N = R;
	vec3 V = R;
	vec3 color = vec3(0.0);
	float totalWeight = 0.0;
	float envMapDim = float(textureSize(samplerEnv, 0).s);
	for(uint i = 0u; i < consts.numSamples; i++) {
		vec2 Xi = Hammersley2d(i, consts.numSamples);
		vec3 H = ImportanceSample_GGX(Xi, roughness, N);
		vec3 L = 2.0 * dot(V, H) * H - V;
		float dotNL = clamp(dot(N, L), 0.0, 1.0);
		if(dotNL > 0.0) {
			// Filtering based on https://placeholderart.wordpress.com/2015/07/28/implementation-notes-runtime-environment-map-filtering-for-image-based-lighting/

			float dotNH = clamp(dot(N, H), 0.0, 1.0);
			float dotVH = clamp(dot(V, H), 0.0, 1.0);

			// Probability Distribution Function
			float pdf = D_GGX(dotNH, roughness) * dotNH / (4.0 * dotVH) + 0.0001;
			// Slid angle of current smple
			float omegaS = 1.0 / (float(consts.numSamples) * pdf);
			// Solid angle of 1 pixel across all cube faces
			float omegaP = 4.0 * PI / (6.0 * envMapDim * envMapDim);
			// Biased (+1.0) mip level for better result
			float mipLevel = roughness == 0.0 ? 0.0 : max(0.5 * log2(omegaS / omegaP) + 1.0, 0.0f);
			color += textureLod(samplerEnv, L, mipLevel).rgb * dotNL;
			totalWeight += dotNL;

		}
	}
	return (color / totalWeight);
}

]]

    local SpecularContribution = [[
vec3 SpecularContribution(vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	// Precalculate vectors and dot products	
	vec3 H = normalize (V + L);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);

	// Light color fixed
	vec3 lightColor = vec3(1.0);

	vec3 color = vec3(0.0);

	if (dotNL > 0.0) {
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness);
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_Schlick(dotNV, F0);		
		vec3 spec = D * F * G / (4.0 * dotNL * dotNV + 0.001);		
		vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);			
		color += (kD * ALBEDO / PI + spec) * dotNL;
	}

	return color;
}
]]

    o.str                      = ""

    o.Append                   = function(inStr)
        o.str = o.str .. inStr
        return o.NewLine()
    end

    o.DontAppend               = function(inStr)
        return o -- do nothing
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
        o.NewLine()
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

    o.uSampler2D               = function(inBinding, inName, inSet)
        if inSet then
            o.str = o.str ..
                string.format("layout(set = %d, binding = %d) uniform sampler2D %s;", inSet, inBinding, inName)
        else
            o.str = o.str ..
                string.format("layout(set = 1, binding = %d) uniform sampler2D %s;", inBinding, inName)
        end
        return o.NewLine()
    end

    o.uSamplerCubeMap          = function(inBinding, inName, inSet)
        if (inSet) then
            o.str = o.str ..
                string.format("layout(set = %d, binding = %d) uniform samplerCube %s;", inSet, inBinding, inName)
        else
            o.str = o.str ..
                string.format("layout(set = 1, binding = %d) uniform samplerCube %s;", inBinding, inName)
        end
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

    o.inTangent                = function()
        o.Append(inTangent)
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

    o.F_SchlickR               = function()
        o.NewLine()
        o.Append(F_SchlickR)
        return o.NewLine()
    end

    o.BRDF                     = function()
        o.NewLine()
        o.Append(BRDF)
        return o.NewLine()
    end

    o.Uncharted2Tonemap        = function()
        o.NewLine()
        o.Append(Uncharted2Tonemap)
        return o.NewLine()
    end

    o.SRGBtoLINEAR             = function()
        o.NewLine()
        o.Append(SRGBtoLINEAR)
        return o.NewLine()
    end

    o.Random                   = function()
        o.NewLine()
        o.Append(Random)
        return o.NewLine()
    end

    o.Hammerslay2d             = function()
        o.NewLine()
        o.Append(Hammerslay2d)
        return o.NewLine()
    end

    o.ImportanceSample_GGX     = function()
        o.NewLine()
        o.Append(ImportanceSample_GGX)
        return o.NewLine()
    end

    o.PrefilterEnvMap          = function()
        o.NewLine()
        o.Append(PrefilterEnvMap)
        return o.NewLine()
    end

    o.PrefilteredReflection    = function()
        o.NewLine()
        o.Append(PrefilteredReflection)
        return o.NewLine()
    end

    o.SpecularContribution     = function()
        o.NewLine()
        o.Append(SpecularContribution)
        return o.NewLine()
    end

    o.BRDF_LUT                 = function()
        o.NewLine()
        o.Append(BRDF_LUT)
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
        local lineNumber = 13

        for line in o.str:gmatch("[^\n]+") do
            print(lineNumber .. ": " .. line)
            lineNumber = lineNumber + 1
        end
        return o
    end
    o.gltfMaterialTextures     = {}
    o.gltfPutMaterialTextures  = function(inGLTF, inMaterialIndex)
        if inGLTF then
            local Materials = inGLTF:GetMaterials()
            local Material = Materials[inMaterialIndex + 1]
            if Material then
                local binding = 3
                if (Material.mBaseColorTextureIndex ~= -1) then
                    o.uSampler2D(binding, "uBaseColorTexture").NewLine()
                    o.gltfMaterialTextures.mBaseColorTexture = true
                    binding = binding + 1
                end
                if (Material.mMetallicRoughnessTextureIndex ~= -1) then
                    o.uSampler2D(binding, "uMetallicRoughnessTexture").NewLine()
                    o.gltfMaterialTextures.mMetallicRoughnessTexture = true
                    binding = binding + 1
                end
                if (Material.mNormalTextureIndex ~= -1) then
                    o.uSampler2D(binding, "uNormalTexture").NewLine()
                    o.gltfMaterialTextures.mNormalTexture = true
                    binding = binding + 1
                end
                if (Material.mOcclusionTextureIndex ~= -1) then
                    o.uSampler2D(binding, "uOcclusionTexture").NewLine()
                    o.gltfMaterialTextures.mOcclusionTexture = true
                    binding = binding + 1
                end
                if (Material.mEmissiveTextureIndex ~= -1) then
                    o.uSampler2D(binding, "uEmissiveTexture").NewLine()
                    o.gltfMaterialTextures.mEmissiveTextureIndex = true
                    binding = binding + 1
                end
            end
        end
        return o
    end

    return o
end

PBR = {}

PBR.Skybox3dV = Engine.Shader()
    .Header(450)
    .NewLine()
    .VLayout()
    .Out(0, "vec3", "vVertUV")
    .Push()
    .Ubo()
    .GlslMainBegin()
    .Indent()
    .Append([[
                vec3 position = mat3(Ubo.view * Push.model) * inPosition.xyz;
                gl_Position = (Ubo.proj * vec4(position, 0.0)).xyzz;
                vVertUV = inPosition.xyz;
        ]]).InvertY()
    .NewLine()
    .GlslMainEnd()
    .NewLine()

PBR.Skybox3dF = Engine.Shader()
    .Header(450)
    .NewLine()
    .In(0, "vec3", "vVertUV")
    .outFragColor()
    .uSamplerCubeMap(20, "samplerCubeMap")
    .Push()
    .Ubo()
    .Uncharted2Tonemap()
    .GlslMainBegin()
    .Indent()
    .Append([[
    vec3 Color = texture(samplerCubeMap, vVertUV).rgb;
    // Tone mapping
    // 0.5 = exposure
	Color = Uncharted2Tonemap(Color * 0.5);
	Color = Color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
          // 2.2 = gamma
	Color = pow(Color, vec3(1.0f / 0.3));
	outFragColor = vec4(Color, 1.0);
        ]])
    .GlslMainEnd()
    .NewLine()


PBR.IBLV = Engine.Shader()
    .Header(450)
    .NewLine()
    .VLayout()
    .Out(0, "vec2", "vUV")
    .Out(1, "vec3", "vNormal")
    .Out(2, "vec3", "vWorldPos")
    .Ubo()
    .Push()
    .GlslMainBegin()
    .Append [[
    vec3 locPos = vec3(Push.model * vec4(inPosition, 1.0));
	vWorldPos = locPos;// + pushConsts.objPos;
	vNormal = mat3(Push.model) * inNormal;
	vUV = inUV;
	vUV.t = 1.0 - inUV.t;
	gl_Position =  Ubo.proj * Ubo.view * vec4(vWorldPos, 1.0);
    ]]
    .GlslMainEnd()


PBR.IBLF = Engine.Shader()
    .Header(450)
    .NewLine()
    .In(0, "vec2", "vUV")
    .In(1, "vec3", "vNormal")
    .In(2, "vec3", "vWorldPos")
    .Ubo()
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
    .uSamplerCubeMap(20, "samplerIrradiance")
    .uSampler2D(3, "samplerBRDFLUT")
    .uSamplerCubeMap(21, "prefilteredMap")
    .outFragColor()
    .PI()
    .Append "#define ALBEDO vec3(material.r, material.g, material.b)"
    .NewLine()
    .Append "vec3 MaterialColor() {return vec3(material.r, material.g, material.b);}"
    .NewLine()
    .Uncharted2Tonemap()
    .D_GGX()
    .G_SchlicksmithGGX()
    .F_SchlickR()
    .F_Schlick()
    .PrefilteredReflection()
    .SpecularContribution()
    .GlslMainBegin()
    .Append [[

    material.roughness = Push.m2[0].x;
    material.metallic = Push.m2[0].y;
    material.specular = Push.m2[0].z;
    material.r = Push.m2[1].x;
    material.g = Push.m2[1].y;
    material.b = Push.m2[1].z;

    vec3 N = normalize(vNormal);
    vec3 V = normalize(Ubo.campos - vWorldPos);
    vec3 R = reflect(-V, N);

    float metallic = material.metallic;
    float roughness = material.roughness;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, ALBEDO, metallic);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < Ubo.lights.length(); i++) {
    	vec3 L = normalize(Ubo.lights[i].xyz - vWorldPos);
    	Lo += SpecularContribution(L, V, N, F0, metallic, roughness) * Ubo.lights[i].w;
    }

    vec2 brdf = texture(samplerBRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 reflection = PrefilteredReflection(R, roughness).rgb;	
    vec3 irradiance = texture(samplerIrradiance, N).rgb;

    // Diffuse based on irradiance
    vec3 diffuse = irradiance * ALBEDO;	

    vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

    // Specular reflectance
    vec3 specular = reflection * (F * brdf.x + brdf.y);

    // Ambient part
    vec3 kD = 1.0 - F;
    kD *= 1.0 - metallic;	
    vec3 ambient = (kD * diffuse + specular);

    vec3 color = ambient + Lo;

    // Tone mapping -- exposure = 0.5
    color = Uncharted2Tonemap(color * 0.5);
    color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	

    // Gamma correction gamma = 3.3
    color = pow(color, vec3(1.0f / 3.3));

    //GL_EXT_debug_printf("color: (%f, %f, %f)", color.x, color.y, color.z);
    outFragColor = vec4(color, 1.0);
    ]]
    .GlslMainEnd()


PBR.IBLF_texture = Engine.Shader()
    .Header(450)
    .NewLine()
    .In(0, "vec2", "vUV")
    .In(1, "vec3", "vNormal")
    .In(2, "vec3", "vWorldPos")
    .Ubo()
    .Push()
    .outFragColor()
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
    .uSamplerCubeMap(20, "samplerIrradiance")
    .uSampler2D(3, "samplerBRDFLUT")
    .uSamplerCubeMap(21, "prefilteredMap")
    .uSampler2D(4, "albedoMap")
    .uSampler2D(5, "normalMap")
    .uSampler2D(6, "aoMap")
    .uSampler2D(7, "metallicMap")
    .uSampler2D(8, "roughnessMap")
    .PI()
    .Append "#define ALBEDO vec3(material.r, material.g, material.b)"
    .NewLine()
    .Append "vec3 MaterialColor() {return vec3(material.r, material.g, material.b);}"
    .NewLine()
    .Uncharted2Tonemap()
    .D_GGX()
    .G_SchlicksmithGGX()
    .F_Schlick()
    .F_SchlickR()
    .PrefilteredReflection()
    .SpecularContribution()
    .Append [[

vec3 calculateNormal()
{
    vec3 tangentNormal = texture(normalMap, inUV).xyz * 2.0 - 1.0;

    vec3 N = normalize(inNormal);
    vec3 T = normalize(inTangent.xyz);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

    ]]
    .GlslMainBegin()
    .Append [[
    vec3 N = calculateNormal();
    vec3 V = normalize(ubo.campos - vWorldPos);
	vec3 R = reflect(-V, N);

	float metallic = texture(metallicMap, vUV).r;
	float roughness = texture(roughnessMap, vUV).r;

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, ALBEDO, metallic);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < uboParams.lights[i].length(); i++) {
		vec3 L = normalize(uboParams.lights[i].xyz - inWorldPos);
		Lo += specularContribution(L, V, N, F0, metallic, roughness);
	}
	
	vec2 brdf = texture(samplerBRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 reflection = prefilteredReflection(R, roughness).rgb;	
	vec3 irradiance = texture(samplerIrradiance, N).rgb;

	// Diffuse based on irradiance
	vec3 diffuse = irradiance * ALBEDO;	

	vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

	// Specular reflectance
	vec3 specular = reflection * (F * brdf.x + brdf.y);

	// Ambient part
	vec3 kD = 1.0 - F;
	kD *= 1.0 - metallic;	
	vec3 ambient = (kD * diffuse + specular) * texture(aoMap, vUV).rrr;
	
	vec3 color = ambient + Lo;

	// Tone mapping exposure = 1.5
	color = Uncharted2Tonemap(color * 1.5);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction gamma = 0.3
	color = pow(color, vec3(1.0f / 0.3));

	outFragColor = vec4(color, 1.0);
    ]]
    .GlslMainEnd()




PBR.GenBrdfLutV = Engine.Shader()
    .Header(450)
    .Out(0, "vec2", "vUV")
    .GlslMainBegin()
    .Append [[
    	vUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
        gl_Position = vec4(vUV * 2.0f - 1.0f, 0.0f, 1.0f);
    ]].InvertY()
    .GlslMainEnd()


PBR.GenBrdfLutF = Engine.Shader()
    .Header(450)
    .outFragColor()
    .In(0, "vec2", "vUV")
    .Append "const uint NUM_SAMPLES = 1024u;"
    .NewLine()
    .PI()
    .Random()
    .Hammerslay2d()
    .ImportanceSample_GGX()
    .G_SchlicksmithGGX()
    .BRDF_LUT()
    .GlslMainBegin()
    .Append [[
        outFragColor = vec4(BRDF(vUV.s, vUV.t), 0.0, 1.0);
    ]]
    .GlslMainEnd()


PBR.FilterCubeV = Engine.Shader()
    .Header(450)
    .VLayout()
    .Ubo()
    .Push()
    .Out(0, "vec3", "vUVW")
    .GlslMainBegin()
    .Append [[
    vUVW = inPosition;
    gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1.0);
    ]]
    .GlslMainEnd()

PBR.PreFilterEnvMapF = Engine.Shader()
    .Header(450)
    .PI()
    .In(0, "vec3", "inPos")
    .Push()
    .Append [[

    struct ConstantStruct {
          float roughness;
          uint numSamples;
    } consts;

    ]]
    .outFragColor()
    .uSamplerCubeMap(20, "samplerEnv")
    .Random()
    .Hammerslay2d()
    .ImportanceSample_GGX()
    .D_GGX()
    .PrefilterEnvMap()
    .GlslMainBegin()
    .Append [[

    consts.roughness = Push.m2[0].x;
    consts.numSamples = uint(Push.m2[0].y);
    vec3 N = normalize(inPos);
    vec3 color = PrefilterEnvMap(N, consts.roughness);
    debugPrintfEXT("PrefilterCubeMap:: Roughness: %f, Samples: %d, color: (%f, %f, %f)\n", consts.roughness, consts.numSamples, color.x, color.y, color.z);
    outFragColor = vec4(color, 1.0);

    ]]
    .GlslMainEnd()


PBR.IrradianceCubeF = Engine.Shader()
    .Header(450)
    .outFragColor()
    .Push()
    .In(0, "vec3", "vUVW")
    .Append [[
    struct {
          float deltaPhi;
          float deltaTheta;
    } consts;
    ]]
    .uSamplerCubeMap(20, "samplerEnv")
    .PI()
    .GlslMainBegin()
    .Append [[
    consts.deltaPhi = Push.m2[0].x;
    consts.deltaTheta = Push.m2[0].y;

	vec3 N = normalize(vUVW);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up = cross(N, right);

	const float TWO_PI = PI * 2.0;
	const float HALF_PI = PI * 0.5;

	uint sampleCount = 0u;
	vec3 color = vec3(0.0);

	for (float phi = 0.0; phi < TWO_PI; phi += consts.deltaPhi) {
		for (float theta = 0.0; theta < HALF_PI; theta += consts.deltaTheta) {
			vec3 tempVec = cos(phi) * right + sin(phi) * up;
			vec3 sampleVector = cos(theta) * N + sin(theta) * tempVec;
			color += texture(samplerEnv, sampleVector).rgb * cos(theta) * sin(theta);
			sampleCount++;
		}
	}
    color  = PI * color / float(sampleCount);
    debugPrintfEXT("deltaPhi = %f, deltaTheta = %f, color: (%f, %f, %f)\n", consts.deltaPhi, consts.deltaTheta, color.x, color.y, color.z);
	outFragColor = vec4(color, 1.0);
    ]]
    .GlslMainEnd()


PBR.BasicCompute = Engine.Shader()
    .Header(450)
    .GlslMainBegin()
    .GlslMainEnd()

PBR.EquirectangularMapToMultiVShader = Engine.Shader()
    .Header(450)
    .VLayout()
    .Push()
    .Out(0, "vec3", "localPos")
    .GlslMainBegin()
    .Append [[
    localPos = inPosition;
    gl_Position = Push.model * Push.m2 * vec4(localPos, 1.0);
    ]]
    .GlslMainEnd()

PBR.EquirectangularMapToMultiFShader = Engine.Shader()
    .Header(450)
    .outFragColor()
    .In(0, "vec3", "localPos")
    .uSampler2D(0, "equirectangularMap", 0)
    .Append [[
    const vec2 invATan = vec2(0.1591, 0.3183);
    vec2 SampleSphericalMap(vec3 v)
    {
        vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
        uv *= invATan;
        uv += 0.5;
        return uv;
    }
    ]]
    .GlslMainBegin()
    .Append [[
    vec2 uv = SampleSphericalMap(normalize(localPos));
    vec3 color = texture(equirectangularMap, uv).rgb;
    outFragColor = vec4(color, 1.0);
    ]]
    .GlslMainEnd()



Deferred = {}

Deferred.ScreenQuadCompositionVertex = Engine.Shader()
    .Header(450)
    .Push()
    .Out(0, "vec2", "vUV")
    .GlslMainBegin()
    .Append [[
        vUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
        gl_Position = vec4(vUV * 2.0 - 1.0, 0.0, 1.0);
    ]]
    .GlslMainEnd()

Deferred.ScreenQuadCompositionFragment = Engine.Shader()
    .Header(450)
    .Ubo()
    .In(0, "vec2", "vUV")
    .uSampler2D(3, "inPositionImage", 1)
    .uSampler2D(4, "inNormalImage", 1)
    .uSampler2D(5, "inAlbedoImage", 1)
    .Push()
    .outFragColor()
    .GlslMainBegin()
    .Append [[
        vec3 fragPos = texture(inPositionImage, vUV).rgb;
        vec3 normal = texture(inNormalImage, vUV).rgb;
        vec4 albedo = texture(inAlbedoImage, vUV);
        #define lightCount 8
        #define ambient 0.1
        vec3 fragColor = albedo.rgb * ambient;

        vec3 N = normalize(normal);

        for(int i = 0; i < 8; ++i)
        {
            vec3 L = Ubo.lights[i].xyz - fragPos;
            float dist = length(L);
            vec3 V = Ubo.campos - fragPos;
            V = normalize(V);
            L = normalize(L);
            float attenuation = Ubo.lights[i].w / (dist * dist + 1.0f);

            // diffuse
            float NdotL = max(0.0f, dot(N, L));
            vec3 lightColor = vec3(1, 1, 1);
            vec3 diff = lightColor * albedo.rgb * NdotL * attenuation;
            // specular
            vec3 R = reflect(-L, N);
            float NdotR = max(0.0f, dot(R, V));
            vec3 spec = lightColor * albedo.a * pow(NdotR, 16.0f) * attenuation;
            fragColor += diff + spec;
        }
        outFragColor = vec4(fragColor, albedo.a);
    ]]
    .GlslMainEnd()

---@diagnostic disable-next-line: duplicate-set-field
function Deferred.GetBasicVertexHeader()
    return Engine.Shader()
        .Header(450)
        .VLayout()
        .Ubo()
        .Push()
        .Out(0, "vec3", "vNormal")
        .Out(1, "vec2", "vUV")
        .Out(2, "vec3", "vColor")
        .Out(3, "vec3", "vTangent")
        .Out(4, "vec3", "vWorldPos")
end

---@diagnostic disable-next-line: duplicate-set-field
function Deferred.GetBasicFragmentHeader()
    return Engine.Shader()
        .Header(450)
        .Ubo()
        .Push()
        .In(0, "vec3", "vNormal")
        .In(1, "vec2", "vUV")
        .In(2, "vec3", "vColor")
        .In(3, "vec3", "vTangent")
        .In(4, "vec3", "vWorldPos")
        .Out(0, "vec4", "outPosition")
        .Out(1, "vec4", "outNormal")
        .Out(2, "vec4", "outAlbedo")
end

Deferred.BasicVertex = Deferred.GetBasicVertexHeader()
    .Append [[
        struct Tangent {
            vec4 mTangent;
        };

        layout(std140, set = 1, binding = 14) readonly buffer TangentSSBOIn {
            Tangent inTangent[];
        };
    ]]
    .GlslMainBegin()
    .Append
    [[
        gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1);
        vWorldPos = vec3(Push.model * vec4(inPosition, 1));
        mat3 mNormal = transpose(inverse(mat3(Push.model)));
        vNormal = mNormal * normalize(inNormal.xyz);
        vTangent = mNormal * normalize(inTangent[gl_VertexIndex].mTangent.xyz);
        vUV = inUV;
        vColor = inColor;
    ]]
    .GlslMainEnd()

Deferred.GetBasicVertex = function()
    return Deferred.GetBasicVertexHeader()
        .Append [[
        struct Tangent {
            vec4 mTangent;
        };

        layout(std140, set = 1, binding = 14) readonly buffer TangentSSBOIn {
            Tangent inTangent[];
        };
    ]]
        .GlslMainBegin()
        .Append
        [[
        gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1);
        vWorldPos = vec3(Push.model * vec4(inPosition, 1));
        mat3 mNormal = transpose(inverse(mat3(Push.model)));
        vNormal = mNormal * normalize(inNormal.xyz);
        vec4 IN_Tangent = inTangent[gl_VertexIndex].mTangent;
        vTangent = mNormal * normalize(IN_Tangent.w > 0 ? IN_Tangent.xyz : vec3(-IN_Tangent.x, IN_Tangent.y, IN_Tangent.z));
        vUV = inUV;
        vColor = inColor;
    ]]
        .InvertY()
        .GlslMainEnd()
end

Deferred.BasicFragment = Deferred.GetBasicFragmentHeader()
    .uSampler2D(3, "samplerColor", 1)
    .uSampler2D(4, "samplerNormal", 1)
    .GlslMainBegin()
    .Append
    [[
        outPosition = vec4(vWorldPos, 1.0);
        // Calculate normal in tangent space
        vec3 N = normalize(vNormal);
        vec3 T = normalize(vTangent);
        vec3 B = cross(N, T);
        mat3 TBN = mat3(T, B, N);
        vec3 tnorm = TBN * normalize(texture(samplerNormal, vUV).xyz * 2.0 - vec3(1.0));
        outNormal = vec4(tnorm, 1.0);
        outAlbedo = texture(samplerColor, vUV);
    ]]
    .GlslMainEnd()

Basics = {}

Basics.GetVertexWithTangent = function()
    return Deferred.GetBasicVertexHeader()
        .Append [[
        struct Tangent {
            vec4 mTangent;
        };

        layout(std140, set = 1, binding = 14) readonly buffer TangentSSBOIn {
            Tangent inTangent[];
        };
        ]]
        .GlslMainBegin()
        .Append [[
        gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1);
        vWorldPos = vec3(Push.model * vec4(inPosition, 1));
        mat3 mNormal = transpose(inverse(mat3(Push.model)));
        vNormal = mNormal * normalize(inNormal.xyz);
        vTangent = mNormal * normalize(inTangent[gl_VertexIndex].mTangent.xyz);
        vUV = inUV;
        vColor = inColor;
        ]]
        .GlslMainEnd()
end

Basics.GetConstantFragmentHeader = function()
    return Engine.Shader()
        .Header(450)
        .Ubo()
        .Push()
        .In(0, "vec3", "vNormal")
        .In(1, "vec2", "vUV")
        .In(2, "vec3", "vColor")
        .In(3, "vec3", "vTangent")
        .In(4, "vec3", "vWorldPos")
        .outFragColor()
end

Basics.GetBasicVertexHeaderWithTangent = function()
    return Deferred.GetBasicVertexHeader()
        .Append [[
        struct Tangent {
            vec4 mTangent;
        };

        layout(std140, set = 1, binding = 14) readonly buffer TangentSSBOIn {
            Tangent inTangent[];
        };
        ]]
end

Basics.GetBasicVertexHeaderWithoutTangent = function()
    return Deferred.GetBasicVertexHeader()
end


Engine.GetAppropriateShader = function(inShaderType, incompilecontext, gltfmodel, materialindex, inskinning, intangent)
    if intangent == nil then intangent = true end
    if inShaderType == "CONSTANT_COLOR" and incompilecontext == Jkr.CompileContext.Default then
        local vshader
        if intangent then
            vshader = Basics.GetBasicVertexHeaderWithTangent()
        else
            vshader = Basics.GetBasicVertexHeaderWithoutTangent()
        end
        vshader.GlslMainBegin()

        if not inskinning then
            vshader.Append [[
                    gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1);
                    vWorldPos = vec3(Push.model * vec4(inPosition, 1));
                    mat3 mNormal = transpose(inverse(mat3(Push.model)));
                    vNormal = mNormal * normalize(inNormal.xyz);
                    vUV = inUV;
                    vColor = inColor;
                    ]]
        else
            vshader.inJointInfluence()
                .inJointMatrices()
                .Append([[
                        vec4 jweight = inJointInfluence[gl_VertexIndex].mJointWeights;
                        vec4 jindex = inJointInfluence[gl_VertexIndex].mJointIndices;
                        mat4 skinMat  =
                                jweight.x * inJointMatrices[int(jindex.x)] +
                                jweight.y * inJointMatrices[int(jindex.y)] +
                                jweight.z * inJointMatrices[int(jindex.z)] +
                                jweight.w * inJointMatrices[int(jindex.w)];
                        ]])
                .Append [[
                        vec4 Pos = Push.model * skinMat * vec4(inPosition, 1.0f);
                        gl_Position = Ubo.proj * Ubo.view * Pos;
                        vWorldPos = vec3(Push.model * vec4(inPosition, 1));
                        mat3 mNormal = transpose(inverse(mat3(Push.model)));
                        vNormal = mNormal * normalize(inNormal.xyz);
                        vUV = inUV;
                        vColor = inColor;
                ]]
        end
        if intangent then
            vshader.Append
            [[
                vTangent = mNormal * normalize(inTangent[gl_VertexIndex].mTangent.xyz);
            ]]
        end

        vshader.InvertY()
        vshader.GlslMainEnd()

        local fshader = Basics.GetConstantFragmentHeader()
            .gltfPutMaterialTextures(gltfmodel, materialindex)
            .GlslMainBegin()
        if fshader.gltfMaterialTextures.mBaseColorTexture == true then
            if fshader.gltfMaterialTextures.mEmissiveTextureIndex == true then
                fshader.Append [[
                    outFragColor = texture(uBaseColorTexture, vUV) + texture(uEmissiveTexture, vUV);
                    ]]
            else
                fshader.Append [[
                    outFragColor = texture(uBaseColorTexture, vUV);
                    ]]
            end
        else
            fshader.Append [[
                outFragColor = vec4(1, 0, 0, 1);
            ]]
        end
        fshader.GlslMainEnd()
        return vshader, fshader
    end
    if inShaderType == "NORMAL" and incompilecontext == Jkr.CompileContext.Deferred then
        local vshader = Deferred.GetBasicVertex()
        local fshader = Deferred.GetBasicFragmentHeader()
            .gltfPutMaterialTextures(gltfmodel, materialindex)
            .GlslMainBegin()
            .Append [[
        outPosition = vec4(vWorldPos, 1.0);
        // Calculate normal in tangent space
        vec3 N = normalize(vNormal);
        vec3 T = normalize(vTangent);
        vec3 B = cross(N, T);
        mat3 TBN = mat3(T, B, N);
            ]]

        if fshader.gltfMaterialTextures.mBaseColorTexture == true then
            fshader.Append [[
                    outAlbedo = texture(uBaseColorTexture, vUV);
                    ]]
        else
            fshader.Append [[
                outAlbedo = vec4(1, 0, 0, 1);
            ]]
        end

        if fshader.gltfMaterialTextures.mNormalTexture == true then
            fshader.Append [[
                vec3 tnorm = TBN * normalize(texture(uNormalTexture, vUV).xyz * 2.0 - vec3(1.0));
                outNormal = vec4(tnorm, 1.0);
            ]]
        else
            fshader.Append [[
                outNormal = vec4(0, 0, 0, 0);
            ]]
        end

        if fshader.gltfMaterialTextures.mEmissiveTextureIndex == true then
            fshader.Append [[
                outAlbedo = outAlbedo * texture(uEmissiveTexture, vUV);
            ]]
        end

        fshader.GlslMainEnd()

        return vshader, fshader
    end
end


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
        .uSampler2D(23, "samplerBRDFLUT", 0)
        .uSamplerCubeMap(24, "samplerIrradiance", 0)
        .uSamplerCubeMap(25, "prefilteredMap", 0)
        .Ubo()
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
    Object3D.mMatrix = gltf:GetNodeMatrixByIndex(NodeIndices)

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
        Object.mMatrix = inGLTFHandle:GetNodeMatrixByIndex(NodeIndices)
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

            --@warning THIS IS BEING DUPLICATED, FIX THIS
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
            object.mDrawable = false
            object.mMatrix = Nodes[NodeIndex]:GetLocalMatrix()
            ---@note This is supposed to be used for storage of the abovematrix
            object.mMatrix3 = Nodes[NodeIndex]:GetLocalMatrix()
            object.mP1 = NodeIndex
            object.mP2 = 1
            Objects[#Objects + 1] = object
        end
    end

    -- @warning You've to store this Objects {} table somewhere
    for i = 1, #Objects, 1 do
        for j = 1, #Objects, 1 do
            if i ~= j then
                if gltfmodel:IsNodeParentOf(Nodes[Objects[i].mP1], Nodes[Objects[j].mP1]) then
                    print("PARENT")
                    Objects[i]:SetParent(Objects[j])
                end
            end
        end
    end
    return Objects
end


local lerp = function(a, b, t)
    return (a * (1 - t) + t * b) * (1 - t) + b * t
end

local lerp_3f = function(a, b, t)
    return vec3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t))
end

Jkr.CreateAnimationPosDimen = function(inCallBuffer, inFrom, inTo, inComponent, inInverseSpeed)
    local InverseSpeed = 0.01
    local t = 0
    if inInverseSpeed then
        InverseSpeed = inInverseSpeed
    end

    local Frame = 1
    while t <= 1 do
        local from_pos      = inFrom.mPosition_3f
        local to_pos        = inTo.mPosition_3f
        local from_dimen    = inFrom.mDimension_3f
        local to_dimen      = inTo.mDimension_3f
        local current_pos   = lerp_3f(from_pos, to_pos, t)
        local current_dimen = lerp_3f(from_dimen, to_dimen, t)
        inCallBuffer:PushOneTime(Jkr.CreateUpdatable(
            function()
                inComponent:Update(current_pos, current_dimen)
            end
        ), Frame)
        t = t + InverseSpeed
        Frame = Frame + 1
    end
    return Frame
end

--[============================================================[
    JKR CALL TYPES
]============================================================]

Jkr.CreateUpdatable = function(inFunction)
    local o = {}
    o.mUpdate = inFunction
    return o
end

Jkr.CreateEventable = function(inFunction)
    local o = {}
    o.mEvent = inFunction
    return o
end

Jkr.CreateDrawable = function(inId, inBatchable, inDrawType, inImageId, inColor_4f)
    local o = {}
    o.mBatchable = inBatchable
    o.mImageId = inImageId
    if inColor_4f then
        o.mColor = inColor_4f
    else
        o.mColor = vec4(1, 1, 1, 1)
    end
    o.mId = inId
    o.mImageId = inImageId
    o.mDrawType = inDrawType -- LINE, SHAPE, TEXT
    return o
end

Jkr.CreateDispatchable = function(inFunction)
    local o = {}
    o.mDispatch = inFunction
    return o
end

Jkr.CreateCallBuffers = function() -- Similar to Comptable in JrkGUI v1
    local o = {}
    o.mDrawables = {}
    o.mDispatchables = {}
    o.mUpdatables = {}
    o.mEventables = {}
    o.mOneTimeDrawables = {}
    o.mOneTimeDispatchables = {}
    o.mOneTimeUpdatables = {}

    o.Push = function(self, inCall)
        if inCall.mDrawType then
            self.mDrawables[#self.mDrawables + 1] = inCall
            return #self.mDrawables
        elseif inCall.mUpdate then
            self.mUpdatables[#self.mUpdatables + 1] = inCall
            return #self.mUpdatables
        elseif inCall.mDispatch then
            self.mDispatchables[#self.mDispatchables + 1] = inCall
            return #self.mDispatchables
        elseif inCall.mEvent then
            self.mEventables[#self.mEventables + 1] = inCall
            return #self.mEventables
        end
    end
    o.PushOneTime = function(self, inCall, inFrame)
        if inCall.mDrawType then
            -- Optimize this
            if self.mOneTimeDrawables[inFrame] then
                self.mOneTimeDrawables[inFrame][#self.mOneTimeDrawables[inFrame] + 1] = inCall
            else
                self.mOneTimeDrawables[inFrame] = {}
                self.mOneTimeDrawables[inFrame][#self.mOneTimeDrawables[inFrame] + 1] = inCall
            end
        elseif inCall.mUpdate then
            if self.mOneTimeUpdatables[inFrame] then
                self.mOneTimeUpdatables[inFrame][#self.mOneTimeUpdatables[inFrame] + 1] = inCall
            else
                self.mOneTimeUpdatables[inFrame] = {}
                self.mOneTimeUpdatables[inFrame][#self.mOneTimeUpdatables[inFrame] + 1] = inCall
            end
        elseif inCall.mDispatch then
            if self.mOneTimeDispatchables[inFrame] then
                self.mOneTimeDispatchables[inFrame][#self.mOneTimeDispatchables[inFrame] + 1] = inCall
            else
                self.mOneTimeDispatchables[inFrame] = {}
                self.mOneTimeDispatchables[inFrame][#self.mOneTimeDispatchables[inFrame] + 1] = inCall
            end
        end
    end

    local cdf = 1 -- CurrentDispatchFrame
    o.Dispatch = function(self)
        --[========================================================]
        -- DISPATCH ONE TIMES
        --[========================================================]
        if self.mOneTimeDispatchables[cdf] then
            local Length = #self.mOneTimeDispatchables[cdf]
            for x = 1, Length, 1 do
                self.mOneTimeDispatchables[cdf][x].mDispatch()
                self.mOneTimeDispatchables[cdf][x] = nil
            end
            self.mOneTimeDispatchables[cdf] = nil
            cdf = cdf + 1
            if not self.mOneTimeDispatchables[cdf] then
                cdf = 1
            end
        end

        --[========================================================]
        -- DISPATCH
        --[========================================================]
        for i = 1, #self.mDispatchables, 1 do
            self.mDispatchables[i].mDispatch()
        end
    end

    local cuf = 1 -- CurrentUpdateFrame
    o.Update = function(self)
        --[========================================================]
        -- UPDATE ONE TIMES
        --[========================================================]
        if self.mOneTimeUpdatables[cuf] then
            local Length = #self.mOneTimeUpdatables[cuf]
            for x = 1, Length, 1 do
                self.mOneTimeUpdatables[cuf][x].mUpdate()
                self.mOneTimeUpdatables[cuf][x] = nil
            end
            self.mOneTimeUpdatables[cuf] = nil
            cuf = cuf + 1
            if not self.mOneTimeUpdatables[cuf] then
                cuf = 1
            end
        end

        --[========================================================]
        -- UPDATE
        --[========================================================]
        for i = 1, #self.mUpdatables, 1 do
            self.mUpdatables[i].mUpdate()
        end
    end

    o.Event = function(self)
        --[========================================================]
        -- EVENT
        --[========================================================]
        for i = 1, #self.mEventables, 1 do
            self.mEventables[i].mEvent()
        end
    end

    return o
end

Jkr.CreateCallExecutor = function(inCallBuffer)
    local o = {}
    o.c = inCallBuffer
    return o
end

Jkr.HLayout = {
    mComponents = nil,
    mRatioTable = nil,
    mPadding = nil,
    mPosition_3f = nil,
    mDimension_3f = nil,

    New = function(self, inPadding)
        local Obj = {
            mPadding = inPadding
        }
        if not inPadding then
            Obj.mPadding = 0
        end
        setmetatable(Obj, self)
        self.__index = self

        return Obj
    end,
    Add = function(self, inComponentListTable, inRatioTable)
        self.mComponents = inComponentListTable
        self.mRatioTable = inRatioTable
        return self
    end,
    Update = function(self, inPosition_3f, inDimension_3f)
        local position = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z)
        local dimension = vec3(inDimension_3f.x, inDimension_3f.y, inDimension_3f.z)
        self.mPosition_3f = inPosition_3f
        self.mDimension_3f = inDimension_3f

        local paddingX = self.mPadding

        if self.mRatioTable then
            for index, value in ipairs(self.mComponents) do
                value:Update(vec3(position.x, position.y, position.z),
                    vec3(dimension.x * self.mRatioTable[index],
                        dimension.y, dimension.z),
                    self.mComponents[index].mText)
                position.x = position.x +
                    dimension.x * self.mRatioTable[index] + paddingX
            end
        end
    end,
    GetComponentPosition = function(self)
        local position = vec3(self.mPosition_3f.x, self.mPosition_3f.y, self.mPosition_3f.z)
        local dimension = vec3(self.mDimension_3f.x, self.mDimension_3f.y, self.mDimension_3f.z)
        local ComponentsPosition = {}
        for index, value in ipairs(self.mComponents) do
            ComponentsPosition[index] = vec3(position.x, position.y, position.z)
            position.x = position.x + dimension.x * self.mRatioTable[index] +
                self.mPadding
        end
        return ComponentsPosition
    end
}

Jkr.VLayout = {
    mComponents = nil,
    mRatioTable = nil,
    mPadding = nil,
    mPosition_3f = nil,
    mDimension_3f = nil,

    New = function(self, inPadding)
        local Obj = {
            mPadding = inPadding
        }
        if not inPadding then
            Obj.mPadding = 0
        end
        setmetatable(Obj, self)
        self.__index = self
        self.__call = Jkr.HLayout.New
        return Obj
    end,
    Add = function(self, inComponentListTable, inRatioTable)
        self.mComponents = inComponentListTable
        self.mRatioTable = inRatioTable
        return self
    end,
    Update = function(self, inPosition_3f, inDimension_3f)
        local position = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z)
        local dimension = vec3(inDimension_3f.x, inDimension_3f.y, inDimension_3f.z)
        self.mPosition_3f = vec3(position.x, position.y, position.z)
        self.mDimension_3f = vec3(dimension.x, dimension.y, dimension.z)
        local paddingY = self.mPadding
        if self.mRatioTable then
            for index, value in ipairs(self.mComponents) do
                value:Update(vec3(position.x, position.y, position.z),
                    vec3(dimension.x,
                        dimension.y *
                        self.mRatioTable[index],
                        dimension.z),
                    self.mComponents[index].mText)
                position.y = position.y +
                    dimension.y * self.mRatioTable[index] + paddingY
            end
        end
    end,
    GetComponentPosition = function(self)
        local position = vec3(self.mPosition_3f.x, self.mPosition_3f.y, self.mPosition_3f.z)
        local dimension = vec3(self.mDimension_3f.x, self.mDimension_3f.y, self.mDimension_3f.z)
        local ComponentsPosition = {}
        for index, value in ipairs(self.mComponents) do
            ComponentsPosition[index] = vec3(position.x, position.y, position.z)
            position.y = position.y + dimension.y * self.mRatioTable[index] +
                self.mPadding
        end
        return ComponentsPosition
    end
}

Jkr.StackLayout = {
    mComponents = nil,
    New = function(self, inChangingZvalue)
        local Obj = {
            mChangingZvalue = inChangingZvalue
        }
        setmetatable(Obj, self)
        self.__index = self
        return Obj
    end,
    Add = function(self, inComponentListTable)
        self.mComponents = inComponentListTable
        return self
    end,
    Update = function(self, inPosition_3f, inDimension_3f)
        local position = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z)
        local dimension = vec3(inDimension_3f.x, inDimension_3f.y, inDimension_3f.z)
        self.mPosition_3f = inPosition_3f
        self.mDimension_3f = inDimension_3f
        for index, value in ipairs(self.mComponents) do
            value:Update(vec3(position.x, position.y, position.z),
                vec3(dimension.x, dimension.y, dimension.z),
                self.mComponents[index].mText)
            position.z = position.z - self.mChangingZvalue
        end
    end
}


Jkr.CreateWidgetRenderer = function(i, w, e)
    local o = {}
    o.i = i
    o.w = w
    o.s = Jkr.CreateShapeRenderer(o.i, o.w)
    o.t = Jkr.CreateTextRendererBestTextAlt(o.i, o.s) -- for now the idea has been dropped

    o.c = Jkr.CreateCallBuffers()
    o.e = Jkr.CreateCallExecutor(o.c)
    o.WindowDimension = o.w:GetWindowDimension()

    o.CreateFont = function(inFontFileName, inFontSize)
        local font = {}
        font.mId = o.t:AddFontFace(inFontFileName, inFontSize)
        font.GetTextDimension = function(self, inText)
            return o.t:GetTextDimensions(inText, font.mId)
        end
        return font
    end

    --[============================================================[
                    TEXT LABEL
          ]============================================================]
    -- Here for each widget we have to follow function(inPosition, inDimension) style
    o.CreateTextLabel = function(inPosition_3f, inDimension_3f, inFont, inText, inColor)
        local textLabel = {}
        textLabel.mText = inText
        textLabel.mFont = inFont
        textLabel.mId = o.t:Add(inFont.mId, inPosition_3f, inText)
        textLabel.PushId = o.c:Push(Jkr.CreateDrawable(textLabel.mId, nil, "TEXT", nil, inColor))

        textLabel.Update = function(self, inPosition_3f, inDimension_3f, inFont, inText, inColor)
            if inFont then self.mFont = inFont end
            if inText then self.mText = inText end
            if inText then
                o.t:Update(self.mId, self.mFont.mId, inPosition_3f, self.mText)
            else
                o.t:UpdatePosOnly(self.mId, self.mFont.mId, inPosition_3f, self.mText)
            end
            if inColor then
                o.c.mDrawables[self.PushId].mColor = inColor
            end
        end

        textLabel.Remove = function(self)
            -- TODO Implement Remove function
        end
        return textLabel
    end

    o.CreateSampledImage = function(inPosition_3f, inDimension_3f, inFileName, inNoDraw, inColor)
        local SampledImage = {}
        if (inFileName) then
            SampledImage.mId = o.s:AddImageByFileName(inFileName)
            SampledImage.mActualSize = o.s:GetImageSize(inFileName)
        else
            SampledImage.mId = o.s:AddImage(inDimension_3f.x, inDimension_3f.y)
            SampledImage.mActualSize = vec2(inDimension_3f.x, inDimension_3f.y)
        end
        local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill, uvec2(inDimension_3f.x, inDimension_3f.y))
        SampledImage.imageViewRect = o.s:Add(Rectangle, inPosition_3f)
        SampledImage.mColor = vec4(1, 1, 1, 1)
        if (inColor) then
            SampledImage.mColor = inColor
        end

        if (not inNoDraw) then
            SampledImage.DrawId = o.c:Push(Jkr.CreateDrawable(SampledImage.imageViewRect, false,
                "IMAGE",
                SampledImage.mId,
                SampledImage.mColor))
        end

        SampledImage.Update = function(self, inPosition_3f, inDimension_3f, inColor)
            local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill,
                uvec2(inDimension_3f.x, inDimension_3f.y))
            o.s:Update(SampledImage.imageViewRect, Rectangle, inPosition_3f)
            if inColor then
                o.c.mDrawables[self.DrawId].mColor = inColor
            end
        end

        SampledImage.CopyToCompute = function(inComputeImage)
            o.s:CopyFromImage(SampledImage.mId, inComputeImage.handle)
        end

        SampledImage.CopyDeferredImageFromWindow = function(inWindow)
            Jkr.CopyWindowDeferredImageToShapeImage(inWindow, o.s.handle, SampledImage.mId)
        end
        return SampledImage
    end

    --[============================================================[
                    COMPUTE IMAGE
                    rendering onto an image using compute shaders,
                    can also create a button by Image.CreateButton() routine
          ]============================================================]
    o.CreateComputeImage = function(inPosition_3f, inDimension_3f, inOptCompatibleSampledImage)
        local ComputeImage = {}
        if inOptCompatibleSampledImage then
            ComputeImage.handle = Jkr.CreateCustomPainterImage(o.i, o.w,
                math.int(inOptCompatibleSampledImage.mActualSize.x),
                math.int(inOptCompatibleSampledImage.mActualSize.y))
        else
            ComputeImage.handle = Jkr.CreateCustomPainterImage(o.i, o.w, math.int(inDimension_3f.x),
                math.int(inDimension_3f.y))
        end

        ComputeImage.RegisterPainter = function(inPainter, inIndex)
            if not inIndex then inIndex = 0 end
            ComputeImage.handle:Register(o.i, inPainter.handle, inIndex)
        end
        ComputeImage.BindPainter = function(inPainter)
            inPainter:Bind(o.w, Jkr.CmdParam.None)
            inPainter:BindImageFromImage(o.w, ComputeImage, Jkr.CmdParam.None)
        end
        ComputeImage.DrawPainter = function(inPainter, inPushConstant, inX, inY, inZ)
            inPainter:Draw(o.w, inPushConstant, inX, inY, inZ, Jkr.CmdParam.None)
        end
        ComputeImage.CopyToSampled = function(inSampledImage)
            o.s:CopyToImage(inSampledImage.mId, ComputeImage.handle)
        end
        return ComputeImage
    end

    o.CreateButton = function(inPosition_3f, inDimension_3f, inOnClickFunction, inContinous)
        local Button = {}
        Button.mBoundedRect = {}
        Button.mBoundedRect.mDepthValue = math.int(inPosition_3f.z)
        Button.mBoundedRect.mId = e:SetBoundedRect(vec2(inPosition_3f.x, inPosition_3f.y),
            vec2(inDimension_3f.x, inDimension_3f.y), math.int(inPosition_3f.z))
        Button.mOnClickFunction = function() end
        Button.mOnHoverFunction = function() end
        if (inOnClickFunction) then
            Button.mOnClickFunction = inOnClickFunction
        end
        if inContinous then
            Button.mBoundedRect.mPushId = o.c:Push(Jkr.CreateUpdatable(
                function()
                    local over = e:IsMouseWithinAtTopOfStack(
                        Button.mBoundedRect.mId,
                        Button.mBoundedRect.mDepthValue
                    )
                    if e:IsLeftButtonPressedContinous() and over then
                        Button.mOnClickFunction()
                    end
                end
            ))
        else
            Button.mBoundedRect.mPushId = o.c:Push(Jkr.CreateEventable(
                function()
                    local over = e:IsMouseWithinAtTopOfStack(
                        Button.mBoundedRect.mId,
                        Button.mBoundedRect.mDepthValue
                    )
                    if e:IsLeftButtonPressed() and over then
                        Button.mOnClickFunction()
                    end
                end
            ))
        end
        Button.Update = function(self, inPosition_3f, inDimension_3f)
            Button.mBoundedRect.mDepthValue = math.int(inPosition_3f.z)
            e:UpdateBoundedRect(Button.mBoundedRect.mId, vec2(inPosition_3f.x, inPosition_3f.y),
                vec2(inDimension_3f.x, inDimension_3f.y), Button.mBoundedRect.mDepthValue)
        end
        return Button
    end

    o.UIMatrix = mat4(0.0)
    --[========================================================]
    --
    --
    --
    -- DRAW DISPATCH UPDATE EVENT Stuffs
    --
    --
    --
    --[========================================================]
    o.WindowDimension = o.w:GetWindowDimension()
    o.FrameDimension = o.w:GetOffscreenFrameDimension()
    o.UIMatrix = Jmath.Ortho(0.0, o.FrameDimension.x, 0.0, o.FrameDimension.y, 1000, -1000)

    o.Update = function(self)
        self.c:Update()
    end

    o.Draw = function(self)
        -- Optimize this
        self.s:BindShapes(self.w, Jkr.CmdParam.UI)
        for i = 1, #self.c.mDrawables, 1 do
            local drawable = self.c.mDrawables[i]
            self.s:BindFillMode(Jkr.FillType.Image, self.w, Jkr.CmdParam.UI)
            if drawable.mDrawType == "IMAGE" then
                self.s:BindImage(self.w, drawable.mImageId, Jkr.CmdParam.UI)
                self.s:Draw(self.w, drawable.mColor, drawable.mId, drawable.mId, self.UIMatrix,
                    Jkr.CmdParam.UI)
            end
            if drawable.mDrawType == "TEXT" then
                self.t:Draw(drawable.mId, self.w, drawable.mColor, self.UIMatrix, Jkr.CmdParam.UI)
            end
        end
    end

    o.Dispatch = function(self)
        self.s:Dispatch(self.w, Jkr.CmdParam.None)
        self.c:Dispatch()
    end

    o.Event = function(self)
        self.c:Event()
    end
    return o
end

Jkr.CreateGeneralWidgetsRenderer = function(inWidgetRenderer, i, w, e)
    local o = {}
    if not inWidgetRenderer then
        o = Jkr.CreateWidgetRenderer(i, w, e)
    end

    o.shaders = {}
    o.shaders.roundedRectangle = Engine.Shader()
        .Header(450)
        .CInvocationLayout(1, 1, 1)
        .uImage2D()
        .ImagePainterPushMatrix2()
        .GlslMainBegin()
        .ImagePainterAssistMatrix2()
        .Append [[
        vec2 center = vec2(p1.x, p1.y);
        vec2 hw = vec2(p1.z, p1.w);
        float radius = p3.x;
        vec2 Q = abs(xy - center) - hw;

        float color = distance(max(Q, vec2(0.0)), vec2(0.0)) + min(max(Q.x, Q.y), 0.0) - radius;
        color = smoothstep(-0.05, 0.05, -color);

        vec4 old_color = imageLoad(storageImage, to_draw_at);
        vec4 final_color = vec4(p2.x * color, p2.y * color, p2.z * color, p2.w * color);
        final_color = mix(final_color, old_color, p3.w);

        imageStore(storageImage, to_draw_at, final_color);
          ]]
        .GlslMainEnd().str

    o.prebuilts = {}
    o.prebuilts.roundedRectanglePainter = Jkr.CreateCustomImagePainter("cache/o.prebuilts.roundedRectangle.glsl",
        o.shaders.roundedRectangle)
    o.prebuilts.roundedRectanglePainter:Store(i, w)

    local op = o.prebuilts;


    o.CreateGeneralButton = function(inPosition_3f,
                                     inDimension_3f,
                                     inOnClickFunction,
                                     inContinous,
                                     inFont,
                                     inText,
                                     inColor,
                                     inBackgroundColor,
                                     inPushConstantForImagePainter,
                                     inImageFilePath,
                                     inImagePainter)
        local button = {}
        if inDimension_3f.x == 0 or inDimension_3f.y == 0 then
            button.shouldUpdateByDimension = vec3(inDimension_3f.x, inDimension_3f.y, inDimension_3f.z)
            inDimension_3f = vec3(100, 100, 1)
        end

        if (inOnClickFunction) then
            button.parent = o.CreateButton(inPosition_3f, inDimension_3f, inOnClickFunction, inContinous)
            setmetatable(button, button.parent)
            button.__index = button.parent
        end

        if not inImagePainter then
            inImagePainter = o.prebuilts.roundedRectanglePainter
        end


        button.roundedRectangle = o.CreateComputeImage(inPosition_3f, inDimension_3f)
        button.roundedRectangle.RegisterPainter(inImagePainter)

        o.c:PushOneTime(Jkr.CreateDispatchable(function()
            button.roundedRectangle.BindPainter(op.roundedRectanglePainter)
            local PushConstant
            if inPushConstantForImagePainter then
                PushConstant = inPushConstantForImagePainter
            else
                PushConstant = Jkr.Matrix2CustomImagePainterPushConstant()
                PushConstant.a = mat4(
                    vec4(0.0, 0.0, 0.85, 0.85),
                    vec4(1),
                    vec4(0.1, 0.5, 0.5, 0.0),
                    vec4(0)
                )
            end
            button.roundedRectangle.DrawPainter(op.roundedRectanglePainter, PushConstant, math.int(inDimension_3f.x),
                math.int(inDimension_3f.y), 1)
        end), 1)

        button.sampledImage = o.CreateSampledImage(inPosition_3f, inDimension_3f, inImageFilePath, nil, inBackgroundColor)
        if not inImageFilePath then
            o.c:PushOneTime(Jkr.CreateDispatchable(function()
                button.roundedRectangle.CopyToSampled(button.sampledImage)
            end), 1)
            button.sampledText = o.CreateTextLabel(inPosition_3f, inDimension_3f, inFont, inText, inColor)
        else

        end

        button.padding = 10

        button.Update = function(self, inPosition_3f, inDimension_3f, inFont, inText, inColor, inBackgroundColor,
                                 inTextOreintation)
            button.sampledImage:Update(inPosition_3f, inDimension_3f, inBackgroundColor)

            if button.parent then
                button.parent:Update(inPosition_3f, inDimension_3f)
            end

            if not inImageFilePath then
                local DelDim = vec3(0, 0, 0)
                local fontDim = button.sampledText.mFont:GetTextDimension(inText or Copy(button.sampledText.mText) or
                    " ")
                DelDim = vec3((inDimension_3f.x - fontDim.x) / 2, (inDimension_3f.y - fontDim.y) / 2, 0)
                local substr = Copy(inText)
                while DelDim.x < 0.0 and substr do
                    substr = string.sub(substr, 1, #substr - 1)
                    fontDim = button.sampledText.mFont:GetTextDimension(substr)
                    DelDim = vec3((inDimension_3f.x - fontDim.x) / 2, (inDimension_3f.y - fontDim.y) / 2, 0)
                end
                button.sampledText:Update(inPosition_3f + DelDim, inDimension_3f, inFont, substr, inColor)

                if inTextOreintation then
                    if inTextOreintation == "LEFT" then
                        DelDim.x = button.padding
                        button.sampledText:Update(inPosition_3f + DelDim, inDimension_3f, inFont, substr, inColor)
                    end
                    if inTextOreintation == "RIGHT" then
                        DelDim.x = inPosition_3f.x - fontDim.x - button.padding
                        DelDim.x = inPosition_3f.x
                        button.sampledText:Update(inPosition_3f + DelDim, inDimension_3f, inFont, substr, inColor)
                    end
                end
            end

            if inText then
                button.sampledText.mText = Copy(inText)
            end
        end

        if button.shouldUpdateByDimension then
            button:Update(inPosition_3f, button.shouldUpdateByDimension)
        end

        return button
    end
    return o
end


          )ijum";
