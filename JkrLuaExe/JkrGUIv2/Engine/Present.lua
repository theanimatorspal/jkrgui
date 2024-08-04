require "JkrGUIv2.WidgetsRefactor"
require "JkrGUIv2.Engine.Engine"
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
Text = function(inText)
          return { Text = inText }
end

--[============================================================[
          PROCESS  FUNCTIONS
          these functions make the presentation engine ready for presentation
Keyframe = {
          FrameIndex = int,
          Texts = {},
          Positions = {},
          Dimensions = {},
}
]============================================================]

local window = {}
local wid = {}
local assets = {}
local literals = {}
local screenElements = {}
local FontMap = {}
local CurrentKey = 1
local WindowDimension = vec2(0)
local FrameKeys = {}
local baseDepth = 50

local index = 0
local Unique = function(inElementName) -- Generate Unique Name
          if type(inElementName) == "number" then
                    index = index + 1
                    return index
          elseif type(inElementName) == "string" then
                    return inElementName
          end
end

local ComputePositionByName = function(inPositionName, inDimension)
          if inPositionName == "CENTER" then
                    print(inDimension.x / 20, inDimension.y / 20)
                    print(WindowDimension.x / 20, WindowDimension.y / 20)
                    return vec3(WindowDimension.x / 2.0 - inDimension.x / 2.0,
                              WindowDimension.y / 2.0 - inDimension.y / 2.0,
                              baseDepth)
          else
                    return inPositionName
          end
end

local ProcessFunctions = {
          TitlePage = function(inPresentation, inValue, inFrameIndex, inElementName)
                    local title = inPresentation.Title
                    local date = inPresentation.Date
                    local author = inPresentation.Author
                    -- TODO title page
          end,
          Enumerate = function(inPresentation, inValue, inFrameIndex, inElementName)
          end,
          Text = function(inPresentation, inValue, inFrameIndex, inElementName)
                    local ElementName = Unique(inElementName)
                    if not screenElements[ElementName] then
                              screenElements[ElementName] = wid.CreateTextLabel(vec3(math.huge),
                                        vec3(math.huge),
                                        FontMap[inValue.f],
                                        inValue.t, inValue.c)
                    end
                    inValue.d = FontMap[inValue.f]:GetTextDimension(inValue.t)
                    FrameKeys[inFrameIndex][#FrameKeys[inFrameIndex] + 1] = {
                              FrameIndex = inFrameIndex,
                              Elements = {
                                        { "TEXT", handle = screenElements[ElementName], value = inValue, name = ElementName },
                              }
                    };
          end
}

local ProcessLiterals = function(inName, inValue)
          literals[inName] = inValue
end


--[============================================================[
          EXECUTE  FUNCTIONS
          these functions actually show the presenation and update it interactively
          element = { "TEXT", handle = screenElements[ElementName], value = inValue, name = ElementName },
]============================================================]

local GetPreviousFrameKeyElement = function(inPresentation, inElement, inFrameIndex)
          local PreviousFrame = FrameKeys[inFrameIndex - 1]
          if PreviousFrame then
                    local Key = PreviousFrame[i]
                    for _, element in pairs(Key.Elements) do
                              if element.name == inElement.name then
                                        return element
                              end
                    end
          end
end

local ExecuteFunction = {
          TEXT = function(inPresentation, inElement, inFrameIndex, t)
                    local PreviousElement = GetPreviousFrameKeyElement(inPresentation, inElement, inFrameIndex)
                    if PreviousElement then
                              -- interpolate
                    else
                              inElement.handle:Update(ComputePositionByName(inElement.value.p, inElement.value.d),
                                        vec3(inElement.value.d.x, inElement.value.d.y, inElement.value.d.z))
                    end
          end
}

local ExecuteFrame = function(inPresentation, inFrameIndex, t)
          local CurrentFrame = FrameKeys[inFrameIndex]
          local CurrentFrameKeyCount = #CurrentFrame
          for i = 1, CurrentFrameKeyCount, 1 do
                    local Key = CurrentFrame[i]
                    for _, element in pairs(Key.Elements) do
                              print(element[1], ExecuteFunction[element[1]])
                              ExecuteFunction[element[1]](inPresentation, element, inFrameIndex, t)
                    end
          end
end

--[============================================================[
          PRESENTATION  FUNCTION
]============================================================]

