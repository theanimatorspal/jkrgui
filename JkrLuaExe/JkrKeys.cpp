#include <SDLWindow.hpp>
#include "JkrLuaExe.hpp"

namespace JkrEXE {

void CreateKeyBindings(sol::state& s) {
    auto Key                       = s["Keyboard"].get_or_create<sol::table>();
    Key["SDLK_RETURN"]             = SDLK_RETURN;
    Key["SDLK_ESCAPE"]             = SDLK_ESCAPE;
    Key["SDLK_BACKSPACE"]          = SDLK_BACKSPACE;
    Key["SDLK_TAB"]                = SDLK_TAB;
    Key["SDLK_SPACE"]              = SDLK_SPACE;
    Key["SDLK_EXCLAIM"]            = SDLK_EXCLAIM;
    Key["SDLK_QUOTEDBL"]           = SDLK_QUOTEDBL;
    Key["SDLK_HASH"]               = SDLK_HASH;
    Key["SDLK_PERCENT"]            = SDLK_PERCENT;
    Key["SDLK_DOLLAR"]             = SDLK_DOLLAR;
    Key["SDLK_AMPERSAND"]          = SDLK_AMPERSAND;
    Key["SDLK_QUOTE"]              = SDLK_QUOTE;
    Key["SDLK_LEFTPAREN"]          = SDLK_LEFTPAREN;
    Key["SDLK_RIGHTPAREN"]         = SDLK_RIGHTPAREN;
    Key["SDLK_ASTERISK"]           = SDLK_ASTERISK;
    Key["SDLK_PLUS"]               = SDLK_PLUS;
    Key["SDLK_COMMA"]              = SDLK_COMMA;
    Key["SDLK_MINUS"]              = SDLK_MINUS;
    Key["SDLK_PERIOD"]             = SDLK_PERIOD;
    Key["SDLK_SLASH"]              = SDLK_SLASH;
    Key["SDLK_0"]                  = SDLK_0;
    Key["SDLK_1"]                  = SDLK_1;
    Key["SDLK_2"]                  = SDLK_2;
    Key["SDLK_3"]                  = SDLK_3;
    Key["SDLK_4"]                  = SDLK_4;
    Key["SDLK_5"]                  = SDLK_5;
    Key["SDLK_6"]                  = SDLK_6;
    Key["SDLK_7"]                  = SDLK_7;
    Key["SDLK_8"]                  = SDLK_8;
    Key["SDLK_9"]                  = SDLK_9;
    Key["SDLK_COLON"]              = SDLK_COLON;
    Key["SDLK_SEMICOLON"]          = SDLK_SEMICOLON;
    Key["SDLK_LESS"]               = SDLK_LESS;
    Key["SDLK_EQUALS"]             = SDLK_EQUALS;
    Key["SDLK_GREATER"]            = SDLK_GREATER;
    Key["SDLK_QUESTION"]           = SDLK_QUESTION;
    Key["SDLK_AT"]                 = SDLK_AT;

    Key["SDLK_LEFTBRACKET"]        = SDLK_LEFTBRACKET;
    Key["SDLK_BACKSLASH"]          = SDLK_BACKSLASH;
    Key["SDLK_RIGHTBRACKET"]       = SDLK_RIGHTBRACKET;
    Key["SDLK_CARET"]              = SDLK_CARET;
    Key["SDLK_UNDERSCORE"]         = SDLK_UNDERSCORE;
    Key["SDLK_BACKQUOTE"]          = SDLK_BACKQUOTE;
    Key["SDLK_a"]                  = SDLK_a;
    Key["SDLK_b"]                  = SDLK_b;
    Key["SDLK_c"]                  = SDLK_c;
    Key["SDLK_d"]                  = SDLK_d;
    Key["SDLK_e"]                  = SDLK_e;
    Key["SDLK_f"]                  = SDLK_f;
    Key["SDLK_g"]                  = SDLK_g;
    Key["SDLK_h"]                  = SDLK_h;
    Key["SDLK_i"]                  = SDLK_i;
    Key["SDLK_j"]                  = SDLK_j;
    Key["SDLK_k"]                  = SDLK_k;
    Key["SDLK_l"]                  = SDLK_l;
    Key["SDLK_m"]                  = SDLK_m;
    Key["SDLK_n"]                  = SDLK_n;
    Key["SDLK_o"]                  = SDLK_o;
    Key["SDLK_p"]                  = SDLK_p;
    Key["SDLK_q"]                  = SDLK_q;
    Key["SDLK_r"]                  = SDLK_r;
    Key["SDLK_s"]                  = SDLK_s;
    Key["SDLK_t"]                  = SDLK_t;
    Key["SDLK_u"]                  = SDLK_u;
    Key["SDLK_v"]                  = SDLK_v;
    Key["SDLK_w"]                  = SDLK_w;
    Key["SDLK_x"]                  = SDLK_x;
    Key["SDLK_y"]                  = SDLK_y;
    Key["SDLK_z"]                  = SDLK_z;

    Key["SDLK_CAPSLOCK"]           = SDLK_CAPSLOCK;

    Key["SDLK_F1"]                 = SDLK_F1;
    Key["SDLK_F2"]                 = SDLK_F2;
    Key["SDLK_F3"]                 = SDLK_F3;
    Key["SDLK_F4"]                 = SDLK_F4;
    Key["SDLK_F5"]                 = SDLK_F5;
    Key["SDLK_F6"]                 = SDLK_F6;
    Key["SDLK_F7"]                 = SDLK_F7;
    Key["SDLK_F8"]                 = SDLK_F8;
    Key["SDLK_F9"]                 = SDLK_F9;
    Key["SDLK_F10"]                = SDLK_F10;
    Key["SDLK_F11"]                = SDLK_F11;
    Key["SDLK_F12"]                = SDLK_F12;

    Key["SDLK_PRINTSCREEN"]        = SDLK_PRINTSCREEN;
    Key["SDLK_SCROLLLOCK"]         = SDLK_SCROLLLOCK;
    Key["SDLK_PAUSE"]              = SDLK_PAUSE;
    Key["SDLK_INSERT"]             = SDLK_INSERT;
    Key["SDLK_HOME"]               = SDLK_HOME;
    Key["SDLK_PAGEUP"]             = SDLK_PAGEUP;
    Key["SDLK_DELETE"]             = SDLK_DELETE;
    Key["SDLK_END"]                = SDLK_END;
    Key["SDLK_PAGEDOWN"]           = SDLK_PAGEDOWN;
    Key["SDLK_RIGHT"]              = SDLK_RIGHT;
    Key["SDLK_LEFT"]               = SDLK_LEFT;
    Key["SDLK_DOWN"]               = SDLK_DOWN;
    Key["SDLK_UP"]                 = SDLK_UP;

    Key["SDLK_NUMLOCKCLEAR"]       = SDLK_NUMLOCKCLEAR;
    Key["SDLK_KP_DIVIDE"]          = SDLK_KP_DIVIDE;
    Key["SDLK_KP_MULTIPLY"]        = SDLK_KP_MULTIPLY;
    Key["SDLK_KP_MINUS"]           = SDLK_KP_MINUS;
    Key["SDLK_KP_PLUS"]            = SDLK_KP_PLUS;
    Key["SDLK_KP_ENTER"]           = SDLK_KP_ENTER;
    Key["SDLK_KP_1"]               = SDLK_KP_1;
    Key["SDLK_KP_2"]               = SDLK_KP_2;
    Key["SDLK_KP_3"]               = SDLK_KP_3;
    Key["SDLK_KP_4"]               = SDLK_KP_4;
    Key["SDLK_KP_5"]               = SDLK_KP_5;
    Key["SDLK_KP_6"]               = SDLK_KP_6;
    Key["SDLK_KP_7"]               = SDLK_KP_7;
    Key["SDLK_KP_8"]               = SDLK_KP_8;
    Key["SDLK_KP_9"]               = SDLK_KP_9;
    Key["SDLK_KP_0"]               = SDLK_KP_0;
    Key["SDLK_KP_PERIOD"]          = SDLK_KP_PERIOD;

    Key["SDLK_APPLICATION"]        = SDLK_APPLICATION;
    Key["SDLK_POWER"]              = SDLK_POWER;
    Key["SDLK_KP_EQUALS"]          = SDLK_KP_EQUALS;
    Key["SDLK_F13"]                = SDLK_F13;
    Key["SDLK_F14"]                = SDLK_F14;
    Key["SDLK_F15"]                = SDLK_F15;
    Key["SDLK_F16"]                = SDLK_F16;
    Key["SDLK_F17"]                = SDLK_F17;
    Key["SDLK_F18"]                = SDLK_F18;
    Key["SDLK_F19"]                = SDLK_F19;
    Key["SDLK_F20"]                = SDLK_F20;
    Key["SDLK_F21"]                = SDLK_F21;
    Key["SDLK_F22"]                = SDLK_F22;
    Key["SDLK_F23"]                = SDLK_F23;
    Key["SDLK_F24"]                = SDLK_F24;
    Key["SDLK_EXECUTE"]            = SDLK_EXECUTE;
    Key["SDLK_HELP"]               = SDLK_HELP;
    Key["SDLK_MENU"]               = SDLK_MENU;
    Key["SDLK_SELECT"]             = SDLK_SELECT;
    Key["SDLK_STOP"]               = SDLK_STOP;
    Key["SDLK_AGAIN"]              = SDLK_AGAIN;
    Key["SDLK_UNDO"]               = SDLK_UNDO;
    Key["SDLK_CUT"]                = SDLK_CUT;
    Key["SDLK_COPY"]               = SDLK_COPY;
    Key["SDLK_PASTE"]              = SDLK_PASTE;
    Key["SDLK_FIND"]               = SDLK_FIND;
    Key["SDLK_MUTE"]               = SDLK_MUTE;
    Key["SDLK_VOLUMEUP"]           = SDLK_VOLUMEUP;
    Key["SDLK_VOLUMEDOWN"]         = SDLK_VOLUMEDOWN;
    Key["SDLK_KP_COMMA"]           = SDLK_KP_COMMA;
    Key["SDLK_KP_EQUALSAS400"]     = SDLK_KP_EQUALSAS400;

    Key["SDLK_ALTERASE"]           = SDLK_ALTERASE;
    Key["SDLK_SYSREQ"]             = SDLK_SYSREQ;
    Key["SDLK_CANCEL"]             = SDLK_CANCEL;
    Key["SDLK_CLEAR"]              = SDLK_CLEAR;
    Key["SDLK_PRIOR"]              = SDLK_PRIOR;
    Key["SDLK_RETURN2"]            = SDLK_RETURN2;
    Key["SDLK_SEPARATOR"]          = SDLK_SEPARATOR;
    Key["SDLK_OUT"]                = SDLK_OUT;
    Key["SDLK_OPER"]               = SDLK_OPER;
    Key["SDLK_CLEARAGAIN"]         = SDLK_CLEARAGAIN;
    Key["SDLK_CRSEL"]              = SDLK_CRSEL;
    Key["SDLK_EXSEL"]              = SDLK_EXSEL;

    Key["SDLK_KP_00"]              = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_00);
    Key["SDLK_KP_000"]             = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_000);
    Key["SDLK_THOUSANDSSEPARATOR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_THOUSANDSSEPARATOR);
    Key["SDLK_DECIMALSEPARATOR"]   = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DECIMALSEPARATOR);
    Key["SDLK_CURRENCYUNIT"]       = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYUNIT);
    Key["SDLK_CURRENCYSUBUNIT"]    = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYSUBUNIT);
    Key["SDLK_KP_LEFTPAREN"]       = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTPAREN);
    Key["SDLK_KP_RIGHTPAREN"]      = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTPAREN);
    Key["SDLK_KP_LEFTBRACE"]       = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTBRACE);
    Key["SDLK_KP_RIGHTBRACE"]      = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTBRACE);
    Key["SDLK_KP_TAB"]             = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_TAB);
    Key["SDLK_KP_BACKSPACE"]       = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BACKSPACE);
    Key["SDLK_KP_A"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_A);
    Key["SDLK_KP_B"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_B);
    Key["SDLK_KP_C"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_C);
    Key["SDLK_KP_D"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_D);
    Key["SDLK_KP_E"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_E);
    Key["SDLK_KP_F"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_F);
    Key["SDLK_KP_XOR"]             = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_XOR);
    Key["SDLK_KP_POWER"]           = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_POWER);
    Key["SDLK_KP_PERCENT"]         = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERCENT);
    Key["SDLK_KP_LESS"]            = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LESS);
    Key["SDLK_KP_GREATER"]         = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_GREATER);
    Key["SDLK_KP_AMPERSAND"]       = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AMPERSAND);
    Key["SDLK_KP_DBLAMPERSAND"]    = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLAMPERSAND);
    Key["SDLK_KP_VERTICALBAR"]     = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_VERTICALBAR);
    Key["SDLK_KP_DBLVERTICALBAR"]  = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLVERTICALBAR);
    Key["SDLK_KP_COLON"]           = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_COLON);
    Key["SDLK_KP_HASH"]            = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HASH);
    Key["SDLK_KP_SPACE"]           = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_SPACE);
    Key["SDLK_KP_AT"]              = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AT);
    Key["SDLK_KP_EXCLAM"]          = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EXCLAM);
    Key["SDLK_KP_MEMSTORE"]        = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSTORE);
    Key["SDLK_KP_MEMRECALL"]       = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMRECALL);
    Key["SDLK_KP_MEMCLEAR"]        = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMCLEAR);
    Key["SDLK_KP_MEMADD"]          = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMADD);
    Key["SDLK_KP_MEMSUBTRACT"]     = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSUBTRACT);
    Key["SDLK_KP_MEMMULTIPLY"]     = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMMULTIPLY);
    Key["SDLK_KP_MEMDIVIDE"]       = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMDIVIDE);
    Key["SDLK_KP_PLUSMINUS"]       = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUSMINUS);
    Key["SDLK_KP_CLEAR"]           = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEAR);
    Key["SDLK_KP_CLEARENTRY"]      = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEARENTRY);
    Key["SDLK_KP_BINARY"]          = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BINARY);
    Key["SDLK_KP_OCTAL"]           = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_OCTAL);
    Key["SDLK_KP_DECIMAL"]         = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DECIMAL);
    Key["SDLK_KP_HEXADECIMAL"]     = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HEXADECIMAL);

    Key["SDLK_LCTRL"]              = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LCTRL);
    Key["SDLK_LSHIFT"]             = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LSHIFT);
    Key["SDLK_LALT"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LALT);
    Key["SDLK_LGUI"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LGUI);
    Key["SDLK_RCTRL"]              = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RCTRL);
    Key["SDLK_RSHIFT"]             = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RSHIFT);
    Key["SDLK_RALT"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RALT);
    Key["SDLK_RGUI"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RGUI);

    Key["SDLK_MODE"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MODE);

    Key["SDLK_AUDIONEXT"]          = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIONEXT);
    Key["SDLK_AUDIOPREV"]          = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOPREV);
    Key["SDLK_AUDIOSTOP"]          = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOSTOP);
    Key["SDLK_AUDIOPLAY"]          = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOPLAY);
    Key["SDLK_AUDIOMUTE"]          = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOMUTE);
    Key["SDLK_MEDIASELECT"]        = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIASELECT);
    Key["SDLK_WWW"]                = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_WWW);
    Key["SDLK_MAIL"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MAIL);
    Key["SDLK_CALCULATOR"]         = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CALCULATOR);
    Key["SDLK_COMPUTER"]           = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_COMPUTER);
    Key["SDLK_AC_SEARCH"]          = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_SEARCH);
    Key["SDLK_AC_HOME"]            = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_HOME);
    Key["SDLK_AC_BACK"]            = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BACK);
    Key["SDLK_AC_FORWARD"]         = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_FORWARD);
    Key["SDLK_AC_STOP"]            = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_STOP);
    Key["SDLK_AC_REFRESH"]         = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_REFRESH);
    Key["SDLK_AC_BOOKMARKS"]       = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BOOKMARKS);

    Key["SDLK_BRIGHTNESSDOWN"]     = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_BRIGHTNESSDOWN);
    Key["SDLK_BRIGHTNESSUP"]       = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_BRIGHTNESSUP);
    Key["SDLK_DISPLAYSWITCH"]      = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DISPLAYSWITCH);
    Key["SDLK_KBDILLUMTOGGLE"]     = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KBDILLUMTOGGLE);
    Key["SDLK_KBDILLUMDOWN"]       = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KBDILLUMDOWN);
    Key["SDLK_KBDILLUMUP"]         = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KBDILLUMUP);
    Key["SDLK_EJECT"]              = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EJECT);
    Key["SDLK_SLEEP"]              = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SLEEP);
    Key["SDLK_APP1"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP1);
    Key["SDLK_APP2"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP2);

    Key["SDLK_AUDIOREWIND"]        = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOREWIND);
    Key["SDLK_AUDIOFASTFORWARD"]   = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOFASTFORWARD);

    Key["SDLK_SOFTLEFT"]           = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SOFTLEFT);
    Key["SDLK_SOFTRIGHT"]          = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SOFTRIGHT);
    Key["SDLK_CALL"]               = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CALL);
    Key["SDLK_ENDCALL"]            = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_ENDCALL);

    Key["SDL_SCANCODE_UNKNOWN"]    = SDL_SCANCODE_UNKNOWN,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
         /* @{ */

         Key["SDL_SCANCODE_A"]       = 4;
    Key["SDL_SCANCODE_B"]            = 5;
    Key["SDL_SCANCODE_C"]            = 6;
    Key["SDL_SCANCODE_D"]            = 7;
    Key["SDL_SCANCODE_E"]            = 8;
    Key["SDL_SCANCODE_F"]            = 9;
    Key["SDL_SCANCODE_G"]            = 10;
    Key["SDL_SCANCODE_H"]            = 11;
    Key["SDL_SCANCODE_I"]            = 12;
    Key["SDL_SCANCODE_J"]            = 13;
    Key["SDL_SCANCODE_K"]            = 14;
    Key["SDL_SCANCODE_L"]            = 15;
    Key["SDL_SCANCODE_M"]            = 16;
    Key["SDL_SCANCODE_N"]            = 17;
    Key["SDL_SCANCODE_O"]            = 18;
    Key["SDL_SCANCODE_P"]            = 19;
    Key["SDL_SCANCODE_Q"]            = 20;
    Key["SDL_SCANCODE_R"]            = 21;
    Key["SDL_SCANCODE_S"]            = 22;
    Key["SDL_SCANCODE_T"]            = 23;
    Key["SDL_SCANCODE_U"]            = 24;
    Key["SDL_SCANCODE_V"]            = 25;
    Key["SDL_SCANCODE_W"]            = 26;
    Key["SDL_SCANCODE_X"]            = 27;
    Key["SDL_SCANCODE_Y"]            = 28;
    Key["SDL_SCANCODE_Z"]            = 29;

    Key["SDL_SCANCODE_1"]            = 30;
    Key["SDL_SCANCODE_2"]            = 31;
    Key["SDL_SCANCODE_3"]            = 32;
    Key["SDL_SCANCODE_4"]            = 33;
    Key["SDL_SCANCODE_5"]            = 34;
    Key["SDL_SCANCODE_6"]            = 35;
    Key["SDL_SCANCODE_7"]            = 36;
    Key["SDL_SCANCODE_8"]            = 37;
    Key["SDL_SCANCODE_9"]            = 38;
    Key["SDL_SCANCODE_0"]            = 39;

    Key["SDL_SCANCODE_RETURN"]       = 40;
    Key["SDL_SCANCODE_ESCAPE"]       = 41;
    Key["SDL_SCANCODE_BACKSPACE"]    = 42;
    Key["SDL_SCANCODE_TAB"]          = 43;
    Key["SDL_SCANCODE_SPACE"]        = 44;

    Key["SDL_SCANCODE_MINUS"]        = 45;
    Key["SDL_SCANCODE_EQUALS"]       = 46;
    Key["SDL_SCANCODE_LEFTBRACKET"]  = 47;
    Key["SDL_SCANCODE_RIGHTBRACKET"] = 48;
    Key["SDL_SCANCODE_BACKSLASH"]    = 49; /**< Located at the lower left of the retur;
                                            *   key on ISO keyboards and at the right end
                                            *   of the QWERTY row on ANSI keyboards.
                                            *   Produces REVERSE SOLIDUS (backslash) and
                                            *   VERTICAL LINE in a US layout, REVERSE
                                            *   SOLIDUS and VERTICAL LINE in a UK Mac
                                            *   layout, NUMBER SIGN and TILDE in a UK
                                            *   Windows layout, DOLLAR SIGN and POUND SIGN
                                            *   in a Swiss German layout, NUMBER SIGN and
                                            *   APOSTROPHE in a German layout, GRAVE
                                            *   ACCENT and POUND SIGN in a French Mac
                                            *   layout, and ASTERISK and MICRO SIGN in a
                                            *   French Windows layout.
                                            */
    Key["SDL_SCANCODE_NONUSHASH"] = 50;    /**< ISO USB keyboards actually use this cod;
                                            *   instead of 49 for the same key, but all
                                            *   OSes I've seen treat the two codes
                                            *   identically. So, as an implementor, unless
                                            *   your keyboard generates both of those
                                            *   codes and your OS treats them differently,
                                            *   you should generate SDL_SCANCODE_BACKSLASH
                                            *   instead of this code. As a user, you
                                            *   should not rely on this code because SDL
                                            *   will never generate it with most (all?)
                                            *   keyboards.
                                            */
    Key["SDL_SCANCODE_SEMICOLON"]  = 51;
    Key["SDL_SCANCODE_APOSTROPHE"] = 52;
    Key["SDL_SCANCODE_GRAVE"]      = 53; /**< Located in the top left corner (on both ANS;
                                          *   and ISO keyboards). Produces GRAVE ACCENT and
                                          *   TILDE in a US Windows layout and in US and UK
                                          *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                                          *   and NOT SIGN in a UK Windows layout, SECTION
                                          *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                                          *   layouts on ISO keyboards, SECTION SIGN and
                                          *   DEGREE SIGN in a Swiss German layout (Mac:
                                          *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                                          *   DEGREE SIGN in a German layout (Mac: only on
                                          *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                                          *   French Windows layout, COMMERCIAL AT and
                                          *   NUMBER SIGN in a French Mac layout on ISO
                                          *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                                          *   SIGN in a Swiss German, German, or French Mac
                                          *   layout on ANSI keyboards.
                                          */
    Key["SDL_SCANCODE_COMMA"]       = 54;
    Key["SDL_SCANCODE_PERIOD"]      = 55;
    Key["SDL_SCANCODE_SLASH"]       = 56;

    Key["SDL_SCANCODE_CAPSLOCK"]    = 57;

    Key["SDL_SCANCODE_F1"]          = 58;
    Key["SDL_SCANCODE_F2"]          = 59;
    Key["SDL_SCANCODE_F3"]          = 60;
    Key["SDL_SCANCODE_F4"]          = 61;
    Key["SDL_SCANCODE_F5"]          = 62;
    Key["SDL_SCANCODE_F6"]          = 63;
    Key["SDL_SCANCODE_F7"]          = 64;
    Key["SDL_SCANCODE_F8"]          = 65;
    Key["SDL_SCANCODE_F9"]          = 66;
    Key["SDL_SCANCODE_F10"]         = 67;
    Key["SDL_SCANCODE_F11"]         = 68;
    Key["SDL_SCANCODE_F12"]         = 69;

    Key["SDL_SCANCODE_PRINTSCREEN"] = 70;
    Key["SDL_SCANCODE_SCROLLLOCK"]  = 71;
    Key["SDL_SCANCODE_PAUSE"]       = 72;
    Key["SDL_SCANCODE_INSERT"]      = 73, /**< insert on PC, help on some Mac keyboards (bu;
                   does send code 73, not 117) */
         Key["SDL_SCANCODE_HOME"]    = 74;
    Key["SDL_SCANCODE_PAGEUP"]       = 75;
    Key["SDL_SCANCODE_DELETE"]       = 76;
    Key["SDL_SCANCODE_END"]          = 77;
    Key["SDL_SCANCODE_PAGEDOWN"]     = 78;
    Key["SDL_SCANCODE_RIGHT"]        = 79;
    Key["SDL_SCANCODE_LEFT"]         = 80;
    Key["SDL_SCANCODE_DOWN"]         = 81;
    Key["SDL_SCANCODE_UP"]           = 82;

    Key["SDL_SCANCODE_NUMLOCKCLEAR"] = 83, /**< num lock on PC, clear on Mac keyboard;
                                            */
         Key["SDL_SCANCODE_KP_DIVIDE"] = 84;
    Key["SDL_SCANCODE_KP_MULTIPLY"]    = 85;
    Key["SDL_SCANCODE_KP_MINUS"]       = 86;
    Key["SDL_SCANCODE_KP_PLUS"]        = 87;
    Key["SDL_SCANCODE_KP_ENTER"]       = 88;
    Key["SDL_SCANCODE_KP_1"]           = 89;
    Key["SDL_SCANCODE_KP_2"]           = 90;
    Key["SDL_SCANCODE_KP_3"]           = 91;
    Key["SDL_SCANCODE_KP_4"]           = 92;
    Key["SDL_SCANCODE_KP_5"]           = 93;
    Key["SDL_SCANCODE_KP_6"]           = 94;
    Key["SDL_SCANCODE_KP_7"]           = 95;
    Key["SDL_SCANCODE_KP_8"]           = 96;
    Key["SDL_SCANCODE_KP_9"]           = 97;
    Key["SDL_SCANCODE_KP_0"]           = 98;
    Key["SDL_SCANCODE_KP_PERIOD"]      = 99;

    Key["SDL_SCANCODE_NONUSBACKSLASH"] = 100; /**< This is the additional key that IS;
                                               *   keyboards have over ANSI ones,
                                               *   located between left shift and Y.
                                               *   Produces GRAVE ACCENT and TILDE in a
                                               *   US or UK Mac layout, REVERSE SOLIDUS
                                               *   (backslash) and VERTICAL LINE in a
                                               *   US or UK Windows layout, and
                                               *   LESS-THAN SIGN and GREATER-THAN SIGN
                                               *   in a Swiss German, German, or French
                                               *   layout. */
    Key["SDL_SCANCODE_APPLICATION"] = 101;
    Key["SDL_SCANCODE_POWER"]       = 102;

    Key["SDL_SCANCODE_KP_EQUALS"]   = 103;
    Key["SDL_SCANCODE_F13"]         = 104;
    Key["SDL_SCANCODE_F14"]         = 105;
    Key["SDL_SCANCODE_F15"]         = 106;
    Key["SDL_SCANCODE_F16"]         = 107;
    Key["SDL_SCANCODE_F17"]         = 108;
    Key["SDL_SCANCODE_F18"]         = 109;
    Key["SDL_SCANCODE_F19"]         = 110;
    Key["SDL_SCANCODE_F20"]         = 111;
    Key["SDL_SCANCODE_F21"]         = 112;
    Key["SDL_SCANCODE_F22"]         = 113;
    Key["SDL_SCANCODE_F23"]         = 114;
    Key["SDL_SCANCODE_F24"]         = 115;
    Key["SDL_SCANCODE_EXECUTE"]     = 116;
    Key["SDL_SCANCODE_HELP"] = 117, Key["SDL_SCANCODE_MENU"] = 118,
    Key["SDL_SCANCODE_SELECT"] = 119;
    Key["SDL_SCANCODE_STOP"] = 120, Key["SDL_SCANCODE_AGAIN"] = 121, Key["SDL_SCANCODE_UNDO"] = 122;
    Key["SDL_SCANCODE_CUT"]        = 123;
    Key["SDL_SCANCODE_COPY"]       = 124, /**< AC Copy **/
         Key["SDL_SCANCODE_PASTE"] = 125; /**< AC Paste **/
    Key["SDL_SCANCODE_FIND"]       = 126; /**< AC Find **/
    Key["SDL_SCANCODE_MUTE"]       = 127;
    Key["SDL_SCANCODE_VOLUMEUP"]   = 128;
    Key["SDL_SCANCODE_VOLUMEDOWN"] = 129;
    /* not sure whether there's a reason to enable these */
    /*     SDL_SCANCODE_LOCKINGCAPSLOCK = 130,  */
    /*     SDL_SCANCODE_LOCKINGNUMLOCK = 131, */
    /*     SDL_SCANCODE_LOCKINGSCROLLLOCK = 132, */
    Key["SDL_SCANCODE_KP_COMMA"]           = 133;
    Key["SDL_SCANCODE_KP_EQUALSAS400"]     = 134;

    Key["SDL_SCANCODE_INTERNATIONAL1"]     = 135;
    Key["SDL_SCANCODE_INTERNATIONAL2"]     = 136;
    Key["SDL_SCANCODE_INTERNATIONAL3"]     = 137;
    Key["SDL_SCANCODE_INTERNATIONAL4"]     = 138;
    Key["SDL_SCANCODE_INTERNATIONAL5"]     = 139;
    Key["SDL_SCANCODE_INTERNATIONAL6"]     = 140;
    Key["SDL_SCANCODE_INTERNATIONAL7"]     = 141;
    Key["SDL_SCANCODE_INTERNATIONAL8"]     = 142;
    Key["SDL_SCANCODE_INTERNATIONAL9"]     = 143;
    Key["SDL_SCANCODE_LANG1"]              = 144;
    Key["SDL_SCANCODE_LANG2"]              = 145;
    Key["SDL_SCANCODE_LANG3"]              = 146;
    Key["SDL_SCANCODE_LANG4"]              = 147;
    Key["SDL_SCANCODE_LANG5"]              = 148;
    Key["SDL_SCANCODE_LANG6"]              = 149;
    Key["SDL_SCANCODE_LANG7"]              = 150;
    Key["SDL_SCANCODE_LANG8"]              = 151;
    Key["SDL_SCANCODE_LANG9"]              = 152;

    Key["SDL_SCANCODE_ALTERASE"]           = 153;
    Key["SDL_SCANCODE_SYSREQ"]             = 154;
    Key["SDL_SCANCODE_CANCEL"]             = 155;
    Key["SDL_SCANCODE_CLEAR"]              = 156;
    Key["SDL_SCANCODE_PRIOR"]              = 157;
    Key["SDL_SCANCODE_RETURN2"]            = 158;
    Key["SDL_SCANCODE_SEPARATOR"]          = 159;
    Key["SDL_SCANCODE_OUT"]                = 160;
    Key["SDL_SCANCODE_OPER"]               = 161;
    Key["SDL_SCANCODE_CLEARAGAIN"]         = 162;
    Key["SDL_SCANCODE_CRSEL"]              = 163;
    Key["SDL_SCANCODE_EXSEL"]              = 164;

    Key["SDL_SCANCODE_KP_00"]              = 176;
    Key["SDL_SCANCODE_KP_000"]             = 177;
    Key["SDL_SCANCODE_THOUSANDSSEPARATOR"] = 178;
    Key["SDL_SCANCODE_DECIMALSEPARATOR"]   = 179;
    Key["SDL_SCANCODE_CURRENCYUNIT"]       = 180;
    Key["SDL_SCANCODE_CURRENCYSUBUNIT"]    = 181;
    Key["SDL_SCANCODE_KP_LEFTPAREN"]       = 182;
    Key["SDL_SCANCODE_KP_RIGHTPAREN"]      = 183;
    Key["SDL_SCANCODE_KP_LEFTBRACE"]       = 184;
    Key["SDL_SCANCODE_KP_RIGHTBRACE"]      = 185;
    Key["SDL_SCANCODE_KP_TAB"]             = 186;
    Key["SDL_SCANCODE_KP_BACKSPACE"]       = 187;
    Key["SDL_SCANCODE_KP_A"]               = 188;
    Key["SDL_SCANCODE_KP_B"]               = 189;
    Key["SDL_SCANCODE_KP_C"]               = 190;
    Key["SDL_SCANCODE_KP_D"]               = 191;
    Key["SDL_SCANCODE_KP_E"]               = 192;
    Key["SDL_SCANCODE_KP_F"]               = 193;
    Key["SDL_SCANCODE_KP_XOR"]             = 194;
    Key["SDL_SCANCODE_KP_POWER"]           = 195;
    Key["SDL_SCANCODE_KP_PERCENT"]         = 196;
    Key["SDL_SCANCODE_KP_LESS"]            = 197;
    Key["SDL_SCANCODE_KP_GREATER"]         = 198;
    Key["SDL_SCANCODE_KP_AMPERSAND"]       = 199;
    Key["SDL_SCANCODE_KP_DBLAMPERSAND"]    = 200;
    Key["SDL_SCANCODE_KP_VERTICALBAR"]     = 201;
    Key["SDL_SCANCODE_KP_DBLVERTICALBAR"]  = 202;
    Key["SDL_SCANCODE_KP_COLON"]           = 203;
    Key["SDL_SCANCODE_KP_HASH"]            = 204;
    Key["SDL_SCANCODE_KP_SPACE"]           = 205;
    Key["SDL_SCANCODE_KP_AT"]              = 206;
    Key["SDL_SCANCODE_KP_EXCLAM"]          = 207;
    Key["SDL_SCANCODE_KP_MEMSTORE"]        = 208;
    Key["SDL_SCANCODE_KP_MEMRECALL"]       = 209;
    Key["SDL_SCANCODE_KP_MEMCLEAR"]        = 210;
    Key["SDL_SCANCODE_KP_MEMADD"]          = 211;
    Key["SDL_SCANCODE_KP_MEMSUBTRACT"]     = 212;
    Key["SDL_SCANCODE_KP_MEMMULTIPLY"]     = 213;
    Key["SDL_SCANCODE_KP_MEMDIVIDE"]       = 214;
    Key["SDL_SCANCODE_KP_PLUSMINUS"]       = 215;
    Key["SDL_SCANCODE_KP_CLEAR"]           = 216;
    Key["SDL_SCANCODE_KP_CLEARENTRY"]      = 217;
    Key["SDL_SCANCODE_KP_BINARY"]          = 218;
    Key["SDL_SCANCODE_KP_OCTAL"]           = 219;
    Key["SDL_SCANCODE_KP_DECIMAL"]         = 220;
    Key["SDL_SCANCODE_KP_HEXADECIMAL"]     = 221;

    Key["SDL_SCANCODE_LCTRL"]              = 224;
    Key["SDL_SCANCODE_LSHIFT"]             = 225;
    Key["SDL_SCANCODE_LALT"]               = 226;
    Key["SDL_SCANCODE_LGUI"]               = 227;
    Key["SDL_SCANCODE_RCTRL"]              = 228;
    Key["SDL_SCANCODE_RSHIFT"]             = 229;
    Key["SDL_SCANCODE_RALT"]               = 230;
    Key["SDL_SCANCODE_RGUI"]               = 231;

    Key["SDL_SCANCODE_MODE"]               = 257;

    /* @} */ /* Usage page 0x07 */

    /**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     *  See https://usb.org/sites/default/files/hut1_2.pdf
     *
     *  There are way more keys in the spec than we can represent in the
     *  current scancode range, so pick the ones that commonly come up in
     *  real world usage.
     */
    /* @{ */

    Key["SDL_SCANCODE_AUDIONEXT"]        = 258;
    Key["SDL_SCANCODE_AUDIOPREV"]        = 259;
    Key["SDL_SCANCODE_AUDIOSTOP"]        = 260;
    Key["SDL_SCANCODE_AUDIOPLAY"]        = 261;
    Key["SDL_SCANCODE_AUDIOMUTE"]        = 262;
    Key["SDL_SCANCODE_MEDIASELECT"]      = 263;
    Key["SDL_SCANCODE_WWW"]              = 264;
    Key["SDL_SCANCODE_MAIL"]             = 265;
    Key["SDL_SCANCODE_CALCULATOR"]       = 266;
    Key["SDL_SCANCODE_COMPUTER"]         = 267;
    Key["SDL_SCANCODE_AC_SEARCH"]        = 268;
    Key["SDL_SCANCODE_AC_HOME"]          = 269;
    Key["SDL_SCANCODE_AC_BACK"]          = 270;
    Key["SDL_SCANCODE_AC_FORWARD"]       = 271;
    Key["SDL_SCANCODE_AC_STOP"]          = 272;
    Key["SDL_SCANCODE_AC_REFRESH"]       = 273;

    Key["SDL_SCANCODE_AC_BOOKMARKS"]     = 274;
    Key["SDL_SCANCODE_BRIGHTNESSDOWN"]   = 275;
    Key["SDL_SCANCODE_BRIGHTNESSUP"]     = 276;
    Key["SDL_SCANCODE_DISPLAYSWITCH"]    = 277;
    Key["SDL_SCANCODE_KBDILLUMTOGGLE"]   = 278;
    Key["SDL_SCANCODE_KBDILLUMDOWN"]     = 279;
    Key["SDL_SCANCODE_KBDILLUMUP"]       = 280;
    Key["SDL_SCANCODE_EJECT"]            = 281;
    Key["SDL_SCANCODE_SLEEP"]            = 282;
    Key["SDL_SCANCODE_APP1"]             = 283;
    Key["SDL_SCANCODE_APP2"]             = 284;
    Key["SDL_SCANCODE_AUDIOREWIND"]      = 285;
    Key["SDL_SCANCODE_AUDIOFASTFORWARD"] = 286;
    Key["SDL_SCANCODE_SOFTLEFT"]         = 287;
    Key["SDL_SCANCODE_SOFTRIGHT"]        = 288;
    Key["SDL_SCANCODE_CALL"]             = 289;
    Key["SDL_SCANCODE_ENDCALL"]          = 290;
    Key["SDL_NUM_SCANCODES"]             = 512;
}

} // namespace JkrEXE