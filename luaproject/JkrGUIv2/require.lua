Engine = {}
MAIN_JKR = "main.jkr"
MAIN_JKR_FILE = MAIN_JKR_FILE or Jkr.File(MAIN_JKR)
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
    return f()
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
	mat4 Matrix1;
	mat4 Matrix2;
} push;


void GlslMain() {
	vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
	gl_Position = push.Matrix2 * dx;
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
	mat4 Matrix1;
	mat4 Matrix2;
} push;

void GlslMain()
{
	outColor = push.Matrix1[1];
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
	mat4 Matrix1;
	mat4 Matrix2;
} push;

void GlslMain()
{
    vec4 color_in = push.Matrix1[1];
	vec4 color = texture(image, inTexCoord);
	outColor = vec4(color.r * color_in.r, color.g * color_in.g, color.b * color_in.b, color.a * color_in.a);
}
                ]]
                -- @warning @todo remove this
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

    -- @warning @todo Remove all of the following

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
        DefaultCaches["Line"]:Store(MAIN_JKR_FILE, "cache2/LineRendererCache.glsl",
            Jkr.GetDefaultResource("Line", "Vertex"),
            Jkr.GetDefaultResource("Line", "Fragment"),
            Jkr.GetDefaultResource(nil, "Compute")
        )
        return DefaultCaches["Line"]
    elseif inRend == "Shape" then
        if DefaultCaches["Shape"] then
            return DefaultCaches["Shape"]
        else
            DefaultCaches["Shape"] = Jkr.ShapeRendererResources()
            DefaultCaches["Shape"]:Add(
                inInstance,
                Jkr.FillType.Fill,
                Jkr.PipelineProperties.Default,
                "cache2/ShapeFillCache.glsl",
                Jkr.GetDefaultResource("ShapeFill", "Vertex"),
                Jkr.GetDefaultResource("ShapeFill", "Fragment"),
                Jkr.GetDefaultResource(nil, "Compute"),
                MAIN_JKR_FILE
            )
            DefaultCaches["Shape"]:Add(
                inInstance,
                Jkr.FillType.Image,
                Jkr.PipelineProperties.Default,
                "cache2/ShapeImageCache.glsl",
                Jkr.GetDefaultResource("ShapeImage", "Vertex"),
                Jkr.GetDefaultResource("ShapeImage", "Fragment"),
                Jkr.GetDefaultResource(nil, "Compute"),
                MAIN_JKR_FILE
            )
            DefaultCaches["Shape"]:Add(
                inInstance,
                Jkr.FillType.ContinousLine,
                Jkr.PipelineProperties.Line,
                "cache2/ShapeFillCache.glsl",
                Jkr.GetDefaultResource("ShapeFill", "Vertex"),
                Jkr.GetDefaultResource("ShapeFill", "Fragment"),
                Jkr.GetDefaultResource(nil, "Compute"),
                MAIN_JKR_FILE
            )
            return DefaultCaches["Shape"]
        end
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
        MAIN_JKR_FILE
    )
    Resource:Add(
        inInstance,
        Jkr.FillType.Image,
        Jkr.PipelineProperties.Default,
        "cache2/ShapeImageCache.glsl",
        Jkr.GetDefaultResource("ShapeImage", "Vertex"),
        Jkr.GetDefaultResource("ShapeImage", "Fragment"),
        Jkr.GetDefaultResource(nil, "Compute"),
        MAIN_JKR_FILE
    )
    Resource:Add(
        inInstance,
        Jkr.FillType.ContinousLine,
        Jkr.PipelineProperties.Line,
        "cache2/ShapeFillCache.glsl",
        Jkr.GetDefaultResource("ShapeFill", "Vertex"),
        Jkr.GetDefaultResource("ShapeFill", "Fragment"),
        Jkr.GetDefaultResource(nil, "Compute"),
        MAIN_JKR_FILE
    )
    return Resource
end


--[============================================================[
    CREATE JKR INSTANCE
]============================================================]


Jkr.CreateInstance = function(inEnableValidation, inVarDesSet, inPoolSize, inThreadsCount)
    if not inVarDesSet then inVarDesSet = 100000 end
    if not inPoolSize then inPoolSize = 100000 end
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
    o.Add = function(self, inGenerator, inP)
        if not self.recycleBin:IsEmpty() then
            local i = self.recycleBin:Get()
            self.sr:Update(inGenerator, inP)
            return i
        else
            return self.sr:Add(inGenerator, inP)
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

    o.Update = function(self, inId, inGenerator, inP)
        self.sr:Update(inId, inGenerator, inP.x, inP.y, inP.z) -- TODO Improve this
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
    o.Add = function(self, inFontId, inP, inText)
        return self.tr:Add(inFontId, inP, inText)
    end
    o.Update = function(self, inTextImageId, inFontId, inP, inText)
        self.tr:Update(inTextImageId, inFontId, inP, inText)
    end
    o.UpdatePosOnly = function(self, inTextImageId, inFontId, inP, inText)
        self.tr:UpdatePosOnly(inTextImageId, inFontId, inP, inText)
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

    o.Store = function(self, file, i, w)
        o.handle:Store(i, w, file)
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

