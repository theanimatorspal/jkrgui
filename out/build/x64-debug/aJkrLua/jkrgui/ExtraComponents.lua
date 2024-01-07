require "jkrgui.PrimitiveComponents"

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

Com.PopupMenu = {
    mMainArea = nil,
    mHeadButton = nil,
    mInfo = nil,
    mButton = nil,
    New = function(self, inDepth, inFontObject, inMaxChars)
        local Obj = {
            mPosition_3f = vec3(0, 0, inDepth),
            mDimension_3f = vec3(0, 0, 0),
        }
        setmetatable(Obj, self)
        self.__index = self
        Obj.mMainArea = Com.AreaObject:New(vec3(0, 0, inDepth), vec3(0, 0, 0))
        Obj.mHeadButton = Com.TextButtonObject:New(string.rep(" ", inMaxChars), inFontObject, vec3(0, 0, inDepth - 4), vec3(0, 0, 0))
        Obj.mHeadButton:TurnOffShadow()
        Obj.mInfo = Com.TextLabelObject:New(string.rep(" ", inMaxChars * 5), vec3(0, 0, inDepth - 3), inFontObject)
        Obj.mButton = Com.TextButtonObject:New(string.rep(" ", inMaxChars), inFontObject, vec3(0, 0, inDepth - 4), vec3(0, 0, 0))
        return Obj
    end,

    Update = function(self, inPosition_3f, inDimension_3f, inHeadString, inInfo)
        self.mMainArea:Update(inPosition_3f, inDimension_3f)
        local buttondimen = vec3(inDimension_3f.x, 25, 1)
        local buttonpos = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z - 4)
        self.mHeadButton:Update(buttonpos, buttondimen, inHeadString)
        local infopos = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z - 4)
        self.mInfo:Update(vec3(inPosition_3f.x + 10, inPosition_3f.y + 25 * 2, inPosition_3f.z - 3), inInfo)
        buttonpos = vec3(inPosition_3f.x + inDimension_3f.y / 2, inPosition_3f.y + 25 * 2.5, inPosition_3f.z - 4)
        buttondimen = vec3(60, 25, 1)
        self.mButton:Update(buttonpos, buttondimen, "Cancel")
    end
}