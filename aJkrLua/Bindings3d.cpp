#include "EntryPoint.hpp"

void Create3DBindings(Instance& i, Window& w, sol::state& l)
{
    auto r = l["r3d"].get_or_create<sol::table>();

    struct UB_Default {
        glm::mat4 m1;
        glm::mat4 m2;
        glm::vec4 v1;
        glm::vec4 v2[10];
    };
    size_t SizeOfUB_Default = sizeof(UB_Default);
    r["SizeOfUB_Default"] = SizeOfUB_Default;
    r["SizeOfUB_Default"] = SizeOfUB_Default;

    struct SSBO {
    };
    size_t SizeOfSSBO_Default = sizeof(SSBO);
    r["SizeOfSSBO_Default"] = SizeOfSSBO_Default;

    struct PCm4m4 {
        glm::mat4 m1;
        glm::mat4 m2;
    };
    struct PCm4v4 {
        glm::mat4 m1;
        glm::vec4 v1;
    };
    struct PCm4v4v4 {
        glm::mat4 m1;
        glm::vec4 v1;
        glm::vec4 v2;
    };

    r["bindpoint"] = l.create_table_with("background",
        Jkr::Window::ParameterContext::Background,
        "ui",
        Jkr::Window::ParameterContext::UI,
        "none",
        Jkr::Window::ParameterContext::None);

    r.new_usertype<Jkr::Renderer::_3D::Shape>(
        "three",
        sol::call_constructor,
        sol::factories(
            [&](int size_ubo, int size_ssbo) { return std::make_unique<_3D::Shape>(i, w, size_ubo, size_ssbo); }),

        "add_painter",
        [&](_3D::Shape& inR,
            string_view filename,
            string_view invertex,
            string_view infragment,
            string_view incompute,
            bool forcestore) -> ui {
            ui id;
            inR.AddPainter(filename,
                invertex,
                infragment,
                incompute,
                w,
                id,
                forcestore);
            return id;
        },
        "add_model",
        [&](_3D::Shape& inR, string_view filename) -> int {
            ui id;
            inR.Add(filename, id);
            return id;
        },

        "add_model_texture_to_painter",
        [&](_3D::Shape& inR,
            int modelid,
            int inpainterid,
            int inoffset,
            int inbinding,
            int indesarrayelem) {
            inR.AddModelTextureToPainter(modelid, inpainterid, inoffset, inbinding, indesarrayelem);
        },

        "write_to_global_ub_default",
        [&](_3D::Shape& inR, glm::mat4 m1, glm::mat4 m2, glm::vec4 v1, std::vector<glm::vec4> v2) {
            UB_Default d;
            d.m1 = m1;
            d.m2 = m2;
            d.v1 = v1;
            for (int i = 0; i < 10; i++) {
                d.v2[i] = v2[i];
            }
            inR.WriteToGlobalUB(d);
        },

        "register_global_ub_to_painter",
        [&](_3D::Shape& inR, int inpainterid, int offset, int binding, int desarrelem) {
            inR.RegisterGlobalUBToPainter(inpainterid, offset, binding, desarrelem);
        },

        "bind",
        [&](_3D::Shape& inR, int bindpoint) {
            inR.Bind(w, (Jkr::Window::ParameterContext)bindpoint);
        },

        "painter_bind_for_draw",
        [&](_3D::Shape& inR, int painterid, int bindpoint) {
            inR.PainterBindForDraw(painterid, w, (Jkr::Window::ParameterContext)bindpoint);
        },

        "painter_draw",
        [&](_3D::Shape& inR,
            int painterid,
            int inmodelid,
            int bindpoint,
            glm::mat4 m1,
            glm::vec4 v1,
            glm::vec4 v2) {
            PCm4v4v4 m = { .m1 = m1, .v1 = v1, .v2 = v2 };
            inR.PainterDraw(painterid, m, w, (Jkr::Window::ParameterContext)bindpoint);
        });
}