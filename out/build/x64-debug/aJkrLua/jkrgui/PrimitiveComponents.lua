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
    mPosition_3f = vec3(0, 0, 0),
    mDimension_3f = vec3(0, 0, 0),
    New = function(self, inText, inPosition_3f, inFontObject)
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
        Com.NewComponent()
        Obj.mIds.x = com_i
        ComTable[com_i] = Jkr.Components.Static.TextObject:New(inText, inPosition_3f, inFontObject)
        -- "TextLabelObject Construction Finished")
        return Obj
    end,
    Update = function(self, inPosition_3f, inDimension_3f, inString)
        self.mPosition_3f = inPosition_3f
        if inString then
            ComTable[self.mIds.x].mString = inString
        end
        ComTable[self.mIds.x]:Update(inPosition_3f)
    end
}

Com.ImageLabelObject = {
    mImageObjectAbs = nil,
    mShapeId = nil,
    New = function (self, inFileName, inPosition_3f, inDimension_3f)
        local Obj = {}
        setmetatable(Obj, self)
        Obj.mImageObjectAbs = Jkr.Components.Abstract.ImageObject:New(0, 0, inFileName)
        Com.NewComponent() 
        ComTable[com_i] = Jkr.Components.Static.ShapeObject:New(inPosition_3f, inDimension_3f, Obj.mImageObjectAbs)
        return Obj
    end,
    NewEmpty  = function (self, inWidth, inHeight, inPosition_3f, inDimension_3f)
        local Obj = {} 
        setmetatable(Obj, self)
        self.__index = self
        Obj.mImageObjectAbs = Jkr.Components.Abstract.ImageObject:New(inWidth, inHeight, nil)
        print(Obj.mImageObjectAbs)
        Com.NewComponent()
        ComTable[com_i] = Jkr.Components.Static.ShapeObject:New(inPosition_3f, inDimension_3f, Obj.mImageObjectAbs)
        Obj.mShapeId = com_i
        return Obj
    end,
    NewExisting = function (self, inImageObject, inPosition_3f, inDimension_3f)
        local Obj = {} 
        setmetatable(Obj, self)
        self.__index = self
        Obj.mImageObjectAbs = inImageObject
        Com.NewComponent()
        ComTable[com_i] = Jkr.Components.Static.ShapeObject:New(inPosition_3f, inDimension_3f, inImageObject)
        return Obj
    end,
    Update = function (self, inPosition_3f, inDimension_3f)
        local dimen = vec2(inDimension_3f.x, inDimension_3f.y)
        local gen = Generator(Shapes.rectangle, dimen)
        S.Update(Int(self.mRectId), gen, inPosition_3f) 
    end,
    TintColor = function (self, inColor_4f)
        ComTable[self.mShapeId].mFillColor = inColor_4f
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
    SetFunction = function(self, inFunction)
        self.mFunction = inFunction
    end
}
