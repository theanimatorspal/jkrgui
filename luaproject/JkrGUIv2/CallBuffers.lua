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

Jkr.CreateDrawable = function(inId, inBatchable, inDrawType, inImageId, inColor_4f, inPush)
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

    o.Push = function(self, inCall)
        if inCall.mDrawType then
            self.mDrawables[#self.mDrawables + 1] = inCall
            return #self.mDrawables
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
            if self.mOneTimeDrawables[inFrame] then
                self.mOneTimeDrawables[inFrame][#self.mOneTimeDrawables[inFrame] + 1] = inCall
            else
                self.mOneTimeDrawables[inFrame] = {}
                self.mOneTimeDrawables[inFrame][#self.mOneTimeDrawables[inFrame] + 1] = inCall
            end
        elseif inCall.mUpdate then
            if self.mOneTimeUpdatables[inFrame] then
                self.mOneTimeUpdatables[inFrame][#self.mOneTimeUpdatables[inFrame] + 1] = inCall
            else
                self.mOneTimeUpdatables[inFrame] = {}
                self.mOneTimeUpdatables[inFrame][#self.mOneTimeUpdatables[inFrame] + 1] = inCall
            end
        elseif inCall.mDispatch then
            if self.mOneTimeDispatchables[inFrame] then
                self.mOneTimeDispatchables[inFrame][#self.mOneTimeDispatchables[inFrame] + 1] = inCall
            else
                self.mOneTimeDispatchables[inFrame] = {}
                self.mOneTimeDispatchables[inFrame][#self.mOneTimeDispatchables[inFrame] + 1] = inCall
            end
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
