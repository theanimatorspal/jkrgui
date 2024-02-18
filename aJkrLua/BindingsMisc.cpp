#include "Neural/SimpleNN.hpp"
#include "EntryPoint.hpp"

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

    lua["key"]["SDL_SCANCODE_UNKNOWN"] = SDL_SCANCODE_UNKNOWN,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
    /* @{ */

    lua["key"]["SDL_SCANCODE_A"] = 4;
    lua["key"]["SDL_SCANCODE_B"] = 5;
    lua["key"]["SDL_SCANCODE_C"] = 6;
    lua["key"]["SDL_SCANCODE_D"] = 7;
    lua["key"]["SDL_SCANCODE_E"] = 8;
    lua["key"]["SDL_SCANCODE_F"] = 9;
    lua["key"]["SDL_SCANCODE_G"] = 10;
    lua["key"]["SDL_SCANCODE_H"] = 11;
    lua["key"]["SDL_SCANCODE_I"] = 12;
    lua["key"]["SDL_SCANCODE_J"] = 13;
    lua["key"]["SDL_SCANCODE_K"] = 14;
    lua["key"]["SDL_SCANCODE_L"] = 15;
    lua["key"]["SDL_SCANCODE_M"] = 16;
    lua["key"]["SDL_SCANCODE_N"] = 17;
    lua["key"]["SDL_SCANCODE_O"] = 18;
    lua["key"]["SDL_SCANCODE_P"] = 19;
    lua["key"]["SDL_SCANCODE_Q"] = 20;
    lua["key"]["SDL_SCANCODE_R"] = 21;
    lua["key"]["SDL_SCANCODE_S"] = 22;
    lua["key"]["SDL_SCANCODE_T"] = 23;
    lua["key"]["SDL_SCANCODE_U"] = 24;
    lua["key"]["SDL_SCANCODE_V"] = 25;
    lua["key"]["SDL_SCANCODE_W"] = 26;
    lua["key"]["SDL_SCANCODE_X"] = 27;
    lua["key"]["SDL_SCANCODE_Y"] = 28;
    lua["key"]["SDL_SCANCODE_Z"] = 29;

    lua["key"]["SDL_SCANCODE_1"] = 30;
    lua["key"]["SDL_SCANCODE_2"] = 31;
    lua["key"]["SDL_SCANCODE_3"] = 32;
    lua["key"]["SDL_SCANCODE_4"] = 33;
    lua["key"]["SDL_SCANCODE_5"] = 34;
    lua["key"]["SDL_SCANCODE_6"] = 35;
    lua["key"]["SDL_SCANCODE_7"] = 36;
    lua["key"]["SDL_SCANCODE_8"] = 37;
    lua["key"]["SDL_SCANCODE_9"] = 38;
    lua["key"]["SDL_SCANCODE_0"] = 39;

    lua["key"]["SDL_SCANCODE_RETURN"] = 40;
    lua["key"]["SDL_SCANCODE_ESCAPE"] = 41;
    lua["key"]["SDL_SCANCODE_BACKSPACE"] = 42;
    lua["key"]["SDL_SCANCODE_TAB"] = 43;
    lua["key"]["SDL_SCANCODE_SPACE"] = 44;

    lua["key"]["SDL_SCANCODE_MINUS"] = 45;
    lua["key"]["SDL_SCANCODE_EQUALS"] = 46;
    lua["key"]["SDL_SCANCODE_LEFTBRACKET"] = 47;
    lua["key"]["SDL_SCANCODE_RIGHTBRACKET"] = 48;
    lua["key"]["SDL_SCANCODE_BACKSLASH"] = 49; /**< Located at the lower left of the retur;
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
    lua["key"]["SDL_SCANCODE_NONUSHASH"] = 50; /**< ISO USB keyboards actually use this cod;
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
    lua["key"]["SDL_SCANCODE_SEMICOLON"] = 51;
    lua["key"]["SDL_SCANCODE_APOSTROPHE"] = 52;
    lua["key"]["SDL_SCANCODE_GRAVE"] = 53; /**< Located in the top left corner (on both ANS;
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
    lua["key"]["SDL_SCANCODE_COMMA"] = 54;
    lua["key"]["SDL_SCANCODE_PERIOD"] = 55;
    lua["key"]["SDL_SCANCODE_SLASH"] = 56;

    lua["key"]["SDL_SCANCODE_CAPSLOCK"] = 57;

    lua["key"]["SDL_SCANCODE_F1"] = 58;
    lua["key"]["SDL_SCANCODE_F2"] = 59;
    lua["key"]["SDL_SCANCODE_F3"] = 60;
    lua["key"]["SDL_SCANCODE_F4"] = 61;
    lua["key"]["SDL_SCANCODE_F5"] = 62;
    lua["key"]["SDL_SCANCODE_F6"] = 63;
    lua["key"]["SDL_SCANCODE_F7"] = 64;
    lua["key"]["SDL_SCANCODE_F8"] = 65;
    lua["key"]["SDL_SCANCODE_F9"] = 66;
    lua["key"]["SDL_SCANCODE_F10"] = 67;
    lua["key"]["SDL_SCANCODE_F11"] = 68;
    lua["key"]["SDL_SCANCODE_F12"] = 69;

    lua["key"]["SDL_SCANCODE_PRINTSCREEN"] = 70;
    lua["key"]["SDL_SCANCODE_SCROLLLOCK"] = 71;
    lua["key"]["SDL_SCANCODE_PAUSE"] = 72;
    lua["key"]["SDL_SCANCODE_INSERT"] = 73, /**< insert on PC, help on some Mac keyboards (bu;
                                   does send code 73, not 117) */
    lua["key"]["SDL_SCANCODE_HOME"] = 74;
    lua["key"]["SDL_SCANCODE_PAGEUP"] = 75;
    lua["key"]["SDL_SCANCODE_DELETE"] = 76;
    lua["key"]["SDL_SCANCODE_END"] = 77;
    lua["key"]["SDL_SCANCODE_PAGEDOWN"] = 78;
    lua["key"]["SDL_SCANCODE_RIGHT"] = 79;
    lua["key"]["SDL_SCANCODE_LEFT"] = 80;
    lua["key"]["SDL_SCANCODE_DOWN"] = 81;
    lua["key"]["SDL_SCANCODE_UP"] = 82;

    lua["key"]["SDL_SCANCODE_NUMLOCKCLEAR"] = 83, /**< num lock on PC, clear on Mac keyboard;
                                     */
    lua["key"]["SDL_SCANCODE_KP_DIVIDE"] = 84;
    lua["key"]["SDL_SCANCODE_KP_MULTIPLY"] = 85;
    lua["key"]["SDL_SCANCODE_KP_MINUS"] = 86;
    lua["key"]["SDL_SCANCODE_KP_PLUS"] = 87;
    lua["key"]["SDL_SCANCODE_KP_ENTER"] = 88;
    lua["key"]["SDL_SCANCODE_KP_1"] = 89;
    lua["key"]["SDL_SCANCODE_KP_2"] = 90;
    lua["key"]["SDL_SCANCODE_KP_3"] = 91;
    lua["key"]["SDL_SCANCODE_KP_4"] = 92;
    lua["key"]["SDL_SCANCODE_KP_5"] = 93;
    lua["key"]["SDL_SCANCODE_KP_6"] = 94;
    lua["key"]["SDL_SCANCODE_KP_7"] = 95;
    lua["key"]["SDL_SCANCODE_KP_8"] = 96;
    lua["key"]["SDL_SCANCODE_KP_9"] = 97;
    lua["key"]["SDL_SCANCODE_KP_0"] = 98;
    lua["key"]["SDL_SCANCODE_KP_PERIOD"] = 99;

    lua["key"]["SDL_SCANCODE_NONUSBACKSLASH"] = 100; /**< This is the additional key that IS;
                                        *   keyboards have over ANSI ones,
                                        *   located between left shift and Y.
                                        *   Produces GRAVE ACCENT and TILDE in a
                                        *   US or UK Mac layout, REVERSE SOLIDUS
                                        *   (backslash) and VERTICAL LINE in a
                                        *   US or UK Windows layout, and
                                        *   LESS-THAN SIGN and GREATER-THAN SIGN
                                        *   in a Swiss German, German, or French
                                        *   layout. */
    lua["key"]["SDL_SCANCODE_APPLICATION"] = 101;
    lua["key"]["SDL_SCANCODE_POWER"] = 102;

    lua["key"]["SDL_SCANCODE_KP_EQUALS"] = 103;
    lua["key"]["SDL_SCANCODE_F13"] = 104;
    lua["key"]["SDL_SCANCODE_F14"] = 105;
    lua["key"]["SDL_SCANCODE_F15"] = 106;
    lua["key"]["SDL_SCANCODE_F16"] = 107;
    lua["key"]["SDL_SCANCODE_F17"] = 108;
    lua["key"]["SDL_SCANCODE_F18"] = 109;
    lua["key"]["SDL_SCANCODE_F19"] = 110;
    lua["key"]["SDL_SCANCODE_F20"] = 111;
    lua["key"]["SDL_SCANCODE_F21"] = 112;
    lua["key"]["SDL_SCANCODE_F22"] = 113;
    lua["key"]["SDL_SCANCODE_F23"] = 114;
    lua["key"]["SDL_SCANCODE_F24"] = 115;
    lua["key"]["SDL_SCANCODE_EXECUTE"] = 116;
    lua["key"]["SDL_SCANCODE_HELP"] = 117,    
    lua["key"]["SDL_SCANCODE_MENU"] = 118,    
    lua["key"]["SDL_SCANCODE_SELECT"] = 119;
    lua["key"]["SDL_SCANCODE_STOP"] = 120,
    lua["key"]["SDL_SCANCODE_AGAIN"] = 121,
    lua["key"]["SDL_SCANCODE_UNDO"] = 122;
    lua["key"]["SDL_SCANCODE_CUT"] = 123;    
    lua["key"]["SDL_SCANCODE_COPY"] = 124,    /**< AC Copy **/
    lua["key"]["SDL_SCANCODE_PASTE"] = 125;   /**< AC Paste **/
    lua["key"]["SDL_SCANCODE_FIND"] = 126;    /**< AC Find **/
    lua["key"]["SDL_SCANCODE_MUTE"] = 127;
    lua["key"]["SDL_SCANCODE_VOLUMEUP"] = 128;
    lua["key"]["SDL_SCANCODE_VOLUMEDOWN"] = 129;
