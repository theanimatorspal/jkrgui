--[[
JkrGUI lua
]]


local AlternativeTextRenderer = true

-- this is for utf8 strings (the string that supports कखग), ignore it

function utf8.sub(s, i, j)
        i = utf8.offset(s, i)
        j = utf8.offset(s, j + 1) - 1
        return string.sub(s, i, j)
end

-- These are aliases (Same name for an identifier), the Lua Bindings that is generated has snake case (snake_case), trying to convert into
-- CamelCase (words separated by Capital letters).


--[[
returns vec2 that gives window's width and height, local dimen = GetWindowDimensions()
dimen.x ani dimen.y garna milxa
]]
GetWindowDimensions = get_window_dimensions


--[[
        returns identity matrix mat4 = {
                1, 0, 0, 0
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
        }
]]
GetIdentityMatrix = get_identity_matrix

--[[
        This is a Usertype, It represents Shapes OR generates shapes
]]
Generator = generator

--[[
        ENUM VANEKO K HO VANNE KURO
        enum class state = {
                running, 
                stopped,
                started
        };
        auto variable = state::running;
        if(variable == state::stopped)
        {
                stop_program();
        }


       THIS is an enum that stores type of shape object
       shapes.rectangle, shapes.bezier2_8, shapes.circle
]]

Shapes = shapes

--[[
        This is the function that translates a matrix by vec3
        local translated_matrix = translate(get_identity_matrix(), vec3(5, 5, 10))
]]
Translate = translate

--[[
        This is also an enum
        fill_type.image
        fill_type.fill
        fill_type.continous_line
]]
FillType = fill_type

--[[
        lua has only number as types of number, not int, float, double, nothing like that

        to convert float to int, we should do math.floor(number)
        local x = 5.0
        x = math.floor(x)
        If a function expects an integer, we should do math.floor(number)
        After this alias, we can do Int(x)
]]

Int = math.floor

--[[
        Jkr is basically a namespace "Renderer" namespace to be precise
]]
Jkr = r;

--[[
       This is the entity that can give information about mouse keyboard etc,
       E.is_left_button_pressed() -- return bool
       E.is_right_button_pressed() -- return bool
]]
E = event_manager -- EventManager

local text_bound = false
local line_bound = false

--[[
        This is the shape renderer, we can do S.something
]]

S = Jkr.sh        -- Shape Renderer
--[[
        We can add shapes with S.Add()
]]
S.Add = S.add
S.AddImage = S.add_image
S.CopyImage = S.copy_image
S.Bind = S.bind
S.BindFillMode = S.bind_fill_mode
S.Draw = S.draw
S.BindImage = S.bind_image
S.Update = S.update

--[[
        This is the line renderer
]]
L = Jkr.ln -- Line Renderer
L.Add = L.add
L.Update = L.update
L.Draw = L.draw
L.Bind = L.bind
--[[
        This is shape renderer
]]
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

-- Suggesstions aaos vanera lekheko ho yo chae
vec2 = vec2
uvec2 = uvec2
vec3 = vec3
vec4 = vec4


Key = key

require "Config" -- #include "Config" vane jastai C ma

--[[
        Depth value ranges from 0 to 100, 
        0 means nearest to the camera
        100 means farthest from the camera
        Depth is basically a  reference taken.
]]
Depth = 75
Time = 0 -- Increments each frame
WindowDimension = GetWindowDimensions() -- Can get Window dimensions just by doing WindowDimension.x, WindowDimension.y

-- To be called at Update Callback
function FrameUpdate()
        Time = Time + 1
        WindowDimension = GetWindowDimensions()
end



--[[
        This is a fontObject, which holds a certain font of certain size
]]

Jkr.FontObject = {
        mPath = " ",
        mId = 0,
        mSize = 0,
        New = function(self, inPath, inSize)
                local Object = {
                        mPath = " ",
                        mId = 0,
                        mSize = 0
                }
                setmetatable(Object, self)
                self.__index = self                     -- Elle garne k ho vane Returned object bata Obj.GetDimension etc garna milxa
                Object.mPath = inPath
                Object.mSize = inSize
                Object.mId = T.AddFontFace(Object.mPath, Object.mSize)
                return Object
        end,
        -- returns vec2
        GetDimension = function(self, inString)
                return T.GetTextDimension(inString, Int(self.mId))
        end
}

