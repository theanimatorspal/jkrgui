--[[
JkrGUI lua
]]

function utf8.sub(s, i, j)
        i = utf8.offset(s, i)
        j = utf8.offset(s, j + 1) - 1
        return string.sub(s, i, j)
end

GetWindowDimensions = get_window_dimensions
GetIdentityMatrix = get_identity_matrix
Generator = generator
Shapes = shapes
Translate = translate
FillType = fill_type

Int = math.floor
Jkr = r;

E = event_manager -- EventManager

S = Jkr.sh        -- Shape Renderer
S.Add = S.add
S.AddImage = S.add_image
S.CopyImage = S.copy_image
S.Bind = S.bind
S.BindFillMode = S.bind_fill_mode
S.Draw = S.draw
S.BindImage = S.bind_image
S.Update = S.update


L = Jkr.ln -- Line Renderer



T = Jkr.bt -- Text Renderer
T.SetCurrentFace = T.set_current_face
T.Add = T.add
T.AddFontFace = T.add_font_face
T.Bind = T.bind
T.Draw = T.draw
T.SetTextProperty = T.set_text_property
T.GetTextDimension = T.get_text_dimension
T.Update = T.update

TextH = text_horizontal
TextV = text_vertical
vec2 = vec2
uvec2 = uvec2
vec3 = vec3
vec4 = vec4


require "Config"

Depth = 75
Time = 0 -- Increments each frame
WindowDimension = GetWindowDimensions()

function FrameUpdate()
        Time = Time + 1
        WindowDimension = GetWindowDimensions()
end




Jkr.FontObject = {
        mPath = " ",
        mId = 0,
        mSize = 0,
        New = function(self, inPath, inSize)
                local Object = {}
                setmetatable(Object, self)
                self.__index = self
                Object.mPath = inPath
                Object.mSize = inSize
                Object.mId = T.AddFontFace(Object.mPath, Object.mSize)
                return Object
        end,

        GetDimension = function(self, inString)
                return T.GetTextDimension(inString, Int(self.mId))
        end

}

local TextObject = {
        mString = "",
        mPosition = vec2(0, 20),
        mId = uvec2(0, 0),
        mDimension = vec2(0, 0),
        mColor = vec4(0, 0, 0, 1),
        mFont = nil,
        New = function(self, inText, inFontObject, inPosition_3f)
                local Object = {}
                setmetatable(Object, self)
                self.__index = self

                Object.mString = inText
                T.SetCurrentFace(inFontObject.mId)
                T.SetTextProperty(TextH.left, TextV.top)
                Object.mId = T.Add(Object.mString, vec3(Object.mPosition.x, Object.mPosition.y, Depth))
                Object.mDimension = inFontObject:GetDimension(Object.mString)
                Object.mFont = inFontObject
                return Object
        end,

        Update = function(self, inText, inPos)
                local newL = utf8.len(inText)
                local oldL = utf8.len(self.mString)
                local Pos = vec3(inPos.x, inPos.y, Depth)

                if newL > oldL then
                        local id = T.Add(string.rep(" ", newL - oldL), vec3(math.huge, math.huge, Depth))
                        self.mId.y = self.mId.y + id.y
                else
                        T.Update(Int(self.mId.x), string.rep(" ", oldL), Pos) -- Clear String
                end

                T.Update(Int(self.mId.x), inText, Pos)
                self.mString = inText
                self.mDimension = self.mFont:GetDimension(self.mString)
        end,

        Draw = function(self)
                T.Bind()
                T.Draw(self.mColor, Int(WindowDimension.x), Int(WindowDimension.y), Int(self.mId.x), Int(self.mId.y),
                        GetIdentityMatrix())
        end
}

