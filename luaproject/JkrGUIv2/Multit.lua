require "JkrGUIv2.Basic"
require "JkrGUIv2.Widgets"
require "JkrGUIv2.Threed"

--[============================================================[
    MultiThreading
]============================================================]

function Jkr.ConfigureMultiThreading(inMultiThreading, inTable)
    inMultiThreading:Inject("mtMultiThreadingFetchInjected", Jkr.MultiThreadingFetchInjected)
    inMultiThreading:Inject("mtGetDefaultResource", Jkr.GetDefaultResource)
    inMultiThreading:Inject("mtCreateShapesHelper", Jkr.CreateShapesHelper)
    inMultiThreading:Inject("ImportShared", ImportShared)
    inMultiThreading:Inject("Jkrmt", Jkrmt)
    if inTable then
        for i = 1, #inTable, 1 do
            inMultiThreading:Inject(inTable[i][1], inTable[i][2])
        end
    end
end
