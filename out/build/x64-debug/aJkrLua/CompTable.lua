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
    mShapeId = 0,
    New = function(self, inPosition_3f, inDimension_3f, inColor_4f)
        print("TextCursorObject Construction")
        local Obj = {}
        setmetatable(Obj, self)
        self.__index = self
        Com.NewComponent()
        ComTable[com_i] = Jkr.Components.Static.ShapeObject:New(inPosition_3f, inDimension_3f, nil, nil)
        ComTable[com_i].mFillColor = inColor_4f
        print("-----------------------")
        print("TextCursorObject Construction Finished")
        Obj.mWidth = inDimension_3f.x
        Obj.mShapeId = com_i
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
        Obj.Update = Com.TextLineEditObject.Update

        Obj.mPositionToParent_3f = vec3(0, 0, 0)
        Obj.mPadding = 5
        Obj.mPressed = false
        Obj.mStartString = string.rep(" ", inMaxStringLength)
        Obj.mCurrentString = ""
        local CursorObjectPosition = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z - 3)
        Obj.mTextCursorObject = Com.TextCursorObject:New(CursorObjectPosition,
            vec3(inCursorWidth, inDimension_3f.y, inDimension_3f.z - 1), vec4(0, 0, 0, 1))
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
                -- should_submit = true
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
            local CursorObjectPosition = vec3(CursorPosByTypedText.x + self.mPosition_3f.x, self.mPosition_3f.y,
                self.mPosition_3f.z - 3)
            local CursorObjectDimension = vec3(self.mTextCursorObject.mWidth, self.mDimension_3f.y,
                self.mDimension_3f.z - 1)
            self.mTextCursorObject:Update(CursorObjectPosition, CursorObjectDimension)
            self.mShouldUpdate = false
        end
    end,
    Update = function(self, inObject)
    end,
    SetParent = function(self, inObject)
        local pos = vec3(inObject.mPosition_3f.x + self.mPositionToParent_3f.x,
            inObject.mPosition_3f.y + self.mPositionToParent_3f.y, self.mPosition_3f.z)
    end
}
