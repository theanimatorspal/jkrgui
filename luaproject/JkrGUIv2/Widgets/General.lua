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
        .ImagePainterPush()
        .GlslMainBegin()
        .ImagePainterAssist()
        .Append [[
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
          ]]
        .GlslMainEnd().str

    o.prebuilts = {}
    o.prebuilts.roundedRectangle = o.CreateComputeImage(vec3(0), vec3(100, 100, 1))
    o.prebuilts.roundedRectanglePainter = Jkr.CreateCustomImagePainter("o.prebuilts.roundedRectangle.glsl",
        o.shaders.roundedRectangle)
    o.prebuilts.roundedRectanglePainter:Store(i, w)
    o.prebuilts.roundedRectangle.RegisterPainter(o.prebuilts.roundedRectanglePainter)
    local op = o.prebuilts;
    o.c:PushOneTime(Jkr.CreateDispatchable(function()
        op.roundedRectangle.BindPainter(op.roundedRectanglePainter)
        local PushConstant = Jkr.DefaultCustomImagePainterPushConstant()
        PushConstant.x = vec4(0.0, 0.0, 0.85, 0.85)
        PushConstant.y = vec4(1)
        PushConstant.z = vec4(0.1, 0.5, 0.5, 0.0)
        op.roundedRectangle.DrawPainter(op.roundedRectanglePainter, PushConstant, 100, 100, 1)
    end), 1)

    o.CreatePressButton = function(inPosition_3f, inDimension_3f, inOnClickFunction, inContinous, inFont, inText,
                                   inColor, inBackgroundColor)
        local button = {}
        button.parent = o.CreateButton(inPosition_3f, inDimension_3f, inOnClickFunction, inContinous)
        setmetatable(button, button.parent)
        button.__index = button.parent

        button.sampledImage = o.CreateSampledImage(inPosition_3f, inDimension_3f, nil, nil, inBackgroundColor)
        o.c:PushOneTime(Jkr.CreateDispatchable(function()
            op.roundedRectangle.CopyToSampled(button.sampledImage)
        end), 1)
        button.sampledText = o.CreateTextLabel(inPosition_3f, inDimension_3f, inFont, inText, inColor)

        button.Update = function(self, inPosition_3f, inDimension_3f, inFont, inText, inColor, inBackgroundColor)
            button.parent:Update(inPosition_3f, inDimension_3f)
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
