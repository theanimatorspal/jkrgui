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
        FontTEST = Jkr.FontObject:New("font.ttf", 6)


        ContextMenu = Com.ContextMenu:New(vec3(100,100,80),vec3(80,20,1),Font,10, 10)


        ContextMenuEntries_FileMenu = {
                [1]= {
                        name = "New",
                        action = function ()
                                print("New")
                        end 
                },
                [2] = {
                        name = "Save",
                        action = function ()
                                
                        end
                },
                [3] = {
                        name = "Exit",
                        action = function ()
                                
                        end
                }
        }
        ContextMenuEntries_EditMenu = {
                [1]= {
                        name = "Undo",
                        action = function ()
                                
                        end 
                },
                [2] = {
                        name = "Redo",
                        action = function ()
                                
                        end
                },
                [3] = {
                        name = "Cut",
                        action = function ()
                                
                        end
                },
                [4] = {
                        name = "Copy",
                        action = function ()
                                
                        end
                }
        }
        ContextMenuEntries_View = {
                [1]= {
                        name = "Appearance",
                        action = function ()
                                
                        end 
                },
                [2] = {
                        name = "Search",
                        action = function ()
                               print("Search") 
                        end
                },
                [3] = {
                        name = "Output",
                        action = function ()
                                
                        end
                }
        }

        ContextMenuEntries_Run = {
                [1]= {
                        name = "Start",
                        action = function ()
                                
                        end 
                },
                [2] = {
                        name = "Add",
                        action = function ()
                                
                        end
                },
                [3] = {
                        name = "Enable",
                        action = function ()
                                
                        end
                }
        }
        ContextMenu_Right = {
                [1] = {
                        name = "Refresh",
                        action = function ()
                                print("Refresh")

                        end
                }
        }

        FileMenux = {
                [1] = {
                        name = "File",
                        action = function (inPosition_3f)
                                ContextMenu:Update(ContextMenuEntries_FileMenu, inPosition_3f, vec3(80,20,1))
                        end
                },
                [2] = {
                        name = "Edit",
                        action = function (inPosition_3f)
                                ContextMenu:Update(ContextMenuEntries_EditMenu, inPosition_3f, vec3(80,20,1))    
                        end
                },
                [3] = {
                        name = "View",
                        action = function (inPosition_3f)
                                ContextMenu:Update(ContextMenuEntries_View, inPosition_3f, vec3(90,20,1))    
                        end
                },
                [4] = {
                        name = "Run",
                        action = function (inPosition_3f)
                                ContextMenu:Update(ContextMenuEntries_Run, inPosition_3f, vec3(80,20,1))    
                        end
                }
        }
        FileMenu = Com.FileMenuBarObject:New(FileMenux, 25, 80, Font)
        Id = r.balt.add(Int(FontTEST.mId), "किं कश्चन", vec2(100, 100), Int(20))
        print("along")
end

Event = function()
        Com.Events()
        ContextMenu:Event()
        FileMenu:Event()
end

Update = function()
        FrameUpdate()
end

Dispatch = function()
end

Draw = function()
        Com.Draws()
        S.Bind()
        S.BindFillMode(FillType.image)
        r.balt.draw(vec4(0, 0, 0, 2), Int(WindowDimension.x), Int(WindowDimension.y), Id, GetIdentityMatrix())
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
