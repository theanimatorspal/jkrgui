require "CompTable"

Com.TerminalEmulator = {
    New = function(self, inPosition_3f, inDimension_3f, inFontObject)
        local Obj = Com.TextMultiLineEditObject:New(inPosition_3f, inDimension_3f, 5, 20, inFontObject,
            Int(inDimension_3f.y / (20 + 1)), 200, nil)
        setmetatable(self, Com.TextMultiLineEditObject)
        setmetatable(Obj, self)
        self.__index = self
        Obj:AddCharacter(true, ">>")
        return Obj
    end,
    Event = function(self)
        Com.TextMultiLineEditObject.Event(self)
        if E.is_keypress_event() and E.is_key_pressed(Key.SDLK_RETURN) then
            local cmd = self.mLineTexts[self.mCurrentLine - 1]
            cmd = string.sub(cmd, 3, #cmd)
            local result = io.popen(cmd):read("a")
            self:AddCharacter(true, "What the fuck")
            print(result)
            -- self:AddCharacter(true, "", false, true)
        end
    end
}