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
    local op = o.prebuilts
    op.roundedRectanglePainter =
        Jkr.CreateCustomImagePainter(
            "cache/op.roundedRectangle.glsl",
            o.shaders.roundedRectangle
        )
    op.roundedRectanglePainter:Store(i, w)
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
                                     inImagePainter)
        local button = {}
        local push = Jkr.Matrix2CustomImagePainterPushConstant()
        button.mColor = inBackgroundColor or vec4(1)
        push.a = mat4(vec4(0.0), button.mColor, vec4(0.3), vec4(0.0))

        if not inImageFilePath then
            button.quad = o.CreateQuad(inPosition_3f, inDimension_3f, push, "showImage", op.sampledImage.mId)
        else
            button.sampledImage = o.CreateSampledImage(vec3(math.huge), vec3(100, 100, 1), inImageFilePath, true, vec4(1))
            button.quad = o.CreateQuad(inPosition_3f, inDimension_3f, push, "showImage", button.sampledImage.mId)
        end

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

        if not inImageFilePath and inFont and inText then
            button.sampledText = o.CreateTextLabel(inPosition_3f, inDimension_3f, inFont, inText, inColor)
        end

        button.padding = 10
        ---@warning Bad Design, Fix in Refactoring
        button.Update = function(self,
                                 inPosition_3f,
                                 inDimension_3f,
                                 inFont,
                                 inText,
                                 inColor,
                                 inBackgroundColor,
                                 inTextOreintation)
            local push = Jkr.Matrix2CustomImagePainterPushConstant()
            push.a = mat4(vec4(0.0), inBackgroundColor or button.mColor, vec4(0.0), vec4(0))
            button.quad:Update(inPosition_3f, inDimension_3f, push);
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
