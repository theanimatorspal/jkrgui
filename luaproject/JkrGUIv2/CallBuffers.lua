require "JkrGUIv2.require"

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
        local from_pos      = inFrom.mP
        local to_pos        = inTo.mP
        local from_dimen    = inFrom.mD
        local to_dimen      = inTo.mD
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

Jkr.CreateDrawable = function(inId, inBatchable, inDrawType, inImageId, inColor_4f, inPush, inMatrix)
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