--[[
        As everything is Widgets in flutter, it is similar here, everything is a ComponentObject,
        It has position, dimension, focus and stuff like those
]]

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
                print("Component Construction Finished")
                return Obj
        end,

        -- This is function that has to run each frame, OR on demand
        Update = function(self, inPosition_3f, inDimension_3f)
                self.mPosition_3f = inPosition_3f
                self.mDimension_3f = inDimension_3f
                local pos = vec2(self.mPosition_3f.x, self.mPosition_3f.y)
                local dim = vec2(self.mDimension_3f.x, self.mDimension_3f.y)
                E.update_bounded_rect(Int(self.mBoundedRectId_i), pos, dim, Int(self.mPosition_3f.z))
        end,

        -- Mouse le maathi gayo vane focus hune nahune
        -- Jastai, mTransparentToMouse_b = true vayo vane, kunai element ko muntira vae pani focus hunxa
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
        mScissorPosition_2f = nil,
        mScissorDimension_2f = nil,
        mComponentObject = nil,
        mShapeId = nil,
        mImageId = nil,
        mImagePainter = nil,
        mFillColor = nil,
        New = function(self, inPosition_3f, inDimension_3f, inImagePainter, inImageSize_2f)
                local Obj = {
                        mScissorPosition_2f = nil,
                        mScissorDimension_2f = nil,
                        mComponentObject = nil,
                        mShapeId = nil,
                        mImageId = nil,
                        mImagePainter = nil,
                        mFillColor = nil
                }

                setmetatable(Obj, self)
                self.__index = self

                Obj.mComponentObject = Jkr.ComponentObject:New(inPosition_3f, inDimension_3f)
                local Dimension = vec2(Obj.mComponentObject.mDimension_3f.x, Obj.mComponentObject.mDimension_3f.y)
                local rect_gen = Generator(Shapes.rectangle, Dimension)
                Obj.mShapeId =  S.Add(rect_gen, Obj.mComponentObject.mPosition_3f)

                Obj.mImagePainter = false or inImagePainter
                if Obj.mImagePainter then
                        Obj.mImageId = S.AddImage(Int(inImageSize_2f.x), Int(inImageSize_2f.y))
                        S.CopyImage(Obj.mImageId, Obj.mImagePainter)
                end
                Obj.mFillColor = nil
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
        end,
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
                local Obj = {
                        mScissorPosition_2f = nil,
                        mScissorDimension_2f = nil,
                        mString = nil,
                        mFont = nil, -- Font Object
                        mId = nil,  
                        mDimension_2f = nil,
                        mColor = Theme.Colors.Text.Normal,
                        mAlt = AlternativeTextRenderer
                }

                setmetatable(Obj, self)
                self.__index = self

                Obj.mString = inText
                Obj.mPosition_3f = inPosition_3f

                if not Obj.mAlt then
                        T.SetCurrentFace(inFontObject.mId)
                        T.SetTextProperty(TextH.left, TextV.top)
                        Obj.mId = T.Add(Obj.mString, vec3(Obj.mPosition_3f.x, Obj.mPosition_3f.y, Depth))
                        Obj.mDimension_2f = inFontObject:GetDimension(Obj.mString)
                else
                        Obj.mId = r.balt.add(inFontObject.mId, Obj.mString, vec3(Obj.mPosition_3f.x, Obj.mPosition_3f.y, Depth))
                end

                Obj.mFont = inFontObject

                return Obj
        end,
        GetLength = function(self)
                return self.mId.y
        end,
        Event = function (self)
        end,
        Draw = function (self)

                if not self.mAlt then
                        T.Bind()
                        T.Draw(self.mColor, Int(WindowDimension.x), Int(WindowDimension.y), Int(self.mId.x), Int(self.mId.y), GetIdentityMatrix())
                else
                        S.Bind()
                        S.BindFillMode(FillType.image)
                        r.balt.draw(self.mColor, Int(WindowDimension.x), Int(WindowDimension.y), self.mId, GetIdentityMatrix())
                end
        end,
        Update = function (self, inPosition_3f)
                self.mPosition_3f = inPosition_3f
                if not self.mAlt then
                        local str = string.rep(" ", self.mId.y)
                        T.Update(Int(self.mId.x), str, self.mPosition_3f)
                        T.Update(Int(self.mId.x), self.mString, self.mPosition_3f)
                else
                        r.balt.update(self.mId, Int(self.mFont.mId), self.mPosition_3f, self.mString)
                end
        end,
        SetScissor = function (self)
                if self.mScissorPosition_2f and self.mScissorDimension_2f then
                        Jkr.set_scissor(self.mScissorPosition_2f, self.mScissorDimension_2f)
                end
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