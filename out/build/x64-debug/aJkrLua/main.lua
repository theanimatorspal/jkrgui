--[[
        Yo chae User le lekhne Code ho
]]

require "jkrgui"
require "PrimitiveComponents"
require "TextEditorComponents"

local __Depth = Depth + 10 -- Farthest (This meant to be used when the ComObject adds Over sth)


Load = function()
        LineId = L.add(vec2(100, 100), vec2(700, 700), 55)
        Font = Jkr.FontObject:New("font.ttf", 4)
        FileMenu = {
                [1] = {
                        name = "File",
                        action = function (inPosition_3f)
                                
                        end
                },
                [2] = {
                        name = "Edit",
                        action = function (inPosition_3f)
                                
                        end
                },
                [3] = {
                        name = "View",
                        action = function (inPosition_3f)
                                
                        end
                },
                [4] = {
                        name = "Run",
                        action = function (inPosition_3f)
                                
                        end
                }
        }
        FileMenu = Com.FileMenuBarObject:New(FileMenu, 25, 80, Font)
end

Event = function()
        Com.Events()
        FileMenu:Event()
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




-- require "presentation_engine"

-- Load = function()
--     JkrPresent.LoadResources()
--     Pt = JkrPresent.CreateAnimatedPointText({"1. There are No Other Libraries", "2. The Libraries are Complex", "3. The Learning time of Library outweighs "}, vec2(300, 200), 20)
--     Line = JkrPresent.CreateAnimLines(8)
--     KeyFrx = JkrPresent.CreateLineLine_Moving_Animation(Line, 4, vec3(100, 100, 5), 100, 0, 0)
--     KeyFry = JkrPresent.CreateCircleLine_Moving_Animation(Line + 4, 4, 100, math.pi / 4, vec3(100, 100, 5))
--     KeryFrame1 = JkrPresent.JoinKeyframe(KeyFrx, KeyFry)
--     KeyFrame2 = JkrPresent.CreateCircleLine_Moving_Animation(Line, 8, 50, math.pi / 4, vec3(400, 400, 5))
-- end

-- Event = function()

-- end


-- Update = function()
--     local t = math.abs(math.sin(Time / 50))
--     JkrPresent.LerpAnimationTableText(Pt.text_id, t, Pt.text_key)
--     JkrPresent.LerpAnimationTableLine(Pt.line_id, t, Pt.line_key)
--     FrameUpdate();
-- end

-- Dispatch = function()

-- end

-- Draw = function()
--     JkrPresent.Draw()
-- end

-- UnLoad = function()
-- end
