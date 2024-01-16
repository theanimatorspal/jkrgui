require "jkrgui.jkrgui"

Lerp = function(t, y1, y2)
        return ((y2 - y1) * t + y1)
end

Line = {
        mPositon1 = vec2(0, 0),
        mPosition2 = vec2(0, 0),
        mLine_Id = nil,
        New = function(self, inPosition1, inPosition2)
                local Obj = {
                        mPosition1 = inPosition1,
                        mPosition2 = inPosition2,
                        mLine_Id = nil,
                }
                setmetatable(Obj, self)
                self.__index = self
                Obj.mLine_Id = L.Add(inPosition1, inPosition2, 1)
                return Obj
        end,
        Draw = function(self)
                L.Bind()
                L.Draw(vec4(0, 0, 0, 1), Int(WindowDimension.x), Int(WindowDimension.y), Int(self.mLine_Id),
                        Int(self.mLine_Id),
                        GetIdentityMatrix())
        end,

        Update = function(self)
                L.Update(Int(self.mLine_Id), self.mPosition1, self.mPosition2, 1)
        end
}

Rect = {
        mDimension_2f = vec2(0, 0),
        mPosition_3f = vec3(0, 0, 0),
        mId1 = nil,


        New = function(self, inDimension_2f, inPosition_3f)
                local Obj = {
                        mDimension_2f = inDimension_2f,
                        mPosition_3f = inPosition_3f,
                        mId1 = nil,

                }
                setmetatable(Obj, self)
                self.__index = self
                local rect_gen1 = Generator(Shapes.rectangle, inDimension_2f)
                Obj.mId1 = S.Add(rect_gen1, Obj.mPosition_3f)
                return Obj
        end,

        Draw = function(self, inColor1)
                S.Bind()
                S.BindFillMode(FillType.fill)
                S.Draw(inColor1, Int(WindowDimension.x), Int(WindowDimension.y), Int(self.mId1), Int(self.mId1),
                        GetIdentityMatrix())
        end,

        Update = function(self)
                local Dimension = vec2(self.mDimension_2f.x, self.mDimension_2f.y)
                local rect_gen = Generator(Shapes.rectangle, Dimension)
                S.Update(Int(self.mId1), rect_gen, self.mPosition_3f)
        end

}

ImgRect = {

        mDimension_2f = vec2(0, 0),
        mPosition_3f = vec3(0, 0, 0),
        mId = nil,
        mImgId = nil,
        New = function(self, inDimension_2f, inPosition_3f, inImageFileName_String)
                local Obj = {
                        mDimension_2f = inDimension_2f,
                        mPosition_3f = inPosition_3f,
                        mId = nil,
                        mImgId = nil,

                }

                setmetatable(Obj, self)
                self.__index = self
                local rect_gen1 = Generator(Shapes.rectangle, inDimension_2f)
                Obj.mId = S.Add(rect_gen1, Obj.mPosition_3f)
                Obj.mImgId = S.AddImage(inImageFileName_String)
                return Obj
        end,

        Draw = function(self)
                S.Bind()
                S.BindFillMode(FillType.image)
                S.BindImage(Int(self.mImgId))
                S.Draw(vec4(1, 1, 1, 1), Int(WindowDimension.x), Int(WindowDimension.y), Int(self.mId), Int(self.mId),
                        GetIdentityMatrix())
        end,


        Update = function(self)
                local Dimension = vec2(self.mDimension_2f.x, self.mDimension_2f.y)
                local rect_gen = Generator(Shapes.rectangle, Dimension)
                S.Update(Int(self.mId), rect_gen, self.mPosition_3f)
        end



}