Jkr.ComponentObject = {
        mPosition_3f = vec3(0, 0, 0),
        mDimension_3f = vec3(0, 0, 0),
        mFocus_b = false,
        mBoundedRectId_i = 0,
        mFocusOnHover_b = false,
        mTransparentToMouse_b = false,
        New = function(self, inPosition_3f, inDimension_3f)
                local Obj = {}
                print("Component Construction")
                setmetatable(Obj, self)
                self.__index = self

                Obj.mPosition_3f = inPosition_3f
                Obj.mDimension_3f = inDimension_3f
                Obj.mFocus_b = false
                Obj.mFocusOnHover_b = true
                Obj.mTransparentToMouse_b = false
                local pos = vec2(Obj.mPosition_3f.x, Obj.mPosition_3f.y)
                local dim = vec2(Obj.mDimension_3f.x, Obj.mDimension_3f.y)
                Obj.mBoundedRectId_i = E.set_bounded_rect(pos, dim, Int(Obj.mPosition_3f.z))
                return Obj
        end,

        Update = function(self, inPosition_3f, inDimension_3f)
                self.mPosition_3f = inPosition_3f
                self.mDimension_3f = inDimension_3f
                local pos = vec2(self.mPosition_3f.x, self.mPosition_3f.y)
                local dim = vec2(self.mDimension_3f.x, self.mDimension_3f.y)
                E.update_bounded_rect(Int(self.mBoundedRectId_i), pos, dim, Int(self.mPosition_3f.z))
        end,

        Event = function(self)
                if self.mFocusOnHover_b or E.is_left_button_pressed() then
                        if self.mTransparentToMouse_b then
                                self.mFocus_b = E.is_mouse_within(Int(self.mBoundedRectId_i), Int(self.mPosition_3f.z))
                        else
                                self.mFocus_b = E.is_mouse_on_top(Int(self.mBoundedRectId_i), Int(self.mPosition_3f.z))
                        end
                else
                        self.mFocus_b = false
                end
        end,

        GetTranslationMatrix = function(self)
                local matrix = GetIdentityMatrix()
                local matrix = Translate(matrix, vec3(self.mPosition.x, self.mPosition.y, 0))
                return matrix
        end
}

Jkr.Components = {}
Jkr.Components.Static = {}

Jkr.Components.Static.ShapeObject = {
        mComponentObject = nil,
        mShapeId = nil,
        mImageId = nil,
        mImagePainter = nil,
        mFillColor = vec4(0, 0, 0, 1),
        New = function(self, inPosition_3f, inDimension_3f, inImagePainter, inImageSize_2f)
                local Obj = {}
                setmetatable(Obj, self)
                self.__index = self

                Obj.mComponentObject = Jkr.ComponentObject:New(inPosition_3f, inDimension_3f)
                print(Obj.mComponentObject.mDimension_3f.x)
                print(Obj.mComponentObject.mDimension_3f.y)
                local Dimension = vec2(Obj.mComponentObject.mDimension_3f.x, Obj.mComponentObject.mDimension_3f.y)
                local rect_gen = Generator(Shapes.rectangle, Dimension)
                Obj.mShapeId =  S.Add(rect_gen, Obj.mComponentObject.mPosition_3f)
                print("ImageButton Build RECT GEN")

                Obj.mImagePainter = false or inImagePainter
                if Obj.mImagePainter then
                        Obj.mImageId = S.AddImage(Int(inImageSize_2f.x), Int(inImageSize_2f.y))
                        S.CopyImage(Obj.mImageId, Obj.mImagePainter)
                end
                print("Image Painter Registered")
                return Obj
        end,
        Update = function (self, inPosition_3f, inDimension_3f, inImagePainter, inImageSize_2f)
               local Dimension = vec2(inDimension_3f.x, inDimension_3f.y)
               local rect_gen = Generator(Shapes.rectangle, Dimension)
               S.Update(Int(self.mShapeId), rect_gen, inPosition_3f) 
               self.mComponentObject:Update(inPosition_3f, inDimension_3f)

               if inImagePainter and inImageSize_2f then
                -- Yet to do
               end
        end,
        Event = function(self)
                self.mComponentObject:Event()
        end,
        Draw = function(self)
                S.Bind()
                if self.mImageId then
                        S.BindFillMode(FillType.image)
                        S.BindImage(self.mImageId)
                else
                        S.BindFillMode(FillType.fill)
                end
                S.Draw(self.mFillColor, Int(WindowDimension.x), Int(WindowDimension.y), Int(self.mShapeId), Int(self.mShapeId), GetIdentityMatrix())
        end,
        CopyFromPainter = function (self)
                S.CopyImage(Int(self.mImageId), self.mImagePainter)
        end
}

