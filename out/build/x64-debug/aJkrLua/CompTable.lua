Com = {}
ComTable = {}

local com_i = 0
Com.NewComponent = function()
    com_i = com_i + 1
end

Com.Events = function()
    for _, com in ipairs(ComTable) do
        com:Event()
    end
end

Com.Draws = function()
    for _, com in ipairs(ComTable) do
        if com.SetScissor then com:SetScissor() end
        com:Draw()
        if com.SetScissor then Jkr.reset_scissor() end
    end
end

Com.GPS = {
    mPadding = 10,
    mPosition_3f = vec3(0, 0, 0),
    mDimension_3f = vec3(0, 0, 0),
    mBackDepthValue = 0,
    mFrontDepthValue = 0,
    New = function(self, inBackDepthValueAbsolute, inFrontDepthValueAbsolute, inPosition_3f)
        local Obj = {
            mPadding = 10,
            mPosition_3f = vec3(0, 0, 0),
            mDimension_3f = vec3(0, 0, 0),
            mBackDepthValue = inBackDepthValueAbsolute,
            mFrontDepthValue = inFrontDepthValueAbsolute
        }
        if not inPosition_3f then
            Obj.mPosition_3f = vec3(Obj.mPadding, Obj.mPadding, inBackDepthValueAbsolute)
        else
            Obj.mPosition_3f = vec3(inPosition_3f.x + Obj.mPadding, inPosition_3f.y + Obj.mPadding,
                inBackDepthValueAbsolute)
        end
        setmetatable(Obj, self)
        self.__index = self
        return Obj
    end,
    StartOver = function(self)
        self.mPosition_3f = vec3(self.mPadding, self.mPadding, self.mPosition_3f.z)
    end,
    PrintCurrent = function(self)
        print(
            string.format(
                [[
                    POS: vec3(%f, %f, %f)
                    DIMEN: vec3(%f, %f, %f)
                ]],
                self.mPosition_3f.x,
                self.mPosition_3f.y,
                self.mPosition_3f.z,
                self.mDimension_3f.x,
                self.mDimension_3f.y,
                self.mDimension_3f.z
            )
        )
    end,
    SetDimension = function(self, inDimension_3f)
        self.mDimension_3f = inDimension_3f
    end,
    Start = function(self)
        self.mPosition_3f = vec3(self.mPadding, self.mPadding, self.mPadding)
    end,
    MoveDown = function(self)
        self.mPosition_3f = vec3(
            self.mPosition_3f.x,
            self.mPosition_3f.y + self.mDimension_3f.y + self.mPadding,
            self.mPosition_3f.z
        )
        self:PrintCurrent()
    end,
    MoveRight = function(self)
        self.mPosition_3f = vec3(
            self.mPosition_3f.x + self.mDimension_3f.x + self.mPadding,
            self.mPosition_3f.y,
            self.mPosition_3f.z
        )
        self:PrintCurrent()
    end,
    MoveUp = function(self)
        self.mPosition_3f = vec3(
            self.mPosition_3f.x,
            self.mPosition_3f.y - (self.mDimension_3f.y + self.mPadding),
            self.mPosition_3f.z
        )
        self:PrintCurrent()
    end,
    MoveLeft = function(self)
        self.mPosition_3f = vec3(
            self.mPosition_3f.x - (self.mDimension_3f.x + self.mPadding),
            self.mPosition_3f.y,
            self.mPosition_3f.z
        )
        self:PrintCurrent()
    end,
    MoveBackAbsolute = function(self)
        self.mPosition_3f = vec3(
            self.mPosition_3f.x,
            self.mPosition_3f.y,
            self.mBackDepthValue
        )
        self:PrintCurrent()
    end,
    MoveFrontAbsolute = function(self)
        self.mPosition_3f = vec3(
            self.mPosition_3f.x,
            self.mPosition_3f.y,
            self.mFrontDepthValue
        )
        self:PrintCurrent()
    end
}



