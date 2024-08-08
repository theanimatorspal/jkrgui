require "JkrGUIv2.require"

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
        inCallBuffer.PushOneTime(Jkr.CreateUpdatable(
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

    o.Push = function(inCall)
        if inCall.mDrawType then
            o.mDrawables[#o.mDrawables + 1] = inCall
            return #o.mDrawables
        elseif inCall.mUpdate then
            o.mUpdatables[#o.mUpdatables + 1] = inCall
            return #o.mUpdatables
        elseif inCall.mDispatch then
            o.mDispatchables[#o.mDispatchables + 1] = inCall
            return #o.mDispatchables
        elseif inCall.mEvent then
            o.mEventables[#o.mEventables + 1] = inCall
            return #o.mEventables
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

    local cdf = 1 -- CurrentDispatchFrame
    o.Dispatch = function()
        --[========================================================]
        -- DISPATCH ONE TIMES
        --[========================================================]
        if o.mOneTimeDispatchables[cdf] then
            local Length = #o.mOneTimeDispatchables[cdf]
            for x = 1, Length, 1 do
                o.mOneTimeDispatchables[cdf][x].mDispatch()
                o.mOneTimeDispatchables[cdf][x] = nil
            end
            o.mOneTimeDispatchables[cdf] = nil
            cdf = cdf + 1
            if not o.mOneTimeDispatchables[cdf] then
                cdf = 1
            end
        end

        --[========================================================]
        -- DISPATCH
        --[========================================================]
        for i = 1, #o.mDispatchables, 1 do
            o.mDispatchables[i].mDispatch()
        end
    end

    local cuf = 1 -- CurrentUpdateFrame
    o.Update = function()
        --[========================================================]
        -- UPDATE ONE TIMES
        --[========================================================]
        if o.mOneTimeUpdatables[cuf] then
            local Length = #o.mOneTimeUpdatables[cuf]
            for x = 1, Length, 1 do
                o.mOneTimeUpdatables[cuf][x].mUpdate()
                o.mOneTimeUpdatables[cuf][x] = nil
            end
            o.mOneTimeUpdatables[cuf] = nil
            cuf = cuf + 1
            if not o.mOneTimeUpdatables[cuf] then
                cuf = 1
            end
        end

        --[========================================================]
        -- UPDATE
        --[========================================================]
        for i = 1, #o.mUpdatables, 1 do
            o.mUpdatables[i].mUpdate()
        end
    end

    o.Event = function()
        --[========================================================]
        -- EVENT
        --[========================================================]
        for i = 1, #o.mEventables, 1 do
            o.mEventables[i].mEvent()
        end
    end

    return o
end

Jkr.CreateCallExecutor = function(inCallBuffer)
    local o = {}
    o.c = inCallBuffer
    return o
end
