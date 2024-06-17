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


Presentation = function(inTable) end
Frame = function(inTable) end
TitlePage = function(inTable) return inTable end
Enumerate = function(inTable) return inTable end
Animation = function(inStyle) return inStyle end
Item = function(inStr) return inStr end


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
