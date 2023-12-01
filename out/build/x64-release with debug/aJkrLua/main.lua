local v = vec2(2.0, 3.0)
function Load()
    id = r.l.add(vec2(0, 0), vec2(100, 100), 5.0)
    print(id)
end

function Draw()
    r.l.draw(vec4(1, 1, 1, 1), 1920 / 2, 1080 /2 , id, id)
end