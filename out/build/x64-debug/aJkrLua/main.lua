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
        Ip = Jkr.Components.Util.ImagePainter:New("cache/ImagePainterRoundedRect.bin", vec2(50, 50), false, Jkr.GLSL.RoundedCircle, 16, 16, 1)
        IpRR = Jkr.Components.Util.ImagePainter:NewFrom(Ip, "cache/ImagePainterRoundedRectangle.bin",  true, Jkr.GLSL.RoundedRectangle, 16, 16, 1)
        ImgLabel = Com.ImageLabelObject:NewEmpty(50, 50, vec3(500, 100, 50), vec3(20, 20, 1))
        ImgLabel:TintColor(vec4(0, 2, 1, 0.5))
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
        Ip:Paint(vec4(0, 0, 0.2, 0.5), vec4(1, 1, 1, 1), vec4(1, 0, 0, 1), ImgLabel.mImageObjectAbs)
end

Draw = function()
        Com.Draws()
end


UnLoad = function()
end