Jkr.Components.Static.TextObject = {
        mScissorPosition_2f = nil,
        mScissorDimension_2f = nil,
        mString = nil,
        mFont = nil, -- Font Object
        mId = nil,  
        mDimension_2f = nil,
        mColor = Theme.Colors.Text.Normal,
        New = function (self, inText, inPosition_3f, inFontObject)
                local Obj = Jkr.ComponentObject:New(inPosition_3f, vec3(0, 0, 0))

                setmetatable(Obj, self)
                self.__index = self

                Obj.mString = inText
                Obj.mPosition_3f = inPosition_3f
                T.SetCurrentFace(inFontObject.mId)
                T.SetTextProperty(TextH.left, TextV.top)
                print("Here")
                Obj.mId = T.Add(Obj.mString, vec3(Obj.mPosition_3f.x, Obj.mPosition_3f.y, Depth))
                Obj.mDimension = inFontObject:GetDimension(Obj.mString)
                Obj.mFont = inFontObject
                return Obj
        end,
        Event = function (self) end,
        Draw = function (self)
                T.Bind()
                T.Draw(self.mColor, Int(WindowDimension.x), Int(WindowDimension.y), Int(self.mId.x), Int(self.mId.y), GetIdentityMatrix())
        end,
        SetScissor = function (self)
                if self.mScissorPosition_2f and self.mScissorDimension_2f then
                        Jkr.set_scissor(self.mScissorPosition_2f, self.mScissorDimension_2f)
                end
        end, 
        Update = function (self, inPosition_3f)
                self.mPosition_3f = inPosition_3f
                T.Update(Int(self.mId.x), self.mString, self.mPosition_3f)
        end
}



Jkr.ImagePainters = {
}

Jkr.ImagePainters.Create = function (inImageSize_2f, inStore_b, inGLSL)
                local Obj =  {}
                Obj.mPainter = Jkr.image_painter("RoundedRectangleCache.bin", inGLSL, vec3(16, 16, 1))

                if inStore_b then
                        Obj.mPainter:store()
                else
                        Obj.mPainter:load()
                end
                print("Created")
                Obj.mPainter:register_image(Int(inImageSize_2f.x), Int(inImageSize_2f.y))
                return Obj
        end

Jkr.ImagePainters.RoundedRectangle = {
        mGLSL = [[
vec2 center = vec2(push.mPosDimen.x, push.mPosDimen.y);
vec2 hw = vec2(push.mPosDimen.z, push.mPosDimen.w);
float radius = push.mParam.x;
vec2 Q = abs(xy - center) - hw;

float color = distance(max(Q, vec2(0.0)), vec2(0.0)) + min(max(Q.x, Q.y), 0.0) - radius;
color = smoothstep(-0.05, 0.05, -color);

vec4 old_color = imageLoad(storageImage, to_draw_at);
vec4 final_color = vec4(push.mColor.x * color, push.mColor.y * color, push.mColor.z * color, push.mColor.w * color);
final_color = mix(final_color, old_color, 0.5);

imageStore(storageImage, to_draw_at, final_color);
]]
}

Jkr.ImagePainters.RoundedCircle = {
        mGLSL = [[
vec2 center = vec2(push.mPosDimen.x, push.mPosDimen.y);
vec2 hw = vec2(push.mPosDimen.z, push.mPosDimen.w);
float radius = push.mParam.x;

float color = distance(xy, center) - radius;
color = smoothstep(-0.05, 0.05, -color);

vec4 old_color = imageLoad(storageImage, to_draw_at);
vec4 final_color = vec4(push.mColor.x * color, push.mColor.y * color, push.mColor.z * color, push.mColor.w * color);
final_color = mix(final_color, old_color, 0.5);

imageStore(storageImage, to_draw_at, final_color);
]]
}