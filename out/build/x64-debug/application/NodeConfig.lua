NodeEditor_Bool = 1
NodeEditor_Vec2 = 2
NodeEditor_Vec3 = 3
NodeEditor_Ivec2 = 4
NodeEditor_Ivec3 = 5
NodeEditor_String = 6



id1 = NodeEditor_CreateNode("Zero", 0.5, 0.4, 0.2, 1.0, 100, 100)
NodeEditor_RegisterNumberOfInputs(id1, NodeEditor_Bool, 0)
NodeEditor_RegisterNumberOfInputs(id1, NodeEditor_Bool, 0)

id2 = NodeEditor_CreateNode("One", 0.5, 0.4, 0.2, 1.0, 100, 200)
NodeEditor_RegisterNumberOfInputs(id2, NodeEditor_Bool, 0)
NodeEditor_RegisterNumberOfInputs(id2, NodeEditor_Bool, 0)
NodeEditor_RegisterNumberOfInputs(id2, NodeEditor_Bool, 0)
NodeEditor_RegisterNumberOfInputs(id2, NodeEditor_Bool, 0)
NodeEditor_RegisterNumberOfInputs(id2, NodeEditor_Bool, 0)

id3 = NodeEditor_CreateNode("Two", 0.5, 0.9, 0.3, 1.0, 100, 300)
NodeEditor_RegisterNumberOfInputs(id3, NodeEditor_Bool, 0)
NodeEditor_RegisterNumberOfInputs(id3, NodeEditor_Bool, 0)
NodeEditor_RegisterNumberOfInputs(id3, NodeEditor_Bool, 0)

id4 = NodeEditor_CreateNode("Two", 0.5, 0.9, 0.3, 1.0, 100, 500)
NodeEditor_RegisterNumberOfInputs(id4, NodeEditor_Bool, 0)
NodeEditor_RegisterNumberOfInputs(id4, NodeEditor_Bool, 0)
NodeEditor_RegisterNumberOfInputs(id4, NodeEditor_Bool, 0)

NodeEditor_AddConnection(id1, id2, 0)
NodeEditor_AddConnection(id1, id2, 1)
NodeEditor_AddConnection(id1, id2, 2)
NodeEditor_AddConnection(id1, id2, 3)
NodeEditor_AddConnection(id1, id2, 4)

NodeEditor_AddConnection(id2, id3, 2)

pos = uvec2.new(500, 100)
NodeEditor_UpdateNodePosition(id3, pos)

function NodeEventCallBack(inCurrentHoveredNode, inMouseButtonPressed, inKeyboardButtonPressed, inMenuBarSelection)
	if(inCurrentHoveredNode ~= -1 and inMouseButtonPressed == 2) then
		NodeEditor_UpdateNodePosition(inCurrentHoveredNode, pos)
	end
	if(inMenuBarSelection.x ~= -100 and inMenuBarSelection.y ~= -100) then
		if(inMenuBarSelection.x == 0 and inMenuBarSelection.y == 1) then
			id3 = NodeEditor_CreateNode("Two", 0.5, 0.9, 0.3, 1.0, 500, 500)
			NodeEditor_RegisterNumberOfInputs(id3, NodeEditor_Bool, 0)
			NodeEditor_RegisterNumberOfInputs(id3, NodeEditor_Bool, 0)
			NodeEditor_RegisterNumberOfInputs(id3, NodeEditor_Bool, 0)
		end
	end
end
