Com = {}
ComTable = {}

com_i = 0
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
    mAreaId = nil,
    mOutlineId = nil,
    mShadowId = nil,
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
        -- "AreaObject Construction")
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
        local sc = Theme.Colors.Shadow
        ComTable[com_i].mFillColor = vec4(sc.x, sc.y, sc.z, sc.w)
        ComTable[com_i].mComponentObject.mFocusOnHover_b = false
        -- com_i)
        Obj.mIds.x = com_i
        Obj.mShadowId = com_i

        Com.NewComponent()
        ComTable[com_i] = Jkr.Components.Static.ShapeObject:New(OutlinePos, inDimension_3f, nil, nil)
        ComTable[com_i].mComponentObject.mFocusOnHover_b = false
        local bc = Theme.Colors.Area.Border
        ComTable[com_i].mFillColor = vec4(bc.x, bc.y, bc.z, bc.w)
        -- com_i)
        Obj.mIds.x = com_i
        Obj.mOutlineId = com_i

        Com.NewComponent()
        ComTable[com_i] = Jkr.Components.Static.ShapeObject:New(AreaPos, AreaDimen, nil, nil)
        local nc = Theme.Colors.Area.Normal
        ComTable[com_i].mFillColor = vec4(nc.x, nc.y, nc.z, nc.w)
        ComTable[com_i].mComponentObject.mFocusOnHover_b = false
        -- com_i)
        Obj.mIds.y = com_i
        Obj.mAreaId = com_i
        -- "No Of Components", com_i)
        -- "AreaObject Construction Finished")
        return Obj
    end,
    TurnOffShadow = function(self)
        ComTable[self.mShadowId].mFillColor.w = 0
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
        -- "TextLabelObject Construction")
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
        -- "TextLabelObject Construction Finished")
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
    Update = function(self, inPosition_3f, inString)
        self.mPosition_3f = inPosition_3f
        if inString then
            ComTable[self.mIds.x].mString = inString
        end
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
        -- "NumberSliderObject Construction")
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
        -- string.format(
        Obj.mRodId = Com.AreaObject:New(RodPosition, RodDimension)
        Obj.mKnobId = Com.AreaObject:New(RodPosition, RodDimension)
        -- "NumberSLiderObject Construction Finished")
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
        if up.x < rodx then
            up = vec3(rodx, p.y, p.z)
            self.mKnobId:Update(up, d)
        elseif up.x > rodx + rodw - d.x then
            up = vec3(rodx + rodw - d.x, p.y, p.z)
            self.mKnobId:Update(up, d)
        else
        end

        self.mFactor = Factor
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

        self.mRodId:Update(RodPosition, RodDimension)
        self.mKnobId:Update(KnobPosition, KnobDimension)

        -- if inObject.mPosition_3f.x > 0 and inObject.mPosition_3f.y > 0 then
        --     ComTable[self.mRodId].mScissorPosition_2f = vec2(inObject.mPosition_3f.x, inObject.mPosition_3f.y)
        --     ComTable[self.mRodId].mScissorDimension_2f = vec2(inObject.mDimension_3f.x, inObject.mDimension_3f.y)
        -- end
        -- TODO
    end
}



Com.TextButtonObject = {
    mPositionToParent_3f = vec3(0, 0, 0),
    mPadding = 5,
    mTextObject = nil,
    mFunction = nil,
    mPressed = false,
    New = function(self, inText, inFontObject, inPosition_3f, inDimension_3f, inParent)
        -- "TextButtonObject")
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
            Obj:SetParent(inParent)
        end
        -- "TextButtonObject Construction Finished")
        return Obj
    end,
    Update = function(self, inPosition_3f, inDimension_3f, inString)
        Com.AreaObject.Update(self, inPosition_3f, inDimension_3f)
        local Position = vec3(inPosition_3f.x + self.mPadding, inPosition_3f.y + inDimension_3f.y - self.mPadding,
            inPosition_3f.z - 3)
        if inString then
            self.mTextObject:Update(Position, inString)
        end
    end,
    Event = function(self)
        if ComTable[self.mAreaId].mComponentObject.mFocus_b then
            self:Press()
            -- self.mTextObject:Update(ComTable[self.mAreaId].mPosition_3f)
            self.mPressed = true
        else
            self:Update(self.mPosition_3f, self.mDimension_3f)
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

