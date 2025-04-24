require "JkrGUIv2.Widgets.Basic"
require "JkrGUIv2.Engine.Shader"

local CompileShaders

function CR(inComponents)
    if type(inComponents) == "number" then
        local each = 1 / inComponents
        return table.Produce(function() return each end, inComponents)
    elseif type(inComponents) == "table" then
        local totalFixed = 0
        local panCount = 0
        local result = {}

        local has_pans = false
        for _, v in ipairs(inComponents) do
            if type(v) == "number" then
                totalFixed = totalFixed + v
            elseif type(v) == "string" and v == "pan" then
                panCount = panCount + 1
                has_pans = true
            end
        end

        if not has_pans then
            local each = 1 / #inComponents
            return table.Produce(function() return each end, #inComponents)
        end

        local remaining = 1 - totalFixed
        local eachPan = panCount > 0 and remaining / panCount or 0

        for _, v in ipairs(inComponents) do
            if type(v) == "number" then
                table.insert(result, v)
            elseif v == "pan" then
                table.insert(result, eachPan)
            end
        end

        return result
    else
        error("CR: Invalid input type - must be table or number")
    end
end

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

function S(inComponents, inDeltaZ)
    inDeltaZ = inDeltaZ or 5
    if not inComponents then
        return Jkr.StackLayout:New(inDeltaZ)
    else
        local h = Jkr.StackLayout:New(inDeltaZ)
        h:Add(inComponents)
        return h
    end
end

-- function U(inValue)
--     if not inValue then inValue = {} end
--     inValue.Update = function(self, inP, inD)
--         inValue.d = vec3(inD)
--         inValue.p = vec3(inP)
--     end
--     return inValue
-- end

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

    do
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
    end

    do
    end

    local CreateCustomRoundedRectangle = function(inPushConstantForImagePainter)
        local rrec = {}
        rrec.roundedRectangle2 = o.CreateComputeImage(vec3(0), vec3(500, 500, 1))
        rrec.roundedRectangle2.RegisterPainter(op.roundedRectanglePainter)

        o.c:PushOneTime(Jkr.CreateDispatchable(function()
            rrec.roundedRectangle2.BindPainter(op.roundedRectanglePainter)
            PushConstant = Jkr.Matrix2CustomImagePainterPushConstant()
            PushConstant.a = mat4(
                vec4(0.0, 0.0, 2, 2),
                vec4(1),
                vec4(0.1, 0.5, 0.5, 0.0),
                vec4(0)
            )
            PushConstant = PushConstant or inPushConstantForImagePainter
            rrec.roundedRectangle2.DrawPainter(op.roundedRectanglePainter, PushConstant, 50,
                50, 1)
        end), 1)
        rrec.sampledImage2 = o.CreateSampledImage(vec3(math.huge), vec3(500, 500, 1), nil, true,
            vec4(1))
        o.c:PushOneTime(Jkr.CreateDispatchable(function()
            rrec.roundedRectangle2.CopyToSampled(rrec.sampledImage2)
        end), 1)
        return rrec
    end

    o.CreateGeneralButton = function(inP,
                                     inD,
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
            if not inPushConstantForImagePainter then
                button.quad = o.CreateQuad(inP, inD, push, "showImage", op.sampledImage.mId,
                    inMatrix)
            else
                button.customrec = CreateCustomRoundedRectangle(inPushConstantForImagePainter)
                button.quad = o.CreateQuad(inP, inD, push, "showImage",
                    button.customrec.sampledImage2.mId,
                    inMatrix)
            end
        else
            button.sampledImage = o.CreateSampledImage(vec3(math.huge), vec3(100, 100, 1), inImageFilePath, true, vec4(1))
            button.quad = o.CreateQuad(inP, inD, push, "showImage", button.sampledImage.mId)
        end

        if inD.x == 0 or inD.y == 0 then
            button.shouldUpdateByDimension = vec3(inD.x, inD.y, inD.z)
            inD = vec3(100, 100, 1)
        end

        if type(inOnClickFunction) == "function" then
            button.parent = o.CreateButton(inP, inD, inOnClickFunction, inContinous)
            setmetatable(button, button.parent)
            button.__index = button.parent
        end

        if not inImagePainter then
            inImagePainter = o.prebuilts.roundedRectanglePainter
        end

        if not inImageFilePath and inFont and inText then
            button.sampledText = o.CreateTextLabel(inP, inD, inFont, inText, inColor)
        end

        button.padding = 10
        button.UpdateBackgroundColor = function(self, inBackgroundColor)
            local push = Jkr.Matrix2CustomImagePainterPushConstant()
            button.mColor = inBackgroundColor or vec4(1)
            push.a = mat4(vec4(0.0), button.mColor, vec4(0.3), vec4(0.0))
            button.quad:Update(button.mP, button.mD, push);
            button.mColor = inBackgroundColor
        end
        ---@warning Bad Design, Fix in Refactoring
        button.Update = function(self,
                                 inP,
                                 inD,
                                 inFont,
                                 inText,
                                 inColor,
                                 inBackgroundColor,
                                 inTextOreintation, inMatrix)
            button.mP = inP
            button.mD = inD

            button.quad:Update(inP, inD, push, inMatrix);
            if button.parent then
                button.parent:Update(inP, inD)
            end

            if inBackgroundColor then
                button:UpdateBackgroundColor(inBackgroundColor)
            end

            if not inImageFilePath and button.sampledText then
                local DelDim = vec3(0, 0, 0)
                local fontDim = button.sampledText.mFont:GetTextDimension(inText or Copy(button.sampledText.mText) or
                    " ")
                DelDim = vec3((inD.x - fontDim.x) / 2, (inD.y - fontDim.y) / 2, 0)
                local substr = Copy(inText)
                while DelDim.x < 0.0 and substr do
                    substr = string.sub(substr, 1, #substr - 1)
                    fontDim = button.sampledText.mFont:GetTextDimension(substr)
                    DelDim = vec3((inD.x - fontDim.x) / 2, (inD.y - fontDim.y) / 2, 0)
                end
                button.sampledText:Update(inP + DelDim, inD, inFont, substr, inColor, inMatrix)

                if inTextOreintation then
                    if inTextOreintation == "LEFT" then
                        DelDim.x = button.padding
                        button.sampledText:Update(inP + DelDim, inD, inFont, substr, inColor,
                            inMatrix)
                    end
                    if inTextOreintation == "RIGHT" then
                        DelDim.x = inP.x - fontDim.x - button.padding
                        DelDim.x = inP.x
                        button.sampledText:Update(inP + DelDim, inD, inFont, substr, inColor,
                            inMatrix)
                    end
                end
            end

            if inText and button.sampledText then
                button.sampledText.mText = Copy(inText)
            end
        end

        if button.shouldUpdateByDimension then
            button:Update(inP, button.shouldUpdateByDimension)
        end

        return button
    end

    o.CreateSlider = function(inP, inD, inRangeStart, inRangeEnd, inDefaultValue, inStep)
        local slider = {}
        local z_difference = 10
        local knob = o.CreateGeneralButton(vec3(inP.x, inP.y, inP.z - z_difference),
            inD)
        local background = o.CreateGeneralButton(inP, inD)
        local knob_button = o.CreateButton(vec3(inP.x, inP.y, inP.z - z_difference),
            inD)
        slider.range_start = inRangeStart
        slider.range_end = inRangeEnd
        slider.current_value = inDefaultValue
        slider.knob_width = 0.1 * inD.x
        slider.knob_height_offset = 5
        slider.knob_color = vec4(0.5)
        slider.step = inStep



        local current_pos, current_dim, knob_pos, knob_dim,
        background_pos, background_dim, normalized_pos

        slider.Update = function(self, inP, inD, inValue)
            current_pos, current_dim = inP, inD
            if not inValue then inValue = slider.current_value else slider.current_value = inValue end
            normalized_pos = (slider.current_value - slider.range_start) / (slider.range_end - slider.range_start)
            knob_pos = vec3(
                -slider.knob_width / 2 + Jmath.Lerp(inP.x,
                    inP.x + inD.x,
                    normalized_pos),
                inP.y - slider.knob_height_offset,
                inP.z - z_difference
            )
            knob_dim = vec3(slider.knob_width, inD.y + slider.knob_height_offset * 2,
                inD.z)
            background_pos = inP
            background_dim = inD

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

        slider:Update(inP, inD, inDefaultValue)
        return slider
    end


    o.CreateWindowScissor = function(inP, inD, inFont, inTitle, inTitlebarTextColor,
                                     inTitlebarBackColor, inContentBackColor, inShouldSetViewport, inIsNotMovable)
        local ws = {}
        local z_difference = 1
        local titlebarheight = inFont:GetTextDimension("X").y + 10
        local titlebarpos = vec3(inP.x, inP.y, inP.z + z_difference)
        local titlebardimen = vec3(inD.x, titlebarheight, inD.z)
        local backgroundpos = vec3(inP.x, inP.y + titlebarheight, inP.z + z_difference)
        local backgrounddimen = vec3(inD.x, inD.y - titlebarheight, 1)

        local push = Jkr.Matrix2CustomImagePainterPushConstant()
        push.a = mat4(
            vec4(0.0, 0.0, 0.85, 0.85),
            vec4(1),
            vec4(0.1, 0.5, 0.5, 0.0),
            vec4(0)
        )

        local mTitlebar = o.CreateGeneralButton(titlebarpos,
            titlebardimen,
            nil,
            nil,
            inFont,
            inTitle,
            inTitlebarTextColor,
            inTitlebarBackColor, push, nil)

        local mTitlebarButton = o.CreateButton(titlebarpos, titlebardimen)

        local mBackground = o.CreateGeneralButton(backgroundpos, backgrounddimen, nil, nil, nil, nil, nil,
            inContentBackColor, push, nil
        )

        ---@note
        ---@todo Create a General button here
        ws.mId = o.CreateScissor(inP, inD, inShouldSetViewport)
        ws.Set = function()
            o.SetCurrentScissor(ws.mId)
        end
        ws.Reset = function()
            o.SetCurrentScissor(1)
        end

        local isMoving = false
        local mCurrentPosition = inP
        local mCurrentDimension = inD
        local mShouldSetViewport = inShouldSetViewport
        ws.mCurrentPosition = inP
        ws.mCurrentDimension = inD

        local mCentralComponent = { Update = function(a, b, c) end }
        ws.SetCentralComponent = function(inComponent)
            mCentralComponent = inComponent
        end

        ws.Update = function(self, inP, inD)
            o.UpdateScissor(ws.mId, inP, inD, mShouldSetViewport)
            local titlebarpos = vec3(inP.x, inP.y, inP.z + z_difference)
            local titlebardimen = vec3(inD.x, titlebarheight, inD.z)
            local backgroundpos = vec3(inP.x, inP.y + titlebarheight, inP.z + z_difference)
            local backgrounddimen = vec3(inD.x, inD.y - titlebarheight, 1)
            mTitlebar:Update(titlebarpos, titlebardimen)
            mTitlebarButton:Update(titlebarpos, titlebardimen)
            mBackground:Update(backgroundpos, backgrounddimen)

            if not mShouldSetViewport then
                mCentralComponent:Update(backgroundpos, backgrounddimen)
            end
            mCurrentPosition = inP
            mCurrentDimension = inD
            ws.mCurrentPosition = inP
            ws.mCurrentDimension = inD
        end

        if not inIsNotMovable then
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
        end
        return ws
    end

    o.CreateMovableButton = function(inP, inD,
                                     inOnClick, inContinous, inFont, inTitle,
                                     inTitlebarTextColor,
                                     inTitlebarBackColor, inPushConstantForImagePainter, inImageFilePath,
                                     inMovingColor_4f,
                                     inHoverColor_4f
    )
        local ws = {}
        local mTitlebar = o.CreateGeneralButton(inP,
            inD,
            inOnClick,
            inContinous,
            inFont,
            inTitle,
            inTitlebarTextColor,
            inTitlebarBackColor, inPushConstantForImagePainter, inImageFilePath)

        local mTitlebarButton = o.CreateButton(inP, inD)

        local isMoving = false
        local mCurrentPosition = inP
        local mCurrentDimension = inD
        ws.mCurrentPosition = inP
        ws.mCurrentDimension = inD


        ws.Update = function(self, inP, inD)
            mTitlebar:Update(inP, inD)
            mTitlebarButton:Update(inP, inD)

            mCurrentPosition = inP
            mCurrentDimension = inD
            ws.mCurrentPosition = inP
            ws.mCurrentDimension = inD
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

    o.CreateTextParagraph = function(inP, inD, inText, inFont, inMaxRows, inTextColor)
        local tp = {}
        local texts = {}
        local font = inFont
        for i = 1, inMaxRows, 1 do
            texts[#texts + 1] = o.CreateTextLabel(vec3(0, 0, 0), vec3(0, 0, 0), inFont, " ", inTextColor)
        end

        tp.Update = function(self, inP, inD, inText, inFont)
            if inFont then font = inFont end
            local WrappedTexts = font:WrapToTextVector(inText, inFont.mId, vec2(inD.x, inD.y))
            local olddim = vec3(0, 0, 0)
            for i = 1, #texts, 1 do
                local dim = font:GetTextDimension(WrappedTexts[i])
                local pos = vec3(inP.x, inP.y + olddim.y, inP.z)
                texts[i]:Update()
                olddim.x = dim.x
                olddim.y = dim.y
            end
        end

        return tp
    end

    o.CreateDisplayTable = function(inP, inD, inElementProducer, inDisplayRowCount,
                                    inDisplayColCount)
        inDisplayColCount = inDisplayColCount or 3
        inDisplayRowCount = inDisplayRowCount or 5
        if not inElementProducer then
            Engine.log("Element Producer function(row, column, text) not supplied", "ERROR")
        end
        local tb = {}
        local hlayouts = {}
        local vlayout
        for row = 1, inDisplayRowCount do
            local columns = {}
            for column = 1, inDisplayColCount do
                columns[column] = inElementProducer(row, column, "")
            end
            hlayouts[row] = H(columns, CR(columns))
        end
        vlayout = V(hlayouts, CR(hlayouts))

        tb.data = {
            { "%d", "%s",   "%.2f" },
            { 1,    "Shit", 425.312 },
            { 2,    "Fuck", 315.3125 },
        }
        tb.Update = function(self, inP, inD, inData, inRowOffset)
            tb.mP = inP
            tb.mD = inD
            tb.data = inData or tb.data
            inRowOffset = inRowOffset or 0
            local tb_length = #tb.data
            for row = 1, inDisplayRowCount do
                local columns = {}
                for column = 1, inDisplayColCount do
                    local text = ""
                    local actualRow = inRowOffset + row + 1
                    local offset = (actualRow - 1) % inDisplayRowCount
                    if offset == 0 then offset = inDisplayRowCount end

                    if offset == row and tb.data[actualRow] then
                        text = string.format(tb.data[1][column], tb.data[actualRow][column])
                        columns[column] = inElementProducer(row, column, text)
                    else
                        columns[column] = inElementProducer(row, column, " ")
                    end
                end
                hlayouts[row] = H(columns, CR(columns))
            end
            vlayout = V(hlayouts, CR(hlayouts))
            vlayout:Update(inP, inD)
        end
        return tb
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
                               inStartP, inEndP,
                               inStartD, inEndD,
                               step)
        if not step then
            step = 0.1
        end
        local t = 0
        local sp = vec2(inStartP)
        local ep = vec2(inEndP)
        local sd = vec2(inStartD)
        local ed = vec2(inEndD)
        local el = inElement
        local Frame = 1
        while t <= 1 do
            local np = alerp_2f(sp, ep, t)
            local nd = alerp_2f(sd, ed, t)
            local np3 = vec3(np, inStartP.z)
            local nd3 = vec3(nd, inStartD.z)
            o.c:PushOneTime(Jkr.CreateUpdatable(
                function()
                    el:Update(np3, nd3)
                end
            ), Frame)
            Frame = Frame + 1
            t = t + step
        end
    end
    o.AnimationGeneralPush = function(inLerpFunction, inStep)
        for i = 1, 1 / inStep do
            o.c:PushOneTime(Jkr.CreateUpdatable(
                function()
                    inLerpFunction(i * inStep)
                end
            ), i)
        end
    end

    do
        --- Widgets for Graphing
        o.m_label_pool = {}
        local m_current_label_pool_index = 1
        local CreateLabelPool = function(inFont)
            if #o.m_label_pool == 0 then
                for _ = 1, 100 do
                    o.m_label_pool[#o.m_label_pool + 1] = o.CreateTextLabel(
                        vec3(100000), vec3(10000), inFont, " ")
                end
            end
        end

        local ClearLabelTexts = function(inFont, inCount)
            for i = 1, inCount do
                o.m_label_pool[i]:Update(vec3(1000000), vec3(10000), inFont, " ")
            end
        end

        local GetLabelFromPool = function(inIndex)
            if inIndex > m_current_label_pool_index then
                local la = o.m_label_pool[inIndex]
                m_current_label_pool_index = m_current_label_pool_index + 1
                return la
            end
            return o.m_label_pool[inIndex]
        end

        local calculate_tickers = function(inP, inD, inFrom_3f, inTo_3f, inStep_3f, inFormatX,
                                           inFormatY)
            inFrom_3f = vec3(inFrom_3f or vec3(0))
            inTo_3f = vec3(inTo_3f or vec3(2))
            inStep_3f = vec3(inStep_3f or vec3(1))
            inFormatX = "%d" or inFormatX
            inFormatY = "%d" or inFormatY
            local x_texts = {}
            local y_texts = {}
            local x_ticks = math.floor((inTo_3f.x - inFrom_3f.x) / inStep_3f.x)
            local y_ticks = math.floor((inTo_3f.y - inFrom_3f.y) / inStep_3f.y)

            local iii = 0
            while (inFrom_3f.x <= inTo_3f.x) do
                x_texts[#x_texts + 1] = { string.format(inFormatX, inFrom_3f.x),
                    inP.x + inD.x / x_ticks * iii,
                    inP.y + inD.y,
                }
                inFrom_3f.x = inFrom_3f.x + inStep_3f.x
                iii = iii + 1
            end

            local iii = 0
            while (inFrom_3f.y <= inTo_3f.y) do
                y_texts[#y_texts + 1] = {
                    string.format(inFormatY, inFrom_3f.y),
                    inP.x,
                    inP.y + inD.y - inD.y / y_ticks * iii
                }
                inFrom_3f.y = inFrom_3f.y + inStep_3f.y
                iii = iii + 1
            end
            return x_texts, y_texts
        end

        local construct = function(scale2d, inP, inD, x_texts, y_texts, inFont, inColor, inMatrix)
            local x = 0
            for i = 1, #x_texts do
                local t = x_texts[i]
                local label = GetLabelFromPool(scale2d.label_pool_index + x)
                label.Align("RIGHT", "TOP")
                label:Update(vec3(t[2], t[3], inP.z),
                    inD, inFont, t[1], inColor, inMatrix)
                x = x + 1
            end

            for i = 1, #y_texts do
                local t = y_texts[i]
                local label = GetLabelFromPool(scale2d.label_pool_index + x)
                label.Align("RIGHT", "CENTER")
                label:Update(vec3(t[2], t[3], inP.z), inD, inFont,
                    t[1], inColor, inMatrix)
                x = x + 1
            end
        end
        o.CreateScale2D = function(inP, inD, inFont,
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
            local x_texts, y_texts = calculate_tickers(inP, inD, inFrom_3f, inTo_3f, inStep_3f,
                inFormatX,
                inFormatY)

            scale2d.label_pool_index = m_current_label_pool_index
            construct(scale2d, inP, inD, x_texts, y_texts, inFont, inColor, inMatrix)

            scale2d.Update = function(inP, inD, inFont,
                                      inTo_3f, inFrom_3f, inStep_3f, inFormatX,
                                      inFormatY, inColor, inMatrix)
                local x_texts, y_texts = calculate_tickers(inP, inD, inFrom_3f, inTo_3f, inStep_3f,
                    inFormatX, inFormatY)
                construct(scale2d, inP, inD, x_texts, y_texts, inFont, inColor, inMatrix)
            end

            scale2d.Clear = function(inFont)
                ClearLabelTexts(inFont)
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
