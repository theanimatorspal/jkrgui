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
        ContextMenu = {
                [1] = {
                        name = "New",
                        action = function(inPosition_3f)
                        end
                },
        
        }
        FileMenux = {
                [1] = {
                        name = "File",
                        action = function (inPosition_3f)
                            print("jhjh")    
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
        FileMenu = Com.FileMenuBarObject:New(FileMenux, 25, 80, Font)
        ContextMenu = Com.ContextMenu:New(vec3(100,100,80),vec3(80,20,1),Font,10, 10)
        print("along")

        ContextMenuEntries = {
                [1]= {
                        name = "Dipesh",
                        action = function (inPosition_3f)
                                
                        end 
                },
                [2] = {
                        name = "Raja",
                        action = function (inPosition_3f)
                                
                        end
                },
                [3] = {
                        name = "Ram",
                        action = function (inPosition_3f)
                                
                        end
                }
        }
       ContextMenu:Update(ContextMenuEntries,vec3(400,300,80), vec3(80,20,1))
       print("j vaye ni mc")
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