Com.AreaObject = {
    mIds = vec2(0, 0),
    mPosition_3f = vec3(0, 0, 0),
    mDimension_3f = vec3(0, 0, 0),
    mAreaId = 0,
    mOutlineId = 0,
    mShadowId = 0,
    mIsResizable = false,
    mIsMovable = false,
    New = function(self, inPosition_3f, inDimension_3f)
        local Obj = {
            mIds = vec2(0, 0),
            mPosition_3f = vec3(0, 0, 0),
            mDimension_3f = vec3(0, 0, 0),
            mAreaId = 0,
            mOutlineId = 0,
            mShadowId = 0,
            mIsResizable = false,
            mIsMovable = false,
        }
        print("AreaObject Construction")
        setmetatable(Obj, self)
        self.__index = self
        Obj.mPosition_3f = inPosition_3f
        Obj.mDimension_3f = inDimension_3f

        local ShadowPos = vec3(inPosition_3f.x + 3, inPosition_3f.y + 3, inPosition_3f.z)
        local OutlinePos = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z - 1)
        local AreaPos = vec3(inPosition_3f.x + 1, inPosition_3f.y + 1, inPosition_3f.z - 2)
        local AreaDimen = vec3(inDimension_3f.x - 2, inDimension_3f.y - 2, inDimension_3f.z)

        Com.NewComponent()
        ComTable[com_i] = Jkr.Components.Static.ShapeObject:New(ShadowPos, inDimension_3f, nil, nil)
        ComTable[com_i].mFillColor = Theme.Colors.Shadow
        ComTable[com_i].mComponentObject.mFocusOnHover_b = false
        Obj.mIds.x = com_i
        Obj.mShadowId = com_i

        Com.NewComponent()
        ComTable[com_i] = Jkr.Components.Static.ShapeObject:New(OutlinePos, inDimension_3f, nil, nil)
        ComTable[com_i].mComponentObject.mFocusOnHover_b = false
        Obj.mIds.x = com_i
        Obj.mOutlineId = com_i

        Com.NewComponent()
        ComTable[com_i] = Jkr.Components.Static.ShapeObject:New(AreaPos, AreaDimen, nil, nil)
        ComTable[com_i].mFillColor = vec4(1, 1, 1, 1)
        ComTable[com_i].mComponentObject.mFocusOnHover_b = false
        Obj.mIds.y = com_i
        Obj.mAreaId = com_i
        print("No Of Components", com_i)
        print("AreaObject Construction Finished")
        return Obj
    end,
    Update = function(self, inPosition_3f, inDimension_3f)
        self.mPosition_3f = inPosition_3f
        self.mDimension_3f = inDimension_3f
        local i = self.mIds.x
        local ShadowPos = vec3(inPosition_3f.x + 3, inPosition_3f.y + 3, inPosition_3f.z)
        local OutlinePos = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z - 1)
        local AreaPos = vec3(inPosition_3f.x + 1, inPosition_3f.y + 1, inPosition_3f.z - 2)
        local AreaDimen = vec3(inDimension_3f.x - 2, inDimension_3f.y - 2, inDimension_3f.z)
        ComTable[self.mShadowId]:Update(ShadowPos, inDimension_3f)
        ComTable[self.mOutlineId]:Update(OutlinePos, inDimension_3f)
        ComTable[self.mAreaId]:Update(AreaPos, AreaDimen)
    end,
    Event = function(self)
        local i = self.mIds.x
        local mousePos = E.get_relative_mouse_pos()
        local isFocusedForMovement = ComTable[self.mAreaId].mComponentObject.mFocus_b -- TopMost Area
        local isfocusedForResize = ComTable[self.mShadowId].mComponentObject.mFocus_b -- TopMost Area
        if self.mIsMovable and isFocusedForMovement then
            local new_pos = vec3(self.mPosition_3f.x + mousePos.x, self.mPosition_3f.y + mousePos.y, self.mPosition_3f.z)
            self:Update(new_pos, self.mDimension_3f)
        elseif self.mIsResizable and isfocusedForResize then
            local new_dimen = vec3(self.mDimension_3f.x + mousePos.x, self.mDimension_3f.y + mousePos.y,
                self.mDimension_3f.z)
            self:Update(self.mPosition_3f, new_dimen)
        end
    end,
    Press = function(self)
        local p = self.mPosition_3f
        local d = self.mDimension_3f
        local ShadowPos = vec3(p.x + 3, p.y + 3, p.z)
        local OutlinePos = vec3(p.x, p.y, p.z - 1)
        local AreaPos = vec3(p.x + 1, p.y + 1, p.z - 2)
        local AreaDimen = vec3(d.x - 2, d.y - 2, d.z)
        AreaPos = ShadowPos
        OutlinePos = ShadowPos
        ComTable[self.mShadowId]:Update(ShadowPos, d)
        ComTable[self.mOutlineId]:Update(ShadowPos, d)
        ComTable[self.mAreaId]:Update(ShadowPos, AreaDimen)
    end
}

