require("JkrGUIv2.CallBuffers")
Jkr.PC          = function(a, b)
    local o = Jkr.Matrix2CustomImagePainterPushConstant()
    o.a = a or Jmath.GetIdentityMatrix4x4()
    o.b = b or Jmath.GetIdentityMatrix4x4()
    return o
end
Jkr.HLayout     = {
    mComponents = nil,
    mRatioTable = nil,
    mPadding = nil,
    mP = nil,
    mD = nil,

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
    Update = function(self, inP, inD)
        local position = vec3(inP.x, inP.y, inP.z)
        local dimension = vec3(inD.x, inD.y, inD.z)
        self.mP = inP
        self.mD = inD

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
        local position = vec3(self.mP.x, self.mP.y, self.mP.z)
        local dimension = vec3(self.mD.x, self.mD.y, self.mD.z)
        local ComponentsPosition = {}
        for index, value in ipairs(self.mComponents) do
            ComponentsPosition[index] = vec3(position.x, position.y, position.z)
            position.x = position.x + dimension.x * self.mRatioTable[index] +
                self.mPadding
        end
        return ComponentsPosition
    end
}

Jkr.VLayout     = {
    mComponents = nil,
    mRatioTable = nil,
    mPadding = nil,
    mP = nil,
    mD = nil,

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
    Update = function(self, inP, inD)
        local position = vec3(inP.x, inP.y, inP.z)
        local dimension = vec3(inD.x, inD.y, inD.z)
        self.mP = vec3(position.x, position.y, position.z)
        self.mD = vec3(dimension.x, dimension.y, dimension.z)
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
        local position = vec3(self.mP.x, self.mP.y, self.mP.z)
        local dimension = vec3(self.mD.x, self.mD.y, self.mD.z)
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
    Update = function(self, inP, inD)
        local position = vec3(inP.x, inP.y, inP.z)
        local dimension = vec3(inD.x, inD.y, inD.z)
        self.mP = inP
        self.mD = inD
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
    o.s = Jkr.CreateShapeRenderer(o.i, o.w, Jkr.GetDefaultCache(o.i, "Shape"))
    o.st = Jkr.CreateShapeRenderer(o.i, o.w, Jkr.GetDefaultCache(o.i, "Shape"))
    o.t = Jkr.CreateTextRendererBestTextAlt(o.i, o.st)

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
        MAIN_JKR_FILE
    )
    o.shape2dShaders.showImage = Jkr.Simple3D(i, w)
    o.shape2dShaders.showImage:Compile(
        i,
        w,
        "cache2/o.shape2dShaders.showImage.glsl",
        Shape2DShaders.GeneralVShader.str,
        Shape2DShaders.ShowImageFShader.str,
        "",
        MAIN_JKR_FILE
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

    o.scissor_matrices = {}
    o.CreateScissor = function(inP, inD, inShouldSetViewport, inMatrix)
        ---@warning mImageId -> inP
        ---@warning mColor -> inD
        ---@warning mPush -> inShouldSetViewport
        local sci = o.c:Push(Jkr.CreateDrawable("START", false, "SCISSOR_VIEWPORT", inP, inD,
            inShouldSetViewport))
        o.scissor_matrices[sci] = inMatrix or Jmath.GetIdentityMatrix4x4()
        return sci
    end

    o.mCurrentScissor = 1

    o.SetCurrentScissor = function(inScissorId)
        if not inScissorId then
            o.mCurrentScissor = 1
        else
            o.mCurrentScissor = inScissorId
        end
    end

    o.UpdateScissor = function(inScissorId, inP, inD, inShouldSetViewport)
        o.c.mSVs[inScissorId].mImageId = inP
        o.c.mSVs[inScissorId].mColor = inD
        o.c.mSVs[inScissorId].mPush = inShouldSetViewport
    end


    -- @warning inShape2DShader refers to the STRING value of o.shape2dShaders.<shader>
    -- e.g. for rounded rectangle use "roundedRectangle"
    -- @warning the second matrix in the push constant cannot be used for anything because it will be sent with the UIMatrix
    o.CreateQuad = function(inP, inD, inPushConstant, inShape2DShader, inSampledImageId, inMatrix)
        local quad = {}
        local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill, uvec2(inD.x, inD.y))
        quad.rect = o.s:Add(Rectangle, inP)
        quad.mColor = vec4(1, 1, 1, 1)

        -- @warning This might not be batchable @todo create a parameter or do something else about it
        quad.DrawId = o.c:Push(Jkr.CreateDrawable(quad.rect, true,
            inShape2DShader,
            inSampledImageId,
            nil, inPushConstant, inMatrix), o.mCurrentScissor)

        quad.Update = function(self, inP, inD, inPushConstant, inMatrix)
            local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill,
                uvec2(inD.x, inD.y))
            o.s:Update(quad.rect, Rectangle, inP)
            if inPushConstant then
                o.c.mDrawables[self.DrawId].mPush = inPushConstant
            end
            if inMatrix then
                o.c.mDrawables[self.DrawId].mMatrix = inMatrix
            end
        end

        return quad
    end

    --[============================================================[
                    TEXT LABEL
          ]============================================================]
    o.CreateTextLabel = function(inP, inD, inFont, inText, inColor, inNotDraw, inMatrix)
        local textLabel = {}
        textLabel.mText = inText
        textLabel.mFont = inFont
        textLabel.mId = o.t:Add(inFont.mId, inP, inText)
        if not inNotDraw then
            textLabel.PushId = o.c:Push(Jkr.CreateDrawable(textLabel.mId, nil, "TEXT", nil, inColor, nil, inMatrix),
                o.mCurrentScissor)
        end
        local alignx = -1
        local aligny = -1
        textLabel.Align = function(inAlignX, inAlignY)
            if inAlignX == "LEFT" then
                alignx = -1
            elseif inAlignX == "CENTER" then
                alignx = 0
            elseif inAlignX == "RIGHT" then
                alignx = 1
            end
            if inAlignY == "TOP" then
                aligny = -1
            elseif inAlignY == "BOTTOM" then
                aligny = 1
            elseif inAlignY == "CENTER" then
                aligny = 0
            end
        end
        local text_dimension = textLabel.mFont:GetTextDimension(textLabel.mText)
        textLabel.Update = function(self, inP, inD, inFont, inText, inColor, inMatrix)
            --tracy.ZoneBeginN("luatextUpdate")
            if inFont then self.mFont = inFont end
            if inText then self.mText = inText end
            text_dimension = self.mFont:GetTextDimension(self.mText)

            if alignx == 0 then
                inP.x = inP.x + text_dimension.x / 2
            elseif alignx == 1 then
                inP.x = inP.x - text_dimension.x
            end

            if aligny == 0 then
                inP.y = inP.y - text_dimension.y / 2
            elseif aligny == 1 then
                inP.y = inP.y - text_dimension.y
            end

            if inText then
                o.t:Update(self.mId, self.mFont.mId, inP, self.mText)
            else
                o.t:UpdatePosOnly(self.mId, self.mFont.mId, inP, self.mText)
            end

            if inColor then
                o.c.mDrawables[self.PushId].mColor = inColor
            end
            if inMatrix then
                o.c.mDrawables[self.PushId].mMatrix = inMatrix
            end
            --tracy.ZoneEnd()
        end

        textLabel.Remove = function(self)
            -- TODO Implement Remove function
        end
        return textLabel
    end

    ---@todo remove inNoDraw paramter
    o.CreateSampledImage = function(inP, inD, inFileName, inNoDraw, inColor)
        local SampledImage = {}
        if (inFileName) then
            SampledImage.mId = o.s:AddImageByFileName(inFileName)
            SampledImage.mActualSize = o.s:GetImageSize(inFileName)
        else
            SampledImage.mId = o.s:AddImage(inD.x, inD.y)
            SampledImage.mActualSize = vec2(inD.x, inD.y)
        end
        -- local Rectangle = Jkr.Generator(Jkr.Shapes.RectangleFill, uvec2(inD.x, inD.y))
        -- SampledImage.imageViewRect = o.s:Add(Rectangle, inP)
        -- SampledImage.mColor = vec4(1, 1, 1, 1)

        if (inColor) then
            SampledImage.mColor = inColor
        end

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
    o.CreateComputeImage = function(inP, inD, inOptCompatibleSampledImage)
        local ComputeImage = {}
        if inOptCompatibleSampledImage then
            ComputeImage.handle = Jkr.CreateCustomPainterImage(o.i, o.w,
                math.int(inOptCompatibleSampledImage.mActualSize.x),
                math.int(inOptCompatibleSampledImage.mActualSize.y))
        else
            ComputeImage.handle = Jkr.CreateCustomPainterImage(o.i, o.w, math.int(inD.x),
                math.int(inD.y))
        end

        ComputeImage.RegisterPainter = function(inPainter, inIndex)
            if not inIndex then inIndex = 0 end
            ComputeImage.handle:Register(o.i, inPainter.handle, inIndex)
        end
        ComputeImage.BindPainter = function(inPainter, inCmd)
            inPainter:Bind(o.w, inCmd or Jkr.CmdParam.None)
            inPainter:BindImageFromImage(o.w, ComputeImage, inCmd or Jkr.CmdParam.None)
        end
        ComputeImage.DrawPainter = function(inPainter, inPushConstant, inX, inY, inZ, inCmd)
            inPainter:Draw(o.w, inPushConstant, inX, inY, inZ, inCmd or Jkr.CmdParam.None)
        end
        ComputeImage.CopyToSampled = function(inSampledImage)
            o.s:CopyToImage(inSampledImage.mId, ComputeImage.handle)
        end
        ComputeImage.CopyFromWindowTargetImage = function(inw)
            Jkr.CopyWindowRenderTargetImageToCustomPainterImage(o.w, inw, ComputeImage.handle)
        end
        return ComputeImage
    end

    o.CreateButton = function(inP, inD, inOnClickFunction, inContinous)
        local Button = {}
        Button.mDepthValue = math.int(inP.z)
        Button.mId = e:SetBoundedRect(vec2(inP.x, inP.y),
            vec2(inD.x, inD.y), math.int(inP.z))
        Button.mOnClickFunction = function() end
        Button.mOnHoverFunction = function() end
        if (inOnClickFunction) then
            Button.mOnClickFunction = inOnClickFunction
            if inContinous then
                Button.mPushId = o.c:Push(Jkr.CreateUpdatable(
                    function()
                        local over = e:IsMouseWithinAtTopOfStack(
                            Button.mId,
                            Button.mDepthValue
                        )
                        if e:IsLeftButtonPressedContinous() and over then
                            Button.mOnClickFunction()
                        end
                    end
                ))
            else
                Button.mPushId = o.c:Push(Jkr.CreateEventable(
                    function()
                        local over = e:IsMouseWithinAtTopOfStack(
                            Button.mId,
                            Button.mDepthValue
                        )
                        if e:IsLeftButtonPressed() and over then
                            Button.mOnClickFunction()
                        end
                    end
                ))
            end
        end
        Button.Update = function(self, inP, inD)
            local new = math.int(inP.z)
            if new ~= Button.mDepthValue then
                e:RemoveBoundedRect(Button.mId, Button.mDepthValue)
                Button.mId = e:SetBoundedRect(vec2(inP.x, inP.y),
                    vec2(inD.x, inD.y), new)
                Button.mDepthValue = new
            end
            e:UpdateBoundedRect(Button.mId, vec2(inP.x, inP.y),
                vec2(inD.x, inD.y), Button.mDepthValue)
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

    o.SetUIMatrix = function(inMatrix)
        o.UIMatrix = inMatrix
    end

    o.Update = function(self)
        self.c:Update()
    end

    local w = o.w
    local s = o.s
    local st = o.st
    local t = o.t
    local cmdparam = Jkr.CmdParam.UI
    local image_filltype = Jkr.FillType.Image
    local fill_filltype = Jkr.FillType.Fill
    local ui_matrix = o.UIMatrix
    local s2ds = o.shape2dShaders
    local identity_matrix = Jmath.GetIdentityMatrix4x4()
    local DrawAll = function(inMap, inindex)
        local scissor_matrix = o.scissor_matrices[inindex]
        -- tracy.ZoneBeginN("luamainDrawALL")
        s:BindShapes(w, cmdparam)
        for key, value in pairs(inMap) do
            if key ~= "TEXT" and key ~= "IMAGE" and key ~= "SCISSOR_VIEWPORT" then
                local shader = s2ds[key]
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
                    drawable.mPush.b =
                        (drawable.mMatrix or ui_matrix) *
                        (scissor_matrix or identity_matrix)
                    Jkr.DrawShape2DWithSimple3D(w, shader, s.handle, drawable.mPush, drawable.mId, drawable
                        .mId,
                        cmdparam)
                end
            end
        end


        st:BindFillMode(image_filltype, w, cmdparam)
        st:BindShapes(w, cmdparam)
        do
            local drawables = inMap["TEXT"]
            if drawables then
                local drawables_count = #drawables
                for i = 1, drawables_count, 1 do
                    local drawable = drawables[i]
                    t:Draw(drawable.mId, w, drawable.mColor,
                        (drawable.mMatrix or ui_matrix) *
                        (scissor_matrix or identity_matrix),
                        cmdparam)
                end
            end
        end
        -- tracy.ZoneEnd()
    end

    o.Draw = function(self)
        --tracy.ZoneBeginN("luamainDraw")
        local SVs = o.c.mSVs
        local DrawablesInSVs = o.c.mDrawablesInSVs
        local count = #SVs
        ---@note The First one is always without Scissors
        DrawAll(DrawablesInSVs[1], 1)
        for i = 2, count, 1 do
            local sv = SVs[i]

            ---@note SetScissor + Viewport
            if sv.mPush then
                o.w:SetViewport(sv.mImageId, sv.mColor, cmdparam)
            end
            o.w:SetScissor(sv.mImageId, sv.mColor, cmdparam)

            DrawAll(DrawablesInSVs[i], i)

            ---@note ResetScissor + Viewport
            o.w:SetDefaultViewport(cmdparam)
            o.w:SetDefaultScissor(cmdparam)
        end
        --tracy.ZoneEnd()
    end

    o.DrawExplicit = function(self, inScissorIds)
        local SVs = o.c.mSVs
        local DrawablesInSVs = o.c.mDrawablesInSVs
        local count = #inScissorIds
        for i = 1, count, 1 do
            local j = inScissorIds[i]
            local sv = SVs[j]

            ---@note SetScissor + Viewport
            if sv.mPush then
                o.w:SetViewport(sv.mImageId, sv.mColor, cmdparam)
            end
            o.w:SetScissor(sv.mImageId, sv.mColor, cmdparam)

            DrawAll(DrawablesInSVs[j], j)

            ---@note ResetScissor + Viewport
            o.w:SetDefaultViewport(cmdparam)
            o.w:SetDefaultScissor(cmdparam)
        end
    end

    o.Dispatch = function(self)
        s:Dispatch(w, Jkr.CmdParam.None)
        st:Dispatch(w, Jkr.CmdParam.None)
        self.c:Dispatch()
    end

    o.Event = function(self)
        self.c:Event()
    end
    return o
end
