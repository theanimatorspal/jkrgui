require("JkrGUIv2.CallBuffers")
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
    o.i = i
    o.w = w
    o.s = Jkr.CreateShapeRenderer(o.i, o.w)
    o.t = Jkr.CreateTextRendererBestTextAlt(o.i, o.s)
    o.c = Jkr.CreateCallBuffers()
    o.e = Jkr.CreateCallExecutor(o.c)
    o.WindowDimension = o.w:GetWindowDimension()

    o.CreateFont = function(inFontFileName, inFontSize)
        local font = {}
        font.mId = o.t:AddFontFace(inFontFileName, inFontSize)
        font.GetTextDimension = function(self, inText)
            return o.t:GetTextDimensions(inText, font.mId)
        end
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
        textLabel.PushId = o.c:Push(Jkr.CreateDrawable(textLabel.mId, nil, "TEXT", nil, inColor))

        textLabel.Update = function(self, inPosition_3f, inDimension_3f, inFont, inText, inColor)
            if inFont then self.mFont = inFont end
            if inText then self.mText = inText end
            if inText then
                o.t:Update(self.mId, self.mFont.mId, inPosition_3f, self.mText)
            else
                o.t:UpdatePosOnly(self.mId, self.mFont.mId, inPosition_3f, self.mText)
            end
            if inColor then
                o.c.mDrawables[self.PushId].mColor = inColor
            end
        end

        textLabel.Remove = function(self)
            -- TODO Implement Remove function
        end
        return textLabel
    end

    o.CreateSampledImage = function(inPosition_3f, inDimension_3f, inFileName, inNoDraw, inColor)
        local SampledImage = {}
        if (inFileName) then
            SampledImage.mId = o.s:AddImageByFileName(inFileName)
            SampledImage.mActualSize = o.s:GetImageSize(inFileName)
        else
            SampledImage.mId = o.s:AddImage(inDimension_3f.x, inDimension_3f.y)
            SampledImage.mActualSize = vec2(inDimension_3f.x, inDimension_3f.y)
        end
        local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill, uvec2(inDimension_3f.x, inDimension_3f.y))
        SampledImage.imageViewRect = o.s:Add(Rectangle, inPosition_3f)
        SampledImage.mColor = vec4(1, 1, 1, 1)
        if (inColor) then
            SampledImage.mColor = inColor
        end

        if (not inNoDraw) then
            SampledImage.DrawId = o.c:Push(Jkr.CreateDrawable(SampledImage.imageViewRect, false,
                "IMAGE",
                SampledImage.mId,
                SampledImage.mColor))
        end

        SampledImage.Update = function(self, inPosition_3f, inDimension_3f, inColor)
            local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill,
                uvec2(inDimension_3f.x, inDimension_3f.y))
            o.s:Update(SampledImage.imageViewRect, Rectangle, inPosition_3f)
            if inColor then
                o.c.mDrawables[self.DrawId].mColor = inColor
            end
        end

        SampledImage.CopyToCompute = function(inComputeImage)
            o.s:CopyFromImage(SampledImage.mId, inComputeImage.mId)
        end
        return SampledImage
    end

    --[============================================================[
                    COMPUTE IMAGE
                    rendering onto an image using compute shaders,
                    can also create a button by Image.CreateButton() routine
          ]============================================================]
    o.CreateComputeImage = function(inPosition_3f, inDimension_3f, inOptCompatibleSampledImage)
        local ComputeImage = {}
        if inOptCompatibleSampledImage then
            ComputeImage.mId = Jkr.CreateCustomPainterImage(o.i, o.w,
                math.int(inOptCompatibleSampledImage.mActualSize.x),
                math.int(inOptCompatibleSampledImage.mActualSize.y))
        else
            ComputeImage.mId = Jkr.CreateCustomPainterImage(o.i, o.w, math.int(inDimension_3f.x),
                math.int(inDimension_3f.y))
        end

        ComputeImage.RegisterPainter = function(inPainter)
            ComputeImage.mId:Register(o.i, inPainter.handle)
        end
        ComputeImage.BindPainter = function(inPainter)
            inPainter:Bind(o.w, Jkr.CmdParam.None)
            inPainter:BindImageFromImage(o.w, ComputeImage, Jkr.CmdParam.None)
        end
        ComputeImage.DrawPainter = function(inPainter, inPushConstant, inX, inY, inZ)
            inPainter:Draw(o.w, inPushConstant, inX, inY, inZ, Jkr.CmdParam.None)
        end
        ComputeImage.CopyToSampled = function(inSampledImage)
            o.s:CopyToImage(inSampledImage.mId, ComputeImage.mId)
        end
        return ComputeImage
    end

    o.CreateButton = function(inPosition_3f, inDimension_3f, inOnClickFunction, inContinous)
        local Button = {}
        Button.mBoundedRect = {}
        Button.mBoundedRect.mDepthValue = math.int(inPosition_3f.z)
        Button.mBoundedRect.mId = e:SetBoundedRect(vec2(inPosition_3f.x, inPosition_3f.y),
            vec2(inDimension_3f.x, inDimension_3f.y), math.int(inPosition_3f.z))
        Button.mOnClickFunction = function() end
        Button.mOnHoverFunction = function() end
        if (inOnClickFunction) then
            Button.mOnClickFunction = inOnClickFunction
        end
        if inContinous then
            Button.mBoundedRect.mPushId = o.c:Push(Jkr.CreateUpdatable(
                function()
                    local over = e:IsMouseWithinAtTopOfStack(
                        Button.mBoundedRect.mId,
                        Button.mBoundedRect.mDepthValue
                    )
                    if e:IsLeftButtonPressedContinous() and over then
                        Button.mOnClickFunction()
                    end
                end
            ))
        else
            Button.mBoundedRect.mPushId = o.c:Push(Jkr.CreateEventable(
                function()
                    local over = e:IsMouseWithinAtTopOfStack(
                        Button.mBoundedRect.mId,
                        Button.mBoundedRect.mDepthValue
                    )
                    if e:IsLeftButtonPressed() and over then
                        Button.mOnClickFunction()
                    end
                end
            ))
        end
        Button.Update = function(self, inPosition_3f, inDimension_3f)
            Button.mBoundedRect.mDepthValue = math.int(inPosition_3f.z)
            e:UpdateBoundedRect(Button.mBoundedRect.mId, vec2(inPosition_3f.x, inPosition_3f.y),
                vec2(inDimension_3f.x, inDimension_3f.y), Button.mBoundedRect.mDepthValue)
        end
        return Button
    end

    o.UIMatrix = mat4(0.0)
    --[========================================================]
    --
    --
    --
    -- DRAW DISPATCH UPDATE EVENT Stuffs
    --
    --
    --
    --[========================================================]
    o.WindowDimension = o.w:GetWindowDimension()
    o.FrameDimension = o.w:GetOffscreenFrameDimension()
    o.UIMatrix = Jmath.Ortho(0.0, o.FrameDimension.x, 0.0, o.FrameDimension.y, 1000, -1000)

    o.Update = function(self)
        self.c:Update()
    end

    o.Draw = function(self)
        -- Optimize this
        self.s:BindShapes(self.w, Jkr.CmdParam.UI)
        for i = 1, #self.c.mDrawables, 1 do
            local drawable = self.c.mDrawables[i]
            self.s:BindFillMode(Jkr.FillType.Image, self.w, Jkr.CmdParam.UI)
            if drawable.mDrawType == "IMAGE" then
                self.s:BindImage(self.w, drawable.mImageId, Jkr.CmdParam.UI)
                self.s:Draw(self.w, drawable.mColor, drawable.mId, drawable.mId, self.UIMatrix,
                    Jkr.CmdParam.UI)
            end
            if drawable.mDrawType == "TEXT" then
                self.t:Draw(drawable.mId, self.w, drawable.mColor, self.UIMatrix, Jkr.CmdParam.UI)
            end
        end
    end

    o.Dispatch = function(self)
        self.s:Dispatch(self.w, Jkr.CmdParam.None)
        self.c:Dispatch()
    end

    o.Event = function(self)
        self.c:Event()
    end
    return o
end
