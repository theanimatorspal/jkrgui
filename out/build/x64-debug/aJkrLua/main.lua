--[[
        Yo chae User le lekhne Code ho
]]
require "mainh"

local __Depth = Depth + 10 -- Farthest (This meant to be used when the ComObject adds Over sth)

Load = function()
        LineId = L.add(vec2(100, 100), vec2(700, 700), 55)
        Font = Jkr.FontObject:New("font.ttf", 4)
        FontTEST = Jkr.FontObject:New("font.ttf", 6)
        FileMenu = Com.FileMenuBarObject:New(FileMenux, 25, 80, Font)
        ContextMenu = Com.ContextMenu:New(vec3(100, 100, 80), vec3(80, 20, 1), Font, 10, 10)
        Pip = Com.PopupMenu:New(50, Font, 10)
        Pip:Update(vec3(100, 100, 50), vec3(180, 100, 1), "Popup", "Error is Encountered")
        Pip:Update(vec3(50, 100, 50), vec3(180, 100, 1), "Popup", "Error is Encountered")
        Ip_RoundedRectangle = Jkr.Components.Util.ImagePainter:New("cache/RoundedRectangle.Compute", vec2(100, 100), true, Jkr.GLSL.RoundedRectangle, 16, 16, 1)
        Ip_RoundedCircle = Jkr.Components.Util.ImagePainter:NewFrom(Ip_RoundedRectangle, "cache/RoundedCircle.Compute", true, Jkr.GLSL.RoundedCircle, 16, 16, 1)
        ImgLab = Com.ImageLabelObject:NewEmpty(100, 100, vec3(300, 200, 80), vec3(100, 100, 1))
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
        Ip_RoundedRectangle:Paint(vec4(0, 0, 0.2, 0.2), vec4(1, 0.5, 0.2, 1), vec4(0.5, 0.5, 0.5, 0.5), ImgLab.mImageObjectAbs)
end

Draw = function()
        Com.Draws()
end


UnLoad = function()
end