function FillTable(inTable, inValue)
    local count = #inTable
    for i = 1, count, 1 do
        inTable[i] = nil
    end
end

function Jkr.FilePicker()
    local file = io.popen([[
    powershell -Command "Add-Type -AssemblyName System.Windows.Forms; $f = New-Object System.Windows.Forms.OpenFileDialog; $f.Filter = 'All Files (*.*)|*.*'; if ($f.ShowDialog() -eq 'OK') { $f.FileName }"
    ]])
    local filePath = file:read("*a"):gsub("%s+$", "") -- Read and trim output
    file:close()
    return filePath
end

Jkr.Java = {}
setmetatable(Jkr.Java, {
    __index = function(_, methodName)
        return function(inString)
            return Jkr.JavaCallVCharMethodStringArg("org/JkrGUI/JkrGUIActivity", methodName, inString)
        end
    end
})

try = function(inFunction, inMessage)
    local success, ret = pcall(inFunction)
    if not success then
        Engine.log(inMessage, "ERROR")
    end
    return ret
end

--- MATH

Jmath.Clamp = function(a, b, t)
    if t < a then
        return a
    elseif t > b then
        return b
    else
        return t
    end
end

Jmath.GetLerps = function(inType)
    local lerp, lerp_2f, lerp_3f, lerp_4f, lerp_mat4f
    if inType == "QUADLINEAR" then
        lerp = function(a, b, t)
            local it = 1 - t
            return (a * it + b * t) * it + b * t
        end
        lerp_2f = function(a, b, t)
            local it = 1 - t
            return vec2(((a.x * it + b.x * t) * it + b.x * t), ((a.y * it + b.y * t) * it + b.y * t))
        end

        lerp_3f = function(a, b, t)
            local it = 1 - t
            return vec3(
                ((a.x * it + b.x * t) * it + b.x * t),
                ((a.y * it + b.y * t) * it + b.y * t),
                ((a.z * it + b.z * t) * it + b.z * t)
            )
        end

        lerp_4f = function(a, b, t)
            local it = 1 - t
            return vec4(
                ((a.x * it + b.x * t) * it + b.x * t),
                ((a.y * it + b.y * t) * it + b.y * t),
                ((a.z * it + b.z * t) * it + b.z * t),
                ((a.w * it + b.w * t) * it + b.w * t)
            )
        end

        lerp_mat4f = function(a, b, t)
            local it = 1 - t
            return mat4(
                vec4(
                    ((a[1].x * it + b[1].x * t) * it + b[1].x * t),
                    ((a[1].y * it + b[1].y * t) * it + b[1].y * t),
                    ((a[1].z * it + b[1].z * t) * it + b[1].z * t),
                    ((a[1].w * it + b[1].w * t) * it + b[1].w * t)),
                vec4(((a[2].x * it + b[2].x * t) * it + b[2].x * t),
                    ((a[2].y * it + b[2].y * t) * it + b[2].y * t),
                    ((a[2].z * it + b[2].z * t) * it + b[2].z * t),
                    ((a[2].w * it + b[2].w * t) * it + b[2].w * t)),
                vec4(((a[3].x * it + b[3].x * t) * it + b[3].x * t),
                    ((a[3].y * it + b[3].y * t) * it + b[3].y * t),
                    ((a[3].z * it + b[3].z * t) * it + b[3].z * t),
                    ((a[3].w * it + b[3].w * t) * it + b[3].w * t)),
                vec4(((a[4].x * it + b[4].x * t) * it + b[4].x * t),
                    ((a[4].y * it + b[4].y * t) * it + b[4].y * t),
                    ((a[4].z * it + b[4].z * t) * it + b[4].z * t),
                    ((a[4].w * it + b[4].w * t) * it + b[4].w * t))
            )
        end
    else -- LINEAR
        lerp = function(a, b, t)
            return a + (b - a) * t
        end
        lerp_2f = function(a, b, t)
            return vec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t)
        end

        lerp_3f = function(a, b, t)
            return vec3(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t
            )
        end

        lerp_4f = function(a, b, t)
            return vec4(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t,
                a.w + (b.w - a.w) * t
            )
        end

        lerp_mat4f = function(a, b, t)
            return mat4(
                vec4(a[1].x + (b[1].x - a[1].x) * t, a[1].y + (b[1].y - a[1].y) * t, a[1].z + (b[1].z - a[1].z) * t,
                    a[1].w + (b[1].w - a[1].w) * t),
                vec4(a[2].x + (b[2].x - a[2].x) * t, a[2].y + (b[2].y - a[2].y) * t, a[2].z + (b[2].z - a[2].z) * t,
                    a[2].w + (b[2].w - a[2].w) * t),
                vec4(a[3].x + (b[3].x - a[3].x) * t, a[3].y + (b[3].y - a[3].y) * t, a[3].z + (b[3].z - a[3].z) * t,
                    a[3].w + (b[3].w - a[3].w) * t),
                vec4(a[4].x + (b[4].x - a[4].x) * t, a[4].y + (b[4].y - a[4].y) * t, a[4].z + (b[4].z - a[4].z) * t,
                    a[4].w + (b[4].w - a[4].w) * t)
            )
        end
    end
    return { lerp = lerp, lerp_2f = lerp_2f, lerp_3f = lerp_3f, lerp_4f = lerp_4f, lerp_mat4f = lerp_mat4f }