Card = {
        mRect = nil,
        mImg = nil,
        mIsCardBackward = false,

        New = function(self, inDimension_2f, inPosition_3f, infileName)
                local Obj = {
                }

                local ImageKoDimension = vec2(inDimension_2f.x - 4, inDimension_2f.y - 4)
                local ImageKoPosition = vec3(inPosition_3f.x + 2, inPosition_3f.y + 2, inPosition_3f.z - 1)
                Obj.mRect = Rect:New(inDimension_2f, inPosition_3f)
                Obj.mImg = ImgRect:New(ImageKoDimension, ImageKoPosition, infileName)
                Obj.mIsCardBackward = false
                setmetatable(Obj, self)
                self.__index = self
                return Obj
        end,

        Draw = function(self)
                self.mRect:Draw(vec4(0, 1, 0, 1))
                self.mImg:Draw()
        end,

        Event = function(self)
                local MousePosition = E.get_mouse_pos()
                local Dimension = self.mRect.mDimension_2f
                local Position = self.mRect.mPosition_3f

                if MousePosition.x > Position.x and MousePosition.x < Position.x + Dimension.x and MousePosition.y > Position.y and MousePosition.y < Dimension.y + Position.y and E.is_left_button_pressed() then
                        if self.mIsCardBackward == false then
                                self.mRect.mPosition_3f = vec3(self.mRect.mPosition_3f.x, self.mRect.mPosition_3f.y,
                                        self.mRect.mPosition_3f.z + 10)
                                self.mImg.mPosition_3f = vec3(self.mImg.mPosition_3f.x, self.mImg.mPosition_3f.y,
                                        self.mImg.mPosition_3f.z + 10)
                                self.mIsCardBackward = true
                        else
                                self.mRect.mPosition_3f = vec3(self.mRect.mPosition_3f.x, self.mRect.mPosition_3f.y,
                                        self.mRect.mPosition_3f.z - 10)
                                self.mImg.mPosition_3f  = vec3(self.mImg.mPosition_3f.x, self.mImg.mPosition_3f.y,
                                        self.mImg.mPosition_3f.z - 10)
                                self.mIsCardBackward    = false
                        end

                        self.mRect:Update()
                        self.mImg:Update()
                end
        end
}

local PresentationTextObjects = {}
local PresentationTextObject_index = 1

local PresentationLineObjects = {}
local PresentationLineObject_index = 1

JkrPresent = {}

JkrPresent.NullifyPositions = function()
        for index, value in ipairs(PresentationTextObjects) do
                value:Update(vec3(0, 0, 1000))
        end
        for index, value in ipairs(PresentationLineObjects) do
                value:Update(vec3(0, 0, 1000))
        end
end




JkrPresent.LoadResources = function()
        GlobalFontObject_large = Jkr.FontObject:New("font.ttf", 13)
        GlobalFontObject_medium = Jkr.FontObject:New("font.ttf", 9)
        GlobalFontObject_small = Jkr.FontObject:New("font.ttf", 6)
end

