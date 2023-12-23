--[[
        Yo chae User le lekhne Code ho
]]

require "jkrgui"
require "PrimitiveComponents"
require "TextEditorComponents"

local __Depth = Depth + 10 -- Farthest (This meant to be used when the ComObject adds Over sth)


Load = function()
        LineId = L.add(vec2(100, 100), vec2(700, 700), 55)
        Font = Jkr.FontObject:New(
        "C:\\Users\\sansk\\OneDrive\\Desktop\\Project\\jkrengine\\out\\build\\x64-release with debug\\application\\font.ttf",
                4)

        local GridGPS = Com.GPS:New(__Depth, Depth)
        GridGPS:SetDimension(vec3(200, 200, 1))
        print("GRID GPS")
        GridGPS:PrintCurrent()

        MovableArea = Com.AreaObject:New(GridGPS.mPosition_3f, GridGPS.mDimension_3f)
        local MovableAreaGPS = Com.GPS:New(Depth, Depth - 2)
        GridGPS:SetDimension(vec3(10, 20, 1))
        MovableAreaGPS:MoveDown()
        SampleText = Com.TextLabelObject:New("अत्र लिख्यताम् : ", MovableAreaGPS.mPosition_3f, Font)
        MovableAreaGPS:MoveDown()
        MovableAreaGPS:SetDimension(vec3(100, 20, 1))
        TextLineEditor = Com.TextLineEditObject:New(MovableAreaGPS.mPosition_3f, MovableAreaGPS.mDimension_3f, 5, Font, 20, MovableArea)
        MovableAreaGPS:MoveDown() 
        TextLineEditorSecond = Com.TextLineEditObject:New(MovableAreaGPS.mPosition_3f, MovableAreaGPS.mDimension_3f, 5, Font, 20,
                MovableArea)
        MovableAreaGPS:MoveDown()
        TextButton = Com.TextButtonObject:New("अस्तु", Font, MovableAreaGPS.mPosition_3f, MovableAreaGPS.mDimension_3f)

        GridGPS:SetDimension(vec3(200, 200, 1)) -- TODO, fix this bug, this shouldn't be required
        GridGPS:MoveRight()
        TextMultiLineEditor = Com.TextMultiLineEditObject:New(
                GridGPS.mPosition_3f, -- position
                GridGPS.mDimension_3f, -- dimension
                5, -- cursor width
                20, -- cursor height
                Font, -- font
                9, -- max no of lines
                40, -- max string length
                nil)

        GridGPS:MoveRight()
        GridGPS:SetDimension(vec3(1000, 200, 1)) -- TODO, fix this bug, this shouldn't be required
        TerminalEmulator = Com.TerminalEmulator:New(
                GridGPS.mPosition_3f, -- position
                GridGPS.mDimension_3f, -- dimension
                Font -- font
                )

        -- TerminalEmulator.mString = "Hello Everyone\nI Love  You\n\nWhat are you doing\nWhat you do\nWhy do you do\nFuck You\n"
        -- TerminalEmulator:SetVisualTextByString(1, 5)
        GridGPS:Start()
        GridGPS:MoveBackAbsolute()
        GridGPS:SetDimension(vec3(200, 200, 1))
        GridGPS:MoveDown()

        SecondRowArea = Com.AreaObject:New(GridGPS.mPosition_3f, GridGPS.mDimension_3f)
        SecondRewAreaGPS = Com.GPS:New(Depth, Depth - 2)
        SecondRewAreaGPS:Start()
        SecondRewAreaGPS:SetDimension(vec3(0, 20, 1))
        SecondRewAreaGPS:MoveDown()
        SecondRewAreaGPS:MoveRight()
        SecondRewAreaGPS:MoveFrontAbsolute()
        SecondRewAreaGPS:SetDimension(vec3(160, 20, 1))
        NumberSlider = Com.NumberSliderObject:New(50, vec2(0, 100), SecondRewAreaGPS.mPosition_3f, SecondRewAreaGPS.mDimension_3f)
        SecondRewAreaGPS:MoveDown()
        local List = {"one", "two", "three", "four", "five"}
        ListSelector = Com.ListSelectorObject:New(List, SecondRewAreaGPS.mPosition_3f, SecondRewAreaGPS.mDimension_3f, 30, Font, 10, SecondRowArea)



        SampleText:SetParent(MovableArea)
        TextButton:SetParent(MovableArea)
        TextLineEditor:SetParent(MovableArea)
        TextLineEditorSecond:SetParent(MovableArea)
        NumberSlider:SetParent(SecondRowArea)
        ListSelector:SetParent(SecondRowArea)
end

Event = function()
        Com.Events()
        MovableArea:Event()
        TextButton:Event()
        TextLineEditor:Event()
        TextLineEditorSecond:Event()
        TextMultiLineEditor:Event()
        TerminalEmulator:Event()
        NumberSlider:Event()
        ListSelector:Event()
end

Update = function()
        SampleText:SetParent(MovableArea)
        TextButton:SetParent(MovableArea)
        TextLineEditor:SetParent(MovableArea)
        TextLineEditorSecond:SetParent(MovableArea)
        ListSelector:SetParent(SecondRowArea)
        TerminalEmulator:Update()
        FrameUpdate()
end

Dispatch = function()
end

Draw = function()
        Com.Draws()
end

UnLoad = function()
end