Com.ListSelectorObject = {
    mPositionToParent_3f = nil,
    mPosition_3f = nil,
    mDimension_3f = nil,
    mLeftbutton = nil,
    mRightButton = nil,
    mTextArea = nil,
    mList = nil,
    mMaxChars = nil,
    mCurrentSelection = 1,
    mButtonWidth = 0,
    New = function(self, inList, inPosition_3f, inDimension_3f, inButtonWidth, inFontObject, inMaxChars, inParent)
        -- "List Selector Construction")
        local p = inPosition_3f
        local d = inDimension_3f
        local lb_p = vec3(p.x, p.y, p.z)
        local lb_d = vec3(inButtonWidth, d.y, d.z)
        local txt_p = vec3(p.x + lb_d.x, p.y, p.z)
        local txt_d = vec3(d.x - 2 * inButtonWidth, d.y, d.z)
        local rb_p = vec3(p.x + txt_d.x + inButtonWidth, p.y, p.z)
        local rb_d = vec3(inButtonWidth, d.y, d.z)
        -- inList[1])
        local Obj = {}
        Obj.mList = inList
        Obj.mCurrentSelection = 1
        Obj.mLeftbutton = Com.TextButtonObject:New(" <", inFontObject, lb_p, lb_d, inParent)
        Obj.mRightButton = Com.TextButtonObject:New(" >", inFontObject, rb_p, rb_d, inParent)
        Obj.mTextArea = Com.TextButtonObject:New(string.rep(" ", inMaxChars), inFontObject, txt_p, txt_d, inParent)
        Obj.mTextArea:Update(txt_p, txt_d, Obj.mList[Obj.mCurrentSelection])
        Obj.mPosition_3f = inPosition_3f
        Obj.mDimension_3f = inDimension_3f
        Obj.mPositionToParent_3f = inPosition_3f
        Obj.mButtonWidth = inButtonWidth
        Obj.mMaxChars = inMaxChars


        setmetatable(Obj, self)
        self.__index = self
        -- "List Selector Construction Finished")
        return Obj
    end,
    Event = function(self)
        self.mLeftbutton:Event()
        self.mRightButton:Event()
        local p = self.mPosition_3f
        local d = self.mDimension_3f
        local lb_p = vec3(p.x, p.y, p.z)
        local lb_d = vec3(self.mButtonWidth, d.y, d.z)
        local txt_p = vec3(p.x + lb_d.x, p.y, p.z)
        local txt_d = vec3(d.x - 2 * self.mButtonWidth, d.y, d.z)
        local rb_p = vec3(p.x + txt_d.x + self.mButtonWidth, p.y, p.z)
        local rb_d = vec3(self.mButtonWidth, d.y, d.z)
        if self.mLeftbutton.mPressed then
            if self.mCurrentSelection > 1 then
                self.mCurrentSelection = self.mCurrentSelection - 1
            end
            self.mTextArea:Update(txt_p, txt_d, string.rep(" ", self.mMaxChars))
            self.mTextArea:Update(txt_p, txt_d, self.mList[self.mCurrentSelection])
        elseif self.mRightButton.mPressed then
            if self.mCurrentSelection < #self.mList then
                self.mCurrentSelection = self.mCurrentSelection + 1
            end
            self.mTextArea:Update(txt_p, txt_d, string.rep(" ", self.mMaxChars))
            self.mTextArea:Update(txt_p, txt_d, self.mList[self.mCurrentSelection])
            -- "Pressed")
        end
    end,
    SetParent = function(self, inObject)
        Com.TextButtonObject.SetParent(self.mLeftbutton, inObject)
        Com.TextButtonObject.SetParent(self.mRightButton, inObject)
        Com.TextButtonObject.SetParent(self.mTextArea, inObject)
    end
}


