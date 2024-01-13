--[[
        Yo chae User le lekhne Code ho
]]
require "mainh"

local __Depth = Depth + 10 -- Farthest (This meant to be used when the ComObject adds Over sth)

Load = function()
        Font = Jkr.FontObject:New("font.ttf", 4)
        FileMenu = Com.FileMenuBarObject:New(FileMenux, 25, 80, Font)
        ContextMenu = Com.ContextMenu:New(vec3(100, 100, 80), vec3(80, 20, 1), Font, 10, 10)
        Pip = Com.PopupMenu:New(50, Font, 10)
        Pip:Update(vec3(100, 100, 50), vec3(180, 100, 1), "Popup", "Error is Encountered")
        Pip:Update(vec3(50, 100, 50), vec3(180, 100, 1), "Popup", "Error is Encountered")
        -- Painter_Image = Jkr.Components.Abstract.PainterImageObject:New(40, 40)

        -- Ip_Clear = Jkr.Components.Util.ImagePainter:New("cache/Clear.Compute", false, Jkr.GLSL.Clear, 16, 16, 1)
        -- Ip_Clear:RegisterImage(Painter_Image)

        -- Ip_RoundedRectangle = Jkr.Components.Util.ImagePainter:New("cache/RoundedRectangle.Compute", false, Jkr.GLSL.RoundedRectangle, 16, 16, 1)
        -- Ip_RoundedCircle = Jkr.Components.Util.ImagePainter:New("cache/RoundedCircle.Compute", false, Jkr.GLSL.RoundedCircle, 16, 16, 1)
        -- ImgLab = Com.ImageLabelObject:NewEmpty(40, 40, vec3(300, 200, 80), vec3(40, 40, 1))
        -- ImgLab:TintColor(vec4(0.5, 1, 1, 1))
        -- ImgLab:PaintByComputeSingleTime(
        --         {
        --                 posdimen = vec4(0, 0, 0.4, 0.4),
        --                 color = vec4(1, 1, 1, 1),
        --                 param = vec4(0.3, 0.5, 0.5, 0),
        --                 painter = Ip_RoundedRectangle,
        --         }, 
        --         Ip_Clear
        -- )
        ImagePreload = Jkr.Components.Abstract.ImageObject:New(0, 0, "icons_material\\4k\\outline.png")

        Window = Com.WindowLayout:New(vec3(400, 400, 80), vec3(100, 100, 1), vec2(100, 30))
        Window:Start()
                ImageLLable = Com.ImageLabelObject:NewExisting(ImagePreload, vec3(100, 100, 80), vec3(20, 20, 1))
                ImageLLable:Update(vec3(700, 100, 80), vec3(100, 100, 1))
                ImageLLable:TintColor(vec4(1, 0, 0, 1))
                Window:SetCentralComponent(ImageLLable)
        Window:End()
end

Event = function()
        Com.Events()
end

Update = function()
        FrameUpdate()
end

Dispatch = function()
        Com.Dispatches()
        -- ImgLab:PaintByComputeDispatch(
        --         {
        --                 posdimen = vec4(0, 0, 0.4, 0.4),
        --                 color = vec4(1, math.sin(Time / 10), 1, 1),
        --                 param = vec4(0.3, 0.5, 0.5, 0.5),
        --                 painter = Ip_RoundedCircle
        --         },
        --         Ip_Clear
        -- )
end

Draw = function()
        Com.Draws()
end


UnLoad = function()
end