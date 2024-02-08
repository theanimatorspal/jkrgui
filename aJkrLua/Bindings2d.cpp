#include "EntryPoint.hpp"
static auto pc = R"""(

layout(push_constant, std430) uniform pc {
	vec4 mPosDimen;
	vec4 mColor;
	vec4 mParam;
} push;

)""";

void Create2DBindings(Instance& i, Window& w, sol::state& l, EventManager& em, Jkr::Renderer::_2d& td, Jkr::Renderer::BestText_Alt& ALT)
{
    /* JKRGUI UI Bindings */
    l.set_function("get_window_dimensions", [&]() -> vec2 {
        auto d = w.GetWindowSize();
        return vec2(d.first, d.second);
    });

    l.set_function("get_display_dimensions", [&]() -> vec2 {
        SDL_DisplayMode mode;
        SDL_GetCurrentDisplayMode(0, &mode);
        return glm::vec2(mode.w, mode.h);
    });

    l.new_usertype<_2d>("renderer", "get", [&]() -> _2d& {
        std::cout << "return renderer" << std::endl;
        return td;
    });
    l.new_usertype<EventManager>(
        "event_manager",

        "get",
        [&]() -> EventManager& {
            std::cout << "return event manager" << std::endl;
            return em;
        },

        "GetDepthValue",
        [&]() { return em.GetDepthValue(); },

        "move_towards_camera",
        [&]() { em.MoveDepthValueTowardsTheCamera(); },

        "move_away_camera",
        [&]() { em.MoveDepthValueAwayFromTheCamera(); },

        "should_quit",
        [&]() { return em.ShouldQuit(); },

        "get_mouse_pos",
        [&]() -> vec2 { return em.GetMousePos(); },

        "get_relative_mouse_pos",
        [&]() -> vec2 { return em.GetRelativeMousePos(); },

        "is_mousepress_event",
        [&]() -> bool {
            return em.GetEventHandle().type == SDL_MOUSEBUTTONDOWN;
        },

        "is_left_button_pressed",
        [&]() -> bool {
            return em.GetEventHandle().button.button == SDL_BUTTON_LEFT;
        },

        "is_right_button_pressed",
        [&]() {
            return em.GetEventHandle().button.button == SDL_BUTTON_RIGHT;
        },

        "is_left_button_pressed_continous",
        [&]() {
            return em.IsLeftButtonPressed();
        },

        "is_right_button_pressed_continous",
        [&]() {
            return em.IsRightButtonPressed();
        },

        "is_key_pressed",
        [&](SDL_Keycode key) {
            return em.GetEventHandle().key.keysym.sym == key;
        },

        "is_key_pressed_continous",
        [&](int inScancode) {
            return em.IsKeyPressedContinous(inScancode);
        },

        "is_keypress_event",
        [&]() -> bool { return em.GetEventHandle().type == SDL_KEYDOWN; },

        "is_mousepress_event",
        [&]() -> bool { return em.GetEventHandle().type == SDL_MOUSEBUTTONDOWN; },

        "is_mod_ctrl_pressed",
        [&]() -> bool { return SDL_GetModState() bitand KMOD_CTRL; },

        "get_clipboard_text",
        [&]() -> string { return string(SDL_GetClipboardText()); },

        "start_text_input",
        [&]() -> void { em.StartTextInput(); },

        "stop_text_input",
        [&]() -> void { em.StopTextInput(); },

        "is_text_being_input",
        [&]() -> bool { return em.GetEventHandle().type == SDL_TEXTINPUT; },

        "get_input_text",
        [&]() -> string { return string(em.GetEventHandle().text.text); },

        "is_mouse_within",
        [&](int id, int depth) -> bool { return em.IsMouseWithin(id, depth); },

        "is_mouse_on_top",
        [&](int id, int depth) -> bool { return em.IsMouseWithinAtTopOfStack(id, depth); },

        "set_bounded_rect",
        [&](glm::vec2 xy, glm::vec2 wh, int depth) -> int {
            return em.SetBoundedRect(xy, wh, depth);
        },

        "update_bounded_rect",
        [&](int id, glm::vec2 xy, glm::vec2 wh, int depth) {
            Jkr::BoundRect2D Rec;
            Rec.mXy = xy;
            Rec.mWh = wh;
            em.UpdateBoundRect(depth, id, Rec);
        },

        "get_current_time",
        [&]() -> int {
            return w.GetWindowCurrentTime();
        }

    );

    l.new_usertype<Jkr::BoundRect2D>(
        "bound_rect2d",
        sol::call_constructor,
        [](vec2 xy, vec2 wh) {
            return Jkr::BoundRect2D { .mXy = xy, .mWh = wh };
        },
        "is_mouse_within",
        &Jkr::BoundRect2D::IsPointWithin);

    l.set_function("print_bound_rect2d",
        [](Jkr::BoundRect2D inb) { std::cout << "vec(" << inb.mXy.x << std::endl; });

    l["shapes"] = l.create_table_with("rectangle",
        Jkr::Shapes::Rectangle_Fill,
        "bezier2_8",
        Jkr::Shapes::Bezier2_8,
        "circle",
        Jkr::Shapes::Circle);

    l.new_usertype<Jkr::Generator>("generator",
        sol::call_constructor,
        sol::factories([](Jkr::Shapes shape, glm::vec2 wh) {
            return Jkr::Generator(shape, glm::uvec2(wh));
        }));

    auto r = l["r"].get_or_create<sol::table>();

    r.set_function("set_size_factor", [&](glm::vec2 inVec2) {
        td.SetSizeFactor(inVec2);
    });

    r.new_usertype<Line>(
        "ln",

        "add",
        [&](vec2 p1, vec2 p2, float inDepth) -> int {
            cout << "add Line" << '\n';
            uint32_t id;
            td.ln.AddLine(p1, p2, inDepth, id);
            return id;
        },

        "update",
        [&](int id, vec2 p1, vec2 p2, float inDepth) { td.ln.UpdateLine(id, p1, p2, inDepth); },

        "draw",
        [&](vec4 color, int ww, int wh, int start, int end, glm::mat4 matrix) -> void {
            td.ln.Draw(w, color, ww, wh, start, end, matrix);
        },

        "draw_all",
        [&]() {
            td.ln.DrawAll(w, glm::vec4(1.0f), 1920 / 2, 1080 / 2, glm::identity<glm::mat4>());
            cout << "Draw all\n";
        },

        "bind",
        [&]() { td.ln.Bind(w); }

    );
    l["text_horizontal"] = l.create_table_with("left",
        Jkr::Renderer::BestText::AlignH::Left,
        "right",
        Jkr::Renderer::BestText::AlignH::Right,
        "center",
        Jkr::Renderer::BestText::AlignH::Center);

    l["text_vertical"] = l.create_table_with("top",
        Jkr::Renderer::BestText::AlignV::Top,
        "bottom",
        Jkr::Renderer::BestText::AlignV::Bottom,
        "center",
        Jkr::Renderer::BestText::AlignV::Center);

    r.new_usertype<Renderer::BestText>(
        "bt",

        "add_font_face",
        [&](string_view font_path, int font_size) -> int {
            uint32_t id;
            td.bt.AddFontFace(font_path, font_size, id);
            return id;
        },

        "set_current_face",
        [&](int font_face) -> void {
            td.bt.SetCurrentFontFace(font_face);
        },

        "get_text_dimension",
        [&](std::string_view inString, int id) -> vec2 {
            auto d = td.bt.GetTextDimensions(inString, id);
            return vec2(d.mWidth, d.mHeight);
        },

        "set_text_property",
        [&](Jkr::Renderer::BestText::AlignH inH, Jkr::Renderer::BestText::AlignV inV) {
            Jkr::Renderer::BestText::TextProperty Prop;
            Prop.H = inH;
            Prop.V = inV;
            td.bt.SetTextProperty(Prop);
        },

        "add",
        [&](string_view text, vec3 pos) -> vec2 {
            uvec2 id_and_length;
            td.bt.AddText(text, pos.x, pos.y, pos.z, id_and_length.x, id_and_length.y);
            return id_and_length;
        },

        "update",
        [&](int id, string_view inString, vec3 inpos) -> void {
            {};
            td.bt.UpdateText(id, inString, inpos.x, inpos.y, inpos.z);
        },

        "bind",
        [&]() -> void {
            {};
            td.bt.Bind(w);
        },

        "draw",
        [&](vec4 color, int ww, int wh, int id, int len, glm::mat4 inMatrix) -> void {
            td.bt.Draw(w, color, ww, wh, id, len, inMatrix);
        });

    l["fill_type"] = l.create_table_with("fill",
        FillType::Fill,
        "image",
        FillType::Image,
        "continous_line",
        FillType::ContinousLine);

    struct ImageInfo {
        std::vector<uc> uc_vec;
        int h, w, c;
    };
    l.new_usertype<ImageInfo>(
        "image_info",
        "uc_vec", &ImageInfo::uc_vec,
        "h", &ImageInfo::h,
        "w", &ImageInfo::w,
        "c", &ImageInfo::c
    );

    l.set_function("read_image_into_uc_vector", [&](string_view inFilename) {
        ImageInfo info;
        auto data = stbi_load(inFilename.data(), &info.w, &info.h, &info.c, 4);
        size_t img_size = info.w * info.h * 4;
        info.uc_vec.resize(img_size);
        std::memcpy(info.uc_vec.data(), data, img_size);
        stbi_image_free(data);
        return info;
    });

    r.new_usertype<Shape>(
        "sh",

        "add",
        [&](Jkr::Generator shape, vec3 vec) -> int {
            uint32_t id;
            td.sh.Add(shape, vec.x, vec.y, vec.z, id);
            return id;
        },

        "update",
        [&](int id, Jkr::Generator shape, vec3 vec) -> void {
            td.sh.Update(id, shape, vec.x, vec.y, vec.z);
        },

        "add_image",
        sol::overload(
            [&](int Width, int Height) -> int {
                uint32_t id;
                td.sh.AddImage(Width, Height, id);
                return id;
            },
            [&](string_view infilename) {
                uint32_t id;
                td.sh.AddImage(infilename, id);
                return id;
            },
            [&](ImageInfo& inInfo) {
                uint32_t id;
                td.sh.AddImage(inInfo.uc_vec, inInfo.w, inInfo.h, id);
                return id;
            }),

        "copy_image",
        [&](int id, CustomPainterImage& inPainterImage) { td.sh.CopyToImage(id, inPainterImage); },

        "bind_fill_mode",
        [&](FillType fill) {
            {};
            td.sh.BindFillMode(fill, w);
        },

        "bind",
        [&]() {
            {};
            td.sh.BindShapes(w);
        },

        "draw",
        [&](vec4 color, int ww, int wh, int sid, int eid, glm::mat4 matrix) {
            td.sh.Draw(w, color, ww, wh, sid, eid, matrix);
        },

        "bind_image",
        [&](int id) {
            {};
            td.sh.BindImage(w, id);
        });

    /* Alternative Implementations */

    r.new_usertype<Jkr::Renderer::BestText_Alt::ImageId>("ImageId");

    r.new_usertype<Jkr::Renderer::BestText_Alt>(
        "balt",

        "add",
        [&](int inFontId, string_view inS, vec3 inposition, bool inAlignBottom)
            -> Jkr::Renderer::BestText_Alt::ImageId {
            BestText_Alt::ImageId outId;
            ALT.Add(inFontId, vec2(inposition.x, inposition.y), inposition.z, inS, outId, i.GetThreadPool(), inAlignBottom);
            return outId;
        },

        "draw",
        [&](vec4 color, int ww, int wh, BestText_Alt::ImageId inId, glm::mat4 matrix) {
            ALT.Draw(inId, w, color, ww, wh, matrix);
        },

        "update",
        [&](BestText_Alt::ImageId inId,
            int inFontId,
            vec3 inposition,
            string_view inText, bool inAlignBottom) {
            ALT.Update(inId, inFontId, vec2(inposition.x, inposition.y), inposition.z, inText, i.GetThreadPool(), inAlignBottom);
        },

        "update_pos_only",
        [&](BestText_Alt::ImageId inId, int inFontId, vec2 inposition, int inDepth) {
        }

    );

    r.set_function("set_scissor", [&](vec2 offset, vec2 extent) {
        vk::Rect2D rect;
        rect.setExtent(vk::Extent2D(extent.x, extent.y));
        rect.setOffset(vk::Offset2D(offset.x, offset.y));
        w.SetScissor(rect);
    });

    r.set_function("reset_scissor",
        [&] {
            w.ResetScissor();
        }

    );

    struct push_constant {
        vec4 mPosDimen;
        vec4 mColor;
        vec4 mParam;
    };

    r.new_usertype<CustomPainterImage>("painter_image",
        sol::call_constructor,
        sol::factories([&](int ww, int wh) {
            return CustomPainterImage(i, w, ww, wh);
        }),
        "register", [&](CustomPainterImage& inImage, CustomImagePainter& inPainter) {
            inImage.Register(i, inPainter);
        });

    r.new_usertype<CustomImagePainter>(
        "image_painter",
        sol::call_constructor,
        sol::factories([&](std::string_view fileName,
                           std::string_view inShaderCode,
                           vec3 in_threads) {
            return CustomImagePainter(
                fileName,
                inShaderCode,
                pc,
                in_threads.x,
                in_threads.y,
                in_threads.z);
        }),

        "load",
        [&](CustomImagePainter& inP) {
            inP.Load(i, w);
        },

        "store",
        [&](CustomImagePainter& inP) { inP.Store(i, w); },

        "paint",
        [&](CustomImagePainter& inP, vec4 inPosDimen, vec4 inColor, vec4 inParam) {
            push_constant push { .mPosDimen = inPosDimen, .mColor = inColor, .mParam = inParam };
            inP.Draw<push_constant>(w, push);
        },

        "paintext",
        [&](CustomImagePainter& inP, vec4 inPosDimen, vec4 inColor, vec4 inParam, int inX, int inY, int inZ) {
            push_constant push { .mPosDimen = inPosDimen, .mColor = inColor, .mParam = inParam };
            inP.Draw<push_constant>(w, push, inX, inY, inZ);
        },

        "bind_image",
        [&](CustomImagePainter& inP) { inP.BindImage(w); },

        "bind_image_from_image",
        [&](CustomImagePainter& inP, CustomPainterImage& inImage) { inP.BindImageFromImage(w, inImage); },

        "bind",
        [&](CustomImagePainter& inP) { inP.Bind(w); },

        "register_image",
        [&](CustomImagePainter& inP, CustomPainterImage& inImg) {
            inP.RegisterImage(i, w, inImg);
        }

    );

    /* Components */
    auto c = l["c"].get_or_create<sol::table>();
    c.new_usertype<Component_base>(

        "com_base",
        sol::call_constructor,
        sol::factories([&]() {
            std::cout << "Component_base Called" << std::endl;
            return Component_base();
        }),

        "set_default_bound_rect2d",
        &Component_base::SetDefaultBoundedRectangle,

        "update_default_bound_rect2d",
        &Component_base::UpdateDefaultBoundedRectangle,

        "dimension",
        sol::property(&Component_base::GetDimension, &Component_base::SetDimension),

        "position",
        sol::property(&Component_base::GetPosition, &Component_base::SetPosition),

        "depth_value",
        sol::property(&Component_base::GetDepthValue, &Component_base::SetDepthValue),

        "is_mouse_on_top",
        &Component_base::IsMouseOnTop,

        "is_mouse_within",
        &Component_base::IsMouseWithin);
}