Com.TextLabelObject = {
    mIds = vec2(0, 0),
    mPositionToParent_3f = vec3(0, 0, 0),
    mPosition_3f = vec3(0, 0, 0),
    mDimension_3f = vec3(0, 0, 0),
    New = function(self, inText, inPosition_3f, inFontObject, inParent)
        print("TextLabelObject Construction")
        local Obj = {
            mIds = vec2(0, 0),
            mPosition_3f = vec3(0, 0, 0),
            mPositionToParent_3f = vec3(0, 0, 0),
            mDimension_3f = vec3(0, 0, 0)
        }
        setmetatable(Obj, self)
        self.__index = self
        Obj.mPosition_3f = inPosition_3f
        Obj.mPositionToParent_3f = inPosition_3f
        Com.NewComponent()
        Obj.mIds.x = com_i
        if inParent then
            Obj.SetParent(inParent)
        end
        ComTable[com_i] = Jkr.Components.Static.TextObject:New(inText, inPosition_3f, inFontObject)
        print("TextLabelObject Construction Finished")
        return Obj
    end,
    SetParent = function(self, inObject)
        local pos = vec3(inObject.mPosition_3f.x + self.mPositionToParent_3f.x,
            inObject.mPosition_3f.y + self.mPositionToParent_3f.y, self.mPosition_3f.z)
        self:Update(pos)
        if inObject.mPosition_3f.x > 0 and inObject.mPosition_3f.y > 0 then
            ComTable[self.mIds.x].mScissorPosition_2f = vec2(inObject.mPosition_3f.x, inObject.mPosition_3f.y)
            ComTable[self.mIds.x].mScissorDimension_2f = vec2(inObject.mDimension_3f.x, inObject.mDimension_3f.y)
        end
    end,
    Update = function(self, inPosition_3f)
        self.mPosition_3f = inPosition_3f
        ComTable[self.mIds.x]:Update(inPosition_3f)
    end
}

