require "JkrGUIv2.Basic"
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
    o.Push = function(inCall)
        if inCall.mDrawType then
            o.mDrawables[#o.mDrawables + 1] = inCall
        elseif inCall.mUpdate then
            o.mUpdatables[#o.mUpdatables + 1] = inCall
        elseif inCall.mDispatch then
            o.mDispatchables[#o.mDispatchables + 1] = inCall
        elseif inCall.mEvent then
            o.mEventables[#o.mEventables + 1] = inCall
        end
    end
    o.PushOneTime = function(inCall, inFrame)
        if inCall.mDrawType then
            -- Optimize this
            if o.mOneTimeDrawables[inFrame] then
                o.mOneTimeDrawables[inFrame][#o.mOneTimeDrawables[inFrame] + 1] = inCall
            else
                o.mOneTimeDrawables[inFrame] = {}
                o.mOneTimeDrawables[inFrame][#o.mOneTimeDrawables[inFrame] + 1] = inCall
            end
        elseif inCall.mUpdate then
            if o.mOneTimeUpdatables[inFrame] then
                o.mOneTimeUpdatables[inFrame][#o.mOneTimeUpdatables[inFrame] + 1] = inCall
            else
                o.mOneTimeUpdatables[inFrame] = {}
                o.mOneTimeUpdatables[inFrame][#o.mOneTimeUpdatables[inFrame] + 1] = inCall
            end
        elseif inCall.mDispatch then
            if o.mOneTimeDispatchables[inFrame] then
                o.mOneTimeDispatchables[inFrame][#o.mOneTimeDispatchables[inFrame] + 1] = inCall
            else
                o.mOneTimeDispatchables[inFrame] = {}
                o.mOneTimeDispatchables[inFrame][#o.mOneTimeDispatchables[inFrame] + 1] = inCall
            end
        end
    end
    return o
end

Jkr.CreateCallExecutor = function(inCallBuffer)
    local o = {}
    o.c = inCallBuffer
    return o
end

Jkr.CreateWidgetRenderer = function(i, w)
    local o = {}
    o.s = Jkr.CreateShapeRenderer(i, w)
    o.t = Jkr.CreateTextRendererBestTextAlt(i, o.s)
    o.c = Jkr.CreateCallBuffers()
    o.e = Jkr.CreateCallExecutor(o.c)

    o.CreateFont = function(inFontFileName, inFontSize)
        local font = {}
        font.mId = o.t:AddFontFace(inFontFileName, inFontSize)
        return font
    end

    -- Here for each widget we have to follow function(inPosition, inDimension) style
    o.CreateTextLabel = function(inPosition_3f, inDimension_3f, inFont, inText, inColor)
        local textLabel = {}
        textLabel.mText = inText
        textLabel.mFont = inFont
        textLabel.mId = o.t:Add(inFont.mId, inPosition_3f, inText)
        o.c.Push(Jkr.CreateDrawable(textLabel.mId, nil, "STATIC_TEXT", nil))

        textLabel.Update = function(self, inPosition_3f, inDimension_3f, inFont, inText)
            if inFont then self.mFont = inFont end
            if inText then self.mText = inText end
            o.t:Update(self.mId, self.mFont.mId, inPosition_3f, self.mText)
        end

        textLabel.Remove = function(self)
            -- TODO Remove function
        end
        return textLabel
    end

    o.Update = function()
        o.WindowDimension = w:GetWindowDimension()
        o.UIMatrix = Jmath.Ortho(0.0, o.WindowDimension.x, 0.0, o.WindowDimension.y, 1000, -1000)
    end

    o.Draw = function()
        -- Optimize this
        for i = 1, #o.c.mDrawables, 1 do
            local drawable = o.c.mDrawables[i]
            if drawable.mDrawType == "STATIC_TEXT" then
                o.s:BindShapes(w)
                o.s:BindFillMode(Jkr.FillType.Image, w)
                o.t:Draw(drawable.mId, w, drawable.mColor, o.UIMatrix, Jkr.CmdParam.UI)
            end
        end
    end
    o.Dispatch = function()
        o.s:Dispatch(w, Jkr.CmdParam.UI)
    end
    o.Event = function()

    end
    return o
end
