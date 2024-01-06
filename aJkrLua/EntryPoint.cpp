#include "BestText.hpp"
#include "BestText_base.hpp"
#include "SDL2/SDL_clipboard.h"
#include "SDL2/SDL_events.h"
#include "Shape_base.hpp"
#include <EventManager.hpp>
#include <Instance.hpp>
#include <Renderers/2d.hpp>
#include <Renderers/BestText_Alt.hpp>
#include <Renderers/ResourceManager.hpp>
#include <Vendor/Tracy/tracy/Tracy.hpp>
#include <Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define SETTER inline void

namespace Jkr::Component {
using namespace Jkr::Renderer;
class Component_base {
public:
    Component_base(const Component_base&) = default;
    Component_base(Component_base&&) = default;
    Component_base& operator=(const Component_base&) = default;
    Component_base& operator=(Component_base&&) = default;

public:
    GETTER& GetFastTexts() { return mFastTexts; }
    GETTER& GetLines() { return mLines; }
    GETTER& GetShapes() { return mShapes; }

private:
    std::vector<glm::uvec2> mFastTexts;
    std::vector<glm::uvec2> mLines;
    std::vector<glm::uvec2> mShapes;

public:
    Component_base(_2d& inR, EventManager& inE)
        : r(inR)
        , e(inE)
    {
        mPosition = { INT_MAX, INT_MAX };
        mDimension = { INT_MAX, INT_MAX };
        mDepthValue = INT_MAX;
        mWindow = nullptr;
        mWh = INT_MAX;
        mWw = INT_MAX;
    }
    void CheckExpectations()
    {
        assert(mPosition.x != static_cast<float>(INT_MAX));
        assert(mPosition.y != static_cast<float>(INT_MAX));
        assert(mDimension.x != static_cast<float>(INT_MAX));
        assert(mDimension.y != static_cast<float>(INT_MAX));
        assert(mDepthValue != static_cast<float>(INT_MAX));
        assert(mWh != INT_MAX);
        assert(mWw != INT_MAX);
        assert(mWindow != nullptr);
    }
    void SetDefaultBoundedRectangle()
    {
        mBoundedRectangleId = e.SetBoundedRect(mPosition, mDimension, mDepthValue);
    }
    void UpdateDefaultBoundedRectangle()
    {
        Jkr::BoundRect2D Rect { .mXy = mPosition, .mWh = mDimension };
        e.UpdateBoundRect(mDepthValue, mBoundedRectangleId, Rect);
    }
    GETTER GetDimension() const { return mDimension; }
    GETTER GetPosition() const { return mPosition; }
    SETTER SetDimension(glm::vec2 inVec) { mDimension = inVec; }
    SETTER SetPosition(glm::vec2 inVec)
    {
        mPosition = inVec;
        mTransformMatrix = glm::identity<glm::mat4>();
        mTransformMatrix = glm::translate(mTransformMatrix, glm::vec3(mPosition.x, mPosition.y, 0));
    }
    SETTER SetDepthValue(uint32_t inDepthValue) { mDepthValue = inDepthValue; }
    GETTER GetDepthValue() { return mDepthValue; }
    GETTER IsFocused() const { return isFocused; }
    GETTER IsMouseOnTop() const { return e.IsMouseWithinAtTopOfStack(mBoundedRectangleId, mDepthValue); }
    GETTER IsMouseWithin() const { return e.IsMouseWithin(mBoundedRectangleId, mDepthValue); }
    SETTER ToggleFocus() { isFocused = !isFocused; }
    SETTER SetFocus(bool inFocus) { isFocused = inFocus; }
    GETTER GetTranslationMatrix() const
    {
        return mTransformMatrix;
    }
    glm::vec2 FromComponent(glm::vec2 inComp)
    {
        return glm::vec2(inComp) + mPosition;
    }
    glm::vec2 FromWorld(glm::vec2 inWorld)
    {
        return glm::vec2(inWorld) - mPosition;
    }
    glm::vec2 ToCentralizedCoords(glm::uvec2 mBoxDimension, glm::uvec2 mBox)
    {
    }

protected:
    _2d& r;
    EventManager& e;
    Window* mWindow = nullptr;
    uint32_t mWh = 0, mWw = 0;

public:
    SETTER SetWindow(Window* inWindow, uint32_t inW, uint32_t inH)
    {
        mWindow = inWindow;
        mWw = inW;
        mWh = inH;
    }
    SETTER SetWindowDimensions(float inW, float inH)
    {
        mWw = inW;
        mWh = inH;
    }
    GETTER GetWindowWidth() const
    {
        return mWw;
    }
    GETTER GetWindowHeight() const
    {
        return mWh;
    }
    GETTER& GetWindow()
    {
        return mWindow;
    }

private:
    glm::vec2 mDimension;
    glm::vec2 mPosition;
    glm::mat4 mTransformMatrix = glm::identity<glm::mat4>();
    uint32_t mDepthValue;
    uint32_t mBoundedRectangleId;
    bool isFocused = false;
};
}

#ifdef WIN32
#include <Windows.h>
#endif

#define SOL_PRINT_ERRORS 1
#define SOL_ALL_SAFETIES_ON 1