Com.NumberSliderObject = {
    mPositionToParent_3f = vec3(0, 0, 0),
    mPosition = vec3(0, 0, 0),
    mRodId = 0,
    mKnobId = 0,
    mValueRange = vec2(0, 0),
    mDimension_3f = vec3(0, 0, 0),
    mFactor = 0.0,
    mShouldSlide = false,
    New = function(self, inValue, inValueRangle_2f, inPosition_3f, inDimension_3f)
        print("NumberSliderObject Construction")
        local Obj = {
            mRodId = 0,
            mKnobId = 0,
            mValueRange = inValueRangle_2f,
            mPosition_3f = inPosition_3f,
            mDimension_3f = inDimension_3f,
            mPositionToParent_3f = inPosition_3f,
            mFactor = 0.0
        }
        setmetatable(Obj, self)
        self.__index = self
        local p = inPosition_3f
        local d = inDimension_3f
        local RodHeight = 10
        local RodPosition = vec3(p.x, p.y + d.y / 2 - RodHeight / 2, p.z)
        local RodDimension = vec3(d.x, RodHeight, 1)

        local Factor = inValue / (Obj.mValueRange.y - Obj.mValueRange.x)
        local KnobWidth = 20
        local KnobPosition = vec3(p.x + d.x * Factor - KnobWidth / 2, p.y, p.z - 3)
        local KnobDimension = vec3(KnobWidth, d.y, d.z)
        Obj.mFactor = Factor
        Obj.mShouldSlide = false
        print(string.format(
            [[
                inPosition : vec3(%f, %f, %f),
                inDimension : vec3(%f, %f, %f),
                RodHeight : %f,
                RodPosition : vec3(%f, %f, %f),
                RodDimension : vec3(%f, %f, %f),
                Factor : %f,
                KnobWidth : %f,
                KnobPosition : vec3(%f, %f, %f),
                KnobDimension : vec3(%f, %f, %f)
            ]]
            , p.x, p.y, p.z,
            d.x, d.y, d.z, RodHeight, RodPosition.x, RodPosition.y, RodPosition.z, RodDimension.x,
            RodDimension.y, RodDimension.z, Factor, KnobWidth, KnobPosition.x, KnobPosition.y, KnobPosition.z,
            KnobDimension.x, KnobDimension.y, KnobDimension.z
        )
        )

        Obj.mRodId = Com.AreaObject:New(RodPosition, RodDimension)
        Obj.mKnobId = Com.AreaObject:New(RodPosition, RodDimension)
        print("NumberSLiderObject Construction Finished")
        return Obj
    end,
    Event = function(self)
        self.mKnobId:Event()
        local RelativeMousePos = E.get_relative_mouse_pos()

        local p = self.mKnobId.mPosition_3f
        local d = self.mKnobId.mDimension_3f
        local up = vec3(p.x, p.y, p.z)
        local Factor = (p.x - d.x) / self.mDimension_3f.x

        if ComTable[self.mKnobId.mAreaId].mComponentObject.mFocus_b or ComTable[self.mRodId.mAreaId].mComponentObject.mFocus_b then
            self.mShouldSlide = true
        end

        if self.mShouldSlide then
            up = vec3(p.x + RelativeMousePos.x, p.y, p.z)
            self.mKnobId:Update(up, d)
            if not E.is_left_button_pressed() then
                self.mShouldSlide = false
            end
        end

        local rodx = self.mRodId.mPosition_3f.x
        local rodw = self.mRodId.mDimension_3f.x
        if up.x < rodx  then
            up = vec3(rodx, p.y, p.z)
            self.mKnobId:Update(up, d)
        elseif up.x > rodx + rodw - d.x then
            up = vec3(rodx + rodw - d.x, p.y, p.z)
            self.mKnobId:Update(up, d)
        else
        end

        self.mFactor = Factor
        print(Factor)
    end,
    SetParent = function(self, inObject)
        local pos = vec3(inObject.mPosition_3f.x + self.mPositionToParent_3f.x,
            inObject.mPosition_3f.y + self.mPositionToParent_3f.y, self.mPosition_3f.z)
        local p = pos
        local d = self.mDimension_3f
        local RodHeight = 10
        local RodPosition = vec3(p.x, p.y + d.y / 2 - RodHeight / 2, p.z)
        local RodDimension = vec3(d.x, RodHeight, d.z)

        local Factor = self.mFactor
        local KnobWidth = 20
        local KnobPosition = vec3(p.x + d.x * Factor - KnobWidth / 2, p.y, p.z)
        local KnobDimension = vec3(KnobWidth, d.y, d.z)

        print(string.format(
            [[
                inPosition : vec3(%f, %f, %f),
                inDimension : vec3(%f, %f, %f),
                RodHeight : %f,
                RodPosition : vec3(%f, %f, %f),
                RodDimension : vec3(%f, %f, %f),
                Factor : %f,
                KnobWidth : %f,
                KnobPosition : vec3(%f, %f, %f),
                KnobDimension : vec3(%f, %f, %f)
            ]]
            , p.x, p.y, p.z,
            d.x, d.y, d.z, RodHeight, RodPosition.x, RodPosition.y, RodPosition.z, RodDimension.x,
            RodDimension.y, RodDimension.z, Factor, KnobWidth, KnobPosition.x, KnobPosition.y, KnobPosition.z,
            KnobDimension.x, KnobDimension.y, KnobDimension.z
        )
        )
        self.mRodId:Update(RodPosition, RodDimension)
        self.mKnobId:Update(KnobPosition, KnobDimension)

        -- if inObject.mPosition_3f.x > 0 and inObject.mPosition_3f.y > 0 then
        --     ComTable[self.mRodId].mScissorPosition_2f = vec2(inObject.mPosition_3f.x, inObject.mPosition_3f.y)
        --     ComTable[self.mRodId].mScissorDimension_2f = vec2(inObject.mDimension_3f.x, inObject.mDimension_3f.y)
        -- end
        -- TODO
    end
}

Com.ListSelectorObject = {

}


Com.TextButtonObject = {
    mPositionToParent_3f = vec3(0, 0, 0),
    mPadding = 5,
    mTextObject = nil,
    mFunction = nil,
    mPressed = false,
    New = function(self, inText, inFontObject, inPosition_3f, inDimension_3f, inParent)
        print("TextButtonObject")
        local Obj = Com.AreaObject:New(inPosition_3f, inDimension_3f)
        setmetatable(self, Com.AreaObject) -- Inherits Com.AreaObject
        setmetatable(Obj, self)
        self.__index = self

        Obj.mTextObject = {}
        Obj.mPositionToParent_3f = {}
        Obj.mPadding = {}
        Obj.mFunction = {}
        Obj.mPressed = {}
        Obj.mPressed = false
        Obj.mPadding = 5
        Obj.mPositionToParent_3f = inPosition_3f
        local Position = vec3(inPosition_3f.x + Obj.mPadding, inPosition_3f.y + inDimension_3f.y - Obj.mPadding,
            inPosition_3f.z - 3)
        Obj.mTextObject = Com.TextLabelObject:New(inText, Position, inFontObject)
        if inParent then
            Obj.SetParent(inParent)
        end
        print("TextButtonObject Construction Finished")
        return Obj
    end,
    Animation = function(self)
    end,
    Event = function(self)
        if ComTable[self.mAreaId].mComponentObject.mFocus_b then
            self.mPressed = true
            self:Press()
            self.mTextObject:Update(ComTable[self.mAreaId].mPosition_3f)
        else
            self.mPressed = false
        end
    end,
    SetParent = function(self, inObject)
        local pos = vec3(inObject.mPosition_3f.x + self.mPositionToParent_3f.x,
            inObject.mPosition_3f.y + self.mPositionToParent_3f.y, self.mPosition_3f.z)
        self:Update(pos, self.mDimension_3f)
        self.mTextObject:SetParent(inObject)
    end,
    SetFunction = function(self, inFunction)
        self.mFunction = inFunction
    end
}

