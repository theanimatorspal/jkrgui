require "JkrGUIv2.Widgets.Basic"
require "JkrGUIv2.Engine.Shader"

local CompileShaders

Jkr.CreateGeneralWidgetsRenderer = function(inWidgetRenderer, i, w, e)
    local o = {}
    if not inWidgetRenderer then
        o = Jkr.CreateWidgetRenderer(i, w, e)
    end

    o.shaders = {}
    CompileShaders(o.shaders)

    o.prebuilts = {}
    o.prebuilts.roundedRectanglePainter =
        Jkr.CreateCustomImagePainter(
            "cache/o.prebuilts.roundedRectangle.glsl",
            o.shaders.roundedRectangle
        )
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
