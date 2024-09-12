---@diagnostic disable: lowercase-global
require("JkrGUIv2.Engine.Engine")
require("JkrGUIv2.Engine.Shader")
require("JkrGUIv2.Widgets.Basic")
require("JkrGUIv2.Widgets.General")
require("JkrGUIv2.require")
inspect = require("JkrGUIv2.inspect")

Engine:Load(true)
i = Engine.i
e = Engine.e
mt = Engine.mt

function SetMT()
          gate = {
                    run = function(f, t)
                              mt:AddJobFIndex(f, t)
                    end
          }
          setmetatable(gate, {
                    __index = function(_, key)
                              return mt:Get(key, StateId)
                    end,
                    __newindex = function(_, k, v)
                              if k == "_run" and type(v) == "function" then
                                        mt:AddJobF(v)
                              elseif type(k) == "number" then
                                        mt:AddJobFIndex(v, k)
                              else
                                        mt:Inject(k, v)
                              end
                    end,
          })
end

SetMT()

gate.Engine = Engine
gate.i = i
gate.e = e
gate.mt = mt
gate.SetMT = SetMT

mt:ExecuteAll(function()
          require("JkrGUIv2.Engine.Engine")
          require("JkrGUIv2.Engine.Shader")
          require("JkrGUIv2.Widgets.Basic")
          require("JkrGUIv2.require")
          inspect = require("JkrGUIv2.inspect")
          SetMT = mt:Get("SetMT", StateId)
          SetMT()
end)


repl = {}
function repl.GetArgs(func)
          local args = {}
          for i = 1, debug.getinfo(func).nparams, 1 do
                    table.insert(args, debug.getlocal(func, i));
          end
          for k, v in pairs(args) do
                    print(k, v)
          end
end

function repl.Window(inThreadIndex, showWindow)
          if not repl.w then
                    -- keep the widnow global
                    repl.w = Jkr.CreateWindow(i, "JkrGUIv2 REPL")
                    inw = repl.w
                    gate.w = repl.w
          end
          if not inThreadIndex then
                    inThreadIndex = 0
          end

          gate.window_cc = vec4(1, 1, 1, 1)

          if not gate.window_up then gate.window_up = function() end end
          if not gate.window_uidraw then gate.window_uidraw = function() end end
          if not gate.window_disp then gate.window_disp = function() end end

          repl.w:Hide()

          gate.window_loop = false
          if showWindow then
                    repl.w:Show()
                    gate[inThreadIndex] = function()
                              gate.window_loop = true
                              local e = gate.e
                              local w = gate.w
                              local i = gate.i
                              while gate.window_loop do
                                        WindowClearColor = gate.window_cc
                                        WindowUpdateFunction = gate.window_up
                                        WindowUIDrawFunction = gate.window_uidraw
                                        WindowDispatchFunction = gate.window_disp
                                        wid = gate.wid

                                        w:BeginUpdates()
                                        WindowUpdateFunction()
                                        w:EndUpdates()

                                        w:BeginDispatches()
                                        WindowDispatchFunction()
                                        w:EndDispatches()

                                        w:BeginUIs()
                                        WindowUIDrawFunction()
                                        w:EndUIs()

                                        w:BeginDraws(WindowClearColor.x, WindowClearColor.y,
                                                  WindowClearColor.z, WindowClearColor.w, 1)
                                        w:ExecuteUIs()
                                        w:EndDraws()
                                        w:Present()
                                        if e:IsCloseWindowEvent() then
                                                  gate.window_loop = false
                                        end
                              end
                              w:Hide()
                    end
          end
end

function repl.WidgetRenderer()
          if not wid then
                    wid = Jkr.CreateWidgetRenderer(gate.i, gate.w, gate.e)
          end
          gate.wid = wid
          gate.window_up = function() wid:Update() end
          gate.window_uidraw = function() wid:Draw() end
          gate.window_disp = function() wid:Dispatch() end
end

gate.repl = repl

-- NETWORKING STUFF
---------------------------------------------------
function repl.SetupClient()
          ip = "192.168.101.9"
          Jkr.AddClient()
          Jkr.ConnectFromClient(0, ip, 6000)
          -- message = Jkr.Message()
          -- message:InsertFloat(1.04)
          -- Jkr.SendMessageFromClient(0, message)
          -- Jkr.GetTrapMessagesBuffer()
end

function repl.SetupServer()
          Jkr.StartServer(6000)
          gate[0] = function()
                    print("Network Loop")
                    while true do
                              Jkr.UpdateServer(50, false)
                              local messages = Jkr.GetTrapMessagesBuffer()
                              if #messages ~= 0 then
                                        gate.nmessage = messages[1]
                                        print("Network Loop End")
                                        return;
                              end
                              Jkr.ReleaseMessagesBuffer()
                    end
          end
end
