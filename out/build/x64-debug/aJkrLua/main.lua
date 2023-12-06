--[[
        Yo chae User le lekhne Code ho
]]

require "jkrgui"
require "CompTable"

local __Depth = Depth + 10 -- Farthest (This meant to be used when the ComObject adds Over sth)
local GPS     = {
        mPosition = vec3(0, 0, 0),
}


Load = function()
        Font = Jkr.FontObject:New(
        "C:\\Users\\sansk\\OneDrive\\Desktop\\Project\\jkrengine\\out\\build\\x64-release with debug\\application\\font.ttf",
                4)
        MovableArea = Com.AreaObject:New(vec3(10, 10, __Depth), vec3(200, 200, 1))
        -- MovableArea.mIsMovable = true
        SampleText = Com.TextLabelObject:New("अत्र लिख्यताम् : ", vec3(10, 20, Depth - 1), Font)
        TextButton = Com.TextButtonObject:New("अस्तु", Font, vec3(140, 170, Depth - 1), vec3(50, 20, 1))
        TextLineEditor = Com.TextLineEditObject:New(vec3(20, 40, Depth - 1), vec3(100, 20, 1), 5, Font, 20, MovableArea)
        TextLineEditorSecond = Com.TextLineEditObject:New(vec3(20, 80, Depth - 1), vec3(100, 20, 1), 5, Font, 20,
                MovableArea)
        TextMultiLineEditor = Com.TextMultiLineEditObject:New(
                vec3(220, 10, __Depth), -- position
                vec3(300, 200, 1), -- dimension
                5, -- cursor width
                20, -- cursor height
                Font, -- font
                5, -- max no of lines
                40, -- max string length
                nil)

        SampleText:SetParent(MovableArea)
        TextButton:SetParent(MovableArea)
        TextLineEditor:SetParent(MovableArea)
        TextLineEditorSecond:SetParent(MovableArea)
end

Event = function()
        SampleText:SetParent(MovableArea)
        TextButton:SetParent(MovableArea)
        TextLineEditor:SetParent(MovableArea)
        TextLineEditorSecond:SetParent(MovableArea)

        Com.Events()
        MovableArea:Event()
        TextButton:Event()
        TextLineEditor:Event()
        TextLineEditorSecond:Event()
        TextMultiLineEditor:Event()
end

Update = function()
        FrameUpdate()
end

Dispatch = function()
end

Draw = function()
        Com.Draws()
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
