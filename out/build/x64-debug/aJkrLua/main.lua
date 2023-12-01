require "jkrgui"

local ComTable = {}
local com_i = 0
local NewComponent = function()
        com_i = com_i + 1
end
local AllComponent_Events = function()
        for _, com in ipairs(ComTable) do
                com:Event()
        end
end
local AllComponent_Draws = function()
        for _, com in ipairs(ComTable) do
                com:Draw()
        end
end

AreaObject = {
        mIds = vec2(0, 0),
        mPosition_3f = vec3(0, 0, 0),
        mDimension_3f = vec3(0, 0, 0),
        New = function(self, inPosition_3f, inDimension_3f)
                Obj = {}
                setmetatable(Obj, self)
                self.__index = self
                local ShadowPos = vec3(inPosition_3f.x + 3, inPosition_3f.y + 3, inPosition_3f.z)
                local OutlinePos = vec3(inPosition_3f.x, inPosition_3f.y, inPosition_3f.z - 1)
                local AreaPos = vec3(inPosition_3f.x + 1, inPosition_3f.y + 1, inPosition_3f.z - 2)
                local AreaDimen = vec3(inDimension_3f.x - 2, inDimension_3f.y - 2, inDimension_3f.z)

                Obj.mPosition_3f = inPosition_3f
                Obj.mDimension_3f = inDimension_3f
                NewComponent()
                ComTable[com_i] = Jkr.Components.Static.ButtonObject:New(ShadowPos, inDimension_3f, nil, nil)
                ComTable[com_i].mFillColor = Theme.Colors.Shadow
                Obj.mIds.x = com_i

                NewComponent()
                ComTable[com_i] = Jkr.Components.Static.ButtonObject:New(OutlinePos, inDimension_3f, nil, nil)

                NewComponent()
                ComTable[com_i] = Jkr.Components.Static.ButtonObject:New(AreaPos, AreaDimen, nil, nil)
                ComTable[com_i].mFillColor = vec4(1, 1, 1, 1)
                Obj.mIds.y = com_i
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
                ComTable[i]:Update(ShadowPos, inDimension_3f)
                ComTable[i + 1]:Update(OutlinePos, inDimension_3f)
                ComTable[i + 2]:Update(AreaPos, AreaDimen)
        end,
        Event = function(self)
                local i = self.mIds.x
                local mousePos = E.get_relative_mouse_pos()
                local isFocused = ComTable[i].mComponentObject.isFocused
                if isFocused then
                        local new_pos = vec3(self.mPosition_3f.x + mousePos.x, self.mPosition_3f.y + mousePos.y, self.mPosition_3f.z + mousePos.z)
                         
                        self:Update(vec3(200, 100, 5), self.mDimension_3f)
                end
        end
}



Load = function()
        MovableArea = AreaObject:New(vec3(100, 100, 5), vec3(200, 200, 1))
end

Event = function()
        AllComponent_Events()
        MovableArea:Event()
end

Update = function()
        FrameUpdate()
end

Dispatch = function()
end

Draw = function()
        AllComponent_Draws()
end

UnLoad = function()
end



--[[

local Cursor = {
        Position = vec2(0, 0),
        Dimension = vec2(5, 20),
        Id = 0,
        ImagePainter = Jkr.image_painter("painter_cache.bin", GPainterGLSL , vec3(16, 16, 1)),
        ImageId = 0,
        Color = vec4(0, 0, 0, 1),
        Load = function (self)
               self.ImagePainter:load()
               self.ImagePainter:register_image(100, 100)

               local Rectangle = Generator(Shapes.rectangle, self.Dimension)
               self.Id = S.Add(Rectangle, vec3(self.Position.x, self.Position.y, 5))

               self.ImageId = S.AddImage(100, 100)
               S.CopyImage(self.ImageId, self.ImagePainter);
        end,

        Dispatch = function (self)
                local PosDimen = vec4(0, 0, 0, 0)
                local Param = vec4(0, 0, 0, 0)
                self.ImagePainter:paint(PosDimen, self.Color, Param)
                S.CopyImage(self.ImageId, self.ImagePainter);
        end,

        MoveDown = function (self)
                self.Position.y = self.Position.y + self.Dimension.y
        end,

        MoveLeft = function (self)
                self.Position.x = 0
        end,

        DrawShapes = function(self)
                S.Bind()
                S.BindFillMode(FillType.image)
                S.BindImage(self.ImageId)
                local matrix = GetIdentityMatrix()
                local matrix = Translate(matrix, vec3(self.Position.x, self.Position.y, 0))
                S.Draw(vec4(1, 1, 1, 1), Int(WindowDimension.x), Int(WindowDimension.y), 0, 0, matrix)
        end
}


function Load()
        Cursor:Load()
        CurrentLine = 1
        StartText = {}
        StartText[CurrentLine] = "लेख्नुहोस्->:"
        StartOffset = utf8.len(StartText[CurrentLine])
        Font = FontObject:New("C:\\Users\\sansk\\OneDrive\\Desktop\\Project\\jkrengine\\out\\build\\x64-debug\\application\\font.ttf", 4)
        Text = {}
        Text[CurrentLine] = TextObject:New(StartText[CurrentLine], Font)
        E.start_text_input()
end

function Event()
        should_update_text = false
        should_create_new_line = false
        local is_backspace = E.is_key_pressed(key.SDLK_BACKSPACE)
        local is_enter = E.is_key_pressed(key.SDLK_RETURN)
        if E.is_text_being_input() and not is_backspace then
                -- print(E.get_input_text())
                StartText[CurrentLine] = StartText[CurrentLine] .. E.get_input_text()
                should_update_text = true
        end

        if E.is_keypress_event() then
                if is_backspace and utf8.len(StartText[CurrentLine]) >= StartOffset then
                        StartText[CurrentLine] = utf8.sub(StartText[CurrentLine], 1, -2)
                        should_update_text = true
                elseif  is_enter then
                        should_create_new_line = true
                end
        end
end

function Update()
   local value = math.sin(Time / 100)
   Cursor.Color = vec4(1, 0, 0, value)
   Cursor.Position = vec2(Text[CurrentLine].mDimension.x, Cursor.Position.y)

   if should_create_new_line then
        Cursor:MoveDown()
        Cursor:MoveLeft()
        CurrentLine = CurrentLine + 1;
        StartText[CurrentLine] = " "
        Text[CurrentLine] = TextObject:New(" ", Font)
        should_create_new_line = false
   end

   local Pos = Text[CurrentLine].mPosition
   Pos.y = Cursor.Position.y + Cursor.Dimension.y
   if should_update_text then
        Text[CurrentLine]:Update(StartText[CurrentLine], Pos)
        should_update_text = false
   end


   FrameUpdate()
end

function Dispatch()
        Cursor:Dispatch()
end

function Draw()
        Cursor:DrawShapes()
        for i, t in ipairs(Text) do
            t:Draw()
        end

end

function UnLoad()

end

]]
--