Com.TextCursorObject = {
    mWidth = 0,
    mHeight = 0,
    mShapeId = 0,
    New = function(self, inPosition_3f, inDimension_3f, inColor_4f)
        print("TextCursorObject Construction")
        local Obj = {}
        setmetatable(Obj, self)
        self.__index = self
        Com.NewComponent()
        ComTable[com_i] = Jkr.Components.Static.ShapeObject:New(inPosition_3f, inDimension_3f, nil, nil)
        ComTable[com_i].mFillColor = inColor_4f
        print("TextCursorObject Construction Finished")
        Obj.mWidth = inDimension_3f.x
        Obj.mShapeId = com_i
        Obj.mHeight = inDimension_3f.y
        return Obj
    end,
    Update = function(self, inPosition_3f, inDimension_3f)
        ComTable[self.mShapeId]:Update(inPosition_3f, inDimension_3f)
    end
}

Com.TextLineEditObject = {
    mPositionToParent_3f = vec3(0, 0, 0),
    mPadding = 5,
    mTextObject = nil,
    mFunction = nil,
    mPressed = false,
    mStartString = " ",
    mCurrentString = " ",
    mTextCursorObject = nil,
    mTextObjectId = 0,
    mShouldUpdate = false,
    mShouldInputText = false,
    New = function(self, inPosition_3f, inDimension_3f, inCursorWidth, inFontObject, inMaxStringLength, inParent)
        print("TextLineEditObject Construction")
        local Obj = Com.AreaObject:New(inPosition_3f, inDimension_3f)
        setmetatable(self, Com.AreaObject) -- Inherits Com.AreaObject
        setmetatable(Obj, self)
        self.__index = self

        Obj.mPositionToParent_3f = inPosition_3f
        Obj.mPadding = 5
        Obj.mPressed = false
        Obj.mStartString = string.rep(" ", inMaxStringLength)
        Obj.mCurrentString = ""
        local CursorObjectPosition = vec3(inPosition_3f.x + inCursorWidth, inPosition_3f.y, inPosition_3f.z - 3)
        Obj.mTextCursorObject = Com.TextCursorObject:New(CursorObjectPosition,
            vec3(inCursorWidth, inDimension_3f.y, inDimension_3f.z - 1), Theme.Colors.Text.Cursor.Normal)
        Obj.mShouldUpdate = false
        Obj.mShouldInputText = false

        Com.NewComponent()
        local TextPosition = vec3(inPosition_3f.x, inPosition_3f.y + inDimension_3f.y, inPosition_3f.z)
        ComTable[com_i] = Jkr.Components.Static.TextObject:New(Obj.mStartString, inPosition_3f, inFontObject)
        Obj.mTextObjectId = com_i
        if inParent then
            Obj:SetParent(inParent)
        end
        print("TextLineEditObject Construction Finished")
        return Obj
    end,
    Event = function(self)
        local isClickedOn = ComTable[self.mAreaId].mComponentObject.mFocus_b

        if isClickedOn then
            self.mShouldInputText = not self.mShouldInputText
            ComTable[self.mTextCursorObject.mShapeId].mFillColor = Theme.Colors.Text.Cursor.Active
        elseif not isClickedOn and E.is_left_button_pressed() then
            self.mShouldInputText = false
            ComTable[self.mTextCursorObject.mShapeId].mFillColor = Theme.Colors.Text.Cursor.Normal
        end

        if self.mShouldInputText then
            local is_backspace = E.is_key_pressed(Key.SDLK_BACKSPACE)
            local is_enter = E.is_key_pressed(Key.SDLK_RETURN)
            if E.is_text_being_input() and not is_backspace then
                self.mCurrentString = self.mCurrentString .. E.get_input_text()
                self.mShouldUpdate = true
            end
            if E.is_keypress_event() then
                if is_backspace and utf8.len(self.mCurrentString) >= 0 then
                    self.mShouldUpdate = true
                    self.mCurrentString = utf8.sub(self.mCurrentString, 1, -2)
                elseif is_enter then
                    self.mShouldInputText = false
                    ComTable[self.mTextCursorObject.mShapeId].mFillColor = Theme.Colors.Text.Cursor.Normal
                end
            end
        end

        if self.mShouldUpdate then
            ComTable[self.mTextObjectId].mString = self.mStartString
            ComTable[self.mTextObjectId]:Update(self.mPosition_3f)

            if self.mCurrentString == "" then
                ComTable[self.mTextObjectId].mString = " "
            else
                ComTable[self.mTextObjectId].mString = self.mCurrentString
            end

            local TextPosition = vec3(self.mPosition_3f.x + self.mPadding,
                self.mPosition_3f.y + self.mDimension_3f.y - self.mPadding, self.mPosition_3f.z - 3)
            ComTable[self.mTextObjectId]:Update(TextPosition)
            local CursorPosByTypedText = ComTable[self.mTextObjectId].mFont:GetDimension(self.mCurrentString)
            local CursorObjectPosition = vec3(
                CursorPosByTypedText.x + self.mPosition_3f.x + self.mTextCursorObject.mWidth, self.mPosition_3f.y,
                self.mPosition_3f.z - 3)
            local CursorObjectDimension = vec3(self.mTextCursorObject.mWidth, self.mDimension_3f.y,
                self.mDimension_3f.z - 1)
            self.mTextCursorObject:Update(CursorObjectPosition, CursorObjectDimension)
            self.mShouldUpdate = false
        end
    end,
    SetParent = function(self, inObject)
        local pos = vec3(inObject.mPosition_3f.x + self.mPositionToParent_3f.x,
            inObject.mPosition_3f.y + self.mPositionToParent_3f.y, self.mPosition_3f.z)
        self:Update(pos, self.mDimension_3f)

        if self.mCurrentString == "" then
            local CursorObjectPosition = vec3(pos.x + self.mTextCursorObject.mWidth, pos.y,
                self.mPosition_3f.z - 3)
            local CursorObjectDimension = vec3(self.mTextCursorObject.mWidth, self.mDimension_3f.y,
                self.mDimension_3f.z - 1)
            self.mTextCursorObject:Update(CursorObjectPosition, CursorObjectDimension)
        end
        if inObject.mPosition_3f.x > 0 and inObject.mPosition_3f.y > 0 then
            ComTable[self.mTextObjectId].mScissorPosition_2f = vec2(inObject.mPosition_3f.x, inObject.mPosition_3f.y)
            ComTable[self.mTextObjectId].mScissorDimension_2f = vec2(inObject.mDimension_3f.x, inObject.mDimension_3f.y)
        end
        self.mShouldUpdate = true
    end,
}

