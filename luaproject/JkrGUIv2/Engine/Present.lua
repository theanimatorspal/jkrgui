require "JkrGUIv2.WidgetsRefactor"
--[============================================================[
          UTILITY FUNCTIONS
]============================================================]

local lerp = function(a, b, t)
          return (a * (1 - t) + t * b) * (1 - t) + b * t
end

local lerp_3f = function(a, b, t)
          return vec3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t))
end


Frame = function(inTable)
          return { Frame = inTable }
end
TitlePage = function(inTable) return { TitlePage = inTable } end
Enumerate = function(inTable) return { Enumerate = inTable } end
Animation = function(inStyle) return { Style = inStyle } end
Item = function(inStr)
          return inStr
end

--[============================================================[
          PROCESS  FUNCTIONS
]============================================================]

local ProcessFunctions = {
          TitlePage = function(inPresentation, inValue)
                    local title = inPresentation.Title
                    local date = inPresentation.Date
                    local author = inPresentation.Author
          end,
          Enumerate = function(inPresentation, inValue)

          end
}

--[============================================================[
          PRESENTATION  FUNCTION
]============================================================]

Presentation = function(inPresentation)
          local Frames = {}
          for key, value in pairs(inPresentation) do
                    if (key == "Frame") then
                              Frames[#Frames + 1] = value
                    end
          end

          local FrameCout = #Frames
          for i = 1, FrameCout, 1 do
                    local FrameContents = Frames[i]
                    for key, value in pairs(FrameContents) do
                              ProcessFunctions[key](inPresentation, value)
                    end
          end
end

Presentation {
          Title = "Introduction to JkrGUI Presentation Engine",
          Date = "18th Jeshtha, 2020",
          Author = { "Darshan Koirala" },
          Animation {
                    Interpolation = "Constant",
          },
          Frame { TitlePage {} },
          Frame {
                    Title = "Introduction",
                    Enumerate {
                              Item "The Best Presentation Engine",
                              Item "I love this Presenation Engine",
                              Item "You will love this too",
                              Item "Thank You",
                    }
          }
}
