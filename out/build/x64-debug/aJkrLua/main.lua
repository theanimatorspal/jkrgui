require "presentation_engine"

Load = function()
    Name = "Darshan Koirala"
    JkrPresent.LoadResources()
    Bishal_Jais_Id = JkrPresent.CreateAnimText("Keep It Simple")

    Dipoesh_Id = JkrPresent.CreateAnimText(Name)

    KeyFrame1 = JkrPresent.CreateCircleText_Moving_animation(Dipoesh_Id, Name, vec3(200, 200, 5),
        50, 10)
    KeyFrame2 = JkrPresent.CreateLineText(Dipoesh_Id, Name, vec3(300, 300, 1), 20, false)
    KeyFrame3 = JkrPresent.CreateCircleText_Moving_animation(Dipoesh_Id, Name, vec3(400, 200, 5), 50, 0)
    KeyFrame4 = JkrPresent.CreateLineText(Dipoesh_Id, Name, vec3(100, 300, 1), 20, true)

    print(Bishal_Jais_Id)
end

Event = function()

end


Update = function()
    local t = math.abs(math.sin(Time / 80))
    JkrPresent.LerpAnimationVarArgs(Dipoesh_Id, t, KeyFrame1, KeyFrame2, KeyFrame3, KeyFrame4)
    print(t)

    FrameUpdate();
end

Dispatch = function()

end

Draw = function()
    JkrPresent.Draw()
end

UnLoad = function()
end