end

function Default(inTable, def)
    if type(inTable) == "table" and type(def) == "table" then
        for key, value in pairs(def) do
            inTable[key] = Default(inTable[key], value)
        end
        return inTable
    else
        if type(inTable) == "boolean" then
            return inTable
        end
        return inTable or def
    end
end

function table.Produce(infunc, intimes)
    local t = {}
    for i = 1, intimes do
        t[i] = infunc(i)
    end
    return t
end

function table.Operate(inTable, inFunc, inFrom, intimes)
    for i = inFrom or 1, intimes or #inTable do
        inFunc(inTable[i], i)
    end
end

function table.Map(func, tbl)
    local result = {}
    for i, v in ipairs(tbl) do
        result[i] = func(v, i)
    end
    return result
end

function table.Filter(func, tbl)
    local result = {}
    for i, v in ipairs(tbl) do
        if func(v, i) then
            table.insert(result, v)
        end
    end
    return result
end

function table.Reduce(func, tbl, init)
    local acc = init or tbl[1]
    local start = init and 1 or 2
    for i = start, #tbl do
        acc = func(acc, tbl[i], i)
    end
    return acc
end

function table.Zip(t1, t2)
    local result = {}
    for i = 1, math.min(#t1, #t2) do
        result[i] = { t1[i], t2[i] }
    end
    return result
end

function table.Range(a, b)
    local t = {}
    for i = a, b do
        table.insert(t, i)
    end
    return t
end

function table.Clone(t)
    local r = {}
    for k, v in pairs(t) do
        r[k] = v
    end
    return r
end

function table.Merge(t1, t2)
    local t = table.Clone(t1)
    for k, v in pairs(t2) do
        t[k] = v
    end
    return t
end

function table.Find(tbl, pred)
    for i, v in ipairs(tbl) do
        if pred(v, i) then return v end
    end
end

function table.Every(tbl, pred)
    for i, v in ipairs(tbl) do
        if not pred(v, i) then return false end
    end
    return true
end

function table.Some(tbl, pred)
    for i, v in ipairs(tbl) do
        if pred(v, i) then return true end
    end
    return false
end

function string.ParseCSV(csv_string, delimiter)
    delimiter = delimiter or ","
    local result = {}

    for line in csv_string:gmatch("[^\r\n]+") do
        local row = {}
        for cell in line:gmatch("([^" .. delimiter .. "]+)") do
            table.insert(row, cell)
        end
        table.insert(result, row)
    end

    return result
end

file = {}

function file.ReadAsString(path)
    local f = assert(io.open(path, "r"))
    local content = f:read("*a")
    f:close()
    return content
end

function file.ReadLines(path)
    local lines = {}
    for line in io.lines(path) do
        table.insert(lines, line)
    end
    return lines
end

function file.EachLine(path, callback)
    for line in io.lines(path) do
        callback(line)
    end
end

function file.Write(path, content)
    local f = assert(io.open(path, "w"))
    f:write(content)
    f:close()
end

function file.Append(path, content)
    local f = assert(io.open(path, "a"))
    f:write(content)
    f:close()
end

function file.WriteLines(path, lines)
    local f = assert(io.open(path, "w"))
    for _, line in ipairs(lines) do
        f:write(line .. "\n")
    end
    f:close()
end

function file.Clear(path)
    local f = assert(io.open(path, "w"))
    f:close()
end

function file.Exists(path)
    local f = io.open(path, "r")
    if f then
        f:close()
        return true
    end
    return false
end

function file.LineCount(path)
    local count = 0
    for _ in io.lines(path) do
        count = count + 1
    end
    return count
end

function file.ReadCSV(path, delimiter)
    delimiter = delimiter or ","
    local result = {}
    for line in io.lines(path) do
        local row = {}
        for cell in line:gmatch("([^" .. delimiter .. "]+)") do
            table.insert(row, cell)
        end
        table.insert(result, row)
    end
    return result
end

function file.TryRead(path)
    local ok, res = pcall(file.ReadAsString, path)
    return ok and res or nil
end

function file.ReadKeyValue(path)
    local kv = {}
    for line in io.lines(path) do
        local k, v = line:match("^(.-)=(.*)$")
        if k and v then
            kv[k:match("^%s*(.-)%s*$")] = v:match("^%s*(.-)%s*$")
        end
    end
    return kv
end

function file.DumpTable(path, tbl)
    local f = assert(io.open(path, "w"))
    for k, v in pairs(tbl) do
        f:write(tostring(k) .. "=" .. tostring(v) .. "\n")
    end
    f:close()
end