Com.FileMenuBarObject = {
    mMainArea = nil,
    mDepth = nil,
    mHeight = nil,
    mFileMenu = nil,
    New = function(self, inFileMenu, inHeight, inDepth, inFontObject)
        local Obj = {}
        setmetatable(Obj, self)
        self.__index = self
        local mainareapos = vec3(0, 0, inDepth)
        local mainareadimen = vec3(WindowDimension.x, inHeight, 1)
        Obj.mMainArea = Com.AreaObject:New(mainareapos, mainareadimen)
        Obj.mMainArea:TurnOffShadow()
        Obj.mDepth = inDepth
        Obj.mHeight = inHeight
        Obj.mButtons = {}
        Obj.mFileMenu = inFileMenu
        local i = 1
        for index, value in ipairs(inFileMenu) do
            local pos = vec3(50 * (i - 1), 0, inDepth - 3)
            local dimen = vec3(50, inHeight, 1)
            Obj.mButtons[#Obj.mButtons + 1] = Com.TextButtonObject:New(inFileMenu[i].name, inFontObject, pos, dimen)
            Com.AreaObject.TurnOffShadow(Obj.mButtons[#Obj.mButtons])
            i = i + 1
        end
        return Obj
    end,
    Event = function(self)
        local mainareapos = vec3(0, 0, self.mDepth)
        local mainareadimen = vec3(WindowDimension.x, self.mHeight, 1)
        self.mMainArea:Update(mainareapos, mainareadimen)
        for index, value in ipairs(self.mFileMenu) do
            self.mButtons[index]:Event()
            local pos = vec3(50 * (index - 1), self.mHeight, self.mDepth - 3)
            if self.mButtons[index].mPressed then
                self.mFileMenu[index].action(pos)
            end
        end
    end
}


Com.ContextMenu = {
    mMainArea = nil,
    mCellDimension_3f = vec3(0, 0, 0),
    mPosition_3f = nil,
    mButtons = nil,
    mMaxNoOfEntries = nil,
    New = function(self, inPosition_3f, inCellDimension_3f, inFontObject, inNoOfEntries, inMaxStringLength)
        local Obj = {
            mPosition_3f = inPosition_3f,
            mCellDimension_3f = inCellDimension_3f,
            mButtons = {},
            mMaxNoOfEntries = inNoOfEntries,
            mCurrentContextMenu = {}
        }
        setmetatable(Obj, self)
        self.__index = self
        local MainAreaDimen = vec3(0, 0, 1)
        Obj.mMainArea = Com.AreaObject:New(inPosition_3f, MainAreaDimen)
        local button_dimension = vec3(0, 0, 0)
        for i = 1, inNoOfEntries, 1 do
            local pos = vec3(inPosition_3f.x, inPosition_3f.y + inCellDimension_3f.y * (i - 1), inPosition_3f.z - 3)
            Obj.mButtons[i] = Com.TextButtonObject:New(string.rep(" ", inMaxStringLength), inFontObject, pos,
                button_dimension)
            Obj.mButtons[i]:TurnOffShadow()
            i = i + 1
        end
        return Obj
    end,
    Update = function(self, inContextMenuTable, inPosition_3f, inCellDimension_3f)
        self.mCurrentContextMenu = inContextMenuTable
        -- Clear Up
        self.mMainArea:Update(vec3(0, 0, self.mMainArea.mPosition_3f.z), vec3(0, 0, 0))
        for index, value in ipairs(self.mButtons) do
            value:Update(vec3(0, 0, value.mPosition_3f.z), vec3(0, 0, 0), " ")
        end

        local inNoOfEntries = #inContextMenuTable
        local MainAreaDimension = vec3(inCellDimension_3f.x, inCellDimension_3f.y * inNoOfEntries, 1)
        local mainareapos = vec3(inPosition_3f.x, inPosition_3f.y, self.mMainArea.mPosition_3f.z)
        self.mMainArea:Update(mainareapos, MainAreaDimension)
        for i = 1, inNoOfEntries, 1 do
            local pos = vec3(inPosition_3f.x, inPosition_3f.y + inCellDimension_3f.y * (i - 1), self.mButtons[i].mPosition_3f.z)
            self.mButtons[i]:Update(pos, inCellDimension_3f, inContextMenuTable[i].name)
        end
    end,
    Event = function (self)
        local NoOfEntries = #self.mCurrentContextMenu
        for i = 1, NoOfEntries, 1 do
            self.mButtons[i]:Event()
            if self.mButtons[i].mPressed then
                self.mCurrentContextMenu[i].action()
            end
        end

        if E.is_left_button_pressed() then
            self:Update({}, vec3(0, 0, 0), vec3(0, 0, 0))
        end
    end
}