/* not sure whether there's a reason to enable these */
/*     SDL_SCANCODE_LOCKINGCAPSLOCK = 130,  */
/*     SDL_SCANCODE_LOCKINGNUMLOCK = 131, */
/*     SDL_SCANCODE_LOCKINGSCROLLLOCK = 132, */
    lua["key"]["SDL_SCANCODE_KP_COMMA"] = 133;
    lua["key"]["SDL_SCANCODE_KP_EQUALSAS400"] = 134;

    lua["key"]["SDL_SCANCODE_INTERNATIONAL1"] = 135;
    lua["key"]["SDL_SCANCODE_INTERNATIONAL2"] = 136;
    lua["key"]["SDL_SCANCODE_INTERNATIONAL3"] = 137;
    lua["key"]["SDL_SCANCODE_INTERNATIONAL4"] = 138;
    lua["key"]["SDL_SCANCODE_INTERNATIONAL5"] = 139;
    lua["key"]["SDL_SCANCODE_INTERNATIONAL6"] = 140;
    lua["key"]["SDL_SCANCODE_INTERNATIONAL7"] = 141;
    lua["key"]["SDL_SCANCODE_INTERNATIONAL8"] = 142;
    lua["key"]["SDL_SCANCODE_INTERNATIONAL9"] = 143;
    lua["key"]["SDL_SCANCODE_LANG1"] = 144;
    lua["key"]["SDL_SCANCODE_LANG2"] = 145;
    lua["key"]["SDL_SCANCODE_LANG3"] = 14;
    lua["key"]["SDL_SCANCODE_LANG4"] = 147;
    lua["key"]["SDL_SCANCODE_LANG5"] = 148;
    lua["key"]["SDL_SCANCODE_LANG6"] = 149;
    lua["key"]["SDL_SCANCODE_LANG7"] = 150;
    lua["key"]["SDL_SCANCODE_LANG8"] = 151;
    lua["key"]["SDL_SCANCODE_LANG9"] = 152;

    lua["key"]["SDL_SCANCODE_ALTERASE"] = 153;
    lua["key"]["SDL_SCANCODE_SYSREQ"] = 154;
    lua["key"]["SDL_SCANCODE_CANCEL"] = 155;
    lua["key"]["SDL_SCANCODE_CLEAR"] = 156;
    lua["key"]["SDL_SCANCODE_PRIOR"] = 157;
    lua["key"]["SDL_SCANCODE_RETURN2"] = 158;
    lua["key"]["SDL_SCANCODE_SEPARATOR"] = 159;
    lua["key"]["SDL_SCANCODE_OUT"] = 160;
    lua["key"]["SDL_SCANCODE_OPER"] = 161;
    lua["key"]["SDL_SCANCODE_CLEARAGAIN"] = 162;
    lua["key"]["SDL_SCANCODE_CRSEL"] = 163;
    lua["key"]["SDL_SCANCODE_EXSEL"] = 164;

    lua["key"]["SDL_SCANCODE_KP_00"] = 176;
    lua["key"]["SDL_SCANCODE_KP_000"] = 177;
    lua["key"]["SDL_SCANCODE_THOUSANDSSEPARATOR"] = 178;
    lua["key"]["SDL_SCANCODE_DECIMALSEPARATOR"] = 179;
    lua["key"]["SDL_SCANCODE_CURRENCYUNIT"] = 180;
    lua["key"]["SDL_SCANCODE_CURRENCYSUBUNIT"] = 181;
    lua["key"]["SDL_SCANCODE_KP_LEFTPAREN"] = 182;
    lua["key"]["SDL_SCANCODE_KP_RIGHTPAREN"] = 183;
    lua["key"]["SDL_SCANCODE_KP_LEFTBRACE"] = 184;
    lua["key"]["SDL_SCANCODE_KP_RIGHTBRACE"] = 185;
    lua["key"]["SDL_SCANCODE_KP_TAB"] = 186;
    lua["key"]["SDL_SCANCODE_KP_BACKSPACE"] = 187;
    lua["key"]["SDL_SCANCODE_KP_A"] = 188;
    lua["key"]["SDL_SCANCODE_KP_B"] = 189;
    lua["key"]["SDL_SCANCODE_KP_C"] = 190;
    lua["key"]["SDL_SCANCODE_KP_D"] = 191;
    lua["key"]["SDL_SCANCODE_KP_E"] = 192;
    lua["key"]["SDL_SCANCODE_KP_F"] = 193;
    lua["key"]["SDL_SCANCODE_KP_XOR"] = 194;
    lua["key"]["SDL_SCANCODE_KP_POWER"] = 195;
    lua["key"]["SDL_SCANCODE_KP_PERCENT"] = 196;
    lua["key"]["SDL_SCANCODE_KP_LESS"] = 197;
    lua["key"]["SDL_SCANCODE_KP_GREATER"] = 198;
    lua["key"]["SDL_SCANCODE_KP_AMPERSAND"] = 199;
    lua["key"]["SDL_SCANCODE_KP_DBLAMPERSAND"] = 200;
    lua["key"]["SDL_SCANCODE_KP_VERTICALBAR"] = 201;
    lua["key"]["SDL_SCANCODE_KP_DBLVERTICALBAR"] = 202;
    lua["key"]["SDL_SCANCODE_KP_COLON"] = 203;
    lua["key"]["SDL_SCANCODE_KP_HASH"] = 204;
    lua["key"]["SDL_SCANCODE_KP_SPACE"] = 205;
    lua["key"]["SDL_SCANCODE_KP_AT"] = 206;
    lua["key"]["SDL_SCANCODE_KP_EXCLAM"] = 207;
    lua["key"]["SDL_SCANCODE_KP_MEMSTORE"] = 208;
    lua["key"]["SDL_SCANCODE_KP_MEMRECALL"] = 209;
    lua["key"]["SDL_SCANCODE_KP_MEMCLEAR"] = 210;
    lua["key"]["SDL_SCANCODE_KP_MEMADD"] = 211;
    lua["key"]["SDL_SCANCODE_KP_MEMSUBTRACT"] = 212;
    lua["key"]["SDL_SCANCODE_KP_MEMMULTIPLY"] = 213;
    lua["key"]["SDL_SCANCODE_KP_MEMDIVIDE"] = 214;
    lua["key"]["SDL_SCANCODE_KP_PLUSMINUS"] = 215;
    lua["key"]["SDL_SCANCODE_KP_CLEAR"] = 216;
    lua["key"]["SDL_SCANCODE_KP_CLEARENTRY"] = 217;
    lua["key"]["SDL_SCANCODE_KP_BINARY"] = 218;
    lua["key"]["SDL_SCANCODE_KP_OCTAL"] = 219;
    lua["key"]["SDL_SCANCODE_KP_DECIMAL"] = 220;
    lua["key"]["SDL_SCANCODE_KP_HEXADECIMAL"] = 221;

    lua["key"]["SDL_SCANCODE_LCTRL"] = 224;
    lua["key"]["SDL_SCANCODE_LSHIFT"] = 225;
    lua["key"]["SDL_SCANCODE_LALT"] = 226;
    lua["key"]["SDL_SCANCODE_LGUI"] = 227;
    lua["key"]["SDL_SCANCODE_RCTRL"] = 228;
    lua["key"]["SDL_SCANCODE_RSHIFT"] = 229;
    lua["key"]["SDL_SCANCODE_RALT"] = 230;
    lua["key"]["SDL_SCANCODE_RGUI"] = 231;

    lua["key"]["SDL_SCANCODE_MODE"] = 257;

    /* @} *//* Usage page 0x07 */

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

    lua["key"]["SDL_SCANCODE_AUDIONEXT"] = 258;
    lua["key"]["SDL_SCANCODE_AUDIOPREV"] = 259;
    lua["key"]["SDL_SCANCODE_AUDIOSTOP"] = 260;
    lua["key"]["SDL_SCANCODE_AUDIOPLAY"] = 261;
    lua["key"]["SDL_SCANCODE_AUDIOMUTE"] = 262;
    lua["key"]["SDL_SCANCODE_MEDIASELECT"] = 263;
    lua["key"]["SDL_SCANCODE_WWW"] = 264;         
    lua["key"]["SDL_SCANCODE_MAIL"] = 265;
    lua["key"]["SDL_SCANCODE_CALCULATOR"] = 266;  
    lua["key"]["SDL_SCANCODE_COMPUTER"] = 267;
    lua["key"]["SDL_SCANCODE_AC_SEARCH"] = 268; 
    lua["key"]["SDL_SCANCODE_AC_HOME"] = 269;  
    lua["key"]["SDL_SCANCODE_AC_BACK"] = 270;  
    lua["key"]["SDL_SCANCODE_AC_FORWARD"] = 271;  
    lua["key"]["SDL_SCANCODE_AC_STOP"] = 272;    
    lua["key"]["SDL_SCANCODE_AC_REFRESH"] = 273;  ;
    lua["key"]["SDL_SCANCODE_AC_BOOKMARKS"] = 274; 
    lua["key"]["SDL_SCANCODE_BRIGHTNESSDOWN"] = 275;
    lua["key"]["SDL_SCANCODE_BRIGHTNESSUP"] = 276;
    lua["key"]["SDL_SCANCODE_DISPLAYSWITCH"] = 277;
    lua["key"]["SDL_SCANCODE_KBDILLUMTOGGLE"] = 278;
    lua["key"]["SDL_SCANCODE_KBDILLUMDOWN"] = 279;
    lua["key"]["SDL_SCANCODE_KBDILLUMUP"] = 280;
    lua["key"]["SDL_SCANCODE_EJECT"] = 281;
    lua["key"]["SDL_SCANCODE_SLEEP"] = 282;
    lua["key"]["SDL_SCANCODE_APP1"] = 283;
    lua["key"]["SDL_SCANCODE_APP2"] = 284;
    lua["key"]["SDL_SCANCODE_AUDIOREWIND"] = 285;
    lua["key"]["SDL_SCANCODE_AUDIOFASTFORWARD"] = 286;
    lua["key"]["SDL_SCANCODE_SOFTLEFT"] = 287;
    lua["key"]["SDL_SCANCODE_SOFTRIGHT"] = 288;
    lua["key"]["SDL_SCANCODE_CALL"] = 289;
    lua["key"]["SDL_SCANCODE_ENDCALL"] = 290;
    lua["key"]["SDL_NUM_SCANCODES"] = 512;
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

    auto mat4_multiply_overloads
        = sol::overload([](const glm::mat4& m1, const glm::vec4& v2) { return m1 * v2; },
            [](const glm::mat4& m1, const glm::mat4& m2) { return m1 * m2; });

    lua.new_usertype<glm::mat4>(
        "mat4",
        sol::call_constructor,
        sol::constructors<glm::mat4(float)>(),
        sol::meta_function::multiplication,
        mat4_multiply_overloads);

    lua.set_function("std_vector_vec3", []() -> std::vector<glm::vec3> { return {}; });
    lua.set_function("std_vector_vec4", []() -> std::vector<glm::vec4> { return {}; });
    lua.set_function("std_vector_mat4", []() -> std::vector<glm::mat4> { return {}; });
    lua.set_function("std_vector_vec2", []() -> std::vector<glm::vec2> { return {}; });
    lua.set_function("std_vector_float", []() -> std::vector<float> { return {}; });
    lua.set_function("std_vector_int", []() -> std::vector<int> { return {}; });
    lua.set_function("std_vector_uint", []() -> std::vector<uint32_t> { return {}; });
    lua.set_function("std_vector_vertex3d", []() -> std::vector<Vertex3D> { return {}; });
}

