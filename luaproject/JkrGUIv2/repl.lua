require("JkrGUIv2.Engine.Engine")
require("JkrGUIv2.Engine.Shader")
require("JkrGUIv2.Widgets.Basic")
require("JkrGUIv2.require")
inspect = require("JkrGUIv2.inspect")
Engine:Load(true)
i = Engine.i
e = Engine.e
mt = Engine.mt

mt:Inject("Engine", Engine)
mt:Inject("i", i)
mt:Inject("e", e)
mt:Inject("mt", mt)

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

function repl.runWindow(inw, inThreadIndex, inevent)
          mt:Inject("w", inw)
          mt:Inject("inevent", inevent)
          -- upvalues are not allowed FUUU
          mt:AddJobFIndex(
                    function()
                              windowShouldRun = true
                              while windowShouldRun do
                                        if e:IsCloseWindowEvent() then
                                                  windowShouldRun = false
                                        end
                              end
                              w:Hide()
                    end, inThreadIndex
          )
end

mt:Inject("repl", repl)
