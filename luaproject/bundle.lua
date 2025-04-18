function jkrgui()
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
    local GetLerp = function(inType)
        inType = inType or "QUADLINEAR"
        if inType == "QUADLINEAR" then
            return function(a, b, t)
                return (a * (1 - t) + t * b) * (1 - t) + b * t
            end
        elseif inType == "LINEAR" then
            return function(a, b, t)
                return a + (1 - t) * b
            end
        elseif inType == "LINEAR_CLAMPED" then
        end
    end

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
        local lerp = GetLerp(inType)
        local lerp_2f = function(a, b, t)
            return vec2(lerp(a.x, b.x, t), lerp(a.y, b.y, t))
        end

        local lerp_3f = function(a, b, t)
            return vec3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t))
        end

        local lerp_4f = function(a, b, t)
            return vec4(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t), lerp(a.w, b.w, t))
        end
        local lerp_mat4f = function(a, b, t)
            return mat4(
                lerp_4f(a[1], b[1], t),
                lerp_4f(a[2], b[2], t),
                lerp_4f(a[3], b[3], t),
                lerp_4f(a[4], b[4], t)
            )
        end
        return lerp, lerp_2f, lerp_3f, lerp_4f, lerp_mat4f
    end


    local ImagePainterPush          = [[
layout(std430, push_constant) uniform pc {
        vec4 mPosDimen;
        vec4 mColor;
        vec4 mParam;
} push;
    ]]

    local ImagePainterPushMatrix2   = [[
layout(std430, push_constant) uniform pc {
    mat4 a;
    mat4 b;
} push;
       ]]

    local ImagePainterAssistMatrix2 = [[
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

    local ImagePainterAssist        = [[
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


    local vLayout               = [[

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
                    ]]

    local push                  = [[

layout(push_constant, std430) uniform pc {
    mat4 model;
    mat4 m2;
} Push;
]]
    local Ubo                   = [[

layout(set = 0, binding = 0) uniform UBO {
   mat4 view;
   mat4 proj;
   vec3 campos;
   vec4 nearFar;
   vec4 lights[8];
   mat4 shadowMatrix;
   vec4 lightDirections[8];
   mat4 shadowMatrixCascades[4];
} Ubo;
]]

    local LinearizeDepth        = [[

float LinearizeDepth(float depth, float near, float far)
{
  float n = near; // TODO Add an entry on uniform
  float f = far;
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));	
}
          ]]

    local ShadowTextureProject  = [[

float ShadowTextureProject(vec4 shadowCoord, vec2 off, float ambient)
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

    local inJointInfluence      = [[

struct JointInfluence {
    vec4 mJointIndices;
    vec4 mJointWeights;
};

layout(std140, set = 1, binding = 2) readonly buffer JointInfluenceSSBOIn {
   JointInfluence inJointInfluence[ ];
};

]]

    local inTangent             = [[
        struct Tangent {
            vec4 mTangent;
        };

        layout(std140, set = 1, binding = 14) readonly buffer TangentSSBOIn {
            Tangent inTangent[];
        };
    ]]

    local inJointMatrices       = [[

layout(std140, set = 1, binding = 1) readonly buffer JointMatrixSSBOIn {
    mat4 inJointMatrices[ ];
};
          ]]
    local BiasMatrix            = [[

    const mat4 BiasMatrix = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0 );
    ]]

    local D_GGX                 = [[
// Normal Distribution Function
float D_GGX(float dotNH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2)/ (PI * denom * denom);
}

    ]]

    local G_SchlicksmithGGX     = [[
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

    local F_Schlick             = [[
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

    local F_SchlickR            = [[
vec3 F_SchlickR(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
    ]]

    local PrefilteredReflection = [[
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


    local shadow_textureProj = [[
float textureProj(vec4 shadowCoord, vec2 offset, uint cascadeIndex, float ambient)
{
    float shadow = 1.0;
    float bias = 0.005;

    if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) {
        float dist = texture(shadowMap, vec3(shadowCoord.st + offset, cascadeIndex)).r;
        if (shadowCoord.w > 0 && dist < shadowCoord.z - bias) {
            shadow = ambient;
        }
    }
    return shadow;

}
]]


    local filterPCF = [[
float filterPCF(vec4 sc, uint cascadeIndex, float ambient)
{
    ivec2 texDim = textureSize(shadowMap, 0).xy;
    float scale = 0.75;
    float dx = scale * 1.0 / float(texDim.x);
    float dy = scale * 1.0 / float(texDim.y);

    float shadowFactor = 0.0;
    int count = 0;
    int range = 1;

    for (int x = -range; x <= range; x++) {
        for (int y = -range; y <= range; y++) {
            shadowFactor += textureProj(sc, vec2(dx*x, dy*y), cascadeIndex, ambient);
            count++;
        }
    }
    return shadowFactor / count;
}
]]

    -- STRING_STRING


    -- @warning this eats up a lot of memory, @todo keep the locals, locally in this file
    -- but before that, ensure that the multithreaded shader compilation works fine
    Engine.Shader = function()
        local o                         = {}

        ---
        ---
        ---
        --- FOR IMAGE PAINTER COMPUTE SHADER
        ---
        ---
        ---

        o.uImage2D                      = function(inBinding, inImageName)
            if not inBinding then inBinding = 0 end
            if not inImageName then inImageName = "storageImage" end
            o.NewLine()
            o.Append(string.format([[
layout(set = 0, binding = %d, rgba8) uniform image2D %s;
    ]], inBinding, inImageName))
            return o.NewLine()
        end

        o.CInvocationLayout             = function(inX, inY, inZ)
            o.NewLine()
            o.Append(
                string.format("layout(local_size_x = %d, local_size_y = %d, local_size_z = %d) in;", inX,
                    inY, inZ)
            )
            return o.NewLine()
        end

        o.ImagePainterPush              = function()
            o.NewLine()
            o.Append(ImagePainterPush)
            return o.NewLine()
        end

        o.ImagePainterPushMatrix2       = function()
            o.NewLine()
            o.Append(ImagePainterPushMatrix2)
            return o
        end

        o.ImagePainterAssistMatrix2     = function()
            o.NewLine()
            o.Append(ImagePainterAssistMatrix2)
            return o;
        end

        o.ImagePainterAssist            = function()
            o.NewLine()
            o.Append(ImagePainterAssist)
            return o.NewLine()
        end

        o.ImagePainterAssistConvolution = function(inImageNameFrom, inImageNameTo)
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


        o.vertexStorageBufferIndex    = 14
        o.indexStorageBufferIndex     = 15

        o.ImagePainterVIStorageLayout = function()
            o.NewLine()
            o.Append(storageVIBufferLayoutCustomImagePainter)
            return o.NewLine()
        end

        ---
        ---
        --- MOSTLY FOR 3D
        ---
        ---

        o.str                         = ""

        o.Append                      = function(inStr)
            o.str = o.str .. inStr
            return o.NewLine()
        end

        o.DontAppend                  = function(inStr)
            return o -- do nothing
        end

        o.Clear                       = function()
            o.str = ""
            return o
        end

        o.NewLine                     = function()
            o.str = o.str .. "\n"
            return o
        end

        o.Indent                      = function()
            o.str = o.str .. "\t"
            return o
        end

        o.Header                      = function(inVersion)
            o.str = o.str .. string.format("#version %d", inVersion)
            o.NewLine()
            o.str = o.str .. "#extension GL_EXT_debug_printf : enable"
            o.NewLine()
            return o.NewLine()
        end

        o.VLayout                     = function()
            o.str = o.str .. vLayout
            return o.NewLine()
        end

        o.Out                         = function(inLocation, inType, inName)
            o.str = o.str .. string.format("layout (location = %d) out %s %s;", inLocation, inType, inName)
            return o.NewLine()
        end

        o.In                          = function(inLocation, inType, inName)
            o.str = o.str .. string.format("layout (location = %d) in %s %s;", inLocation, inType, inName)
            return o.NewLine()
        end

        o.outFragColor                = function()
            o.str = o.str .. "layout( location = 0 ) out vec4 outFragColor;"
            return o.NewLine()
        end

        o.Push                        = function()
            o.str = o.str .. push
            return o.NewLine()
        end

        o.Ubo                         = function()
            o.str = o.str .. Ubo
            return o.NewLine()
        end

        o.GlslMainBegin               = function()
            o.str = o.str .. "void GlslMain()"
            o.NewLine()
            o.str = o.str .. "{"
            return o.NewLine()
        end

        o.GlslMainEnd                 = function()
            o.str = o.str .. "}"
            return o.NewLine()
        end

        o.ShadowGLPosition            = function()
            o.Indent()
            o.str = o.str .. "gl_Position = Ubo.proj * Ubo.shadowMatrix * Push.model * vec4(inPosition, 1.0);"
            return o.NewLine()
        end

        o.InvertY                     = function()
            o.Indent()
            o.str = o.str .. "gl_Position.y = -gl_Position.y;"
            return o.NewLine()
        end

        o.uSampler2D                  = function(inBinding, inName, inSet)
            if inSet then
                o.str = o.str ..
                    string.format("layout(set = %d, binding = %d) uniform sampler2D %s;", inSet, inBinding, inName)
            else
                o.str = o.str ..
                    string.format("layout(set = 1, binding = %d) uniform sampler2D %s;", inBinding, inName)
            end
            return o.NewLine()
        end

        o.uSamplerCubeMap             = function(inBinding, inName, inSet)
            if (inSet) then
                o.str = o.str ..
                    string.format("layout(set = %d, binding = %d) uniform samplerCube %s;", inSet, inBinding, inName)
            else
                o.str = o.str ..
                    string.format("layout(set = 1, binding = %d) uniform samplerCube %s;", inBinding, inName)
            end
            return o.NewLine()
        end

        o.LinearizeDepth              = function()
            o.NewLine()
            o.str = o.str .. LinearizeDepth
            return o.NewLine()
        end

        o.ShadowTextureProject        = function()
            o.NewLine()
            o.str = o.str .. ShadowTextureProject
            return o.NewLine()
        end

        o.inJointInfluence            = function()
            o.Append(inJointInfluence)
            return o.NewLine()
        end

        o.inJointMatrices             = function()
            o.Append(inJointMatrices)
            return o.NewLine()
        end

        o.inTangent                   = function()
            o.Append(inTangent)
            return o.NewLine()
        end

        o.BiasMatrix                  = function()
            o.Append(BiasMatrix)
            return o.NewLine()
        end

        o.PI                          = function()
            o.Append("const float PI = 3.141592653;")
            return o.NewLine()
        end

        o.D_GGX                       = function()
            o.NewLine()
            o.Append(D_GGX)
            return o.NewLine()
        end

        o.G_SchlicksmithGGX           = function()
            o.NewLine()
            o.Append(G_SchlicksmithGGX)
            return o.NewLine()
        end

        o.F_Schlick                   = function()
            o.NewLine()
            o.Append(F_Schlick)
            return o.NewLine()
        end

        o.F_SchlickR                  = function()
            o.NewLine()
            o.Append(F_SchlickR)
            return o.NewLine()
        end

        o.BRDF                        = function()
            o.NewLine()
            o.Append(BRDF)
            return o.NewLine()
        end

        o.Uncharted2Tonemap           = function()
            o.NewLine()
            o.Append(Uncharted2Tonemap)
            return o.NewLine()
        end

        o.SRGBtoLINEAR                = function()
            o.NewLine()
            o.Append(SRGBtoLINEAR)
            return o.NewLine()
        end

        o.Random                      = function()
            o.NewLine()
            o.Append(Random)
            return o.NewLine()
        end

        o.Hammerslay2d                = function()
            o.NewLine()
            o.Append(Hammerslay2d)
            return o.NewLine()
        end

        o.ImportanceSample_GGX        = function()
            o.NewLine()
            o.Append(ImportanceSample_GGX)
            return o.NewLine()
        end

        o.PrefilterEnvMap             = function()
            o.NewLine()
            o.Append(PrefilterEnvMap)
            return o.NewLine()
        end

        o.PrefilteredReflection       = function()
            o.NewLine()
            o.Append(PrefilteredReflection)
            return o.NewLine()
        end

        o.SpecularContribution        = function()
            o.NewLine()
            o.Append(SpecularContribution)
            return o.NewLine()
        end

        o.BRDF_LUT                    = function()
            o.NewLine()
            o.Append(BRDF_LUT)
            return o.NewLine()
        end

        o.MaterialColorBegin          = function()
            o.str = o.str .. "vec3 MaterialColor()"
            o.NewLine()
            o.str = o.str .. "{"
            return o.NewLine()
        end

        o.MaterialColorEnd            = function()
            o.str = o.str .. "}"
            return o.NewLine()
        end

        o.Define                      = function(inName, inValue)
            o.str = o.str .. string.format("#define %s %s", inName, inValue)
            return o.NewLine().NewLine()
        end

        o.Shadow_textureProj          = function()
            return o.Append(shadow_textureProj).NewLine()
        end

        o.filterPCF                   = function()
            return o.Append(filterPCF).NewLine()
        end

        o.Print                       = function()
            local lineNumber = 13

            for line in o.str:gmatch("[^\n]+") do
                print(lineNumber .. ": " .. line)
                lineNumber = lineNumber + 1
            end
            return o
        end
        o.gltfMaterialTextures        = {}
        o.gltfPutMaterialTextures     = function(inGLTF, inMaterialIndex)
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
    //debugPrintfEXT("PrefilterCubeMap:: Roughness: %f, Samples: %d, color: (%f, %f, %f)\n", consts.roughness, consts.numSamples, color.x, color.y, color.z);
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
            float sinTheta = sin(theta);
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            float cosTheta = cos(theta);
            vec3 tangentSample = vec3(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
            vec3 sampleVector = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
            color += texture(samplerEnv, sampleVector).rgb * cosTheta * sinTheta;
            sampleCount++;
        }
    }

    color  = PI * color / float(sampleCount);
    //debugPrintfEXT("deltaPhi = %f, deltaTheta = %f, color: (%f, %f, %f)\n", consts.deltaPhi, consts.deltaTheta, color.x, color.y, color.z);
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
    gl_Position = Push.m2 * vec4(localPos, 1.0);
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

    Shape2DShaders = {}
    Shape2DShaders.GeneralVShader = Engine.Shader()
        .Header(450)
        .In(0, "vec3", "inPosition")
        .In(1, "vec2", "inTexCoord")
        .Out(0, "vec2", "outTexCoord")
        .ImagePainterPushMatrix2()
        .GlslMainBegin()
        .Append [[
    vec4 dx = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
    gl_Position = push.b * dx;
    outTexCoord = inTexCoord;
]]
        .GlslMainEnd()

    Shape2DShaders.RoundedRectangleFShader = Engine.Shader()
        .Header(450)
        .outFragColor()
        .In(0, "vec2", "inTexCoord")
        .ImagePainterPushMatrix2()
        .GlslMainBegin()
        .Append [[
    vec4 p1 = push.a[0];
    vec4 p2 = push.a[1]; // Color
    vec4 p3 = push.a[3]; // Has Radius
    vec2 center = vec2(p1.x, p1.y);
    vec2 hw = vec2(p1.z, p1.w);
    float radius = p3.x;
    vec2 xy = inTexCoord * 2.0f - vec2(1.0f);
    vec2 Q = abs(xy - center) - hw;

    float color = distance(max(Q, vec2(0.0)), vec2(0.0)) + min(max(Q.x, Q.y), 0.0) - radius;
    color = smoothstep(-0.05, 0.05, -color);
    outFragColor = p2 * color;
    ]].GlslMainEnd()

    Shape2DShaders.ShowImageFShader = Engine.Shader()
        .Header(450)
        .outFragColor()
        .In(0, "vec2", "inTexCoord")
        .Append [[
        layout(set = 0, binding = 0) uniform sampler2D image;
    ]]
        .ImagePainterPushMatrix2()
        .GlslMainBegin()
        .Append [[
    vec4 p1 = push.a[0];
    vec4 p2 = push.a[1]; // Color
    vec4 p3 = push.a[3]; // Has Radius
    outFragColor = p2 * texture(image, inTexCoord);
    ]].GlslMainEnd()


    TwoDimensionalIPs = {}
    TwoDimensionalIPs.Header = function()
        return Engine.Shader()
            .Header(450)
            .CInvocationLayout(16, 16, 1)
            .uImage2D()
            ---@warning for uniform pipelinelayout for descriptors
            .ImagePainterVIStorageLayout()
            .ImagePainterPushMatrix2()
            .GlslMainBegin()
            .ImagePainterAssistMatrix2()
    end

    TwoDimensionalIPs.HeaderWithoutBegin = function()
        return Engine.Shader()
            .Header(450)
            .CInvocationLayout(16, 16, 1)
            .uImage2D()
            ---@warning for uniform pipelinelayout for descriptors (spirv reflect)
            .ImagePainterVIStorageLayout()
            .ImagePainterPushMatrix2()
            .Append [[

vec2 NormalizeToImage(vec2 inVec, ivec2 image_size)
{
    return vec2(
        (inVec.x - image_size.x / 2.0f) / (image_size.x / 2.0f),
        (image_size.y / 2.0f - inVec.y) / (image_size.y / 2.0f)
    );
}

        ]]
    end

    TwoDimensionalIPs.ConstantColor =
        TwoDimensionalIPs.Header()
        .Append [[
        imageStore(storageImage, to_draw_at, p4);
    ]]
        .GlslMainEnd()

    TwoDimensionalIPs.RoundedRectangle =
        TwoDimensionalIPs.Header()
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
        .GlslMainEnd()

    ---@note @todo Later make this ellipse
    TwoDimensionalIPs.Circle =
        TwoDimensionalIPs.Header()
        .Append [[
        vec2 center = vec2(p1.x, p1.y);
        float radius = p1.z;
        vec2 Q = abs(xy - center);
        float color = length(Q) - radius;
        //color = smoothstep(-0.05, 0.05, -color);
        color = -color;

        vec4 old_color = imageLoad(storageImage, to_draw_at);
        vec4 final_color = vec4(p2.x * color, p2.y * color, p2.z * color, p2.w * color);
        final_color = mix(final_color, old_color, p3.w);
        //debugPrintfEXT("final_color %f %f %f", final_color.x, final_color.y, final_color.z);

        imageStore(storageImage, to_draw_at, final_color);
          ]]
        .GlslMainEnd()

    TwoDimensionalIPs.Line =
        TwoDimensionalIPs.HeaderWithoutBegin()
        .GlslMainBegin()
        .ImagePainterAssistMatrix2()
        .Append [[
            vec2 point1 = vec2(push.b * vec4(p1.x, p1.y, 0, 1));
            vec2 point2 = vec2(push.b * vec4(p1.z, p1.w, 0, 1));
            float inv_thickness = p3.x;
            float radius = p3.y;
            float draw_thickness = p3.z;
            vec2 np_1 = NormalizeToImage(point1, image_size);
            vec2 np_2 = NormalizeToImage(point2, image_size);
            vec2 pa = xy - np_1;
            vec2 ba = np_2 - np_1;
            float h = min(1.0,
                    max(0.0, dot(pa, ba) / dot(ba, ba))
            );
            float k = 1 - (length(pa - ba * h) * inv_thickness * 10 - radius);
            if (k > draw_thickness)
            {
                imageStore(storageImage, to_draw_at, p2 * k);
            }
    ]]
        .GlslMainEnd()

    TwoDimensionalIPs.Line3D =
        TwoDimensionalIPs.HeaderWithoutBegin()
        .GlslMainBegin()
        .ImagePainterAssistMatrix2()
        .Append [[
            vec2 point1 = vec2(push.b * vec4(p1.x, p1.y, p1.z, 1));
            vec2 point2 = vec2(push.b * vec4(p2.x, p2.y, p2.z, 1));
            float inv_thickness = p3.x;
            float radius = p3.y;
            float draw_thickness = p3.z;
            vec2 np_1 = NormalizeToImage(point1, image_size);
            vec2 np_2 = NormalizeToImage(point2, image_size);
            vec2 pa = xy - np_1;
            vec2 ba = np_2 - np_1;
            float h = min(1.0,
                    max(0.0, dot(pa, ba) / dot(ba, ba))
            );
            float k = 1 - (length(pa - ba * h) * inv_thickness * 10 - radius);
            if (k > draw_thickness)
            {
                imageStore(storageImage, to_draw_at, p4 * k);
            }
    ]]
        .GlslMainEnd()
    TwoDimensionalIPs.Clear = TwoDimensionalIPs.Header()
        .Append [[
    imageStore(storageImage, to_draw_at, p1);
    ]]
        .GlslMainEnd()


    TwoDimensionalIPs.Sound = TwoDimensionalIPs.Header()
        .PI()
        .Append [[
    float at_ = float(to_draw_at.x + (to_draw_at.y * image_size.x));
    float Base = p1.x;
    float Frequency = p1.y;
    float Amplitude = p1.z;
    float sample1 = Base + Amplitude * sin(2.0 * PI * Frequency * at_ / 44100.0f);
    float sample2 = Base + Amplitude * sin(2.0 * PI * Frequency * (at_ + 1.0) / 44100.0f);
    vec4 new_s = vec4(
            sample1 * p2.x,
            sample1 * p2.y,
            sample2 * p2.z,
            sample2 * p2.w
        ) / 255.0;
    imageStore(storageImage, to_draw_at, new_s);
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

    PBR.PreCalcImages = function(inShader)
        return inShader.uSamplerCubeMap(20, "samplerCubeMap", 0)
            .uSampler2D(23, "samplerBRDFLUT", 0)
            .uSamplerCubeMap(24, "samplerIrradiance", 0)
            .uSamplerCubeMap(25, "prefilteredMap", 0)
    end

    Engine.GetAppropriateShader = function(inShaderType, incompilecontext, gltfmodel, materialindex, inskinning,
                                           intangent,
                                           inextraInfo)
        if intangent == nil then if gltfmodel then intangent = true else intangent = false end end
        if inShaderType == "NORMAL" and incompilecontext == Jkr.CompileContext.Default then
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
                .Append [[
            layout(set = 0, binding = 32) uniform sampler2DArray shadowMap;
            ]]

            if inextraInfo and inextraInfo.baseColorTexture == true then
                fshader.uSampler2D(3, "uBaseColorTexture")
            end

            PBR.PreCalcImages(fshader)

            fshader.GlslMainBegin()
            if fshader.gltfMaterialTextures.mBaseColorTexture == true then
                if fshader.gltfMaterialTextures.mEmissiveTextureIndex == true then
                    fshader.Append [[
                    outFragColor = texture(uBaseColorTexture, vUV) + texture(uEmissiveTexture, vUV);
                    ]]
                else
                    fshader.Append [[
                    vec4 color = Push.m2[0];
                    vec4 emission_color = Push.m2[1];
                    vec4 outC = texture(uBaseColorTexture, vUV);
                    outFragColor = vec4(outC.x * color.x, outC.y * color.y, outC.z * color.z, outC.w * color.w) + emission_color;
                    ]]
                end
            elseif inextraInfo and inextraInfo.baseColorTexture == true then
                fshader.Append [[
                    vec4 color = Push.m2[0];
                    vec4 emission_color = Push.m2[1];
                    vec4 outC = texture(uBaseColorTexture, vUV);
                    outFragColor = vec4(outC.x * color.x, outC.y * color.y, outC.z * color.z, outC.w * color.w) + emission_color;
                    ]]
            else
                fshader.Append [[
                vec4 color = Push.m2[0];
                vec4 emission_color = Push.m2[1];
                outFragColor = vec4(vColor.x * color.x, vColor.y * color.y, vColor.z * color.z, 1 * color.w) + emission_color;
            ]]
            end
            fshader.GlslMainEnd()
            return vshader, fshader
        end
        if inShaderType == "CONSTANT_COLOR" and incompilecontext == Jkr.CompileContext.Default then
            local vshader
            if intangent then
                vshader = Basics.GetBasicVertexHeaderWithTangent()
            else
                vshader = Basics.GetBasicVertexHeaderWithoutTangent()
            end
            vshader.GlslMainBegin()
            vshader.Append [[
                    gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1);
                    vWorldPos = vec3(Push.model * vec4(inPosition, 1));
                    mat3 mNormal = transpose(inverse(mat3(Push.model)));
                    vNormal = mNormal * normalize(inNormal.xyz);
                    vUV = inUV;
                    vColor = inColor;
                    ]]
            vshader.InvertY()
            vshader.GlslMainEnd()

            local fshader = Basics.GetConstantFragmentHeader()
            PBR.PreCalcImages(fshader)
            fshader.GlslMainBegin()
            fshader.Append [[
            vec4 param = Push.m2[0];
            outFragColor = param;
        ]]
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

        if inShaderType == "PBR" then
            local out = Engine.CreatePBRShaderByGLTFMaterial(gltfmodel, materialindex)
            return out.vShader, out.fShader
        end
        if inShaderType == "PBR_SHADOW" then
            local out = Engine.CreatePBRShaderByGLTFMaterial(gltfmodel, materialindex, true)
            return out.vShader, out.fShader
        end
        if inShaderType == "GENERAL_UNIFORM" then
            local vShader = Engine.Shader()
                .Header(450)
                .NewLine()
                .VLayout()
                .Push()
                .Ubo()
                .GlslMainBegin()
                .Append [[
                    gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(inPosition, 1);
            ]]
                .GlslMainEnd()

            local fShader = Engine.Shader()
                .Header(450)
                .NewLine()
                .Ubo()
                .Append [[
            layout(set = 0, binding = 32) uniform sampler2DArray shadowMap;
            ]]

            PBR.PreCalcImages(fShader)

            fShader.outFragColor()
                .GlslMainBegin()
                .Append
                [[
                outFragColor = vec4(0.0f);
            ]]
                .GlslMainEnd()
            return vShader, fShader
        end
        if inShaderType == "SKYBOX" then
            local vShader = Engine.Shader()
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
        ]])
                .NewLine()
                .GlslMainEnd()
                .NewLine()

            local fShader = Engine.Shader()
                .Header(450)
                .NewLine()
                .In(0, "vec3", "vVertUV")
                .outFragColor()
                .Ubo()

            PBR.PreCalcImages(fShader)
            fShader
                .Push()
                .Append [[
            layout(set = 0, binding = 32) uniform sampler2DArray shadowMap;
            ]]
                .GlslMainBegin()
                .Indent()
                .Append([[
            outFragColor = texture(prefilteredMap, vVertUV) * 0.5;
            outFragColor = texture(samplerIrradiance, vVertUV) * 0.5;
            outFragColor = texture(samplerCubeMap, vVertUV) * 1;
        ]])
                .GlslMainEnd()
                .NewLine()
            return vShader, fShader
        end

        if inShaderType == "CASCADED_SHADOW_DEPTH_PASS" then
            local vShader = Engine.Shader()
                .Header(450)
                .VLayout()
                .Define("SHADOW_MAP_CASCADE_COUNT", "4")
                .Push()
                .Ubo()
                .Out(0, "vec2", "outUV")
                .GlslMainBegin()
                .Append [[
                vec4 Index = Push.m2[0];
                outUV = inUV;
                vec3 pos = inPosition + vec3(Ubo.lights[int(Index.y)]);
                gl_Position = Ubo.shadowMatrixCascades[int(Index.x)] * vec4(pos, 1.0);
            ]]
                .GlslMainEnd()

            local fShader = Engine.Shader()
                .Header(450)
                .Ubo()
                .Push()
                .Append [[
            layout(set = 0, binding = 32) uniform sampler2DArray shadowMap;
            ]]

            PBR.PreCalcImages(fShader)
            fShader
                .outFragColor()
                .GlslMainBegin()
                .Append [[ outFragColor = vec4(1.0); ]]
                .GlslMainEnd()
            return vShader, fShader
        end

        if inShaderType == "CASCADE_SHADOWED" then
            local vShader = Engine.Shader()
                .Header(450)
                .VLayout()
                .Ubo()
                .Push()
                .Out(0, "vec3", "outNormal")
                .Out(1, "vec3", "outColor")
                .Out(2, "vec3", "outViewPos")
                .Out(3, "vec3", "outPos")
                .Out(4, "vec2", "outUV")
                .Append [[
            out gl_PerVertex {
                vec4 gl_Position;
            };
            ]]
                .GlslMainBegin()
                .Append [[
                vec4 Index = Push.m2[0];
                outColor = inColor;
                outNormal = inNormal;
                outUV = inUV;
                vec3 pos = inPosition + Ubo.lights[int(Index.x)].xyz;
                outPos = pos;
                outViewPos = (Ubo.view * vec4(pos.xyz, 1.0)).xyz;
                gl_Position = Ubo.proj * Ubo.view * Push.model * vec4(pos.xyz, 1.0);
            ]]
                .GlslMainEnd()

            local fShader = Engine.Shader()
                .Header(450)
                .Define("SHADOW_MAP_CASCADE_COUNT", "4")
                .Append [[
            layout(set = 0, binding = 1) uniform sampler2DArray shadowMap;
            ]]
                .In(0, "vec3", "inNormal")
                .In(1, "vec3", "inColor")
                .In(2, "vec3", "inViewPos")
                .In(3, "vec3", "inPos")
                .In(4, "vec2", "inUV")
                .outFragColor()
                .Define("ambient", "0.3")
                .Ubo()
                .Push()
                .Append [[
            const mat4 biasMat = mat4(
                0.5, 0.0, 0.0, 0.0,
                0.0, 0.5, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.5, 0.5, 0.0, 1.0
            );
            ]]
                .Append(shadow_textureProj)
                .Append(filterPCF)
                .GlslMainBegin()
                .Append [[
            vec4 cascadeSplits = Push.m2[1];
            int enablePCF = 1;
            uint cascadeIndex = 0;
            for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
            {
                if(inViewPos.z < cascadeSplits[i])
                {
                    cascadeIndex = i + 1;
                }
            }
            vec4 shadowCoord = (biasMat * Ubo.shadowMatrix[cascadeIndex]) * vec4 (inPos, 1.0);

            float shadow = 0;
            if (enablePCF == 1) {
                shadow = filterPCF(shadowCoord / shadowCoord.w, cascadeIndex);
            } else {
                shadow = textureProj(shadowCoord / shadowCoord.w, vec2(0.0), cascadeIndex);
            }

            vec3 N = normalize(inNormal);
            vec4 lightDir = -(Ubo.lightDirections[int(Index.x)]);
            vec3 L = normalize(vec3(lightDir.x, lightDir.y, lightDir.z));
            vec3 H = normalize(L + inViewPos);
            float diffuse = max(dot(N, L), ambient);
            vec3 lightColor = vec3(1.0);
            outFragColor.rgb = max(lightColor * (diffuse), vec3(0.0));
            outFragColor.rgba *= shadow;
            ]]
                .GlslMainEnd()
            return vShader, fShader
        end
    end


    PBR.Setup = function(w, world3d, buffer3d, inSkyboxModelId, inWorldUniformHandle, cacheprefix, hdr_path)
        Jkr.SetupPBR(Engine.i,
            w,
            inWorldUniformHandle,
            world3d,
            buffer3d, -- shape 3d
            math.floor(inSkyboxModelId),
            cacheprefix,
            PBR.GenBrdfLutV.str,
            PBR.GenBrdfLutF.str,
            PBR.FilterCubeV.str,
            PBR.IrradianceCubeF.str,
            PBR.FilterCubeV.str,
            PBR.PreFilterEnvMapF.str,
            PBR.EquirectangularMapToMultiVShader.str,
            PBR.EquirectangularMapToMultiFShader.str,
            hdr_path);
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
                            end
                            return msg
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
                        elseif type(inToSend) == "number" then
                            msg.mHeader.mId = 2
                            msg:InsertFloat(inToSend)
                        elseif type(inToSend) == "function" then
                            msg.mHeader.mId = 3
                            msg:InsertFunction(inToSend)
                        else
                            msg = inToSend
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
                        elseif type(inToSend) == "number" then
                            msg.mHeader.mId = 2
                            msg:InsertFloat(inToSend)
                        elseif type(inToSend) == "function" then
                            msg.mHeader.mId = 3
                            msg:InsertFunction(inToSend)
                        else
                            msg = inToSend
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
                            end
                            return msg
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
                    self.net.listenOnceUDP = function(inTypeExample)
                        if not Jkr.IsMessagesBufferEmptyUDP() then
                            local msg = Jkr.PopFrontMessagesBufferUDP()
                            return Jkr.ConvertFromVChar(inTypeExample, msg)
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

    Engine.CreatePBRShaderByGLTFMaterial = function(inGLTF, inMaterialIndex, inShadow)
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
            .Out(5, "vec3", "vViewPos")
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
              vec3 pos = inPosition + Ubo.lights[0].xyz;
              vViewPos = (Ubo.view * vec4(pos.xyz, 1.0)).xyz;
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
            .In(5, "vec3", "vViewPos")
            .uSamplerCubeMap(20, "samplerCubeMap", 0)
            .uSampler2D(23, "samplerBRDFLUT", 0)
            .uSamplerCubeMap(24, "samplerIrradiance", 0)
            .uSamplerCubeMap(25, "prefilteredMap", 0)
            .Append [[
        layout (set = 0, binding = 32) uniform sampler2DArray shadowMap;
        ]]

        fShader.Ubo()
            .outFragColor()
            .Push()

        if inShadow then
            fShader.Append [[
        const mat4 biasMat = mat4(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.5, 0.5, 0.0, 1.0
        );
        ]]
                .Shadow_textureProj()
                .filterPCF()
        end
        fShader
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

        if Material.mNormalTextureIndex ~= -1 then
            fShader
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
        else
            fShader.Append [[
vec3 calculateNormal()
{
    return vec3(0, 0, 1);
}
        ]]
        end


        fShader.GlslMainBegin()
        if Material.mNormalTextureIndex ~= -1 then
            fShader.Append [[
    vec3 N = calculateNormal();
    ]]
        else
            fShader.Append [[
    vec3 N = vNormal;
    ]]
        end

        fShader.Append [[
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

    vec3 Emissive = vec3(0);
        ]]
        end

        fShader.Append [[


    vec3 F0 = vec3(0.04);
    F0 = mix(F0, vec4(ALBEDO, 1.0).xyz, metallic);
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < Ubo.lights[i].length(); i++) {
        vec3 L = normalize(Ubo.lights[i].xyz - vWorldPos) * Ubo.lights[i].w;
        Lo += SpecularContribution(L, V, N, F0, metallic, roughness);
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

        if inShadow then
            fShader.Append [[
    int SHADOW_MAP_CASCADE_COUNT = 4;
    vec4 cascadeSplits = Push.m2[1];
    int enablePCF = 0;
    uint cascadeIndex = 0;
    for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        if(vViewPos.z < cascadeSplits[i])
        {
            cascadeIndex = i + 1;
        }
    }
    vec4 shadowCoord = (biasMat * Ubo.shadowMatrix[cascadeIndex]) * vec4 (vWorldPos + Ubo.lights[0].xyz, 1.0);

    float shadow = 0;
    if (enablePCF == 1) {
        shadow = filterPCF(shadowCoord / shadowCoord.w, cascadeIndex, length(ambient));
    } else {
        shadow = textureProj(shadowCoord / shadowCoord.w, vec2(0.0), cascadeIndex, length(ambient));
    }
    ambient *= shadow;
        ]]
        end

        fShader.Append [[

    vec4 p1 = Push.m2[0];
    vec4 p2 = Push.m2[1];
    vec4 p3 = Push.m2[2];
    vec4 p4 = Push.m2[3];

    vec3 color = ambient + Lo;
    color = Uncharted2Tonemap(color * p4.x); // Tone mapping exposure = 1.5
    color = color * (1.0f / Uncharted2Tonemap(vec3(p4.y)));	// norma 11.2
    color = pow(color, vec3(p4.z)); // Gamma correction gamma = 1/2.2

    outFragColor = vec4(color + Emissive, 1.0);
    ]]
        -- if inShadow then
        --     fShader.Append [[
        --    outFragColor = outFragColor.rgba * shadow;
        --    ]]
        -- end

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
            .Append [[
            layout(set = 0, binding = 32) uniform sampler2DArray shadowMap;
            ]]
    end


    Engine.AddObject = function(modObjects, modObjectsVector, inId, inAssociatedModel, inUniformIndex, inSimple3dIndex,
                                inGLTFHandle,
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
        modObjects[#modObjects + 1] = Object
        modObjectsVector:add(Object)
        return #modObjectsVector
    end

    Engine.AddAndConfigureGLTFToWorld = function(w,
                                                 inworld3d,
                                                 inshape3d,
                                                 ingltfmodelname,
                                                 inshadertype,
                                                 incompilecontext,
                                                 inskinning,
                                                 inhdrenvfilename)
        local shouldload = false
        if not incompilecontext then incompilecontext = Jkr.CompileContext.Default end
        local SkyboxObject
        if inshadertype == "PBR" or inshadertype == "PBR_SHADOW" then
            local globaluniformhandle = inworld3d:GetUniform3D(0)
            local Skybox = Jkr.Generator(Jkr.Shapes.Cube3D, vec3(1, 1, 1))
            local skyboxId = inshape3d:Add(Skybox, vec3(0, 0, 0))
            PBR.Setup(w, inworld3d, inshape3d, skyboxId, globaluniformhandle, "PBR_FREAK", inhdrenvfilename)
            SkyboxObject = Jkr.Object3D()
            SkyboxObject.mId = skyboxId
            SkyboxObject.mP2 = 1
            local skybox_shaderindex = inworld3d:AddSimple3D(Engine.i, w)
            local skybox_shader = inworld3d:GetSimple3D(skybox_shaderindex)
            local vshader, fshader = Engine.GetAppropriateShader("SKYBOX")
            skybox_shader:CompileEXT(
                Engine.i,
                w,
                "cache/constant_color.glsl",
                vshader.str,
                fshader.str,
                "",
                shouldload,
                incompilecontext
            )
            SkyboxObject.mAssociatedSimple3D = skybox_shaderindex
        end

        local gltfmodelindex = inworld3d:AddGLTFModel(ingltfmodelname)
        local gltfmodel = inworld3d:GetGLTFModel(gltfmodelindex)
        local shapeindex = inshape3d:Add(gltfmodel) -- this ACUTALLY loads the GLTF Model
        local Nodes = gltfmodel:GetNodesRef()
        Engine.GetGLTFInfo(gltfmodel)
        local Objects = {}
        local materials = {}

        for NodeIndex = 1, #Nodes, 1 do
            local primitives = Nodes[NodeIndex].mMesh.mPrimitives

            for PrimitiveIndex = 1, #primitives, 1 do
                local inprimitive = primitives[PrimitiveIndex]
                local materialindex = inprimitive.mMaterialIndex
                if not materials[materialindex] then
                    local uniform3dindex = inworld3d:AddUniform3D(Engine.i)
                    local uniform = inworld3d:GetUniform3D(uniform3dindex)
                    local shaderindex = inworld3d:AddSimple3D(Engine.i, w)
                    local shader = inworld3d:GetSimple3D(shaderindex)
                    local vshader, fshader = Engine.GetAppropriateShader(inshadertype, incompilecontext, gltfmodel,
                        materialindex,
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
                    materials[materialindex] = { shaderindex = shaderindex, uniformindex = uniform3dindex }
                end

                local object = Jkr.Object3D()
                object.mId = shapeindex;
                object.mAssociatedModel = gltfmodelindex;
                object.mAssociatedUniform = materials[materialindex].uniformindex;
                object.mAssociatedSimple3D = materials[materialindex].shaderindex;
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
                object.mP2 = 1 -- This indicates the object is the root object
                Objects[#Objects + 1] = object
            end
        end

        -- @warning You've to store this Objects {} table somewhere
        for i = 1, #Objects, 1 do
            for j = 1, #Objects, 1 do
                if i ~= j then
                    if gltfmodel:IsNodeParentOf(Nodes[Objects[i].mP1], Nodes[Objects[j].mP1]) then
                        Objects[i]:SetParent(Objects[j])
                    end
                end
            end
        end
        ---@note If there is a single object in the gltf file then it has to be the root object
        if #Objects == 1 then
            Objects[1].mP2 = 1
        end
        Objects[#Objects + 1] = SkyboxObject
        return Objects
    end

    Engine.CreateWorld3D = function(w, inshaper3d)
        local world3d = Jkr.World3D(inshaper3d)
        local camera3d = Jkr.Camera3D()
        camera3d:SetAttributes(vec3(0, 0, 0), vec3(-0.12, 1.14, -2.25))
        camera3d:SetPerspective(45.0 * 180.0 / math.pi, 16 / 9, 0.001, 10000)
        world3d:AddCamera(camera3d)
        local dummypipelineindex = world3d:AddSimple3D(Engine.i, w);
        local dummypipeline = world3d:GetSimple3D(dummypipelineindex)
        local light0 = {
            pos = vec4(-1, 5, 3, 40),
            dir = Jmath.Normalize(vec4(0, 0, 0, 0) - vec4(10, 10, -10, 1))
        }
        world3d:AddLight3D(light0.pos, light0.dir)

        local vshader, fshader = Engine.GetAppropriateShader("GENERAL_UNIFORM",
            Jkr.CompileContext.Default, nil, nil, false, false
        )

        dummypipeline:CompileEXT(
            Engine.i,
            w,
            "cache/dummyshader.glsl",
            vshader.str,
            fshader.str,
            "",
            MAIN_JKR_FILE,
            Jkr.CompileContext.Default
        )

        local globaluniformindex = world3d:AddUniform3D(Engine.i)
        local globaluniformhandle = world3d:GetUniform3D(globaluniformindex)
        globaluniformhandle:Build(dummypipeline)
        world3d:AddWorldInfoToUniform3D(globaluniformindex)
        Jkr.RegisterShadowPassImageToUniform3D(w, globaluniformhandle, 32)
        return world3d, camera3d
    end

    -- Define ANSI color codes for different log levels
    local colors = {
        INFO = "\27[34m",    -- Blue
        WARNING = "\27[33m", -- Yellow
        ERROR = "\27[31m",   -- Red
        DEBUG = "\27[36m",   -- Cyan
        RESET = "\27[0m"     -- Reset color
    }

    Engine.log = function(msg, type)
        type = type or "INFO"
        local color = colors[type] or colors.INFO -- Default to blue if type is unknown

        local log = string.format("%s[JKR %s]: %s%s\n", color, type, msg, colors.RESET)
        io.write(log)
    end


    Engine.GetResource = function(inFileName)
        return MAIN_JKR_FILE:Read(inFileName, std_vector_char(0))
    end

    Engine.GameFramework = function(inf)
        local f = inf or {}
        f.GetResource = Engine.GetResource
        Engine:Load(f.validation)
        Jkr.GetLayoutsAsVH()
        f.els = {}                       -- 2D elements
        f.run = true
        f.fd  = f.fd or vec2(800, 800)   -- frame dimenstion offscreen
        f.wd  = f.wd or vec2(800, 800)   -- initialwindow dimension
        f.bc  = f.bc or vec4(1, 0, 0, 1) -- background color
        f.tc  = f.tc or 3                -- thread count
        f.bd  = f.bd or 50               -- base depth, from camera
        f.nfs = f.nfs or 16              -- normal fontsize
        f.lfs = f.lfs or 24              -- normal fontsize
        f.sfs = f.sfs or 8               -- normal fontsize
        f.e   = Engine.e
        f.i   = Engine.i
        f.w   = Jkr.CreateWindow(Engine.i, inf.heading or "Game Framework", f.wd, f.tc, f.fd)
        f.w:BuildShadowPass()
        Engine.log("Built Shadow Pass")
        f.wr = Jkr.CreateGeneralWidgetsRenderer(nil, Engine.i, f.w, f.e)
        Engine.log("Created Widgets Renderer")
        f.nf = f.wr.CreateFont(f.GetResource("res/font.ttf"), f.nfs)
        f.lf = f.wr.CreateFont(f.GetResource("res/font.ttf"), f.lfs)
        f.sf = f.wr.CreateFont(f.GetResource("res/font.ttf"), f.sfs)
        Engine.log(string.format("Created Fonts [%s] -> %d, %d, %d", "res/font.ttf", f.nfs, f.lfs, f.sfs))

        if not f.validation then
            Engine.log("Validation Disabled", "WARNING")
        end
        -- ThreeD
        f.sha        = Jkr.CreateShapeRenderer3D(f.i, f.w)
        f.wor, f.cam = Engine.CreateWorld3D(f.w, f.sha)

        if f.initialize_painters then
            if not f.painters then
                f.painters = {}
                f.painters.line = Jkr.CreateCustomImagePainter("cache/LINE2D.glsl", TwoDimensionalIPs.Line.str)
                f.painters.clear = Jkr.CreateCustomImagePainter("cache/CLEAR2D.glsl", TwoDimensionalIPs.Clear.str)
                f.painters.sound = Jkr.CreateCustomImagePainter("cache/SOUND.glsl", TwoDimensionalIPs.Sound.str)
                f.painters.line:Store(MAIN_JKR_FILE, f.i, f.w)
                f.painters.clear:Store(MAIN_JKR_FILE, f.i, f.w)
                f.painters.sound:Store(MAIN_JKR_FILE, f.i, f.w)
                Engine.log("Painters Initialized Successfully", "INFO")
            end
        end
        Engine.e:SetEventCallBack(
            function()
                f.wr:Event()
            end
        )
        local currentTime  = f.w:GetWindowCurrentTime()
        local residualTime = 0
        f.stepTime         = f.stepTime or 0.001
        f.loop             = function()
            local e = f.e
            local w = f.w
            local bc = f.bc
            local Update = f.Update
            local Dispatch = f.Dispatch
            local Draw = f.Draw
            local stepTime = f.stepTime
            while (not e:ShouldQuit()) and f.run do
                local deltaTime = (w:GetWindowCurrentTime() - currentTime) / 1000
                if deltaTime > stepTime then
                    Update()
                    residualTime = deltaTime
                    currentTime = w:GetWindowCurrentTime()
                else
                    residualTime = residualTime / 1000 + stepTime - deltaTime
                end
                e:ProcessEventsEXT(w)
                w:Wait()
                w:AcquireImage()
                w:BeginRecording()
                Dispatch()
                w:BeginUIs()
                Draw()
                w:EndUIs()

                w:BeginDraws(bc.x, bc.y, bc.z, bc.w, 1)
                w:ExecuteUIs()
                w:EndDraws()

                w:BlitImage()
                w:EndRecording()

                w:Present()
            end
        end
        f.PC               = function(a, b)
            local o = Jkr.Matrix2CustomImagePainterPushConstant()
            o.a = a or Jmath.GetIdentityMatrix4x4()
            o.b = b or Jmath.GetIdentityMatrix4x4()
            return o
        end
        f.I                = function(inPosition_3f, inDimension_3f)
            local o = {}
            local p = f.PC()
            p.a = mat4(vec4(0), vec4(1), vec4(0.3), vec4(0.0))
            o[1] = f.wr.CreateComputeImage(vec3(0), inDimension_3f)
            o[2] = f.wr.CreateSampledImage(vec3(0), inDimension_3f)
            o[3] = f.wr.CreateQuad(inPosition_3f, inDimension_3f, p, "showImage", o[2].mId)
            o[1].RegisterPainter(f.painters.line)

            o.Bind = function(inPainter)
                inPainter:Bind(f.w, Jkr.CmdParam.None)
                inPainter:BindImageFromImage(f.w, o[1], Jkr.CmdParam.None)
            end
            o.Update = function(inPosition_3f, inDimension_3f)
                o[3]:Update(inPosition_3f, inDimension_3f)
            end
            o.Draw = function(inPainter, inPC)
                inPainter:Draw(f.w, inPC, math.ceil(inDimension_3f.x / 16.0), math.ceil(inDimension_3f.y / 16.0), 1,
                    Jkr.CmdParam.None)
            end
            o.DrawDebugLines = function(inv_t)
                f.painters.line:Bind(f.w, Jkr.CmdParam.None)
                f.painters.line:BindImageFromImage(f.w, o[1], Jkr.CmdParam.None)
                inv_t = inv_t or 4
                local line1 = vec4(0, 0, inDimension_3f.x, 0)
                local line2 = vec4(0, 0, 0, inDimension_3f.y)
                local line3 = vec4(inDimension_3f.x, 0, inDimension_3f.x, inDimension_3f.y)
                local line4 = vec4(0, inDimension_3f.y, inDimension_3f.x, inDimension_3f.y)
                local color = vec4(1, 0, 0, 1)
                local thickness = vec4(inv_t, 0, 0, 0)
                o.Draw(f.painters.line, f.PC(mat4(line1, color, thickness, vec4(0))))
                o.Draw(f.painters.line, f.PC(mat4(line3, color, thickness, vec4(0))))
                o.Draw(f.painters.line, f.PC(mat4(line4, color, thickness, vec4(0))))
            end
            o.Copy = function()
                o[1].CopyToSampled(o[2])
                o[1].handle:SyncAfter(f.w, Jkr.CmdParam.None)
            end
            o.GetVector = function()
                return o[1].handle:GetVector(f.i)
            end
            return o
        end
        f.B                = function(v)
            if not v.e then
                v.p = v.p or vec3(0, 0, f.bd)
                v.onclick = v.onclick or function() end
                v.c = v.c or vec4(vec3(0), 1)
                v.bc = v.bc or vec4(0.9, 0.8, 0.95, 0.6)
                v.f = v.f or f.nf
                local dim = v.f:GetTextDimension(v.t or " ")
                v.d = v.d or vec3(dim.x + 5, dim.y + 5, 1)
                local Value = f.wr.CreateGeneralButton(
                    v.p,
                    v.d,
                    v.onclick,
                    v.continous,
                    v.f,
                    v.t,
                    v.c,
                    v.bc,
                    v.pc,
                    v.img,
                    v.imgp,
                    v.matrix
                )
                Value:Update(v.p, v.d)
                if v.en then
                    f.els[v.en] = Value
                end
                v = {}
                v = Value
            else
                v.Update = function(self, _, _) end
            end
            return v
        end

        return f
    end


    local glerp, glerp_2f, glerp_3f, glerp_4f, glerp_mat4f = Jmath.GetLerps()

    Jkr.CreateCustomAnimation = function(inCallBuffer, inValue1, inValue2, funcable, inFrame, inInverseSpeed)
        local InverseSpeed = 0.01
        local t = 0
        if inInverseSpeed then
            InverseSpeed = inInverseSpeed
        end
        local Frame = inFrame or 1
        if type(inValue1) == "userdata" then
            while t <= 1 do
                local mat = glerp_mat4f(inValue1, inValue2)
                inCallBuffer:PushOneTime(function()
                    funcable(mat)
                end, Frame)
                t = t + InverseSpeed
                Frame = Frame + 1
            end
        elseif type(inValue1) == "table" then
            while t <= 1 do
                local value = {}
                for i = 1, #inValue1 do
                    value[#value + 1] = glerp_mat4f(inValue1[i], inValue2[i])
                end
                inCallBuffer:PushOneTime(function()
                    funcable(value)
                end, Frame)
                t = t + InverseSpeed
                Frame = Frame + 1
            end
        end
        return Frame
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
            local current_pos   = glerp_3f(from_pos, to_pos, t)
            local current_dimen = glerp_3f(from_dimen, to_dimen, t)
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

    Jkr.CreateUpdatable    = function(inFunction)
        local o = {}
        if type(inFunction) == "function" then
            o.mUpdate = inFunction
        else
            Engine.log("Jkr.CreateUpdatable: Pass a function", "ERROR")
        end
        return o
    end

    Jkr.CreateEventable    = function(inFunction)
        local o = {}
        if type(inFunction) == "function" then
            o.mEvent = inFunction
        else
            Engine.log("Jkr.CreateEventable: Pass a function", "ERROR")
        end
        return o
    end

    Jkr.CreateDrawable     = function(inId, inBatchable, inDrawType, inImageId, inColor_4f, inPush, inMatrix)
        local o = {}
        o.mBatchable = inBatchable
        o.mImageId = inImageId
        o.mPush = inPush
        if inColor_4f then
            o.mColor = inColor_4f
        else
            o.mColor = vec4(1, 1, 1, 1)
        end
        o.mId = inId
        o.mImageId = inImageId
        o.mDrawType = inDrawType -- LINE, SHAPE, TEXT
        o.mMatrix = inMatrix
        return o
    end

    Jkr.CreateDispatchable = function(inFunction)
        local o = {}
        if type(inFunction) == "function" then
            o.mDispatch = inFunction
        else
            Engine.log("Jkr.CreateDispatchable: Pass a function", "ERROR")
        end
        return o
    end

    Jkr.CreateCallBuffers  = function() -- Similar to Comptable in JrkGUI v1
        local o = {}
        o.mDrawables = {}
        o.mDispatchables = {}
        o.mUpdatables = {}
        o.mEventables = {}
        o.mOneTimeDrawables = {}
        o.mOneTimeDispatchables = {}
        o.mOneTimeUpdatables = {}

        local sv_index_count = 1 ---@note Current Scissor OR Viewport
        o.mSVs = {}
        o.mDrawablesInSVs = {}
        o.mDrawablesInSVs[1] = {}
        o.mSVs[1] = {}

        o.PushDraw = function(self, inCall, inSVid)
            local itd = self.mDrawables
            if inCall.mDrawType == "SCISSOR_VIEWPORT" then
                sv_index_count = sv_index_count + 1
                local sv_elements = {}
                self.mDrawablesInSVs[sv_index_count] = sv_elements
                self.mSVs[sv_index_count] = inCall
                return sv_index_count
            else
                if not itd[inCall.mDrawType] then
                    itd[inCall.mDrawType] = {}
                end

                itd[inCall.mDrawType][#itd[inCall.mDrawType] + 1] = inCall
                self.mDrawables[#self.mDrawables + 1] = inCall

                if not self.mDrawablesInSVs[inSVid][inCall.mDrawType] then
                    self.mDrawablesInSVs[inSVid][inCall.mDrawType] = {}
                end

                local l = self.mDrawablesInSVs[inSVid][inCall.mDrawType]
                l[#l + 1] = inCall

                return #self.mDrawables
            end
        end

        ---@note this inSVid is only needed for draw parameters
        o.Push = function(self, inCall, inSVId)
            if inCall.mDrawType then
                return o:PushDraw(inCall, inSVId)
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
                if not self.mOneTimeDrawables[inFrame] then
                    self.mOneTimeDrawables[inFrame] = {}
                end
                self.mOneTimeDrawables[inFrame][#self.mOneTimeDrawables[inFrame] + 1] = inCall
            elseif inCall.mUpdate then
                if not self.mOneTimeUpdatables[inFrame] then
                    self.mOneTimeUpdatables[inFrame] = {}
                end
                self.mOneTimeUpdatables[inFrame][#self.mOneTimeUpdatables[inFrame] + 1] = inCall
            elseif inCall.mDispatch then
                if not self.mOneTimeDispatchables[inFrame] then
                    self.mOneTimeDispatchables[inFrame] = {}
                end
                self.mOneTimeDispatchables[inFrame][#self.mOneTimeDispatchables[inFrame] + 1] = inCall
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
            print("HERE")
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

    Jkr.PC                 = function(a, b)
        local o = Jkr.Matrix2CustomImagePainterPushConstant()
        o.a = a or Jmath.GetIdentityMatrix4x4()
        o.b = b or Jmath.GetIdentityMatrix4x4()
        return o
    end
    Jkr.HLayout            = {
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

    Jkr.VLayout            = {
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

    Jkr.StackLayout        = {
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
        o.s = Jkr.CreateShapeRenderer(o.i, o.w, Jkr.GetDefaultCache(o.i, "Shape"))
        o.st = Jkr.CreateShapeRenderer(o.i, o.w, Jkr.GetDefaultCache(o.i, "Shape"))
        o.t = Jkr.CreateTextRendererBestTextAlt(o.i, o.st)

        o.c = Jkr.CreateCallBuffers()
        o.e = Jkr.CreateCallExecutor(o.c)
        o.WindowDimension = o.w:GetWindowDimension()

        o.shape2dShaders = {}
        o.shape2dShaders.roundedRectangle = Jkr.Simple3D(i, w)
        o.shape2dShaders.roundedRectangle:Compile(
            i,
            w,
            "cache2/o.shape2dShaders.roundedRectangle.glsl",
            Shape2DShaders.GeneralVShader.str,
            Shape2DShaders.RoundedRectangleFShader.str,
            "",
            MAIN_JKR_FILE
        )
        o.shape2dShaders.showImage = Jkr.Simple3D(i, w)
        o.shape2dShaders.showImage:Compile(
            i,
            w,
            "cache2/o.shape2dShaders.showImage.glsl",
            Shape2DShaders.GeneralVShader.str,
            Shape2DShaders.ShowImageFShader.str,
            "",
            MAIN_JKR_FILE
        )

        o.CreateFont = function(inFontFileName, inFontSize)
            local font = {}
            font.mId = o.t:AddFontFace(inFontFileName, inFontSize)
            font.GetTextDimension = function(self, inText)
                return o.t:GetTextDimensions(inText, font.mId)
            end
            return font
        end

        ---@note Scissor are viewport both are created unifiedly i.e. both should always be in the same dimension

        o.scissor_matrices = {}
        o.CreateScissor = function(inPosition_3f, inDimension_3f, inShouldSetViewport, inMatrix)
            ---@warning mImageId -> inPosition_3f
            ---@warning mColor -> inDimension_3f
            ---@warning mPush -> inShouldSetViewport
            local sci = o.c:Push(Jkr.CreateDrawable("START", false, "SCISSOR_VIEWPORT", inPosition_3f, inDimension_3f,
                inShouldSetViewport))
            o.scissor_matrices[sci] = inMatrix or Jmath.GetIdentityMatrix4x4()
            return sci
        end

        o.mCurrentScissor = 1

        o.SetCurrentScissor = function(inScissorId)
            if not inScissorId then
                o.mCurrentScissor = 1
            else
                o.mCurrentScissor = inScissorId
            end
        end

        o.UpdateScissor = function(inScissorId, inPosition_3f, inDimension_3f, inShouldSetViewport)
            o.c.mSVs[inScissorId].mImageId = inPosition_3f
            o.c.mSVs[inScissorId].mColor = inDimension_3f
            o.c.mSVs[inScissorId].mPush = inShouldSetViewport
        end


        -- @warning inShape2DShader refers to the STRING value of o.shape2dShaders.<shader>
        -- e.g. for rounded rectangle use "roundedRectangle"
        -- @warning the second matrix in the push constant cannot be used for anything because it will be sent with the UIMatrix
        o.CreateQuad = function(inPosition_3f, inDimension_3f, inPushConstant, inShape2DShader, inSampledImageId,
                                inMatrix)
            local quad = {}
            local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill, uvec2(inDimension_3f.x, inDimension_3f.y))
            quad.rect = o.s:Add(Rectangle, inPosition_3f)
            quad.mColor = vec4(1, 1, 1, 1)

            -- @warning This might not be batchable @todo create a parameter or do something else about it
            quad.DrawId = o.c:Push(Jkr.CreateDrawable(quad.rect, true,
                inShape2DShader,
                inSampledImageId,
                nil, inPushConstant, inMatrix), o.mCurrentScissor)

            quad.Update = function(self, inPosition_3f, inDimension_3f, inPushConstant, inMatrix)
                local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill,
                    uvec2(inDimension_3f.x, inDimension_3f.y))
                o.s:Update(quad.rect, Rectangle, inPosition_3f)
                if inPushConstant then
                    o.c.mDrawables[self.DrawId].mPush = inPushConstant
                end
                if inMatrix then
                    o.c.mDrawables[self.DrawId].mMatrix = inMatrix
                end
            end

            return quad
        end

        --[============================================================[
                    TEXT LABEL
          ]============================================================]
        o.CreateTextLabel = function(inPosition_3f, inDimension_3f, inFont, inText, inColor, inNotDraw, inMatrix)
            local textLabel = {}
            textLabel.mText = inText
            textLabel.mFont = inFont
            textLabel.mId = o.t:Add(inFont.mId, inPosition_3f, inText)
            if not inNotDraw then
                textLabel.PushId = o.c:Push(Jkr.CreateDrawable(textLabel.mId, nil, "TEXT", nil, inColor, nil, inMatrix),
                    o.mCurrentScissor)
            end
            textLabel.Update = function(self, inPosition_3f, inDimension_3f, inFont, inText, inColor, inMatrix)
                --tracy.ZoneBeginN("luatextUpdate")
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
                if inMatrix then
                    o.c.mDrawables[self.PushId].mMatrix = inMatrix
                end
                --tracy.ZoneEnd()
            end

            textLabel.Remove = function(self)
                -- TODO Implement Remove function
            end
            return textLabel
        end

        ---@todo remove inNoDraw paramter
        o.CreateSampledImage = function(inPosition_3f, inDimension_3f, inFileName, inNoDraw, inColor)
            local SampledImage = {}
            if (inFileName) then
                SampledImage.mId = o.s:AddImageByFileName(inFileName)
                SampledImage.mActualSize = o.s:GetImageSize(inFileName)
            else
                SampledImage.mId = o.s:AddImage(inDimension_3f.x, inDimension_3f.y)
                SampledImage.mActualSize = vec2(inDimension_3f.x, inDimension_3f.y)
            end
            -- local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill, uvec2(inDimension_3f.x, inDimension_3f.y))
            -- SampledImage.imageViewRect = o.s:Add(Rectangle, inPosition_3f)
            -- SampledImage.mColor = vec4(1, 1, 1, 1)

            if (inColor) then
                SampledImage.mColor = inColor
            end

            ---@note @warning Sampled IMage is not for drawing
            -- if (not inNoDraw) then
            --     -- SampledImage.DrawId = o.c:Push(Jkr.CreateDrawable(SampledImage.imageViewRect, false,
            --     --     "IMAGE",
            --     --     SampledImage.mId,
            --     --     SampledImage.mColor), o.mCurrentScissor)
            -- end

            -- SampledImage.Update = function(self, inPosition_3f, inDimension_3f, inColor)
            --     -- local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill,
            --     --     uvec2(inDimension_3f.x, inDimension_3f.y))
            --     -- o.s:Update(SampledImage.imageViewRect, Rectangle, inPosition_3f)
            --     -- if inColor then
            --     --     o.c.mDrawables[self.DrawId].mColor = inColor
            --     -- end
            -- end

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
            ComputeImage.CopyFromWindowTargetImage = function(inw)
                Jkr.CopyWindowRenderTargetImageToCustomPainterImage(o.w, inw, ComputeImage.handle)
            end
            return ComputeImage
        end

        o.CreateButton = function(inPosition_3f, inDimension_3f, inOnClickFunction, inContinous)
            local Button = {}
            Button.mDepthValue = math.int(inPosition_3f.z)
            Button.mId = e:SetBoundedRect(vec2(inPosition_3f.x, inPosition_3f.y),
                vec2(inDimension_3f.x, inDimension_3f.y), math.int(inPosition_3f.z))
            Button.mOnClickFunction = function() end
            Button.mOnHoverFunction = function() end
            if (inOnClickFunction) then
                Button.mOnClickFunction = inOnClickFunction
                if inContinous then
                    Button.mPushId = o.c:Push(Jkr.CreateUpdatable(
                        function()
                            local over = e:IsMouseWithinAtTopOfStack(
                                Button.mId,
                                Button.mDepthValue
                            )
                            if e:IsLeftButtonPressedContinous() and over then
                                Button.mOnClickFunction()
                            end
                        end
                    ))
                else
                    Button.mPushId = o.c:Push(Jkr.CreateEventable(
                        function()
                            local over = e:IsMouseWithinAtTopOfStack(
                                Button.mId,
                                Button.mDepthValue
                            )
                            if e:IsLeftButtonPressed() and over then
                                Button.mOnClickFunction()
                            end
                        end
                    ))
                end
            end
            Button.Update = function(self, inPosition_3f, inDimension_3f)
                Button.mDepthValue = math.int(inPosition_3f.z)
                e:UpdateBoundedRect(Button.mId, vec2(inPosition_3f.x, inPosition_3f.y),
                    vec2(inDimension_3f.x, inDimension_3f.y), Button.mDepthValue)
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

        o.SetUIMatrix = function(inMatrix)
            o.UIMatrix = inMatrix
        end

        o.Update = function(self)
            self.c:Update()
        end

        local w = o.w
        local s = o.s
        local st = o.st
        local t = o.t
        local cmdparam = Jkr.CmdParam.UI
        local image_filltype = Jkr.FillType.Image
        local fill_filltype = Jkr.FillType.Fill
        local ui_matrix = o.UIMatrix
        local s2ds = o.shape2dShaders
        local identity_matrix = Jmath.GetIdentityMatrix4x4()
        local DrawAll = function(inMap, inindex)
            local scissor_matrix = o.scissor_matrices[inindex]
            -- tracy.ZoneBeginN("luamainDrawALL")
            s:BindShapes(w, cmdparam)
            for key, value in pairs(inMap) do
                if key ~= "TEXT" and key ~= "IMAGE" and key ~= "SCISSOR_VIEWPORT" then
                    local shader = s2ds[key]
                    shader:Bind(w, cmdparam)
                    local drawables = value
                    local drawables_count = #value
                    for i = 1, drawables_count, 1 do
                        local drawable = drawables[i]
                        if drawable.mImageId then
                            s:BindFillMode(image_filltype, w, cmdparam)
                            s:BindImage(w, drawable.mImageId, cmdparam)
                        else
                            s:BindFillMode(fill_filltype, w, cmdparam)
                        end
                        drawable.mPush.b =
                            (drawable.mMatrix or ui_matrix) *
                            (scissor_matrix or identity_matrix)
                        Jkr.DrawShape2DWithSimple3D(w, shader, s.handle, drawable.mPush, drawable.mId, drawable
                            .mId,
                            cmdparam)
                    end
                end
            end


            st:BindFillMode(image_filltype, w, cmdparam)
            st:BindShapes(w, cmdparam)
            do
                local drawables = inMap["TEXT"]
                if drawables then
                    local drawables_count = #drawables
                    for i = 1, drawables_count, 1 do
                        local drawable = drawables[i]
                        t:Draw(drawable.mId, w, drawable.mColor,
                            (drawable.mMatrix or ui_matrix) *
                            (scissor_matrix or identity_matrix),
                            cmdparam)
                    end
                end
            end
            -- tracy.ZoneEnd()
        end

        o.Draw = function(self)
            --tracy.ZoneBeginN("luamainDraw")
            local SVs = o.c.mSVs
            local DrawablesInSVs = o.c.mDrawablesInSVs
            local count = #SVs
            ---@note The First one is always without Scissors
            DrawAll(DrawablesInSVs[1], 1)
            for i = 2, count, 1 do
                local sv = SVs[i]

                ---@note SetScissor + Viewport
                if sv.mPush then
                    o.w:SetViewport(sv.mImageId, sv.mColor, cmdparam)
                end
                o.w:SetScissor(sv.mImageId, sv.mColor, cmdparam)

                DrawAll(DrawablesInSVs[i], i)

                ---@note ResetScissor + Viewport
                o.w:SetDefaultViewport(cmdparam)
                o.w:SetDefaultScissor(cmdparam)
            end
            --tracy.ZoneEnd()
        end

        o.DrawExplicit = function(self, inScissorIds)
            local SVs = o.c.mSVs
            local DrawablesInSVs = o.c.mDrawablesInSVs
            local count = #inScissorIds
            for i = 1, count, 1 do
                local j = inScissorIds[i]
                local sv = SVs[j]

                ---@note SetScissor + Viewport
                if sv.mPush then
                    o.w:SetViewport(sv.mImageId, sv.mColor, cmdparam)
                end
                o.w:SetScissor(sv.mImageId, sv.mColor, cmdparam)

                DrawAll(DrawablesInSVs[j], j)

                ---@note ResetScissor + Viewport
                o.w:SetDefaultViewport(cmdparam)
                o.w:SetDefaultScissor(cmdparam)
            end
        end

        o.Dispatch = function(self)
            s:Dispatch(w, Jkr.CmdParam.None)
            st:Dispatch(w, Jkr.CmdParam.None)
            self.c:Dispatch()
        end

        o.Event = function(self)
            self.c:Event()
        end
        return o
    end


    local CompileShaders
    function Jkr.GetLayoutsAsVH()
        function V(inComponents, inComponentsRatio)
            if not inComponents and not inComponentsRatio then
                return Jkr.VLayout:New()
            else
                local v = Jkr.VLayout:New()
                v:Add(inComponents, inComponentsRatio)
                return v
            end
        end

        function H(inComponents, inComponentsRatio)
            if not inComponents and not inComponentsRatio then
                return Jkr.HLayout:New()
            else
                local h = Jkr.HLayout:New()
                h:Add(inComponents, inComponentsRatio)
                return h
            end
        end

        -- function U(inValue)
        --     if not inValue then inValue = {} end
        --     inValue.Update = function(self, inPosition_3f, inDimension_3f)
        --         inValue.d = vec3(inDimension_3f)
        --         inValue.p = vec3(inPosition_3f)
        --     end
        --     return inValue
        -- end
    end

    Jkr.CreateGeneralWidgetsRenderer = function(inWidgetRenderer, i, w, e)
        local o = {}
        if not inWidgetRenderer then
            o = Jkr.CreateWidgetRenderer(i, w, e)
        end

        o.shaders = {}
        CompileShaders(o.shaders)

        o.prebuilts = {}
        local op = o.prebuilts
        op.roundedRectanglePainter =
            Jkr.CreateCustomImagePainter(
                "cache/op.roundedRectangle.glsl",
                o.shaders.roundedRectangle
            )
        op.roundedRectanglePainter:Store(MAIN_JKR_FILE, i, w)
        op.roundedRectangle = o.CreateComputeImage(vec3(0), vec3(500, 500, 1))
        op.roundedRectangle.RegisterPainter(op.roundedRectanglePainter)

        o.c:PushOneTime(Jkr.CreateDispatchable(function()
            op.roundedRectangle.BindPainter(op.roundedRectanglePainter)
            PushConstant = Jkr.Matrix2CustomImagePainterPushConstant()
            PushConstant.a = mat4(
                vec4(0.0, 0.0, 0.85, 0.85),
                vec4(1),
                vec4(0.1, 0.5, 0.5, 0.0),
                vec4(0)
            )
            op.roundedRectangle.DrawPainter(op.roundedRectanglePainter, PushConstant, 50,
                50, 1)
        end), 1)
        op.sampledImage = o.CreateSampledImage(vec3(math.huge), vec3(500, 500, 1), nil, true,
            vec4(1))
        o.c:PushOneTime(Jkr.CreateDispatchable(function()
            op.roundedRectangle.CopyToSampled(op.sampledImage)
        end), 1)


        o.CreateGeneralButton = function(inPosition_3f,
                                         inDimension_3f,
                                         inOnClickFunction,
                                         inContinous,
                                         inFont,
                                         inText,
                                         inColor,
                                         inBackgroundColor,
                                         inPushConstantForImagePainter, ---@todo Remove this
                                         inImageFilePath,
                                         inImagePainter,
                                         inMatrix)
            local button = {}
            local push = Jkr.Matrix2CustomImagePainterPushConstant()
            button.mColor = inBackgroundColor or vec4(1)
            push.a = mat4(vec4(0.0), button.mColor, vec4(0.3), vec4(0.0))

            if not inImageFilePath then
                button.quad = o.CreateQuad(inPosition_3f, inDimension_3f, push, "showImage", op.sampledImage.mId,
                    inMatrix)
            else
                button.sampledImage = o.CreateSampledImage(vec3(math.huge), vec3(100, 100, 1), inImageFilePath, true,
                    vec4(1))
                button.quad = o.CreateQuad(inPosition_3f, inDimension_3f, push, "showImage", button.sampledImage.mId)
            end

            if inDimension_3f.x == 0 or inDimension_3f.y == 0 then
                button.shouldUpdateByDimension = vec3(inDimension_3f.x, inDimension_3f.y, inDimension_3f.z)
                inDimension_3f = vec3(100, 100, 1)
            end

            if type(inOnClickFunction) == "function" then
                button.parent = o.CreateButton(inPosition_3f, inDimension_3f, inOnClickFunction, inContinous)
                setmetatable(button, button.parent)
                button.__index = button.parent
            end

            if not inImagePainter then
                inImagePainter = o.prebuilts.roundedRectanglePainter
            end

            if not inImageFilePath and inFont and inText then
                button.sampledText = o.CreateTextLabel(inPosition_3f, inDimension_3f, inFont, inText, inColor)
            end

            button.padding = 10
            button.UpdateBackgroundColor = function(self, inBackgroundColor)
                local push = Jkr.Matrix2CustomImagePainterPushConstant()
                push.a = mat4(vec4(0.0), inBackgroundColor or button.mColor, vec4(0.0), vec4(0))
                button.quad:Update(button.mPosition_3f, button.mDimension_3f, push);
                button.mColor = inBackgroundColor
            end
            ---@warning Bad Design, Fix in Refactoring
            button.Update = function(self,
                                     inPosition_3f,
                                     inDimension_3f,
                                     inFont,
                                     inText,
                                     inColor,
                                     inBackgroundColor,
                                     inTextOreintation, inMatrix)
                button.mPosition_3f = inPosition_3f
                button.mDimension_3f = inDimension_3f
                local push = Jkr.Matrix2CustomImagePainterPushConstant()
                push.a = mat4(vec4(0.0), inBackgroundColor or button.mColor, vec4(0.0), vec4(0))
                button.quad:Update(inPosition_3f, inDimension_3f, push, inMatrix);
                if button.parent then
                    button.parent:Update(inPosition_3f, inDimension_3f)
                end

                if not inImageFilePath and button.sampledText then
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
                    button.sampledText:Update(inPosition_3f + DelDim, inDimension_3f, inFont, substr, inColor, inMatrix)

                    if inTextOreintation then
                        if inTextOreintation == "LEFT" then
                            DelDim.x = button.padding
                            button.sampledText:Update(inPosition_3f + DelDim, inDimension_3f, inFont, substr, inColor,
                                inMatrix)
                        end
                        if inTextOreintation == "RIGHT" then
                            DelDim.x = inPosition_3f.x - fontDim.x - button.padding
                            DelDim.x = inPosition_3f.x
                            button.sampledText:Update(inPosition_3f + DelDim, inDimension_3f, inFont, substr, inColor,
                                inMatrix)
                        end
                    end
                end

                if inText and button.sampledText then
                    button.sampledText.mText = Copy(inText)
                end
            end

            if button.shouldUpdateByDimension then
                button:Update(inPosition_3f, button.shouldUpdateByDimension)
            end

            return button
        end

        o.CreateSlider = function(inPosition_3f, inDimension_3f, inRangeStart, inRangeEnd, inDefaultValue, inStep)
            local slider = {}
            local z_difference = 10
            local knob = o.CreateGeneralButton(vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z - z_difference),
                inDimension_3f)
            local background = o.CreateGeneralButton(inPosition_3f, inDimension_3f)
            local knob_button = o.CreateButton(vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z - z_difference),
                inDimension_3f)
            slider.range_start = inRangeStart
            slider.range_end = inRangeEnd
            slider.current_value = inDefaultValue
            slider.knob_width = 0.1 * inDimension_3f.x
            slider.knob_height_offset = 5
            slider.knob_color = vec4(0.5)
            slider.step = inStep



            local current_pos, current_dim, knob_pos, knob_dim,
            background_pos, background_dim, normalized_pos

            slider.Update = function(self, inPosition_3f, inDimension_3f, inValue)
                current_pos, current_dim = inPosition_3f, inDimension_3f
                if not inValue then inValue = slider.current_value else slider.current_value = inValue end
                normalized_pos = (slider.current_value - slider.range_start) / (slider.range_end - slider.range_start)
                knob_pos = vec3(
                    -slider.knob_width / 2 + Jmath.Lerp(inPosition_3f.x,
                        inPosition_3f.x + inDimension_3f.x,
                        normalized_pos),
                    inPosition_3f.y - slider.knob_height_offset,
                    inPosition_3f.z - z_difference
                )
                knob_dim = vec3(slider.knob_width, inDimension_3f.y + slider.knob_height_offset * 2,
                    inDimension_3f.z)
                background_pos = inPosition_3f
                background_dim = inDimension_3f

                knob:Update(knob_pos, knob_dim, nil, nil, nil, slider.knob_color)
                background:Update(background_pos, background_dim)
                knob_button:Update(knob_pos, knob_dim)
            end

            local isMoving = false
            o.c:Push(Jkr.CreateUpdatable(function()
                local mouseRel = e:GetRelativeMousePos().x
                if e:IsLeftButtonPressedContinous() and (e:IsMouseWithinAtTopOfStack(knob_button.mId, knob_button.mDepthValue)) then
                    isMoving = true
                end

                if e:IsLeftButtonPressedContinous() and isMoving then
                    local oldValue = slider.current_value
                    local knob_x = current_pos.x + normalized_pos * current_dim.x
                    knob_x = Jmath.Clamp(knob_x + mouseRel, current_pos.x, current_dim.x + current_pos.x)
                    local newValue = Jmath.Lerp(slider.range_start, slider.range_end,
                        (knob_x - current_pos.x) / current_dim.x)
                    slider.current_value = newValue
                    slider:Update(current_pos, current_dim)
                else
                    isMoving = false
                end
            end), o.mCurrentScissor)

            slider:Update(inPosition_3f, inDimension_3f, inDefaultValue)
            return slider
        end


        o.CreateWindowScissor = function(inPosition_3f, inDimension_3f, inFont, inTitle, inTitlebarTextColor,
                                         inTitlebarBackColor, inContentBackColor, inShouldSetViewport)
            local ws = {}
            local z_difference = 10
            local titlebarheight = inFont:GetTextDimension("X").y + 10
            local titlebarpos = vec3(inPosition_3f.x, inPosition_3f.y - titlebarheight, inPosition_3f.z + z_difference)
            local titlebardimen = vec3(inDimension_3f.x, titlebarheight, inDimension_3f.z)
            local backgroundpos = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z + z_difference)

            local mTitlebar = o.CreateGeneralButton(titlebarpos,
                titlebardimen,
                nil,
                nil,
                inFont,
                inTitle,
                inTitlebarTextColor,
                inTitlebarBackColor, nil, nil)

            local mTitlebarButton = o.CreateButton(titlebarpos, titlebardimen)

            local mBackground = o.CreateGeneralButton(backgroundpos, inDimension_3f, nil, nil, nil, nil, nil,
                inContentBackColor, nil, nil
            )

            ---@note
            ---@todo Create a General button here
            ws.mId = o.CreateScissor(inPosition_3f, inDimension_3f, inShouldSetViewport)
            ws.Set = function()
                o.SetCurrentScissor(ws.mId)
            end
            ws.Reset = function()
                o.SetCurrentScissor(1)
            end

            local isMoving = false
            local mCurrentPosition = inPosition_3f
            local mCurrentDimension = inDimension_3f
            local mShouldSetViewport = inShouldSetViewport
            ws.mCurrentPosition = inPosition_3f
            ws.mCurrentDimension = inDimension_3f

            local mCentralComponent
            ws.SetCentralComponent = function(inComponent)
                mCentralComponent = inComponent
            end

            ws.Update = function(self, inPosition_3f, inDimension_3f)
                o.UpdateScissor(ws.mId, inPosition_3f, inDimension_3f, mShouldSetViewport)
                local titlebarpos = vec3(inPosition_3f.x, inPosition_3f.y - titlebarheight,
                    inPosition_3f.z + z_difference)
                local titlebardimen = vec3(inDimension_3f.x, titlebarheight, inDimension_3f.z)
                local backgroundpos = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z + z_difference)
                mTitlebar:Update(titlebarpos, titlebardimen)
                mTitlebarButton:Update(titlebarpos, titlebardimen)
                mBackground:Update(backgroundpos, inDimension_3f)

                if not mShouldSetViewport then
                    mCentralComponent:Update(inPosition_3f, inDimension_3f)
                end
                mCurrentPosition = inPosition_3f
                mCurrentDimension = inDimension_3f
                ws.mCurrentPosition = inPosition_3f
                ws.mCurrentDimension = inDimension_3f
            end

            o.c:Push(Jkr.CreateUpdatable(function()
                local mouseRel = e:GetRelativeMousePos()
                if e:IsLeftButtonPressedContinous() and (e:IsMouseWithinAtTopOfStack(mTitlebarButton.mId, mTitlebarButton.mDepthValue)) then
                    isMoving = true
                end
                if e:IsLeftButtonPressedContinous() and isMoving then
                    ws.Update(nil,
                        vec3(mCurrentPosition.x + mouseRel.x,
                            mCurrentPosition.y + mouseRel.y,
                            mCurrentPosition.z),
                        mCurrentDimension
                    )
                else
                    isMoving = false
                end
            end))
            return ws
        end

        o.CreateMovableButton = function(inPosition_3f, inDimension_3f,
                                         inOnClick, inContinous, inFont, inTitle,
                                         inTitlebarTextColor,
                                         inTitlebarBackColor, inPushConstantForImagePainter, inImageFilePath,
                                         inMovingColor_4f,
                                         inHoverColor_4f
        )
            local ws = {}
            local mTitlebar = o.CreateGeneralButton(inPosition_3f,
                inDimension_3f,
                inOnClick,
                inContinous,
                inFont,
                inTitle,
                inTitlebarTextColor,
                inTitlebarBackColor, inPushConstantForImagePainter, inImageFilePath)

            local mTitlebarButton = o.CreateButton(inPosition_3f, inDimension_3f)

            local isMoving = false
            local mCurrentPosition = inPosition_3f
            local mCurrentDimension = inDimension_3f
            ws.mCurrentPosition = inPosition_3f
            ws.mCurrentDimension = inDimension_3f


            ws.Update = function(self, inPosition_3f, inDimension_3f)
                mTitlebar:Update(inPosition_3f, inDimension_3f)
                mTitlebarButton:Update(inPosition_3f, inDimension_3f)

                mCurrentPosition = inPosition_3f
                mCurrentDimension = inDimension_3f
                ws.mCurrentPosition = inPosition_3f
                ws.mCurrentDimension = inDimension_3f
            end

            local color = vec4(mTitlebar.mColor)
            o.c:Push(Jkr.CreateUpdatable(function()
                local mouseRel = e:GetRelativeMousePos()
                if e:IsLeftButtonPressedContinous() and (e:IsMouseWithinAtTopOfStack(mTitlebarButton.mId, mTitlebarButton.mDepthValue)) then
                    isMoving = true
                end
                if e:IsLeftButtonPressedContinous() and isMoving then
                    mTitlebar:UpdateBackgroundColor(inMovingColor_4f or vec4(1, 0, 0, 1))
                    ws.Update(nil,
                        vec3(mCurrentPosition.x + mouseRel.x,
                            mCurrentPosition.y + mouseRel.y,
                            mCurrentPosition.z),
                        mCurrentDimension
                    )
                else
                    isMoving = false
                    mTitlebar:UpdateBackgroundColor(color)
                end
            end))
            return ws
        end

        o.CreateTextParagraph = function(inPosition_3f, inDimension_3f, inText, inFont, inMaxRows, inTextColor)
            local tp = {}
            local texts = {}
            local font = inFont
            for i = 1, inMaxRows, 1 do
                texts[#texts + 1] = o.CreateTextLabel(vec3(0, 0, 0), vec3(0, 0, 0), inFont, " ", inTextColor)
            end

            tp.Update = function(self, inPosition_3f, inDimension_3f, inText, inFont)
                if inFont then font = inFont end
                local WrappedTexts = font:WrapToTextVector(inText, inFont.mId, vec2(inDimension_3f.x, inDimension_3f.y))
                local olddim = vec3(0, 0, 0)
                for i = 1, #texts, 1 do
                    local dim = font:GetTextDimension(WrappedTexts[i])
                    local pos = vec3(inPosition_3f.x, inPosition_3f.y + olddim.y, inPosition_3f.z)
                    texts[i]:Update()
                    olddim.x = dim.x
                    olddim.y = dim.y
                end
            end

            return tp
        end
        local alerp = function(a, b, t)
            return (a * (1 - t) + t * b) * (1 - t) + b * t
        end

        local alerp_2f = function(a, b, t)
            return vec2(alerp(a.x, b.x, t), alerp(a.y, b.y, t))
        end

        local alerp_3f = function(a, b, t)
            return vec3(alerp(a.x, b.x, t), alerp(a.y, b.y, t), alerp(a.z, b.z, t))
        end

        local alerp_4f = function(a, b, t)
            return vec4(alerp(a.x, b.x, t), alerp(a.y, b.y, t), alerp(a.z, b.z, t), alerp(a.w, b.w, t))
        end

        o.AnimationPush = function(inElement,
                                   inStartPosition_3f, inEndPosition_3f,
                                   inStartDimension_3f, inEndDimension_3f,
                                   step)
            if not step then
                step = 0.1
            end
            local t = 0
            local sp = vec2(inStartPosition_3f)
            local ep = vec2(inEndPosition_3f)
            local sd = vec2(inStartDimension_3f)
            local ed = vec2(inEndDimension_3f)
            local el = inElement
            local Frame = 1
            while t <= 1 do
                local np = alerp_2f(sp, ep, t)
                local nd = alerp_2f(sd, ed, t)
                local np3 = vec3(np, inStartPosition_3f.z)
                local nd3 = vec3(nd, inStartDimension_3f.z)
                o.c:PushOneTime(Jkr.CreateUpdatable(
                    function()
                        el:Update(np3, nd3)
                    end
                ), Frame)
                Frame = Frame + 1
                t = t + step
            end
        end

        return o
    end

    CompileShaders                   = function(shaders)
        shaders.roundedRectangle = Engine.Shader()
            .Header(450)
            .CInvocationLayout(16, 16, 1)
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
    end
end

jkrgui()
