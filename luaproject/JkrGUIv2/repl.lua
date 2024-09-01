---@diagnostic disable: lowercase-global
require("JkrGUIv2.Engine.Engine")
require("JkrGUIv2.Engine.Shader")
require("JkrGUIv2.Widgets.Basic")
require("JkrGUIv2.require")
inspect = require("JkrGUIv2.inspect")

Engine:Load(true)
i = Engine.i
e = Engine.e
mt = Engine.mt

function SetMT()
          gate = {}
          setmetatable(gate, {
                    __index = function(_, key)
                              return mt:Get(key, StateId)
                    end,
                    __newindex = function(_, k, v)
                              if k == "_run" and type(v) == "function" then
                                        mt:AddJobF(v)
                              else
                                        mt:Inject(k, v)
                              end
                    end
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

function repl.Window(inThreadIndex)
          if not inw then
                    -- keep the widnow global
                    repl.w = Jkr.CreateWindow(i, "JkrGUIv2 REPL")
                    inw = repl.w
          end
          if not inThreadIndex then
                    inThreadIndex = 0
          end
          mt:Inject("w", inw)
          mt:AddJobFIndex(
                    function()
                              local windowShouldRun = true
                              local e = gate.e
                              local w = gate.w
                              local i = gate.i
                              while windowShouldRun do
                                        local f = gate.windowFunction
                                        if e:IsCloseWindowEvent() then
                                                  windowShouldRun = false
                                                  if type(f) == "function" then
                                                            f()
                                                  end
                                        end
                              end
                              w:Hide()
                    end, inThreadIndex
          )
end

mt:Inject("repl", repl)