Com.TextMultiLineEditObject = {
    mPositionToParent_3f = vec3(0, 0, 0),
    mPadding = 5,
    mTextObject = nil,
    mFunction = nil,
    mPressed = false,
    mTextCursorObject = nil,
    mShouldUpdate = false,
    mShouldInputText = false,
    mCurrentLine = 1,
    mTextObjectIds = {},
    mLineTexts = {},
    mMaxNoOfLines = nil,
    mMaxStringLength = nil,
    mString = "",
    mStringCurrentLine = 1,
    mShouldAddCharacter = false,
    mAddCharacter = "",
    New = function(self, inPosition_3f, inDimension_3f, inCursorWidth, inCursorHeight, inFontObject, inMaxNoOfLines,
                   inMaxStringLength, inParent)
        print("TextMultiLineEditObject Construction")
        local Obj = Com.AreaObject:New(inPosition_3f, inDimension_3f)
        setmetatable(self, Com.AreaObject) -- Inherits Com.AreaObject
        setmetatable(Obj, self)
        self.__index = self

        Obj.mPositionToParent_3f = inPosition_3f
        Obj.mPadding = 5
        Obj.mPressed = false
        local CursorObjectPosition = vec3(inPosition_3f.x + inCursorWidth, inPosition_3f.y, inPosition_3f.z - 3)
        Obj.mTextCursorObject = Com.TextCursorObject:New(CursorObjectPosition,
            vec3(inCursorWidth, inCursorHeight, inDimension_3f.z - 1), Theme.Colors.Text.Cursor.Normal)
        Obj.mShouldUpdate = false
        Obj.mShouldInputText = false
        Obj.mTextObjectIds = {}
        Obj.mLineTexts = {}
        Obj.mCurrentLine = 1
        Obj.mMaxNoOfLines = inMaxNoOfLines
        Obj.mMaxStringLength = inMaxStringLength
        Obj.mString = ""
        Obj.mStringCurrentLine = 1
        for i = 1, inMaxNoOfLines, 1 do
            Com.NewComponent()
            local TextPosition = vec3(inPosition_3f.x + Obj.mPadding, inPosition_3f.y + inCursorHeight * i,
                inPosition_3f.z)
            local StartString = string.rep(" ", inMaxStringLength)
            ComTable[com_i] = Jkr.Components.Static.TextObject:New(StartString, TextPosition, inFontObject)
            Obj.mTextObjectIds[#Obj.mTextObjectIds + 1] = com_i
            Obj.mLineTexts[#Obj.mLineTexts + 1] = ""
        end


        if inParent then
            Obj:SetParent(inParent)
        end
        print("TextMultiLineEditObject Construction Finished")
        return Obj
    end,
    SetVisualTextByString = function(self, inStartLine, inEndline)
        local i__ = 1
        local j__ = 1
        local end__ = inEndline or self.mMaxNoOfLines
        print(self.mString)

        print("-------------")
        print("-------------")
        local index = 0
        for Line in self.mString:gmatch("(.-)\n") do
            index = index + string.len(Line)
            print(Line)
        end
        local remaining_str = string.sub(self.mString, index)
        if remaining_str ~= "" then
            print(remaining_str)
        end
        print("-------------")
        print("-------------")


        for Line in self.mString:gmatch("(.-)\n") do
            if i__ <= self.mMaxNoOfLines and j__ >= inStartLine then
                if i__ <= inEndline then
                    self.mLineTexts[i__] = Line
                end
                i__ = i__ + 1
            end
            j__ = j__ + 1
            print(self.mString)
            print(
                string.format(
                    [[
                            i__  : %d,
                            j__ : %d,
                            inStartLine : %d,
                            inEndline : %d,
                            self.mMaxNoOfLines : %d
                            Line : %s,
                            self.mLineTexts[i__] : %s
                        ]], i__, j__, inStartLine, inEndline, self.mMaxNoOfLines, Line, self.mLineTexts[i__]
                )
            )
        end

        print(i__)
        for i = i__, self.mMaxNoOfLines, 1 do
            self.mLineTexts[i] = ""
        end

        -- This is when there is only one line in mString
        if i__ == 1 and inStartLine < inEndline then
            print("i__ == 1 and inStartLine < inEndline", i__ == 1 and inStartLine < inEndline)
            print(self.mString)
            self.mLineTexts[i__] = self.mString
        end


        for i = 1, i__, 1 do
            local t_obj = self.mTextObjectIds[i]
            ComTable[t_obj].mString = string.rep(" ", self.mMaxStringLength)
            ComTable[t_obj]:Update(self.mPosition_3f)
            if self.mLineTexts[i] == "" then
                ComTable[t_obj].mString = " "
            else
                ComTable[t_obj].mString = self.mLineTexts[i]
            end

            local TextPosition = vec3(self.mPosition_3f.x + self.mPadding,
                self.mPosition_3f.y + self.mTextCursorObject.mHeight * i,
                self.mPosition_3f.z - 3)
            ComTable[t_obj]:Update(TextPosition)

            local CursorPosByTypedText = ComTable[t_obj].mFont:GetDimension(self.mLineTexts[i])
            local CursorObjectPosition = vec3(
                CursorPosByTypedText.x + self.mPosition_3f.x,
                self.mPosition_3f.y + (self.mTextCursorObject.mHeight) * (i - 1),
                self.mPosition_3f.z - 3)
            local CursorObjectDimension = vec3(self.mTextCursorObject.mWidth, self.mTextCursorObject.mHeight,
                self.mDimension_3f.z - 1)
            self.mTextCursorObject:Update(CursorObjectPosition, CursorObjectDimension)
            self.mCurrentLine = i
        end
        self.mShouldUpdate = true
    end,
    AddCharacter = function(self, inShouldAddAuto, inS, inBackspace, inEnter)
        local eid = self.mCurrentLine
        local string_length = utf8.len(self.mLineTexts[eid])
        local CurrentTextObjectId = self.mTextObjectIds[eid]
        local CurrentTextDimension = ComTable[CurrentTextObjectId].mFont:GetDimension(self.mLineTexts[eid])


        if self.mShouldInputText or inShouldAddAuto then
            local is_backspace = E.is_key_pressed(Key.SDLK_BACKSPACE) or inBackspace
            local is_enter = E.is_key_pressed(Key.SDLK_RETURN) or inEnter
            local function dbg()
                if inShouldAddAuto then
                    print("Called Auto")
                    print(inS)
                    print("is_enter:", is_enter)
                end
            end

            if (E.is_text_being_input() or inShouldAddAuto) and not is_backspace then
                if inShouldAddAuto then
                    self.mLineTexts[eid] = self.mLineTexts[eid] .. inS
                    self.mString = self.mString .. inS
                else
                    self.mLineTexts[eid] = self.mLineTexts[eid] .. E.get_input_text()
                    self.mString = self.mString .. E.get_input_text()
                end
                self.mShouldUpdate = true
            end

            if E.is_keypress_event() or inShouldAddAuto then
                self.mShouldUpdate = true
                if is_backspace and string_length > 0 then
                    self.mLineTexts[eid] = utf8.sub(self.mLineTexts[eid], 1, -2)
                    if self.mString:sub(-1) == "\n" then
                        self.mString = utf8.sub(self.mString, 1, -2)
                    end
                    self.mString = utf8.sub(self.mString, 1, -2)
                elseif is_backspace and string_length == 0 and eid ~= 1 then
                    if self.mStringCurrentLine <= self.mCurrentLine then
                        self.mCurrentLine = self.mCurrentLine - 1
                        self.mStringCurrentLine = self.mStringCurrentLine - 1
                    else
                        self.mStringCurrentLine = self.mStringCurrentLine - 1
                        self:SetVisualTextByString(self.mStringCurrentLine - self.mCurrentLine + 1, self.mMaxNoOfLines)
                    end
                elseif is_enter or string_length >= self.mMaxStringLength or CurrentTextDimension.x >= self.mDimension_3f.x - self.mPadding * 4 then
                    self.mStringCurrentLine = self.mStringCurrentLine + 1
                    self.mString = self.mString .. "\n"

                    if self.mStringCurrentLine <= self.mMaxNoOfLines then
                        self.mCurrentLine = self.mCurrentLine + 1
                    else
                        self:SetVisualTextByString(self.mStringCurrentLine - self.mCurrentLine + 1,
                            self.mMaxNoOfLines - 1)
                    end
                end
            end
        end

        local eid = self.mCurrentLine

        if self.mShouldUpdate then
            local t_obj = self.mTextObjectIds[eid]

            ComTable[t_obj].mString = string.rep(" ", self.mMaxStringLength)
            ComTable[t_obj]:Update(self.mPosition_3f)


            if self.mLineTexts[eid] == "" then
                ComTable[t_obj].mString = " "
            else
                ComTable[t_obj].mString = self.mLineTexts[eid]
            end

            local TextPosition = vec3(self.mPosition_3f.x + self.mPadding,
                self.mPosition_3f.y + self.mTextCursorObject.mHeight * eid,
                self.mPosition_3f.z - 3)
            ComTable[t_obj]:Update(TextPosition)
            local CursorPosByTypedText = ComTable[t_obj].mFont:GetDimension(self.mLineTexts[eid])
            local CursorObjectPosition = vec3(
                CursorPosByTypedText.x + self.mPosition_3f.x,
                self.mPosition_3f.y + (self.mTextCursorObject.mHeight) * (eid - 1),
                self.mPosition_3f.z - 3)
            local CursorObjectDimension = vec3(self.mTextCursorObject.mWidth, self.mTextCursorObject.mHeight,
                self.mDimension_3f.z - 1)
            self.mTextCursorObject:Update(CursorObjectPosition, CursorObjectDimension)
            self.mShouldUpdate = false
            -- print(string.format(
            --     [[
            --         EID: %d
            --         LINETEXT: %s
            --         POS: vec3(%f, %f, %f)
            --         STRLEN: %d
            --         ]],
            --     eid,
            --     self.mLineTexts[eid],
            --     TextPosition.x,
            --     TextPosition.y,
            --     TextPosition.z,
            --     string_length
            -- ))
        end
    end,
    Event = function(self)
        local isClickedOn = ComTable[self.mAreaId].mComponentObject.mFocus_b
        if isClickedOn then
            self.mShouldInputText = not self.mShouldInputText
            ComTable[self.mTextCursorObject.mShapeId].mFillColor = Theme.Colors.Text.Cursor.Active
        elseif not isClickedOn and E.is_left_button_pressed() then
            self.mShouldInputText = false
            ComTable[self.mTextCursorObject.mShapeId].mFillColor = Theme.Colors.Text.Cursor.Normal
        end
        self:AddCharacter(false)
    end

}
