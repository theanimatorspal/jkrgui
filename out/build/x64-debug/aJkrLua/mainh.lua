require "jkrgui.ExtraComponents"
require "jkrgui.Resources"
require "jkrgui.LayoutComponents"

ContextMenuEntries_FileMenu = {
    [1] = {
        name = "New",
        action = function()
            print("New")
        end
    },
    [2] = {
        name = "Save",
        action = function()

        end
    },
    [3] = {
        name = "Exit",
        action = function()

        end
    }
}
ContextMenuEntries_EditMenu = {
    [1] = {
        name = "Undo",
        action = function()

        end
    },
    [2] = {
        name = "Redo",
        action = function()

        end
    },
    [3] = {
        name = "Cut",
        action = function()

        end
    },
    [4] = {
        name = "Copy",
        action = function()

        end
    }
}
ContextMenuEntries_View = {
    [1] = {
        name = "Appearance",
        action = function()

        end
    },
    [2] = {
        name = "Search",
        action = function()
            print("Search")
        end
    },
    [3] = {
        name = "Output",
        action = function()

        end
    }
}

ContextMenuEntries_Run = {
    [1] = {
        name = "Start",
        action = function()

        end
    },
    [2] = {
        name = "Add",
        action = function()

        end
    },
    [3] = {
        name = "Enable",
        action = function()

        end
    }
}
ContextMenu_Right = {
    [1] = {
        name = "Refresh",
        action = function()
            print("Refresh")
        end
    }
}

FileMenux = {
    [1] = {
        name = "File",
        action = function(inPosition_3f)
            ContextMenu:Update(ContextMenuEntries_FileMenu, inPosition_3f, vec3(80, 20, 1))
        end
    },
    [2] = {
        name = "Edit",
        action = function(inPosition_3f)
            ContextMenu:Update(ContextMenuEntries_EditMenu, inPosition_3f, vec3(80, 20, 1))
        end
    },
    [3] = {
        name = "View",
        action = function(inPosition_3f)
            ContextMenu:Update(ContextMenuEntries_View, inPosition_3f, vec3(90, 20, 1))
        end
    },
    [4] = {
        name = "Run",
        action = function(inPosition_3f)
            ContextMenu:Update(ContextMenuEntries_Run, inPosition_3f, vec3(80, 20, 1))
        end
    }
}
