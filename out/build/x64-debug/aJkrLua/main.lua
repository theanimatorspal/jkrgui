--[[
        Yo chae User le lekhne Code ho
]]
require "mainh"

local __Depth = Depth + 10 -- Farthest (This meant to be used when the ComObject adds Over sth)

Load = function()
        FileMenu = Com.FileMenuBarObject:New(FileMenux, 25, 80, Font)
        Pip = Com.PopupMenu:New(50, Font, 10)
        Pip:Update(vec3(100, 100, 50), vec3(180, 100, 1), "Popup", "Error is Encountered")
        Pip:Update(vec3(50, 100, 50), vec3(180, 100, 1), "Popup", "Error is Encountered")
        Ip = Jkr.Components.Util.ImagePainter:New("cache/ImagePainterRoundedRect.bin", vec2(100, 100), false, Jkr.GLSL.RoundedCircle, 16, 16, 1)
        Obj = Com.ImageLabelObject:NewEmpty(vec3(100, 100, 50), vec3(100, 100, 1))
end

Event = function()
        Com.Events()
        ContextMenu:Event()
        FileMenu:Event()
        Ip:Paint(vec4(0.5, 0.5, 0.5, 0.5), vec4(1, 0, 0, 1), vec4(1, 0, 0, 1), Ip.mImageObjectAbs)
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