#include <Vendor/sol/sol.hpp>
#include <Window.hpp>

int my_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_execption, sol::string_view description)
{
    std::cout << "An Exception has occured in the function here is what it says: ";
    if (maybe_execption) {
        std::cout << "(Straight from the exception) ";
        const std::exception& ex = *maybe_execption;
        std::cout << ex.what() << std::endl;
    } else {
        std::cout << "(From the Descriptor Parameter) ";
        std::cout.write(description.data(), static_cast<std::streamsize>(description.size()));
        std::cout << std::endl;
    }
    return sol::stack::push(L, description);
}

void CreateSDLEventBindings(sol::state& lua)
{
    lua["key"] = lua.create_table_with("SDLK_UNKNOWN", SDLK_UNKNOWN);
    lua["key"]["SDLK_RETURN"] = SDLK_RETURN;
    lua["key"]["SDLK_ESCAPE"] = SDLK_ESCAPE;
    lua["key"]["SDLK_BACKSPACE"] = SDLK_BACKSPACE;
    lua["key"]["SDLK_TAB"] = SDLK_TAB;
    lua["key"]["SDLK_SPACE"] = SDLK_SPACE;
    lua["key"]["SDLK_EXCLAIM"] = SDLK_EXCLAIM;
    lua["key"]["SDLK_QUOTEDBL"] = SDLK_QUOTEDBL;
    lua["key"]["SDLK_HASH"] = SDLK_HASH;
    lua["key"]["SDLK_PERCENT"] = SDLK_PERCENT;
    lua["key"]["SDLK_DOLLAR"] = SDLK_DOLLAR;
    lua["key"]["SDLK_AMPERSAND"] = SDLK_AMPERSAND;
    lua["key"]["SDLK_QUOTE"] = SDLK_QUOTE;
    lua["key"]["SDLK_LEFTPAREN"] = SDLK_LEFTPAREN;
    lua["key"]["SDLK_RIGHTPAREN"] = SDLK_RIGHTPAREN;
    lua["key"]["SDLK_ASTERISK"] = SDLK_ASTERISK;
    lua["key"]["SDLK_PLUS"] = SDLK_PLUS;
    lua["key"]["SDLK_COMMA"] = SDLK_COMMA;
    lua["key"]["SDLK_MINUS"] = SDLK_MINUS;
    lua["key"]["SDLK_PERIOD"] = SDLK_PERIOD;
    lua["key"]["SDLK_SLASH"] = SDLK_SLASH;
    lua["key"]["SDLK_0"] = SDLK_0;
    lua["key"]["SDLK_1"] = SDLK_1;
    lua["key"]["SDLK_2"] = SDLK_2;
    lua["key"]["SDLK_3"] = SDLK_3;
    lua["key"]["SDLK_4"] = SDLK_4;
    lua["key"]["SDLK_5"] = SDLK_5;
    lua["key"]["SDLK_6"] = SDLK_6;
    lua["key"]["SDLK_7"] = SDLK_7;
    lua["key"]["SDLK_8"] = SDLK_8;
    lua["key"]["SDLK_9"] = SDLK_9;
    lua["key"]["SDLK_COLON"] = SDLK_COLON;
    lua["key"]["SDLK_SEMICOLON"] = SDLK_SEMICOLON;
    lua["key"]["SDLK_LESS"] = SDLK_LESS;
    lua["key"]["SDLK_EQUALS"] = SDLK_EQUALS;
    lua["key"]["SDLK_GREATER"] = SDLK_GREATER;
    lua["key"]["SDLK_QUESTION"] = SDLK_QUESTION;
    lua["key"]["SDLK_AT"] = SDLK_AT;

    lua["key"]["SDLK_LEFTBRACKET"] = SDLK_LEFTBRACKET,
    lua["key"]["SDLK_BACKSLASH"] = SDLK_BACKSLASH;
    lua["key"]["SDLK_RIGHTBRACKET"] = SDLK_RIGHTBRACKET;
    lua["key"]["SDLK_CARET"] = SDLK_CARET;
    lua["key"]["SDLK_UNDERSCORE"] = SDLK_UNDERSCORE;
    lua["key"]["SDLK_BACKQUOTE"] = SDLK_BACKQUOTE;
    lua["key"]["SDLK_a"] = SDLK_a;
    lua["key"]["SDLK_b"] = SDLK_b;
    lua["key"]["SDLK_c"] = SDLK_c;
    lua["key"]["SDLK_d"] = SDLK_d;
    lua["key"]["SDLK_e"] = SDLK_e;
    lua["key"]["SDLK_f"] = SDLK_f;
    lua["key"]["SDLK_g"] = SDLK_g;
    lua["key"]["SDLK_h"] = SDLK_h;
    lua["key"]["SDLK_i"] = SDLK_i;
    lua["key"]["SDLK_j"] = SDLK_j;
    lua["key"]["SDLK_k"] = SDLK_k;
    lua["key"]["SDLK_l"] = SDLK_l;
    lua["key"]["SDLK_m"] = SDLK_m;
    lua["key"]["SDLK_n"] = SDLK_n;
    lua["key"]["SDLK_o"] = SDLK_o;
    lua["key"]["SDLK_p"] = SDLK_p;
    lua["key"]["SDLK_q"] = SDLK_q;
    lua["key"]["SDLK_r"] = SDLK_r;
    lua["key"]["SDLK_s"] = SDLK_s;
    lua["key"]["SDLK_t"] = SDLK_t;
    lua["key"]["SDLK_u"] = SDLK_u;
    lua["key"]["SDLK_v"] = SDLK_v;
    lua["key"]["SDLK_w"] = SDLK_w;
    lua["key"]["SDLK_x"] = SDLK_x;
    lua["key"]["SDLK_y"] = SDLK_y;
    lua["key"]["SDLK_z"] = SDLK_z;

    lua["key"]["SDLK_CAPSLOCK"] = SDLK_CAPSLOCK;

    lua["key"]["SDLK_F1"] = SDLK_F1;
    lua["key"]["SDLK_F2"] = SDLK_F2;
    lua["key"]["SDLK_F3"] = SDLK_F3;
    lua["key"]["SDLK_F4"] = SDLK_F4;
    lua["key"]["SDLK_F5"] = SDLK_F5;
    lua["key"]["SDLK_F6"] = SDLK_F6;
    lua["key"]["SDLK_F7"] = SDLK_F7;
    lua["key"]["SDLK_F8"] = SDLK_F8;
    lua["key"]["SDLK_F9"] = SDLK_F9;
    lua["key"]["SDLK_F10"] = SDLK_F10;
    lua["key"]["SDLK_F11"] = SDLK_F11;
    lua["key"]["SDLK_F12"] = SDLK_F12;

    lua["key"]["SDLK_PRINTSCREEN"] = SDLK_PRINTSCREEN;
    lua["key"]["SDLK_SCROLLLOCK"] = SDLK_SCROLLLOCK;
    lua["key"]["SDLK_PAUSE"] = SDLK_PAUSE;
    lua["key"]["SDLK_INSERT"] = SDLK_INSERT;
    lua["key"]["SDLK_HOME"] = SDLK_HOME;
    lua["key"]["SDLK_PAGEUP"] = SDLK_PAGEUP;
    lua["key"]["SDLK_DELETE"] = SDLK_DELETE;
    lua["key"]["SDLK_END"] = SDLK_END;
    lua["key"]["SDLK_PAGEDOWN"] = SDLK_PAGEDOWN;
    lua["key"]["SDLK_RIGHT"] = SDLK_RIGHT;
    lua["key"]["SDLK_LEFT"] = SDLK_LEFT;
    lua["key"]["SDLK_DOWN"] = SDLK_DOWN;
    lua["key"]["SDLK_UP"] = SDLK_UP;

    lua["key"]["SDLK_NUMLOCKCLEAR"] = SDLK_NUMLOCKCLEAR;
    lua["key"]["SDLK_KP_DIVIDE"] = SDLK_KP_DIVIDE;
    lua["key"]["SDLK_KP_MULTIPLY"] = SDLK_KP_MULTIPLY;
    lua["key"]["SDLK_KP_MINUS"] = SDLK_KP_MINUS;
    lua["key"]["SDLK_KP_PLUS"] = SDLK_KP_PLUS;
    lua["key"]["SDLK_KP_ENTER"] = SDLK_KP_ENTER;
    lua["key"]["SDLK_KP_1"] = SDLK_KP_1;
    lua["key"]["SDLK_KP_2"] = SDLK_KP_2;
    lua["key"]["SDLK_KP_3"] = SDLK_KP_3;
    lua["key"]["SDLK_KP_4"] = SDLK_KP_4;
    lua["key"]["SDLK_KP_5"] = SDLK_KP_5;
    lua["key"]["SDLK_KP_6"] = SDLK_KP_6;
    lua["key"]["SDLK_KP_7"] = SDLK_KP_7;
    lua["key"]["SDLK_KP_8"] = SDLK_KP_8;
    lua["key"]["SDLK_KP_9"] = SDLK_KP_9;
    lua["key"]["SDLK_KP_0"] = SDLK_KP_0;
    lua["key"]["SDLK_KP_PERIOD"] = SDLK_KP_PERIOD;

    lua["key"]["SDLK_APPLICATION"] = SDLK_APPLICATION;
    lua["key"]["SDLK_POWER"] = SDLK_POWER;
    lua["key"]["SDLK_KP_EQUALS"] = SDLK_KP_EQUALS;
    lua["key"]["SDLK_F13"] = SDLK_F13;
    lua["key"]["SDLK_F14"] = SDLK_F14;
    lua["key"]["SDLK_F15"] = SDLK_F15;
    lua["key"]["SDLK_F16"] = SDLK_F16;
    lua["key"]["SDLK_F17"] = SDLK_F17;
    lua["key"]["SDLK_F18"] = SDLK_F18;
    lua["key"]["SDLK_F19"] = SDLK_F19;
    lua["key"]["SDLK_F20"] = SDLK_F20;
    lua["key"]["SDLK_F21"] = SDLK_F21;
    lua["key"]["SDLK_F22"] = SDLK_F22;
    lua["key"]["SDLK_F23"] = SDLK_F23;
    lua["key"]["SDLK_F24"] = SDLK_F24;
    lua["key"]["SDLK_EXECUTE"] = SDLK_EXECUTE;
    lua["key"]["SDLK_HELP"] = SDLK_HELP;
    lua["key"]["SDLK_MENU"] = SDLK_MENU;
    lua["key"]["SDLK_SELECT"] = SDLK_SELECT;
    lua["key"]["SDLK_STOP"] = SDLK_STOP;
    lua["key"]["SDLK_AGAIN"] = SDLK_AGAIN;
    lua["key"]["SDLK_UNDO"] = SDLK_UNDO;
    lua["key"]["SDLK_CUT"] = SDLK_CUT;
    lua["key"]["SDLK_COPY"] = SDLK_COPY;
    lua["key"]["SDLK_PASTE"] = SDLK_PASTE;
    lua["key"]["SDLK_FIND"] = SDLK_FIND;
    lua["key"]["SDLK_MUTE"] = SDLK_MUTE;
    lua["key"]["SDLK_VOLUMEUP"] = SDLK_VOLUMEUP;
    lua["key"]["SDLK_VOLUMEDOWN"] = SDLK_VOLUMEDOWN;
    lua["key"]["SDLK_KP_COMMA"] = SDLK_KP_COMMA;
    lua["key"]["SDLK_KP_EQUALSAS400"] = SDLK_KP_EQUALSAS400;

    lua["key"]["SDLK_ALTERASE"] = SDLK_ALTERASE;
    lua["key"]["SDLK_SYSREQ"] = SDLK_SYSREQ;
    lua["key"]["SDLK_CANCEL"] = SDLK_CANCEL;
    lua["key"]["SDLK_CLEAR"] = SDLK_CLEAR;
    lua["key"]["SDLK_PRIOR"] = SDLK_PRIOR;
    lua["key"]["SDLK_RETURN2"] = SDLK_RETURN2;
    lua["key"]["SDLK_SEPARATOR"] = SDLK_SEPARATOR;
    lua["key"]["SDLK_OUT"] = SDLK_OUT;
    lua["key"]["SDLK_OPER"] = SDLK_OPER;
    lua["key"]["SDLK_CLEARAGAIN"] = SDLK_CLEARAGAIN;
    lua["key"]["SDLK_CRSEL"] = SDLK_CRSEL;
    lua["key"]["SDLK_EXSEL"] = SDLK_EXSEL;

    lua["key"]["SDLK_KP_00"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_00);
    lua["key"]["SDLK_KP_000"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_000);
    lua["key"]["SDLK_THOUSANDSSEPARATOR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_THOUSANDSSEPARATOR);
    lua["key"]["SDLK_DECIMALSEPARATOR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DECIMALSEPARATOR);
    lua["key"]["SDLK_CURRENCYUNIT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYUNIT);
    lua["key"]["SDLK_CURRENCYSUBUNIT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYSUBUNIT);
    lua["key"]["SDLK_KP_LEFTPAREN"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTPAREN);
    lua["key"]["SDLK_KP_RIGHTPAREN"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTPAREN);
    lua["key"]["SDLK_KP_LEFTBRACE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTBRACE);
    lua["key"]["SDLK_KP_RIGHTBRACE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTBRACE);
    lua["key"]["SDLK_KP_TAB"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_TAB);
    lua["key"]["SDLK_KP_BACKSPACE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BACKSPACE);
    lua["key"]["SDLK_KP_A"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_A);
    lua["key"]["SDLK_KP_B"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_B);
    lua["key"]["SDLK_KP_C"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_C);
    lua["key"]["SDLK_KP_D"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_D);
    lua["key"]["SDLK_KP_E"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_E);
    lua["key"]["SDLK_KP_F"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_F);
    lua["key"]["SDLK_KP_XOR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_XOR);
    lua["key"]["SDLK_KP_POWER"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_POWER);
    lua["key"]["SDLK_KP_PERCENT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERCENT);
    lua["key"]["SDLK_KP_LESS"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LESS);
    lua["key"]["SDLK_KP_GREATER"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_GREATER);
    lua["key"]["SDLK_KP_AMPERSAND"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AMPERSAND);
    lua["key"]["SDLK_KP_DBLAMPERSAND"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLAMPERSAND);
    lua["key"]["SDLK_KP_VERTICALBAR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_VERTICALBAR);
    lua["key"]["SDLK_KP_DBLVERTICALBAR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLVERTICALBAR);
    lua["key"]["SDLK_KP_COLON"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_COLON);
    lua["key"]["SDLK_KP_HASH"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HASH);
    lua["key"]["SDLK_KP_SPACE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_SPACE);
    lua["key"]["SDLK_KP_AT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AT);
    lua["key"]["SDLK_KP_EXCLAM"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EXCLAM);
    lua["key"]["SDLK_KP_MEMSTORE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSTORE);
    lua["key"]["SDLK_KP_MEMRECALL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMRECALL);
    lua["key"]["SDLK_KP_MEMCLEAR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMCLEAR);
    lua["key"]["SDLK_KP_MEMADD"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMADD);
    lua["key"]["SDLK_KP_MEMSUBTRACT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSUBTRACT);
    lua["key"]["SDLK_KP_MEMMULTIPLY"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMMULTIPLY);
    lua["key"]["SDLK_KP_MEMDIVIDE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMDIVIDE);
    lua["key"]["SDLK_KP_PLUSMINUS"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUSMINUS);
    lua["key"]["SDLK_KP_CLEAR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEAR);
    lua["key"]["SDLK_KP_CLEARENTRY"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEARENTRY);
    lua["key"]["SDLK_KP_BINARY"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BINARY);
    lua["key"]["SDLK_KP_OCTAL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_OCTAL);
    lua["key"]["SDLK_KP_DECIMAL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DECIMAL);
    lua["key"]["SDLK_KP_HEXADECIMAL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HEXADECIMAL);

    lua["key"]["SDLK_LCTRL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LCTRL);
    lua["key"]["SDLK_LSHIFT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LSHIFT);
    lua["key"]["SDLK_LALT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LALT);
    lua["key"]["SDLK_LGUI"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LGUI);
    lua["key"]["SDLK_RCTRL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RCTRL);
    lua["key"]["SDLK_RSHIFT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RSHIFT);
    lua["key"]["SDLK_RALT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RALT);
    lua["key"]["SDLK_RGUI"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RGUI);

    lua["key"]["SDLK_MODE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MODE);

    lua["key"]["SDLK_AUDIONEXT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIONEXT);
    lua["key"]["SDLK_AUDIOPREV"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOPREV);
    lua["key"]["SDLK_AUDIOSTOP"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOSTOP);
    lua["key"]["SDLK_AUDIOPLAY"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOPLAY);
    lua["key"]["SDLK_AUDIOMUTE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOMUTE);
    lua["key"]["SDLK_MEDIASELECT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIASELECT);
    lua["key"]["SDLK_WWW"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_WWW);
    lua["key"]["SDLK_MAIL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MAIL);
    lua["key"]["SDLK_CALCULATOR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CALCULATOR);
    lua["key"]["SDLK_COMPUTER"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_COMPUTER);
    lua["key"]["SDLK_AC_SEARCH"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_SEARCH);
    lua["key"]["SDLK_AC_HOME"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_HOME);
    lua["key"]["SDLK_AC_BACK"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BACK);
    lua["key"]["SDLK_AC_FORWARD"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_FORWARD);
    lua["key"]["SDLK_AC_STOP"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_STOP);
    lua["key"]["SDLK_AC_REFRESH"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_REFRESH);
    lua["key"]["SDLK_AC_BOOKMARKS"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BOOKMARKS);

    lua["key"]["SDLK_BRIGHTNESSDOWN"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_BRIGHTNESSDOWN);
    lua["key"]["SDLK_BRIGHTNESSUP"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_BRIGHTNESSUP);
    lua["key"]["SDLK_DISPLAYSWITCH"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DISPLAYSWITCH);
    lua["key"]["SDLK_KBDILLUMTOGGLE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KBDILLUMTOGGLE);
    lua["key"]["SDLK_KBDILLUMDOWN"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KBDILLUMDOWN);
    lua["key"]["SDLK_KBDILLUMUP"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KBDILLUMUP);
    lua["key"]["SDLK_EJECT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EJECT);
    lua["key"]["SDLK_SLEEP"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SLEEP);
    lua["key"]["SDLK_APP1"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP1);
    lua["key"]["SDLK_APP2"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP2);

    lua["key"]["SDLK_AUDIOREWIND"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOREWIND);
    lua["key"]["SDLK_AUDIOFASTFORWARD"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOFASTFORWARD);

    lua["key"]["SDLK_SOFTLEFT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SOFTLEFT);
    lua["key"]["SDLK_SOFTRIGHT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SOFTRIGHT);
    lua["key"]["SDLK_CALL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CALL);
    lua["key"]["SDLK_ENDCALL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_ENDCALL);
}

void CreateGLMBindings(sol::state& lua)
{
    auto vec2_multiply_overloads = sol::overload(
        [](const glm::vec2& v1, const glm::vec2& v2) { return v1 * v2; },
        [](const glm::vec2& v1, float value) { return v1 * value; },
        [](float value, const glm::vec2& v1) { return v1 * value; });

    auto vec2_divide_overloads = sol::overload(
        [](const glm::vec2& v1, const glm::vec2& v2) { return v1 / v2; },
        [](const glm::vec2& v1, float value) { return v1 / value; },
        [](float value, const glm::vec2& v1) { return v1 / value; });

    auto vec2_addition_overloads = sol::overload(
        [](const glm::vec2& v1, const glm::vec2& v2) { return v1 + v2; },
        [](const glm::vec2& v1, float value) { return v1 + value; },
        [](float value, const glm::vec2& v1) { return v1 + value; });

    auto vec2_subtraction_overloads = sol::overload(
        [](const glm::vec2& v1, const glm::vec2& v2) { return v1 - v2; },
        [](const glm::vec2& v1, float value) { return v1 - value; },
        [](float value, const glm::vec2& v1) { return v1 - value; });

    lua.new_usertype<glm::vec2>(
        "vec2",
        sol::call_constructor,
        sol::constructors<glm::vec2(float), glm::vec2(float, float)>(),
        "x", &glm::vec2::x,
        "y", &glm::vec2::y,
        sol::meta_function::multiplication, vec2_multiply_overloads,
        sol::meta_function::division, vec2_divide_overloads,
        sol::meta_function::addition, vec2_addition_overloads,
        sol::meta_function::subtraction, vec2_subtraction_overloads,
        "length", [](const glm::vec2& v) { return glm::length(v); });

    auto vec3_multiply_overloads = sol::overload(
        [](const glm::vec3& v1, const glm::vec3& v2) { return v1 * v2; },
        [](const glm::vec3& v1, float value) { return v1 * value; },
        [](float value, const glm::vec3& v1) { return v1 * value; });

    auto vec3_divide_overloads = sol::overload(
        [](const glm::vec3& v1, const glm::vec3& v2) { return v1 / v2; },
        [](const glm::vec3& v1, float value) { return v1 / value; },
        [](float value, const glm::vec3& v1) { return v1 / value; });

    auto vec3_addition_overloads = sol::overload(
        [](const glm::vec3& v1, const glm::vec3& v2) { return v1 + v2; },
        [](const glm::vec3& v1, float value) { return v1 + value; },
        [](float value, const glm::vec3& v1) { return v1 + value; });

    auto vec3_subtraction_overloads = sol::overload(
        [](const glm::vec3& v1, const glm::vec3& v2) { return v1 - v2; },
        [](const glm::vec3& v1, float value) { return v1 - value; },
        [](float value, const glm::vec3& v1) { return v1 - value; });

    lua.new_usertype<glm::vec3>(
        "vec3",
        sol::call_constructor,
        sol::constructors<glm::vec3(float), glm::vec3(float, float, float)>(),
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z,
        sol::meta_function::multiplication, vec3_multiply_overloads,
        sol::meta_function::division, vec3_divide_overloads,
        sol::meta_function::addition, vec3_addition_overloads,
        sol::meta_function::subtraction, vec3_subtraction_overloads);

    auto vec4_multiply_overloads = sol::overload(
        [](const glm::vec4& v1, const glm::vec4& v2) { return v1 * v2; },
        [](const glm::vec4& v1, float value) { return v1 * value; },
        [](float value, const glm::vec4& v1) { return v1 * value; });

    auto vec4_divide_overloads = sol::overload(
        [](const glm::vec4& v1, const glm::vec4& v2) { return v1 / v2; },
        [](const glm::vec4& v1, float value) { return v1 / value; },
        [](float value, const glm::vec4& v1) { return v1 / value; });

    auto vec4_addition_overloads = sol::overload(
        [](const glm::vec4& v1, const glm::vec4& v2) { return v1 + v2; },
        [](const glm::vec4& v1, float value) { return v1 + value; },
        [](float value, const glm::vec4& v1) { return v1 + value; });

    auto vec4_subtraction_overloads = sol::overload(
        [](const glm::vec4& v1, const glm::vec4& v2) { return v1 - v2; },
        [](const glm::vec4& v1, float value) { return v1 - value; },
        [](float value, const glm::vec4& v1) { return v1 - value; });

    lua.new_usertype<glm::vec4>(
        "vec4",
        sol::call_constructor,
        sol::constructors<glm::vec4(float), glm::vec4(float, float, float, float)>(),
        "x", &glm::vec4::x,
        "y", &glm::vec4::y,
        "z", &glm::vec4::z,
        "w", &glm::vec4::w,
        sol::meta_function::multiplication, vec4_multiply_overloads,
        sol::meta_function::division, vec4_divide_overloads,
        sol::meta_function::addition, vec4_addition_overloads,
        sol::meta_function::subtraction, vec4_subtraction_overloads

    );

    auto uvec2_multiply_overloads
        = sol::overload([](const glm::uvec2& v1, const glm::uvec2& v2) { return v1 * v2; },
            [](const glm::uvec2& v1, uint32_t value) { return v1 * value; },
            [](uint32_t value, const glm::uvec2& v1) { return v1 * value; });

    auto uvec2_divide_overloads
        = sol::overload([](const glm::uvec2& v1, const glm::uvec2& v2) { return v1 / v2; },
            [](const glm::uvec2& v1, uint32_t value) { return v1 / value; },
            [](uint32_t value, const glm::uvec2& v1) { return v1 / value; });

    auto uvec2_addition_overloads
        = sol::overload([](const glm::uvec2& v1, const glm::uvec2& v2) { return v1 + v2; },
            [](const glm::uvec2& v1, uint32_t value) { return v1 + value; },
            [](uint32_t value, const glm::uvec2& v1) { return v1 + value; });

    auto uvec2_subtraction_overloads
        = sol::overload([](const glm::uvec2& v1, const glm::uvec2& v2) { return v1 - v2; },
            [](const glm::uvec2& v1, uint32_t value) { return v1 - value; },
            [](uint32_t value, const glm::uvec2& v1) { return v1 - value; });

    lua.new_usertype<glm::uvec2>("uvec2",
        sol::call_constructor,
        sol::constructors<glm::uvec2(uint32_t),
            glm::uvec2(uint32_t, uint32_t),
            glm::uvec2(float, float)>(),
        "x",
        &glm::uvec2::x,
        "y",
        &glm::uvec2::y,
        sol::meta_function::multiplication,
        uvec2_multiply_overloads,
        sol::meta_function::division,
        uvec2_divide_overloads,
        sol::meta_function::addition,
        uvec2_addition_overloads,
        sol::meta_function::subtraction,
        uvec2_subtraction_overloads

    );

    lua.new_usertype<glm::mat4>(
        "mat4",
        sol::call_constructor,
        sol::constructors<glm::mat4(float)>());
}

void BindMathFunctions(sol::state& lua)
{
    lua.set_function("distance", sol::overload([](glm::vec2& a, glm::vec2& b) { return glm::distance(a, b); }, [](glm::vec3& a, glm::vec3& b) { return glm::distance(a, b); }, [](glm::vec4& a, glm::vec4& b) { return glm::distance(a, b); }));
    lua.set_function("lerp", [](float a, float b, float t) { return std::lerp(a, b, t); });
    lua.set_function("clamp", sol::overload([](float value, float min, float max) { return std::clamp(value, min, max); }, [](double value, double min, double max) { return std::clamp(value, min, max); }, [](int value, int min, int max) { return std::clamp(value, min, max); }));
    lua.set_function("translate",
        sol::overload([](glm::mat4 inmatrix, glm::vec3 invector) -> glm::mat4 {
            return glm::translate(inmatrix, invector);
        }));
    lua.set_function("scale", sol::overload([](glm::mat4 inmatrix, glm::vec3 invector) -> glm::mat4 {
        return glm::translate(inmatrix, invector);
    }));
    lua.set_function("get_identity_matrix", []() -> glm::mat4 {
        {};
        return glm::identity<glm::mat4>();
    });
}

using namespace std;
using namespace glm;
using namespace Jkr;
using namespace Jkr::Renderer;
using namespace Jkr::Component;

constexpr bool toBool(string_view inString)
{
    if (inString == "true")
        return true;
    else
        return false;
}

float toFloat(string_view inString)
{
    return std::stof(string(inString));
}

const string gHeight = to_string(1080 / 2);
const string gWidth = to_string(1920 / 2);
unordered_map<string_view, string_view> gConfiguration { { "-title", "JkrGUI" },
    { "-height", gHeight },
    { "-width", gWidth },
    { "-store", "true" },
    { "-bgr", "1.0" },
    { "-bgg", "1.0" },
    { "-bgb", "1.0" },
    { "-bga", "1.0" },
    { "-des_size", "1000" },
    { "-var_des_size", "5000" },
    { "-cache_folder", "cache/" },
    { "-main_file", "main.lua" } };

auto FillConfig(const vector<string_view>& inArguments)
{
    for (auto i = inArguments.begin(); i < inArguments.end(); i += 2) {
        auto arg_index = *i;
        auto argument = *(i + 1);
        gConfiguration[arg_index] = argument;
    }
}

auto main(int ArgCount, char* ArgStrings[]) -> int
{
    auto& cf = gConfiguration;
    const vector<string_view> Arguments(ArgStrings + 1, ArgStrings + ArgCount);
    FillConfig(Arguments);

    auto i = Instance(stoi(string(cf["-des_size"])));
    auto w = Window(i, cf["-title"], stoi(string(cf["-height"])), stoi(string(cf["-width"])));
    auto em = EventManager();
    auto rr = ResourceManager(cf["-cache_folder"]);
    auto VarDesCount = stoi(string(cf["-var_des_size"]));
    if (toBool(cf["-store"])) {
        // rr.Load(i, VarDesCount);
        rr.Store(i);
    } else {
        rr.Load(i, VarDesCount);
    }
    auto td = _2d(i, w, rr, VarDesCount);

    sol::state l;
    l.open_libraries();
    l.set_exception_handler(&my_exception_handler);
    CreateGLMBindings(l);
    BindMathFunctions(l);
    CreateSDLEventBindings(l);

    l.set_function("get_window_dimensions", [&]() -> vec2 {
        auto d = w.GetWindowSize();
        return vec2(d.first, d.second);
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

        "is_left_button_pressed",
        [&]() -> bool {
            {};
            return em.IsLeftButtonPressed();
        },

        "is_right_button_pressed",
        [&]() { return em.IsRightButtonPressed(); },

        "is_key_pressed",
        [&](SDL_Keycode key) {
            {};
            return em.GetEventHandle().key.keysym.sym == key;
        },

        "is_keypress_event",
        [&]() -> bool { return em.GetEventHandle().type == SDL_KEYDOWN; },

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
        }

    );

    l.new_usertype<Jkr::BoundRect2D>(
        "bound_rect2d",
        sol::call_constructor,
        [](vec2 xy, vec2 wh) {
            std::cout << "new  bound rect" << std::endl;
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

    /* Renderers */
    {
        auto r = l["r"].get_or_create<sol::table>();

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
            [&](string font_path, int font_size) -> int {
                uint32_t id;
                td.bt.AddFontFace(font_path, font_size, id);
                return id;
            },

            "set_current_face",
            [&](int font_face) -> void {
                td.bt.SetCurrentFontFace(font_face);
            },

            "get_text_dimension",
            [&](std::string inString, int id) -> vec2 {
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
            [&](string text, vec3 pos) -> vec2 {
                uvec2 id_and_length;
                td.bt.AddText(text, pos.x, pos.y, pos.z, id_and_length.x, id_and_length.y);
                return id_and_length;
            },

            "update",
            [&](int id, string inString, vec3 inpos) -> void {
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
            [&](int Width, int Height) -> int {
                uint32_t id;
                td.sh.AddImage(Width, Height, id);
                return id;
            },

            "copy_image",
            [&](int id, CustomImagePainter& inPainter) { td.sh.CopyToImage(id, inPainter); },

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
        Jkr::Renderer::BestText_Alt ALT(td.sh, td.bt);

        r.new_usertype<Jkr::Renderer::BestText_Alt::ImageId>("ImageId");

        r.new_usertype<Jkr::Renderer::BestText_Alt>(
            "balt",

            "add",
            [&](int inFontId, string inS, vec3 inposition)
                -> Jkr::Renderer::BestText_Alt::ImageId {
                BestText_Alt::ImageId outId;
                ALT.Add(inFontId, vec2(inposition.x, inposition.y), inposition.z, inS, outId);
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
                string inText) {
                ALT.Update(inId, inFontId, vec2(inposition.x, inposition.y), inposition.z, inText);
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
                {};
                w.ResetScissor();
            }

        );

        struct push_constant {
            vec4 mPosDimen;
            vec4 mColor;
            vec4 mParam;
        };

        auto pc = R"""(

layout(push_constant, std430) uniform pc {
	vec4 mPosDimen;
	vec4 mColor;
	vec4 mParam;
} push;

)""";
        r.new_usertype<CustomImagePainter>(
            "image_painter",
            sol::call_constructor,
            sol::factories([&](std::string fileName, std::string inShaderCode, vec3 in_threads) {
                return CustomImagePainter(i,
                    fileName,
                    inShaderCode,
                    pc,
                    in_threads.x,
                    in_threads.y,
                    in_threads.z);
            }),

            "make_from",
            [&](CustomImagePainter &inP, std::string fileName, std::string inShaderCode) {
                return CustomImagePainter(inP, fileName, inShaderCode, pc);
            },

            "load",
            [&](CustomImagePainter &inP) {
                cout << "Load CustomImagePainter" << endl;
                inP.Load(w);
            },

            "store",
            [&](CustomImagePainter &inP) {
                cout << "Store CustomImagePainter" << endl;
                inP.Store(w);
            },

            "paint",
            [&](CustomImagePainter &inP, vec4 inPosDimen, vec4 inColor, vec4 inParam) {
                push_constant push { .mPosDimen = inPosDimen, .mColor = inColor, .mParam = inParam };
                inP.Draw<push_constant>(w, push);
            },

            "register_image",
            [&](CustomImagePainter &inP, int inHeight, int inWidth) {
                inP.RegisterImageToBeDrawnTo(w, inHeight, inWidth);
            },

            "register_image_existing",
            [&](CustomImagePainter &inP) { inP.RegisterImageToBeDrawnTo(w, inP); }

        );
    }

    /* Components */
    {
        auto c = l["c"].get_or_create<sol::table>();
        c.new_usertype<Component_base>(

            "com_base",
            sol::call_constructor,
            sol::factories([&]() {
                std::cout << "Component_base Called" << std::endl;
                return Component_base(td, em);
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

            "focus",
            sol::property(&Component_base::IsFocused, &Component_base::SetFocus),

            "is_mouse_on_top",
            &Component_base::IsMouseOnTop,

            "is_mouse_within",
            &Component_base::IsMouseWithin,

            "translation_matrix",
            &Component_base::GetTranslationMatrix,

            "from_component",
            &Component_base::FromComponent,

            "from_world",
            &Component_base::FromWorld,

            "update_window",
            &Component_base::SetWindowDimensions

        );
    }

    sol::protected_function_result result = l.safe_script_file(string(cf["-main_file"]), &sol::script_pass_on_error);
    if (not result.valid()) {
        sol::error error = result;
        cout << error.what() << endl;
        exit(-1);
    }

    sol::function draw_callback = l["Draw"];
    sol::function load_callback = l["Load"];
    sol::function event_callback = l["Event"];
    sol::function disptach_callback = l["Dispatch"];
    sol::function update_callback = l["Update"];

    auto Event = [&](void*) { event_callback(); };
    em.SetEventCallBack(Event);
    auto Draw = [&](void* data) {
        draw_callback();
    };
    w.SetDrawCallBack(Draw);
    auto Update = [&](void* data) { update_callback(); };
    w.SetUpdateCallBack(Update);
    auto Dispatch = [&](void* data) {
        disptach_callback();

        td.ln.Dispatch(w);
        td.sh.Dispatch(w);
        td.bt.Dispatch(w);
    };
    w.SetComputeDispatchCallBack(Dispatch);
    load_callback();

    array<float, 4> bg = { toFloat(cf["-bgr"]),
        toFloat(cf["-bgg"]),
        toFloat(cf["-bgb"]),
        toFloat(cf["-bga"]) };

    while (!em.ShouldQuit()) {
        em.ProcessEvents();
        w.Draw(bg[0], bg[1], bg[2], bg[3]);
    }
}
