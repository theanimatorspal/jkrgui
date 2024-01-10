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
        Painter_Image = Jkr.Components.Abstract.PainterImageObject:New(40, 40)

        Ip_Clear = Jkr.Components.Util.ImagePainter:New("cache/Clear.Compute", true, Jkr.GLSL.Clear, 16, 16, 1)
        Ip_Clear:RegisterImage(Painter_Image)

        Ip_RoundedRectangle = Jkr.Components.Util.ImagePainter:New("cache/RoundedRectangle.Compute", true, Jkr.GLSL.RoundedRectangle, 16, 16, 1)
        Ip_RoundedCircle = Jkr.Components.Util.ImagePainter:New("cache/RoundedCircle.Compute", true, Jkr.GLSL.RoundedCircle, 16, 16, 1)
        ImgLab = Com.ImageLabelObject:NewEmpty(40, 40, vec3(300, 200, 80), vec3(40, 40, 1))
        ImgLab:TintColor(vec4(2, 1, 1, 1))
        ImgLab:PaintByComputeSingleTime(
                {
                        posdimen = vec4(0, 0, 0.4, 0.4),
                        color = vec4(1, 1, 1, 1),
                        param = vec4(0.3, 0.5, 0.5, 0),
                        painter = Ip_RoundedRectangle,
                }, 
                Ip_Clear
        )

        ImgLab2 = Com.ImageLabelObject:NewEmpty(40, 40, vec3(300, 200, 80), vec3(40, 40, 1))
        ImgLab2:TintColor(vec4(1, 0, 0, 1))
        ImgLab2:PaintByComputeSingleTime(
                {
                        posdimen = vec4(0, 0, 0.4, 0.4),
                        color = vec4(1, 1, 1, 1),
                        param = vec4(0.3, 0.5, 0.5, 0),
                        painter = Ip_RoundedCircle
                },
                Ip_Clear
        )
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
        Com.Dispatches()
end

Draw = function()
        Com.Draws()
end


UnLoad = function()
end