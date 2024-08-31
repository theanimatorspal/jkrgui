require("JkrGUIv2.Engine.Engine")
require("JkrGUIv2.Engine.Shader")
require("JkrGUIv2.Widgets.Basic")
require("JkrGUIv2.require")
inspect = require("JkrGUIv2.inspect")
Engine:Load(true)
i = Engine.i
e = Engine.e
mt = Engine.mt

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

mt:Inject("repl", repl)
