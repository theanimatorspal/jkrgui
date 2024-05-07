require "JkrGUIv2.Basic"
require "JkrGUIv2.Widgets"
require "JkrGUIv2.Threed"
require "JkrGUIv2.Multit"
require "JkrGUIv2.ShaderFactory"

Engine = {}
Engine.Load = function(self, inEnableValidation)
          self.i = Jkr.CreateInstance(inEnableValidation)
          self.e = Jkr.CreateEventManager()
          self.mt = Jkr.MultiThreading(self.i)
end
