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
        if not inPadding then
            Obj.mPadding = 0
        end
        setmetatable(Obj, self)
        self.__index = self

        return Obj
    end,
    Add = function(self, inComponentListTable, inRatioTable)
        self.mComponents = inComponentListTable
        self.mRatioTable = inRatioTable
        return self
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
        if not inPadding then
            Obj.mPadding = 0
        end
        setmetatable(Obj, self)
        self.__index = self
        self.__call = Jkr.HLayout.New
        return Obj
    end,
    Add = function(self, inComponentListTable, inRatioTable)
        self.mComponents = inComponentListTable
        self.mRatioTable = inRatioTable
        return self
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
    Add = function(self, inComponentListTable)
        self.mComponents = inComponentListTable
        return self
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
    o.t = Jkr.CreateTextRendererBestTextAlt(o.i, o.s) -- for now the idea has been dropped

    o.c = Jkr.CreateCallBuffers()
    o.e = Jkr.CreateCallExecutor(o.c)
    o.WindowDimension = o.w:GetWindowDimension()

    o.shape2dShaders = {}
    o.shape2dShaders.roundedRectangle = Jkr.Simple3D(i, w)
    o.shape2dShaders.roundedRectangle:Compile(
        i,
        w,
        "cache2/o.shape2dShaders.roundedRectangle.glsl",
        Shape2DShaders.GeneralVShader.str,
        Shape2DShaders.RoundedRectangleFShader.str,
        "",
        false
    )
    o.shape2dShaders.showImage = Jkr.Simple3D(i, w)
    o.shape2dShaders.showImage:Compile(
        i,
        w,
        "cache2/o.shape2dShaders.showImage.glsl",
        Shape2DShaders.GeneralVShader.str,
        Shape2DShaders.ShowImageFShader.str,
        "",
        false
    )

    o.CreateFont = function(inFontFileName, inFontSize)
        local font = {}
        font.mId = o.t:AddFontFace(inFontFileName, inFontSize)
        font.GetTextDimension = function(self, inText)
            return o.t:GetTextDimensions(inText, font.mId)
        end
        return font
    end

    ---@note Scissor are viewport both are created unifiedly i.e. both should always be in the same dimension

    o.CreateScissor = function(inPosition_3f, inDimension_3f, inShouldSetViewport)
        ---@warning mImageId -> inPosition_3f
        ---@warning mColor -> inDimension_3f
        ---@warning mPush -> inShouldSetViewport
        return o.c:Push(Jkr.CreateDrawable("START", false, "SCISSOR_VIEWPORT", inPosition_3f, inDimension_3f,
            inShouldSetViewport))
    end

    o.mCurrentScissor = 1

    o.SetCurrentScissor = function(inScissorId)
        if not inScissorId then
            o.mCurrentScissor = 1
        else
            o.mCurrentScissor = inScissorId
        end
    end


    -- @warning inShape2DShader refers to the STRING value of o.shape2dShaders.<shader>
    -- e.g. for rounded rectangle use "roundedRectangle"
    -- @warning the second matrix in the push constant cannot be used for anything because it will be sent with the UIMatrix
    o.CreateQuad = function(inPosition_3f, inDimension_3f, inPushConstant, inShape2DShader, inSampledImageId)
        local quad = {}
        local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill, uvec2(inDimension_3f.x, inDimension_3f.y))
        quad.rect = o.s:Add(Rectangle, inPosition_3f)
        quad.mColor = vec4(1, 1, 1, 1)

        -- @warning This might not be batchable @todo create a parameter or do something else about it
        quad.DrawId = o.c:Push(Jkr.CreateDrawable(quad.rect, true,
            inShape2DShader,
            inSampledImageId,
            nil, inPushConstant), o.mCurrentScissor)

        quad.Update = function(self, inPosition_3f, inDimension_3f, inPushConstant)
            local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill,
                uvec2(inDimension_3f.x, inDimension_3f.y))
            o.s:Update(quad.rect, Rectangle, inPosition_3f)
            if inPushConstant then
                o.c.mDrawables[self.DrawId].mPush = inPushConstant
            end
        end

        return quad
    end

    --[============================================================[
                    TEXT LABEL
          ]============================================================]
    o.CreateTextLabel = function(inPosition_3f, inDimension_3f, inFont, inText, inColor)
        local textLabel = {}
        textLabel.mText = inText
        textLabel.mFont = inFont
        textLabel.mId = o.t:Add(inFont.mId, inPosition_3f, inText)
        textLabel.PushId = o.c:Push(Jkr.CreateDrawable(textLabel.mId, nil, "TEXT", nil, inColor), o.mCurrentScissor)

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
        -- local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill, uvec2(inDimension_3f.x, inDimension_3f.y))
        -- SampledImage.imageViewRect = o.s:Add(Rectangle, inPosition_3f)
        -- SampledImage.mColor = vec4(1, 1, 1, 1)

        if (inColor) then
            SampledImage.mColor = inColor
        end

        ---@note @warning Sampled IMage is not for drawing
        -- if (not inNoDraw) then
        --     -- SampledImage.DrawId = o.c:Push(Jkr.CreateDrawable(SampledImage.imageViewRect, false,
        --     --     "IMAGE",
        --     --     SampledImage.mId,
        --     --     SampledImage.mColor), o.mCurrentScissor)
        -- end

        -- SampledImage.Update = function(self, inPosition_3f, inDimension_3f, inColor)
        --     -- local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill,
        --     --     uvec2(inDimension_3f.x, inDimension_3f.y))
        --     -- o.s:Update(SampledImage.imageViewRect, Rectangle, inPosition_3f)
        --     -- if inColor then
        --     --     o.c.mDrawables[self.DrawId].mColor = inColor
        --     -- end
        -- end

        SampledImage.CopyToCompute = function(inComputeImage)
            o.s:CopyFromImage(SampledImage.mId, inComputeImage.handle)
        end

        SampledImage.CopyDeferredImageFromWindow = function(inWindow)
            Jkr.CopyWindowDeferredImageToShapeImage(inWindow, o.s.handle, SampledImage.mId)
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
            ComputeImage.handle = Jkr.CreateCustomPainterImage(o.i, o.w,
                math.int(inOptCompatibleSampledImage.mActualSize.x),
                math.int(inOptCompatibleSampledImage.mActualSize.y))
        else
            ComputeImage.handle = Jkr.CreateCustomPainterImage(o.i, o.w, math.int(inDimension_3f.x),
                math.int(inDimension_3f.y))
        end

        ComputeImage.RegisterPainter = function(inPainter, inIndex)
            if not inIndex then inIndex = 0 end
            ComputeImage.handle:Register(o.i, inPainter.handle, inIndex)
        end
        ComputeImage.BindPainter = function(inPainter)
            inPainter:Bind(o.w, Jkr.CmdParam.None)
            inPainter:BindImageFromImage(o.w, ComputeImage, Jkr.CmdParam.None)
        end
        ComputeImage.DrawPainter = function(inPainter, inPushConstant, inX, inY, inZ)
            inPainter:Draw(o.w, inPushConstant, inX, inY, inZ, Jkr.CmdParam.None)
        end
        ComputeImage.CopyToSampled = function(inSampledImage)
            o.s:CopyToImage(inSampledImage.mId, ComputeImage.handle)
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

    local w = o.w
    local s = o.s
    local cmdparam = Jkr.CmdParam.UI
    local image_filltype = Jkr.FillType.Image
    local fill_filltype = Jkr.FillType.Fill
    local ui_matrix = o.UIMatrix
    o.DrawAll = function(self, inMap)
        for key, value in pairs(inMap) do
            if key ~= "TEXT" and key ~= "IMAGE" and key ~= "SCISSOR_VIEWPORT" then
                local shader = o.shape2dShaders[key]
                shader:Bind(w, cmdparam)
                local drawables = value
                local drawables_count = #value
                for i = 1, drawables_count, 1 do
                    local drawable = drawables[i]
                    if drawable.mImageId then
                        s:BindFillMode(image_filltype, w, cmdparam)
                        s:BindImage(w, drawable.mImageId, cmdparam)
                    else
                        s:BindFillMode(fill_filltype, w, cmdparam)
                    end
                    drawable.mPush.b = ui_matrix
                    Jkr.DrawShape2DWithSimple3D(w, shader, s.handle, drawable.mPush, drawable.mId, drawable
                        .mId,
                        cmdparam)
                end
            end
        end


        s:BindFillMode(image_filltype, self.w, cmdparam)
        do
            local drawables = inMap["TEXT"]
            if drawables then
                local drawables_count = #drawables
                for i = 1, drawables_count, 1 do
                    local drawable = drawables[i]
                    self.t:Draw(drawable.mId, w, drawable.mColor, ui_matrix, cmdparam)
                end
            end
        end
    end

    o.Draw = function(self)
        self.s:BindShapes(w, cmdparam)
        local SVs = o.c.mSVs
        local DrawablesInSVs = o.c.mDrawablesInSVs
        local count = #SVs
        ---@note The First one is always without Scissors
        o:DrawAll(DrawablesInSVs[1])
        for i = 2, count, 1 do
            local sv = SVs[i]

            ---@note SetScissor + Viewport
            if sv.mPush then
                o.w:SetViewport(sv.mImageId, sv.mColor, cmdparam)
            end
            o.w:SetScissor(sv.mImageId, sv.mColor, cmdparam)

            o:DrawAll(DrawablesInSVs[i])

            ---@note ResetScissor + Viewport
            o.w:SetDefaultViewport(cmdparam)
            o.w:SetDefaultScissor(cmdparam)
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
