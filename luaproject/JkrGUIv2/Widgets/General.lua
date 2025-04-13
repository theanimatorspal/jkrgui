require "JkrGUIv2.Widgets.Basic"
require "JkrGUIv2.Engine.Shader"

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
            button.quad = o.CreateQuad(inPosition_3f, inDimension_3f, push, "showImage", op.sampledImage.mId, inMatrix)
        else
            button.sampledImage = o.CreateSampledImage(vec3(math.huge), vec3(100, 100, 1), inImageFilePath, true, vec4(1))
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
            local titlebarpos = vec3(inPosition_3f.x, inPosition_3f.y - titlebarheight, inPosition_3f.z + z_difference)
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

    do
        --- Widgets for Graphing
        local m_label_pool = {}
        local m_current_label_pool_index = 1
        local CreateLabelPool = function(inFont)
            if #m_label_pool == 0 then
                for _ = 1, 100 do
                    m_label_pool[#m_label_pool + 1] = o.CreateTextLabel(
                        vec3(100000), vec3(10000), inFont, " ")
                end
            end
        end

        local GetLabelFromPool = function(inIndex)
            if (not inIndex) or inIndex >= m_current_label_pool_index then
                local label = m_label_pool[m_current_label_pool_index]
                m_current_label_pool_index = m_current_label_pool_index + 1
                return label
            else
                return m_label_pool[inIndex]
            end
        end

        local calculate_tickers = function(inPosition_3f, inDimension_3f, inFrom_3f, inTo_3f, inStep_3f, inFormatX,
                                           inFormatY)
            local x_texts = {}
            local y_texts = {}
            local x_ticks = math.floor((inTo_3f.x - inFrom_3f.x) / inStep_3f.x)
            local y_ticks = math.floor((inTo_3f.y - inFrom_3f.y) / inStep_3f.y)

            local iii = 0
            while (inFrom_3f.x <= inTo_3f.x) do
                x_texts[#x_texts + 1] = { string.format(inFormatX, inFrom_3f.x),
                    inPosition_3f.x + inDimension_3f.x / x_ticks * iii,
                    inPosition_3f.y + inDimension_3f.y,
                }
                inFrom_3f.x = inFrom_3f.x + inStep_3f.x
                iii = iii + 1
            end

            local iii = 0
            while (inFrom_3f.y <= inTo_3f.y) do
                y_texts[#y_texts + 1] = {
                    string.format(inFormatY, inFrom_3f.y),
                    inPosition_3f.x,
                    inPosition_3f.y + inDimension_3f.y - inDimension_3f.y / y_ticks * iii
                }
                inFrom_3f.y = inFrom_3f.y + inStep_3f.y
                iii = iii + 1
            end
            return x_texts, y_texts
        end

        o.CreateScale2D = function(inPosition_3f, inDimension_3f, inFont,
                                   inTo_3f, inFrom_3f, inStep_3f, inFormatX,
                                   inFormatY,
                                   inColor,
                                   inMatrix)
            inFormatX = inFormatX or "%d"
            inFormatY = inFormatY or "%d"
            local inTo_3f = vec3(inTo_3f)
            local inFrom_3f = vec3(inFrom_3f)
            local inStep_3f = vec3(inStep_3f)
            CreateLabelPool(inFont)

            local scale2d = {}
            local x_texts, y_texts = calculate_tickers(inPosition_3f, inDimension_3f, inFrom_3f, inTo_3f, inStep_3f,
                inFormatX,
                inFormatY)

            scale2d.label_pool_index = m_current_label_pool_index

            local construct = function(x_texts, y_texts)
                local x = 0
                for i = 1, #x_texts do
                    local t = x_texts[i]
                    GetLabelFromPool(scale2d.label_pool_index + x):Update(vec3(t[2], t[3], inPosition_3f.z),
                        inDimension_3f,
                        inFont,
                        t[1], inColor,
                        inMatrix)
                    x = x + 1
                end

                for i = 1, #y_texts do
                    local t = y_texts[i]
                    local label = GetLabelFromPool(scale2d.label_pool_index + x)
                    label.Align("LEFT")
                    label:Update(vec3(t[2], t[3], inPosition_3f.z), inDimension_3f, inFont,
                        t[1], inColor,
                        inMatrix)
                    x = x + 1
                end
            end
            construct(x_texts, y_texts)

            scale2d.Update = function(inPosition_3f, inDimension_3f, inFont,
                                      inTo_3f, inFrom_3f, inStep_3f, inFormatX,
                                      inFormatY, inColor, inMatrix)
                local x_texts, y_texts = calculate_tickers(inPosition_3f, inDimension_3f, inFrom_3f, inTo_3f, inStep_3f,
                    inFormatX, inFormatY)
                construct(x_texts, y_texts)
            end
            return scale2d
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