Presentation = function(inPresentation)
          local Log = function(inContent)
                    print(string.format("[JkrGUI Present: ] %s", inContent))
          end
          local shouldRun = true
          local Validation = false
          Engine:Load(Validation)
          window = Jkr.CreateWindow(Engine.i, "Hello", vec2(900, 480), 3)
          WindowDimension = window:GetWindowDimension()
          wid = Jkr.CreateWidgetRenderer(Engine.i, window, Engine.e)

          if inPresentation.Config then
                    local conf = inPresentation.Config
                    FontMap.Tiny = wid.CreateFont(conf.Font.Tiny[1], conf.Font.Tiny[2])
                    FontMap.Small = wid.CreateFont(conf.Font.Small[1], conf.Font.Small[2])
                    FontMap.Normal = wid.CreateFont(conf.Font.Normal[1], conf.Font.Normal[2])
                    FontMap.large = wid.CreateFont(conf.Font.large[1], conf.Font.large[2])
                    FontMap.Large = wid.CreateFont(conf.Font.Large[1], conf.Font.Large[2])
                    FontMap.huge = wid.CreateFont(conf.Font.huge[1], conf.Font.huge[2])
                    FontMap.Huge = wid.CreateFont(conf.Font.Huge[1], conf.Font.Huge[2])
                    FontMap.gigantic = wid.CreateFont(conf.Font.gigantic[1], conf.Font.gigantic[2])
                    FontMap.Gigantic = wid.CreateFont(conf.Font.Gigantic[1], conf.Font.Gigantic[2])
          else
                    Log("Error: No Config provided.")
          end

          if shouldRun then
                    --[[
                    Presentation {
                              {Frame = table}
                    }
                    ]]

                    local FrameIndex = 1
                    for _, elements in ipairs(inPresentation) do
                              for __, value in pairs(elements) do
                                        if (__ == "Frame") then
                                                  FrameKeys[FrameIndex] = {}
                                                  local frameElements = value
                                                  --[[==================================================]]
                                                  for felementName, felement in pairs(frameElements) do
                                                            if type(felement) == "table" then
                                                                      for processFunctionIndex, ElementValue in pairs(felement) do
                                                                                ProcessFunctions[processFunctionIndex](
                                                                                          inPresentation, ElementValue,
                                                                                          FrameIndex, felementName)
                                                                                print("processFunctionIndex : ",
                                                                                          processFunctionIndex,
                                                                                          "elementName: ",
                                                                                          felementName)
                                                                      end
                                                            else
                                                                      ProcessLiterals(felementName, felement)
                                                            end
                                                  end
                                                  --[[==================================================]]
                                                  FrameIndex = FrameIndex + 1
                                        end
                              end
                    end


                    WindowClearColor = vec4(0)
                    ExecuteFrame(inPresentation, 1, 0)

                    local function Update()
                              wid.Update()
                    end

                    local function Dispatch()
                              wid.Dispatch()
                    end

                    local function Draw()
                              wid.Draw()
                    end

                    local function MultiThreadedDraws()
                    end

                    local function MultiThreadedExecute()
                    end

                    local oldTime = 0.0
                    local frameCount = 0
                    local e = Engine.e
                    local w = window
                    local mt = Engine.mt
                    while not e:ShouldQuit() and shouldRun do
                              oldTime = w:GetWindowCurrentTime()
                              e:ProcessEvents()
                              w:BeginUpdates()
                              Update()
                              WindowDimension = w:GetWindowDimension()
                              w:EndUpdates()

                              w:BeginDispatches()
                              Dispatch()
                              w:EndDispatches()

                              MultiThreadedDraws()
                              w:BeginUIs()
                              Draw()
                              w:EndUIs()

                              w:BeginDraws(WindowClearColor.x, WindowClearColor.y, WindowClearColor.z,
                                        WindowClearColor.w, 1)
                              MultiThreadedExecute()
                              w:ExecuteUIs()
                              w:EndDraws()
                              w:Present()
                              local delta = w:GetWindowCurrentTime() - oldTime
                              if (frameCount % 100 == 0) then
                                        w:SetTitle("Samprahar Frame Rate" .. 1000 / delta)
                              end
                              frameCount = frameCount + 1
                              mt:InjectToGate("__MtDrawCount", 0)
                    end
          end
end