void BindMathFunctions(sol::state& lua)
{
    lua.set_function("distance", sol::overload([](glm::vec2& a, glm::vec2& b) { return glm::distance(a, b); }, [](glm::vec3& a, glm::vec3& b) { return glm::distance(a, b); }, [](glm::vec4& a, glm::vec4& b) { return glm::distance(a, b); }));
    lua.set_function("lerp", [](float a, float b, float t) { return std::lerp(a, b, t); });
    lua.set_function("clamp", sol::overload([](float value, float min, float max) { return std::clamp(value, min, max); }, [](double value, double min, double max) { return std::clamp(value, min, max); }, [](int value, int min, int max) { return std::clamp(value, min, max); }));

    lua.set_function("get_identity_matrix", []() -> glm::mat4 {
        {};
        return glm::identity<glm::mat4>();
    });


    lua.create_named_table(
        "jmath3D",
        "lookat",
        [](glm::vec3 eye, glm::vec3 center, glm::vec3 up) -> glm::mat4 {
            return glm::lookAt(eye, center, up);
        },
        "perspective",
        [](float fov, float aspect, float nearz, float farz) -> glm::mat4 {
            return glm::perspective(fov, aspect, nearz, farz);
        },
        "ortho",
        [](float left, float right, float bottom, float top, float nearz, float farz) -> glm::mat4 {
            return glm::ortho(left, right, bottom, top, nearz, farz);
        },

        "translate",
        [](glm::mat4 inmatrix, glm::vec3 invector) -> glm::mat4 {
            return glm::translate(inmatrix, invector);
        },

        "scale", [](glm::mat4 inmatrix, glm::vec3 invector) -> glm::mat4 {
            return glm::scale(inmatrix, invector);
        },
        "rotate", [](glm::mat4 inmatrix, float angle_rad, glm::vec3 invector) -> glm::mat4 {
            return glm::rotate(inmatrix, angle_rad, invector);
        },
        "rotate_deg", [](glm::mat4 inmatrix, float angle_deg, glm::vec3 invector) -> glm::mat4 {
            return glm::rotate(inmatrix, glm::radians(angle_deg), invector);
        },
        "normalize", [](glm::vec3 invec) {
            return glm::normalize(invec);
        },
        "magnitude", [](glm::vec3 invec) {
            return glm::length(invec);
        });
}

