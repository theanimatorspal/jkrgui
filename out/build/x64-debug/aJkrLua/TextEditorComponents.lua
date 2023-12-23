require "PrimitiveComponents"

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

        local index = 0
        for Line in self.mString:gmatch("(.-)\n") do
            index = index + string.len(Line)
        end
        local remaining_str = string.sub(self.mString, index)
        if remaining_str ~= "" then
        end


        for Line in self.mString:gmatch("(.-)\n") do
            if i__ <= self.mMaxNoOfLines and j__ >= inStartLine then
                if i__ <= inEndline then
                    self.mLineTexts[i__] = Line
                end
                i__ = i__ + 1
            end
            j__ = j__ + 1
            -- print(self.mString)
            -- print(
            --     string.format(
            --         [[
            --                 i__  : %d,
            --                 j__ : %d,
            --                 inStartLine : %d,
            --                 inEndline : %d,
            --                 self.mMaxNoOfLines : %d
            --                 Line : %s,
            --                 self.mLineTexts[i__] : %s
            --             ]], i__, j__, inStartLine, inEndline, self.mMaxNoOfLines, Line, self.mLineTexts[i__]
            --     )
            -- )
        end

        for i = i__, self.mMaxNoOfLines, 1 do
            self.mLineTexts[i] = ""
        end

        -- This is when there is only one line in mString
        if i__ == 1 and inStartLine < inEndline then
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


Com.TerminalEmulator = {
    mShouldUpdate = false,
    mCharacterToAdd = "",
    New = function(self, inPosition_3f, inDimension_3f, inFontObject)
        local Obj = Com.TextMultiLineEditObject:New(inPosition_3f, inDimension_3f, 5, 20, inFontObject,
            Int(inDimension_3f.y / (20 + 1)), 200, nil)
        setmetatable(self, Com.TextMultiLineEditObject)
        setmetatable(Obj, self)
        self.__index = self
        Obj:AddCharacter(true, ">>")
        Obj.mShouldUpdate = false
        Obj.mCharacterToAdd = ""
        return Obj
    end,
    Event = function(self)
        Com.TextMultiLineEditObject.Event(self)
        if E.is_keypress_event() and E.is_key_pressed(Key.SDLK_RETURN) then
            local cmd = self.mLineTexts[self.mCurrentLine - 1]
            cmd = string.sub(cmd, 3, #cmd)
            local result = io.popen(cmd):read("a")
            self.mShouldUpdate = true
            self.mCharacterToAdd = result
            -- self:AddCharacter(true, "", false, true)
        end
    end,
    Update = function (self)
       if self.mShouldUpdate then 
            for Line in self.mCharacterToAdd:gmatch("(.-)\n") do
                self:AddCharacter(true, Line)
                self:AddCharacter(true, "", false, true)
            end
            self:AddCharacter(true, ">> ")
            self.mCharacterToAdd = ""
            self.mShouldUpdate = false
       end
    end
}