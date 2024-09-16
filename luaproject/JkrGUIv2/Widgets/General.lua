require "JkrGUIv2.Widgets.Basic"
require "JkrGUIv2.Engine.Shader"

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


    o.CreatePressButton = function(inPosition_3f, inDimension_3f, inOnClickFunction, inContinous, inFont, inText,
                                   inColor, inBackgroundColor, inPushConstantForImagePainter)
        local button = {}
        if (inOnClickFunction) then
            button.parent = o.CreateButton(inPosition_3f, inDimension_3f, inOnClickFunction, inContinous)
            setmetatable(button, button.parent)
            button.__index = button.parent
        end
        button.roundedRectangle = o.CreateComputeImage(inPosition_3f, inDimension_3f)
        button.roundedRectangle.RegisterPainter(o.prebuilts.roundedRectanglePainter)

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

        button.sampledImage = o.CreateSampledImage(inPosition_3f, inDimension_3f, nil, nil, inBackgroundColor)
        o.c:PushOneTime(Jkr.CreateDispatchable(function()
            button.roundedRectangle.CopyToSampled(button.sampledImage)
        end), 1)
        button.sampledText = o.CreateTextLabel(inPosition_3f, inDimension_3f, inFont, inText, inColor)

        button.Update = function(self, inPosition_3f, inDimension_3f, inFont, inText, inColor, inBackgroundColor)
            if button.parent then
                button.parent:Update(inPosition_3f, inDimension_3f)
            end
            button.sampledImage:Update(inPosition_3f, inDimension_3f, inBackgroundColor)
            local DelDim = vec3(0, 0, 0)
            local fontDim = button.sampledText.mFont:GetTextDimension(self.sampledText.mText)
            DelDim = vec3((inDimension_3f.x - fontDim.x) / 2, (inDimension_3f.y - fontDim.y) / 2, 0)
            button.sampledText:Update(inPosition_3f + DelDim, inDimension_3f, inFont, inText, inColor)
        end
        return button
    end
    return o
end
