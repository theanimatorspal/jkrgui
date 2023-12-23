require "presentation_engine"

Load = function()
    JkrPresent.LoadResources()
    Pt = JkrPresent.CreateAnimatedPointText({"1. There are No Other Libraries", "2. The Libraries are Complex", "3. The Learning time of Library outweighs "}, vec2(300, 200), 20)
    Line = JkrPresent.CreateAnimLines(8)
    KeyFrx = JkrPresent.CreateLineLine_Moving_Animation(Line, 4, vec3(100, 100, 5), 100, 0, 0)
    KeyFry = JkrPresent.CreateCircleLine_Moving_Animation(Line + 4, 4, 100, math.pi / 4, vec3(100, 100, 5))
    KeryFrame1 = JkrPresent.JoinKeyframe(KeyFrx, KeyFry)
    KeyFrame2 = JkrPresent.CreateCircleLine_Moving_Animation(Line, 8, 50, math.pi / 4, vec3(400, 400, 5))
end

Event = function()

end


Update = function()
    local t = math.abs(math.sin(Time / 50))
    JkrPresent.LerpAnimationTableText(Pt.text_id, t, Pt.text_key)
    JkrPresent.LerpAnimationTableLine(Line, t, {KeryFrame1, KeyFrame2})
    FrameUpdate();
end

Dispatch = function()

end

Draw = function()
    JkrPresent.Draw()
end

UnLoad = function()
end
