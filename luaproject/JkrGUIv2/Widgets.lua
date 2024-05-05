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

Jkr.HLayout = {
    mComponents = nil,
    mRatioTable = nil,
    mPadding = nil,
    mPosition_3f = nil,
    mDimension_3f = nil,

    New = function(self, inPadding)
        local Obj = {
            mPadding = inPadding
        }
        setmetatable(Obj, self)
        self.__index = self

        return Obj
    end,
    AddComponents = function(self, inComponentListTable, inRatioTable)
        self.mComponents = inComponentListTable
        self.mRatioTable = inRatioTable
    end,
    Update = function(self, inPosition_3f, inDimension_3f)
        local position = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z)
        local dimension = vec3(inDimension_3f.x, inDimension_3f.y, inDimension_3f.z)
        self.mPosition_3f = inPosition_3f
        self.mDimension_3f = inDimension_3f

        local paddingX = self.mPadding

        if self.mRatioTable then
            for index, value in ipairs(self.mComponents) do
                value:Update(vec3(position.x, position.y, position.z),
                    vec3(dimension.x * self.mRatioTable[index],
                        dimension.y, dimension.z),
                    self.mComponents[index].mText)
                position.x = position.x +
                    dimension.x * self.mRatioTable[index] + paddingX
            end
        end
    end,
    GetComponentPosition = function(self)
        local position = vec3(self.mPosition_3f.x, self.mPosition_3f.y, self.mPosition_3f.z)
        local dimension = vec3(self.mDimension_3f.x, self.mDimension_3f.y, self.mDimension_3f.z)
        local ComponentsPosition = {}
        for index, value in ipairs(self.mComponents) do
            ComponentsPosition[index] = vec3(position.x, position.y, position.z)
            position.x = position.x + dimension.x * self.mRatioTable[index] +
                self.mPadding
        end
        return ComponentsPosition
    end
}

Jkr.VLayout = {
    mComponents = nil,
    mRatioTable = nil,
    mPadding = nil,
    mPosition_3f = nil,
    mDimension_3f = nil,

    New = function(self, inPadding)
        local Obj = {
            mPadding = inPadding
        }
        setmetatable(Obj, self)
        self.__index = self
        return Obj
    end,
    AddComponents = function(self, inComponentListTable, inRatioTable)
        self.mComponents = inComponentListTable
        self.mRatioTable = inRatioTable
    end,
    Update = function(self, inPosition_3f, inDimension_3f)
        local position = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z)
        local dimension = vec3(inDimension_3f.x, inDimension_3f.y, inDimension_3f.z)
        self.mPosition_3f = vec3(position.x, position.y, position.z)
        self.mDimension_3f = vec3(dimension.x, dimension.y, dimension.z)
        local paddingY = self.mPadding
        if self.mRatioTable then
            for index, value in ipairs(self.mComponents) do
                value:Update(vec3(position.x, position.y, position.z),
                    vec3(dimension.x,
                        dimension.y *
                        self.mRatioTable[index],
                        dimension.z),
                    self.mComponents[index].mText)
                position.y = position.y +
                    dimension.y * self.mRatioTable[index] + paddingY
            end
        end
    end,
    GetComponentPosition = function(self)
        local position = vec3(self.mPosition_3f.x, self.mPosition_3f.y, self.mPosition_3f.z)
        local dimension = vec3(self.mDimension_3f.x, self.mDimension_3f.y, self.mDimension_3f.z)
        local ComponentsPosition = {}
        for index, value in ipairs(self.mComponents) do
            ComponentsPosition[index] = vec3(position.x, position.y, position.z)
            position.y = position.y + dimension.y * self.mRatioTable[index] +
                self.mPadding
        end
        return ComponentsPosition
    end
}

Jkr.StackLayout = {
    mComponents = nil,
    New = function(self, inChangingZvalue)
        local Obj = {
            mChangingZvalue = inChangingZvalue
        }
        setmetatable(Obj, self)
        self.__index = self
        return Obj
    end,
    AddComponents = function(self, inComponentListTable)
        self.mComponents = inComponentListTable
    end,
    Update = function(self, inPosition_3f, inDimension_3f)
        local position = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z)
        local dimension = vec3(inDimension_3f.x, inDimension_3f.y, inDimension_3f.z)
        self.mPosition_3f = inPosition_3f
        self.mDimension_3f = inDimension_3f
        for index, value in ipairs(self.mComponents) do
            value:Update(vec3(position.x, position.y, position.z),
                vec3(dimension.x, dimension.y, dimension.z),
                self.mComponents[index].mText)
            position.z = position.z - self.mChangingZvalue
        end
    end
}