void GlobalAccessBindingsGenerator::CreateGlobalAccessBindings(Instance& i, Window& w, sol::state& l)
{
    auto r = l["global"].get_or_create<sol::table>();

    r["set_value"] = [this](std::string_view inView, GlobalDataType inType) {
        std::scoped_lock lock(mMutex);
    };
}

void NeuralBindings(sol::state& s) {
    using namespace Jkr::Neural;
	auto neural = s["neural"].get_or_create<sol::table>();
	neural.new_usertype<Neural::Network>(
		"network",
		sol::call_constructor,
		sol::factories(
			[&](std::vector<int>& inTopology) {
				return Neural::Network(inTopology);
			},
			[&](std::vector<int>& inTopology, Neural::real inLearningRate) {
				return Neural::Network(inTopology, inLearningRate);
			}
		),
		"weight_of_connection",
		[](Neural::Network& inNetwork, int inLayer, int inNeuron1, int inNeuron2) -> Neural::real
		{
			return inNetwork.mWeights[inLayer].coeff(inNeuron1, inNeuron2);
		},
		"value_of_neuron",
		[](Neural::Network& inNetwork, int inLayer, int inNeuron)
		{
			return inNetwork.mNeuronLayers[inLayer].coeff(inNeuron);
		},
		"get_layer_vector_float",
		[](Neural::Network& inNetwork, int inLayer) -> std::vector<float>
		{
			std::vector<float> outvec;
			auto& rowv = inNetwork.mNeuronLayers[inLayer];
			for (int i = 0; i < rowv.size(); i++)
			{
				outvec.push_back(rowv.coeff(i));
			}
			return outvec;
		},
		"propagate_forward",
		[](Neural::Network& inNetwork, std::vector<float> inFloat){
			rowV v(inFloat.size());
			for (int i = 0; i < inFloat.size(); i++)
			{
				v.coeffRef(i) = inFloat[i];
			}
			inNetwork.PropagateForward(v);
		},

		"propagate_backward_current",
		[](Neural::Network& inNetwork, std::vector<float> inFloat)
		{
			rowV v(inFloat.size());
			for (int i = 0; i < inFloat.size(); i++)
			{
				v.coeffRef(i) = inFloat[i];
			}
			inNetwork.PropagateBackward(v);
		}
	);
}