JkrPresent.Draw = function()
        for i, value in ipairs(PresentationTextObjects) do
                value:Draw()
        end
        StartLineId = PresentationLineObjects[1].mLine_Id
        EndLineId = PresentationLineObjects[#PresentationLineObjects].mLine_Id
        L.Bind()
        L.Draw(vec4(0, 0, 0, 1), Int(WindowDimension.x), Int(WindowDimension.y), Int(StartLineId), Int(EndLineId),
                GetIdentityMatrix())
end

JkrPresent.CreateAnimText = function(inString)
        local noOfCharacters = #inString
        local i = PresentationTextObject_index
        local FunctionTobeCalledForEachCharacter = function(c) -- estaii khalko function expect garxa string:gsub vanne function le
                PresentationTextObjects[PresentationTextObject_index] = Jkr.Components.Static.TextObject:New(c,
                        vec3(math.huge, math.huge, 150), GlobalFontObject_medium)
                PresentationTextObject_index = PresentationTextObject_index + 1
        end
        inString:gsub(".", FunctionTobeCalledForEachCharacter)
        return i
end

JkrPresent.CreateCircleText_Moving_animation = function(inId, inString, inPosition_3f, inMovingRadius,
                                                        inMovingTheta, inColor)
        local noOfCharacters = #inString
        local delThita = 2 * math.pi / (noOfCharacters)
        local theta = inMovingTheta
        local i = inId
        local j = 1
        local keyframe = {}
        local FunctionTobeCalledForEachCharacter = function(c) -- estaii khalko function expect garxa string:gsub vanne function le
                local x = inMovingRadius * math.cos(theta) + inPosition_3f.x
                local y = inMovingRadius * math.sin(theta) + inPosition_3f.y
                keyframe[j] = { pos = vec3(x, y, inPosition_3f.z), color = inColor }
                j = j + 1
                theta = theta + delThita
                i = i + 1
        end
        inString:gsub(".", FunctionTobeCalledForEachCharacter)
        return keyframe
end


JkrPresent.CreateLineText_Moving_Animation = function(inId, inString, inPosition_3f, inMovingDel, inMovingDelY, inColor)
        local noOfCharacters = #inString
        local i = inId
        local x = inPosition_3f.x
        local y = inPosition_3f.y
        local delX = inMovingDel
        local delY = inMovingDelY
        local keyframe = {}
        local j = 1
        local FunctionTobeCalledForEachCharacter = function(c)
                x = x + delX
                y = y + delY
                i = i + 1
                keyframe[j] = { pos = vec3(x, y, inPosition_3f.z), color = inColor }
                j = j + 1
        end
        inString:gsub(".", FunctionTobeCalledForEachCharacter)
        return keyframe
end

JkrPresent.LerpAnimationText = function(inId, inT, keyframe1, keyframe2)
        local j = inId
        for i = 1, #keyframe1, 1 do
                local x = Lerp(inT, keyframe1[i].pos.x, keyframe2[i].pos.x)
                local y = Lerp(inT, keyframe1[i].pos.y, keyframe2[i].pos.y)
                local z = Lerp(inT, keyframe1[i].pos.z, keyframe2[i].pos.z)
                local newposition = vec3(x, y, z)
                local xc = Lerp(inT, keyframe1[i].color.x, keyframe2[i].color.x)
                local yc = Lerp(inT, keyframe1[i].color.y, keyframe2[i].color.y)
                local zc = Lerp(inT, keyframe1[i].color.z, keyframe2[i].color.z)
                local wc = Lerp(inT, keyframe1[i].color.w, keyframe2[i].color.w)
                local newcolor = vec4(xc, yc, zc, wc)
                PresentationTextObjects[j].mColor = newcolor
                PresentationTextObjects[j]:Update(newposition)
                j = j + 1
        end
end

JkrPresent.LerpAnimationVarArgsText = function(inId, inT, ...) -- ... vaneko keyframes ho, arg vaneko keyframe ko table
        local args = table.pack(...)
        local noOfAgruments = args.n
        local t = inT * (noOfAgruments - 1)
        local oldKeyframe = nil
        local newKeyframe = nil
        for i = 2, noOfAgruments, 1 do
                oldKeyframe = args[i - 1]
                newKeyframe = args[i]
                if t < i - 1 and t > i - 2 then
                        JkrPresent.LerpAnimationText(inId, t - (i - 2), oldKeyframe, newKeyframe)
                end
        end
end

JkrPresent.LerpAnimationTableText = function(inId, inT, inKeyFrameTable) -- ... vaneko keyframes ho, arg vaneko keyframe ko table
        local args = inKeyFrameTable
        local noOfAgruments = #inKeyFrameTable
        local t = inT * (noOfAgruments - 1)
        local oldKeyframe = nil
        local newKeyframe = nil
        for i = 2, noOfAgruments, 1 do
                oldKeyframe = args[i - 1]
                newKeyframe = args[i]
                if t < i - 1 and t > i - 2 then
                        JkrPresent.LerpAnimationText(inId, t - (i - 2), oldKeyframe, newKeyframe)
                end
        end
end


JkrPresent.JoinKeyframe = function(...)
        local t = {}
        for n = 1, select("#", ...) do
                local arg = select(n, ...)
                if type(arg) == "table" then
                        for _, v in ipairs(arg) do
                                t[#t + 1] = v
                        end
                else
                        t[#t + 1] = arg
                end
        end
        return t
end





-- THIS IS THE LINE ANIMATION MODULE

JkrPresent.CreateAnimLines = function(inNoOfLines)
        local i = PresentationLineObject_index
        for j = i, i + inNoOfLines, 1 do
                PresentationLineObjects[j] = Line:New(vec2(0, 0), vec2(0, 0))
                PresentationLineObject_index = PresentationLineObject_index + 1
        end
        return i
end

JkrPresent.CreateCircleLine_Moving_Animation = function(inId, inNoOfLines, inRadius, inDelTheta, inCenter)
        local theta = inDelTheta
        local delTheta = 2 * math.pi / inNoOfLines
        local position = {}
        local j = 1
        for i = inId, inNoOfLines + inId - 1, 1 do
                local x = inRadius * math.cos(theta) + inCenter.x
                local y = inRadius * math.sin(theta) + inCenter.y
                local xNext = inRadius * math.cos(theta + delTheta) + inCenter.x
                local yNext = inRadius * math.sin(theta + delTheta) + inCenter.y
                position[j] = vec3(x, y, 1)
                PresentationLineObjects[i].mPosition1 = vec2(x, y)
                position[j + 1] = vec3(xNext, yNext, 1)
                PresentationLineObjects[i].mPosition2 = vec2(xNext, yNext)
                j = j + 2
                theta = theta + delTheta
                -- PresentationLineObjects[i]:Update()
        end

        return position
end

JkrPresent.CreateLineLine_Moving_Animation = function(inId, noOfLines, inPosition1, inMovingDelX, inMovingDelY,
                                                      inMovingGap)
        local j = 1
        local k = 1
        local position = {}

        for i = inId, noOfLines + inId - 1, 1 do
                local x = inPosition1.x + inMovingDelX * (j - 1) + inMovingGap
                local y = inPosition1.y + inMovingDelY * (j - 1)
                local xNext = inPosition1.x + inMovingDelX * (j - 1) + inMovingDelX
                local yNext = y + inMovingDelY
                position[k] = vec3(x, y, 1)
                PresentationLineObjects[i].mPosition1 = vec2(x, y)
                position[k + 1] = vec3(xNext, yNext, 1)
                PresentationLineObjects[i].mPosition2 = vec2(xNext, yNext)
                -- PresentationLineObjects[i]:Update()
                j = j + 1
                k = k + 2
        end
        return position
end


JkrPresent.LerpAnimationLine = function(inId, inT, inPositionTable1, inPositionTable2)
        local j = inId
        for i = 1, #inPositionTable2, 2 do
                local x = Lerp(inT, inPositionTable1[i].x, inPositionTable2[i].x)
                local y = Lerp(inT, inPositionTable1[i].y, inPositionTable2[i].y)
                local z = Lerp(inT, inPositionTable1[i].z, inPositionTable2[i].z)
                PresentationLineObjects[j].mPosition1 = vec2(x, y)
                local xNext = Lerp(inT, inPositionTable1[i + 1].x, inPositionTable2[i + 1].x)
                local yNext = Lerp(inT, inPositionTable1[i + 1].y, inPositionTable2[i + 1].y)
                local zNext = Lerp(inT, inPositionTable1[i + 1].z, inPositionTable2[i + 1].z)
                PresentationLineObjects[j].mPosition2 = vec2(xNext, yNext)
                PresentationLineObjects[j]:Update()
                j = j + 1
        end
end

JkrPresent.LerpAnimationTableLine = function(inId, inT, inKeyFrames) -- ... vaneko keyframes ho, arg vaneko keyframe ko table
        local args = inKeyFrames
        local noOfAgruments = #args
        local t = inT * (noOfAgruments - 1)
        local oldKeyframe = nil
        local newKeyframe = nil
        for i = 2, noOfAgruments, 1 do
                oldKeyframe = args[i - 1]
                newKeyframe = args[i]
                if t < i - 1 and t > i - 2 then
                        JkrPresent.LerpAnimationLine(inId, t - (i - 2), oldKeyframe, newKeyframe)
                end
        end
end


-- THIS IS THE PRESENATATION MODULE

JkrPresent.CreateAnimatedPointText = function(inPoints, inPosition, inMovingDel)
        local large_string = ""
        local lengths = { 0 }
        for i = 1, #inPoints, 1 do
                large_string = large_string .. inPoints[i]
                lengths[#lengths + 1] = string.len(inPoints[i])
        end

        local no_of_segments = 8
        local linec = no_of_segments * #inPoints
        local Line_id = JkrPresent.CreateAnimLines(linec)

        local id = JkrPresent.CreateAnimText(large_string)

        local text_keyframes = {}
        local line_keyframes = {}
        local point_arrived_color = vec4(0, 0, 0, 1)
        local point__not_arrived_color = vec4(0, 0, 0, 1)
        local point_not_arrived_position = vec3(1920 * 2, 1920 * 2, 5)


        for j = 1, #inPoints + 1, 1 do
                local text_unjoined_keys = {}
                local line_unjoined_keys = {}

                for i = 1, #inPoints, 1 do
                        local point_pos = vec3(inPosition.x, inPosition.y * i, 5)
                        local point_rect_pos = vec3(point_pos.x - inMovingDel * 2, point_pos.y, point_pos.z)
                        if i < j then
                                text_unjoined_keys[i] = JkrPresent.CreateLineText_Moving_Animation(id + lengths[i],
                                        inPoints[i], point_pos, inMovingDel, 0, point_arrived_color)
                                line_unjoined_keys[i] = JkrPresent.CreateCircleLine_Moving_Animation(
                                Line_id + no_of_segments * (i - 1), no_of_segments, 20, 0, point_rect_pos)
                        else
                                text_unjoined_keys[i] = JkrPresent.CreateCircleText_Moving_animation(id + lengths[i],
                                        inPoints[i], point_not_arrived_position, WindowDimension.x, 100 * i,
                                        point__not_arrived_color)
                                line_unjoined_keys[i] = JkrPresent.CreateLineLine_Moving_Animation(
                                Line_id + no_of_segments * (i - 1), no_of_segments, point_rect_pos, 200, 0, 0)
                        end
                end

                local text_joinedkey = {}
                for i = 1, #inPoints, 1 do
                        text_joinedkey = JkrPresent.JoinKeyframe(text_joinedkey, text_unjoined_keys[i])
                end
                text_keyframes[j] = text_joinedkey

                local line_joined_key = {}
                for i = 1, #inPoints, 1 do
                        line_joined_key = JkrPresent.JoinKeyframe(line_joined_key, line_unjoined_keys[i])
                end
                line_keyframes[j] = line_joined_key
        end

        line_keyframes[#line_keyframes + 1] = JkrPresent.CreateCircleLine_Moving_Animation(Line_id, linec, 500, 0,
                vec3(WindowDimension.x / 2, WindowDimension.y / 2, 5))
        text_keyframes[#text_keyframes + 1] = JkrPresent.CreateCircleText_Moving_animation(id, large_string,
                vec3(WindowDimension.x / 2, WindowDimension.y / 2, 5), 500, 0, vec4(1, 0, 1, 1))

        return { text_id = id, text_key = text_keyframes, text_count = large_string:len(), line_id = Line_id, line_key =
        line_keyframes, line_count = linec }
end


local Keys = {}
local ikey1 = 0
local ikey2 = 1
local is_left_animation = false

JkrPresent.AddSlide = function(inKeys) Keys[#Keys+1] = inKeys end

JkrPresent.Event = function()
        if E.is_keypress_event() then
                Time = 0

                if E.is_key_pressed(Key.SDLK_RIGHT) and not is_left_animation then
                        ikey1  = ikey1 + 1
                end

                if E.is_key_pressed(Key.SDLK_LEFT) and is_left_animation then
                        ikey2  = ikey2 + 1
                end
        end
end

JkrPresent.Update = function(Keys)
        for index, value in ipairs(Keys) do
                
        end
end