Jkr.CreateWidgetRenderer = function(i, w, e)
    local o = {}
    o.s = Jkr.CreateShapeRenderer(i, w)
    o.t = Jkr.CreateTextRendererBestTextAlt(i, o.s)
    o.c = Jkr.CreateCallBuffers()
    o.e = Jkr.CreateCallExecutor(o.c)
    o.WindowDimension = w:GetWindowDimension()

    o.CreateFont = function(inFontFileName, inFontSize)
        local font = {}
        font.mId = o.t:AddFontFace(inFontFileName, inFontSize)
        return font
    end

    --[============================================================[
        TEXT LABEL
    ]============================================================]
    -- Here for each widget we have to follow function(inPosition, inDimension) style
    o.CreateTextLabel = function(inPosition_3f, inDimension_3f, inFont, inText, inColor)
        local textLabel = {}
        textLabel.mText = inText
        textLabel.mFont = inFont
        textLabel.mId = o.t:Add(inFont.mId, inPosition_3f, inText)
        textLabel.PushId = o.c.Push(Jkr.CreateDrawable(textLabel.mId, nil, "TEXT", nil, inColor))

        textLabel.Update = function(self, inPosition_3f, inDimension_3f, inFont, inText)
            if inFont then self.mFont = inFont end
            if inText then self.mText = inText end
            o.t:Update(self.mId, self.mFont.mId, inPosition_3f, self.mText)
        end

        textLabel.Remove = function(self)
            -- TODO Implement Remove function
        end
        return textLabel
    end

    --[============================================================[
        COMPUTE IMAGE
        rendering onto an image using compute shaders,
        can also create a button by Image.CreateButton() routine
    ]============================================================]
    o.CreateComputeImage = function(inPosition_3f, inDimension_3f, inNoDraw)
        local Image = {}
        Image.computeImage = Jkr.CreateCustomPainterImage(i, w, math.int(inDimension_3f.x), math.int(inDimension_3f.y))
        Image.sampledImage = o.s:AddImage(inDimension_3f.x, inDimension_3f.y)
        local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill, uvec2(inDimension_3f.x, inDimension_3f.y))
        Image.imageViewRect = o.s:Add(Rectangle, inPosition_3f)
        if (not inNoDraw) then
            Image.DrawId = o.c.Push(Jkr.CreateDrawable(Image.imageViewRect, false, "IMAGE", Image.sampledImage,
                vec4(1, 1, 1, 1)))
        end

        Image.RegisterPainter = function(inPainter)
            Image.computeImage:Register(i, inPainter.handle)
        end
        Image.BindPainter = function(inPainter)
            inPainter:Bind(w, Jkr.CmdParam.None)
            inPainter:BindImageFromImage(w, Image.computeImage, Jkr.CmdParam.None)
        end
        Image.DrawPainter = function(inPainter, inPushConstant, inX, inY, inZ)
            inPainter:Draw(w, inPushConstant, inX, inY, inZ, Jkr.CmdParam.None)
        end
        Image.CopyToSampled = function()
            o.s:CopyToImage(Image.sampledImage, Image.computeImage)
        end
        Image.CreateButton = function(inPosition_3f, inDimension_3f, inOnclickFunction)
            Image.buttonBoundedRect = {}
            Image.buttonBoundedRect.mDepthValue = math.int(inPosition_3f.z)
            Image.buttonBoundedRect.mId = e:SetBoundedRect(vec2(inPosition_3f.x, inPosition_3f.y),
                vec2(inDimension_3f.x, inDimension_3f.y), math.int(inPosition_3f.z))
            Image.buttonBoundedRect.mPushId = o.c.Push(Jkr.CreateUpdatable(function()
                local over = e:IsMouseWithinAtTopOfStack(Image.buttonBoundedRect.mId, Image.buttonBoundedRect
                    .mDepthValue)
                if e:IsLeftButtonPressed() and over then
                    inOnclickFunction()
                end
            end))
            Image.buttonBoundedRect.Update = function(inPosition_3f, inDimension_3f)
                local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill,
                    uvec2(inDimension_3f.x, inDimension_3f.y))
                o.s:Update(Image.imageViewRect, Rectangle, inPosition_3f)
                e:UpdateBoundedRect(Image.buttonBoundedRect.mId, vec2(inPosition_3f.x, inPosition_3f.y),
                    vec2(inDimension_3f.x, inDimension_3f.y), Image.buttonBoundedRect.mDepthValue)
            end
            return Image.buttonBoundedRect
        end
        return Image
    end

    o.CreateTextButton = function(inPosition_3f, inDimension_3f, inFont, inText, inTextColor, inBgColor)
        local textButton = {}
        textButton.mTextLabel = o.CreateTextLabel(inPosition_3f, inDimension_3f, inFont, inText, inTextColor)
        local Rect = Jkr.Generator(Jkr.Shapes.RectangleFill, uvec2(inDimension_3f.x))
        textButton.mId = o.s:Add(Rect)

        textButton.Update = function(self, inPosition_3f, inDimension_3f, inFont, inText, inTextColor, inBgColor)
            if (inFont) then textButton.mTextLabel.mFont = inFont end
            if (inText) then textButton.mTextLabel.mText = inText end
            -- TODO inTextColor and inBgColor
        end
    end

    --[========================================================]
    --
    --
    --
    -- DRAW DISPATCH UPDATE EVENT Stuffs
    --
    --
    --
    --[========================================================]
    o.Update = function()
        o.WindowDimension = w:GetWindowDimension()
        o.UIMatrix = Jmath.Ortho(0.0, o.WindowDimension.x, 0.0, o.WindowDimension.y, 1000, -1000)
        o.c.Update()
    end

    o.Draw = function()
        -- Optimize this
        o.s:BindShapes(w)
        for i = 1, #o.c.mDrawables, 1 do
            local drawable = o.c.mDrawables[i]
            o.s:BindFillMode(Jkr.FillType.Image, w, Jkr.CmdParam.UI)
            if drawable.mDrawType == "TEXT" then
                o.t:Draw(drawable.mId, w, drawable.mColor, o.UIMatrix, Jkr.CmdParam.UI)
            end
            if drawable.mDrawType == "IMAGE" then
                o.s:BindImage(w, drawable.mImageId, Jkr.CmdParam.UI)
                o.s:Draw(w, drawable.mColor, drawable.mId, drawable.mId, o.UIMatrix, Jkr.CmdParam.UI)
            end
        end
    end

    o.Dispatch = function()
        o.s:Dispatch(w, Jkr.CmdParam.None)
        o.c.Dispatch()
    end

    o.Event = function()
        o.c.Event()
    end
    return o
end
