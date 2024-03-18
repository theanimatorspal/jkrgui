#include "JkrLuaExe.hpp"

namespace JkrEXE {

void CreateKeyBindings(sol::state& s) {
          auto lua = s.new_enum("Key");
          lua["SDLK_RETURN"] = SDLK_RETURN;
          lua["SDLK_ESCAPE"] = SDLK_ESCAPE;
          lua["SDLK_BACKSPACE"] = SDLK_BACKSPACE;
          lua["SDLK_TAB"] = SDLK_TAB;
          lua["SDLK_SPACE"] = SDLK_SPACE;
          lua["SDLK_EXCLAIM"] = SDLK_EXCLAIM;
          lua["SDLK_QUOTEDBL"] = SDLK_QUOTEDBL;
          lua["SDLK_HASH"] = SDLK_HASH;
          lua["SDLK_PERCENT"] = SDLK_PERCENT;
          lua["SDLK_DOLLAR"] = SDLK_DOLLAR;
          lua["SDLK_AMPERSAND"] = SDLK_AMPERSAND;
          lua["SDLK_QUOTE"] = SDLK_QUOTE;
          lua["SDLK_LEFTPAREN"] = SDLK_LEFTPAREN;
          lua["SDLK_RIGHTPAREN"] = SDLK_RIGHTPAREN;
          lua["SDLK_ASTERISK"] = SDLK_ASTERISK;
          lua["SDLK_PLUS"] = SDLK_PLUS;
          lua["SDLK_COMMA"] = SDLK_COMMA;
          lua["SDLK_MINUS"] = SDLK_MINUS;
          lua["SDLK_PERIOD"] = SDLK_PERIOD;
          lua["SDLK_SLASH"] = SDLK_SLASH;
          lua["SDLK_0"] = SDLK_0;
          lua["SDLK_1"] = SDLK_1;
          lua["SDLK_2"] = SDLK_2;
          lua["SDLK_3"] = SDLK_3;
          lua["SDLK_4"] = SDLK_4;
          lua["SDLK_5"] = SDLK_5;
          lua["SDLK_6"] = SDLK_6;
          lua["SDLK_7"] = SDLK_7;
          lua["SDLK_8"] = SDLK_8;
          lua["SDLK_9"] = SDLK_9;
          lua["SDLK_COLON"] = SDLK_COLON;
          lua["SDLK_SEMICOLON"] = SDLK_SEMICOLON;
          lua["SDLK_LESS"] = SDLK_LESS;
          lua["SDLK_EQUALS"] = SDLK_EQUALS;
          lua["SDLK_GREATER"] = SDLK_GREATER;
          lua["SDLK_QUESTION"] = SDLK_QUESTION;
          lua["SDLK_AT"] = SDLK_AT;

          lua["SDLK_LEFTBRACKET"] = SDLK_LEFTBRACKET, lua["SDLK_BACKSLASH"] = SDLK_BACKSLASH;
          lua["SDLK_RIGHTBRACKET"] = SDLK_RIGHTBRACKET;
          lua["SDLK_CARET"] = SDLK_CARET;
          lua["SDLK_UNDERSCORE"] = SDLK_UNDERSCORE;
          lua["SDLK_BACKQUOTE"] = SDLK_BACKQUOTE;
          lua["SDLK_a"] = SDLK_a;
          lua["SDLK_b"] = SDLK_b;
          lua["SDLK_c"] = SDLK_c;
          lua["SDLK_d"] = SDLK_d;
          lua["SDLK_e"] = SDLK_e;
          lua["SDLK_f"] = SDLK_f;
          lua["SDLK_g"] = SDLK_g;
          lua["SDLK_h"] = SDLK_h;
          lua["SDLK_i"] = SDLK_i;
          lua["SDLK_j"] = SDLK_j;
          lua["SDLK_k"] = SDLK_k;
          lua["SDLK_l"] = SDLK_l;
          lua["SDLK_m"] = SDLK_m;
          lua["SDLK_n"] = SDLK_n;
          lua["SDLK_o"] = SDLK_o;
          lua["SDLK_p"] = SDLK_p;
          lua["SDLK_q"] = SDLK_q;
          lua["SDLK_r"] = SDLK_r;
          lua["SDLK_s"] = SDLK_s;
          lua["SDLK_t"] = SDLK_t;
          lua["SDLK_u"] = SDLK_u;
          lua["SDLK_v"] = SDLK_v;
          lua["SDLK_w"] = SDLK_w;
          lua["SDLK_x"] = SDLK_x;
          lua["SDLK_y"] = SDLK_y;
          lua["SDLK_z"] = SDLK_z;

          lua["SDLK_CAPSLOCK"] = SDLK_CAPSLOCK;

          lua["SDLK_F1"] = SDLK_F1;
          lua["SDLK_F2"] = SDLK_F2;
          lua["SDLK_F3"] = SDLK_F3;
          lua["SDLK_F4"] = SDLK_F4;
          lua["SDLK_F5"] = SDLK_F5;
          lua["SDLK_F6"] = SDLK_F6;
          lua["SDLK_F7"] = SDLK_F7;
          lua["SDLK_F8"] = SDLK_F8;
          lua["SDLK_F9"] = SDLK_F9;
          lua["SDLK_F10"] = SDLK_F10;
          lua["SDLK_F11"] = SDLK_F11;
          lua["SDLK_F12"] = SDLK_F12;

          lua["SDLK_PRINTSCREEN"] = SDLK_PRINTSCREEN;
          lua["SDLK_SCROLLLOCK"] = SDLK_SCROLLLOCK;
          lua["SDLK_PAUSE"] = SDLK_PAUSE;
          lua["SDLK_INSERT"] = SDLK_INSERT;
          lua["SDLK_HOME"] = SDLK_HOME;
          lua["SDLK_PAGEUP"] = SDLK_PAGEUP;
          lua["SDLK_DELETE"] = SDLK_DELETE;
          lua["SDLK_END"] = SDLK_END;
          lua["SDLK_PAGEDOWN"] = SDLK_PAGEDOWN;
          lua["SDLK_RIGHT"] = SDLK_RIGHT;
          lua["SDLK_LEFT"] = SDLK_LEFT;
          lua["SDLK_DOWN"] = SDLK_DOWN;
          lua["SDLK_UP"] = SDLK_UP;

          lua["SDLK_NUMLOCKCLEAR"] = SDLK_NUMLOCKCLEAR;
          lua["SDLK_KP_DIVIDE"] = SDLK_KP_DIVIDE;
          lua["SDLK_KP_MULTIPLY"] = SDLK_KP_MULTIPLY;
          lua["SDLK_KP_MINUS"] = SDLK_KP_MINUS;
          lua["SDLK_KP_PLUS"] = SDLK_KP_PLUS;
          lua["SDLK_KP_ENTER"] = SDLK_KP_ENTER;
          lua["SDLK_KP_1"] = SDLK_KP_1;
          lua["SDLK_KP_2"] = SDLK_KP_2;
          lua["SDLK_KP_3"] = SDLK_KP_3;
          lua["SDLK_KP_4"] = SDLK_KP_4;
          lua["SDLK_KP_5"] = SDLK_KP_5;
          lua["SDLK_KP_6"] = SDLK_KP_6;
          lua["SDLK_KP_7"] = SDLK_KP_7;
          lua["SDLK_KP_8"] = SDLK_KP_8;
          lua["SDLK_KP_9"] = SDLK_KP_9;
          lua["SDLK_KP_0"] = SDLK_KP_0;
          lua["SDLK_KP_PERIOD"] = SDLK_KP_PERIOD;

          lua["SDLK_APPLICATION"] = SDLK_APPLICATION;
          lua["SDLK_POWER"] = SDLK_POWER;
          lua["SDLK_KP_EQUALS"] = SDLK_KP_EQUALS;
          lua["SDLK_F13"] = SDLK_F13;
          lua["SDLK_F14"] = SDLK_F14;
          lua["SDLK_F15"] = SDLK_F15;
          lua["SDLK_F16"] = SDLK_F16;
          lua["SDLK_F17"] = SDLK_F17;
          lua["SDLK_F18"] = SDLK_F18;
          lua["SDLK_F19"] = SDLK_F19;
          lua["SDLK_F20"] = SDLK_F20;
          lua["SDLK_F21"] = SDLK_F21;
          lua["SDLK_F22"] = SDLK_F22;
          lua["SDLK_F23"] = SDLK_F23;
          lua["SDLK_F24"] = SDLK_F24;
          lua["SDLK_EXECUTE"] = SDLK_EXECUTE;
          lua["SDLK_HELP"] = SDLK_HELP;
          lua["SDLK_MENU"] = SDLK_MENU;
          lua["SDLK_SELECT"] = SDLK_SELECT;
          lua["SDLK_STOP"] = SDLK_STOP;
          lua["SDLK_AGAIN"] = SDLK_AGAIN;
          lua["SDLK_UNDO"] = SDLK_UNDO;
          lua["SDLK_CUT"] = SDLK_CUT;
          lua["SDLK_COPY"] = SDLK_COPY;
          lua["SDLK_PASTE"] = SDLK_PASTE;
          lua["SDLK_FIND"] = SDLK_FIND;
          lua["SDLK_MUTE"] = SDLK_MUTE;
          lua["SDLK_VOLUMEUP"] = SDLK_VOLUMEUP;
          lua["SDLK_VOLUMEDOWN"] = SDLK_VOLUMEDOWN;
          lua["SDLK_KP_COMMA"] = SDLK_KP_COMMA;
          lua["SDLK_KP_EQUALSAS400"] = SDLK_KP_EQUALSAS400;

          lua["SDLK_ALTERASE"] = SDLK_ALTERASE;
          lua["SDLK_SYSREQ"] = SDLK_SYSREQ;
          lua["SDLK_CANCEL"] = SDLK_CANCEL;
          lua["SDLK_CLEAR"] = SDLK_CLEAR;
          lua["SDLK_PRIOR"] = SDLK_PRIOR;
          lua["SDLK_RETURN2"] = SDLK_RETURN2;
          lua["SDLK_SEPARATOR"] = SDLK_SEPARATOR;
          lua["SDLK_OUT"] = SDLK_OUT;
          lua["SDLK_OPER"] = SDLK_OPER;
          lua["SDLK_CLEARAGAIN"] = SDLK_CLEARAGAIN;
          lua["SDLK_CRSEL"] = SDLK_CRSEL;
          lua["SDLK_EXSEL"] = SDLK_EXSEL;

          lua["SDLK_KP_00"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_00);
          lua["SDLK_KP_000"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_000);
          lua["SDLK_THOUSANDSSEPARATOR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_THOUSANDSSEPARATOR);
          lua["SDLK_DECIMALSEPARATOR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DECIMALSEPARATOR);
          lua["SDLK_CURRENCYUNIT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYUNIT);
          lua["SDLK_CURRENCYSUBUNIT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYSUBUNIT);
          lua["SDLK_KP_LEFTPAREN"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTPAREN);
          lua["SDLK_KP_RIGHTPAREN"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTPAREN);
          lua["SDLK_KP_LEFTBRACE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTBRACE);
          lua["SDLK_KP_RIGHTBRACE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTBRACE);
          lua["SDLK_KP_TAB"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_TAB);
          lua["SDLK_KP_BACKSPACE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BACKSPACE);
          lua["SDLK_KP_A"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_A);
          lua["SDLK_KP_B"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_B);
          lua["SDLK_KP_C"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_C);
          lua["SDLK_KP_D"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_D);
          lua["SDLK_KP_E"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_E);
          lua["SDLK_KP_F"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_F);
          lua["SDLK_KP_XOR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_XOR);
          lua["SDLK_KP_POWER"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_POWER);
          lua["SDLK_KP_PERCENT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERCENT);
          lua["SDLK_KP_LESS"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LESS);
          lua["SDLK_KP_GREATER"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_GREATER);
          lua["SDLK_KP_AMPERSAND"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AMPERSAND);
          lua["SDLK_KP_DBLAMPERSAND"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLAMPERSAND);
          lua["SDLK_KP_VERTICALBAR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_VERTICALBAR);
          lua["SDLK_KP_DBLVERTICALBAR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLVERTICALBAR);
          lua["SDLK_KP_COLON"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_COLON);
          lua["SDLK_KP_HASH"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HASH);
          lua["SDLK_KP_SPACE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_SPACE);
          lua["SDLK_KP_AT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AT);
          lua["SDLK_KP_EXCLAM"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EXCLAM);
          lua["SDLK_KP_MEMSTORE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSTORE);
          lua["SDLK_KP_MEMRECALL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMRECALL);
          lua["SDLK_KP_MEMCLEAR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMCLEAR);
          lua["SDLK_KP_MEMADD"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMADD);
          lua["SDLK_KP_MEMSUBTRACT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSUBTRACT);
          lua["SDLK_KP_MEMMULTIPLY"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMMULTIPLY);
          lua["SDLK_KP_MEMDIVIDE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMDIVIDE);
          lua["SDLK_KP_PLUSMINUS"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUSMINUS);
          lua["SDLK_KP_CLEAR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEAR);
          lua["SDLK_KP_CLEARENTRY"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEARENTRY);
          lua["SDLK_KP_BINARY"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BINARY);
          lua["SDLK_KP_OCTAL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_OCTAL);
          lua["SDLK_KP_DECIMAL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DECIMAL);
          lua["SDLK_KP_HEXADECIMAL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HEXADECIMAL);

          lua["SDLK_LCTRL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LCTRL);
          lua["SDLK_LSHIFT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LSHIFT);
          lua["SDLK_LALT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LALT);
          lua["SDLK_LGUI"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LGUI);
          lua["SDLK_RCTRL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RCTRL);
          lua["SDLK_RSHIFT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RSHIFT);
          lua["SDLK_RALT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RALT);
          lua["SDLK_RGUI"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RGUI);

          lua["SDLK_MODE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MODE);

          lua["SDLK_AUDIONEXT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIONEXT);
          lua["SDLK_AUDIOPREV"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOPREV);
          lua["SDLK_AUDIOSTOP"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOSTOP);
          lua["SDLK_AUDIOPLAY"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOPLAY);
          lua["SDLK_AUDIOMUTE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOMUTE);
          lua["SDLK_MEDIASELECT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIASELECT);
          lua["SDLK_WWW"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_WWW);
          lua["SDLK_MAIL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MAIL);
          lua["SDLK_CALCULATOR"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CALCULATOR);
          lua["SDLK_COMPUTER"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_COMPUTER);
          lua["SDLK_AC_SEARCH"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_SEARCH);
          lua["SDLK_AC_HOME"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_HOME);
          lua["SDLK_AC_BACK"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BACK);
          lua["SDLK_AC_FORWARD"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_FORWARD);
          lua["SDLK_AC_STOP"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_STOP);
          lua["SDLK_AC_REFRESH"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_REFRESH);
          lua["SDLK_AC_BOOKMARKS"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BOOKMARKS);

          lua["SDLK_BRIGHTNESSDOWN"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_BRIGHTNESSDOWN);
          lua["SDLK_BRIGHTNESSUP"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_BRIGHTNESSUP);
          lua["SDLK_DISPLAYSWITCH"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DISPLAYSWITCH);
          lua["SDLK_KBDILLUMTOGGLE"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KBDILLUMTOGGLE);
          lua["SDLK_KBDILLUMDOWN"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KBDILLUMDOWN);
          lua["SDLK_KBDILLUMUP"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KBDILLUMUP);
          lua["SDLK_EJECT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EJECT);
          lua["SDLK_SLEEP"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SLEEP);
          lua["SDLK_APP1"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP1);
          lua["SDLK_APP2"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP2);

          lua["SDLK_AUDIOREWIND"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOREWIND);
          lua["SDLK_AUDIOFASTFORWARD"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOFASTFORWARD);

          lua["SDLK_SOFTLEFT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SOFTLEFT);
          lua["SDLK_SOFTRIGHT"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SOFTRIGHT);
          lua["SDLK_CALL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CALL);
          lua["SDLK_ENDCALL"] = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_ENDCALL);

          lua["SDL_SCANCODE_UNKNOWN"] = SDL_SCANCODE_UNKNOWN,

          /**
           *  \name Usage page 0x07
           *
           *  These values are from usage page 0x07 (USB keyboard page).
           */
           /* @{ */

           lua["SDL_SCANCODE_A"] = 4;
          lua["SDL_SCANCODE_B"] = 5;
          lua["SDL_SCANCODE_C"] = 6;
          lua["SDL_SCANCODE_D"] = 7;
          lua["SDL_SCANCODE_E"] = 8;
          lua["SDL_SCANCODE_F"] = 9;
          lua["SDL_SCANCODE_G"] = 10;
          lua["SDL_SCANCODE_H"] = 11;
          lua["SDL_SCANCODE_I"] = 12;
          lua["SDL_SCANCODE_J"] = 13;
          lua["SDL_SCANCODE_K"] = 14;
          lua["SDL_SCANCODE_L"] = 15;
          lua["SDL_SCANCODE_M"] = 16;
          lua["SDL_SCANCODE_N"] = 17;
          lua["SDL_SCANCODE_O"] = 18;
          lua["SDL_SCANCODE_P"] = 19;
          lua["SDL_SCANCODE_Q"] = 20;
          lua["SDL_SCANCODE_R"] = 21;
          lua["SDL_SCANCODE_S"] = 22;
          lua["SDL_SCANCODE_T"] = 23;
          lua["SDL_SCANCODE_U"] = 24;
          lua["SDL_SCANCODE_V"] = 25;
          lua["SDL_SCANCODE_W"] = 26;
          lua["SDL_SCANCODE_X"] = 27;
          lua["SDL_SCANCODE_Y"] = 28;
          lua["SDL_SCANCODE_Z"] = 29;

          lua["SDL_SCANCODE_1"] = 30;
          lua["SDL_SCANCODE_2"] = 31;
          lua["SDL_SCANCODE_3"] = 32;
          lua["SDL_SCANCODE_4"] = 33;
          lua["SDL_SCANCODE_5"] = 34;
          lua["SDL_SCANCODE_6"] = 35;
          lua["SDL_SCANCODE_7"] = 36;
          lua["SDL_SCANCODE_8"] = 37;
          lua["SDL_SCANCODE_9"] = 38;
          lua["SDL_SCANCODE_0"] = 39;

          lua["SDL_SCANCODE_RETURN"] = 40;
          lua["SDL_SCANCODE_ESCAPE"] = 41;
          lua["SDL_SCANCODE_BACKSPACE"] = 42;
          lua["SDL_SCANCODE_TAB"] = 43;
          lua["SDL_SCANCODE_SPACE"] = 44;

          lua["SDL_SCANCODE_MINUS"] = 45;
          lua["SDL_SCANCODE_EQUALS"] = 46;
          lua["SDL_SCANCODE_LEFTBRACKET"] = 47;
          lua["SDL_SCANCODE_RIGHTBRACKET"] = 48;
          lua["SDL_SCANCODE_BACKSLASH"] = 49; /**< Located at the lower left of the retur;
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
          lua["SDL_SCANCODE_NONUSHASH"] = 50; /**< ISO USB keyboards actually use this cod;
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
          lua["SDL_SCANCODE_SEMICOLON"] = 51;
          lua["SDL_SCANCODE_APOSTROPHE"] = 52;
          lua["SDL_SCANCODE_GRAVE"] = 53; /**< Located in the top left corner (on both ANS;
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
          lua["SDL_SCANCODE_COMMA"] = 54;
          lua["SDL_SCANCODE_PERIOD"] = 55;
          lua["SDL_SCANCODE_SLASH"] = 56;

          lua["SDL_SCANCODE_CAPSLOCK"] = 57;

          lua["SDL_SCANCODE_F1"] = 58;
          lua["SDL_SCANCODE_F2"] = 59;
          lua["SDL_SCANCODE_F3"] = 60;
          lua["SDL_SCANCODE_F4"] = 61;
          lua["SDL_SCANCODE_F5"] = 62;
          lua["SDL_SCANCODE_F6"] = 63;
          lua["SDL_SCANCODE_F7"] = 64;
          lua["SDL_SCANCODE_F8"] = 65;
          lua["SDL_SCANCODE_F9"] = 66;
          lua["SDL_SCANCODE_F10"] = 67;
          lua["SDL_SCANCODE_F11"] = 68;
          lua["SDL_SCANCODE_F12"] = 69;

          lua["SDL_SCANCODE_PRINTSCREEN"] = 70;
          lua["SDL_SCANCODE_SCROLLLOCK"] = 71;
          lua["SDL_SCANCODE_PAUSE"] = 72;
          lua["SDL_SCANCODE_INSERT"] = 73, /**< insert on PC, help on some Mac keyboards (bu;
                    does send code 73, not 117) */
           lua["SDL_SCANCODE_HOME"] = 74;
          lua["SDL_SCANCODE_PAGEUP"] = 75;
          lua["SDL_SCANCODE_DELETE"] = 76;
          lua["SDL_SCANCODE_END"] = 77;
          lua["SDL_SCANCODE_PAGEDOWN"] = 78;
          lua["SDL_SCANCODE_RIGHT"] = 79;
          lua["SDL_SCANCODE_LEFT"] = 80;
          lua["SDL_SCANCODE_DOWN"] = 81;
          lua["SDL_SCANCODE_UP"] = 82;

          lua["SDL_SCANCODE_NUMLOCKCLEAR"] = 83, /**< num lock on PC, clear on Mac keyboard;
                                                  */
           lua["SDL_SCANCODE_KP_DIVIDE"] = 84;
          lua["SDL_SCANCODE_KP_MULTIPLY"] = 85;
          lua["SDL_SCANCODE_KP_MINUS"] = 86;
          lua["SDL_SCANCODE_KP_PLUS"] = 87;
          lua["SDL_SCANCODE_KP_ENTER"] = 88;
          lua["SDL_SCANCODE_KP_1"] = 89;
          lua["SDL_SCANCODE_KP_2"] = 90;
          lua["SDL_SCANCODE_KP_3"] = 91;
          lua["SDL_SCANCODE_KP_4"] = 92;
          lua["SDL_SCANCODE_KP_5"] = 93;
          lua["SDL_SCANCODE_KP_6"] = 94;
          lua["SDL_SCANCODE_KP_7"] = 95;
          lua["SDL_SCANCODE_KP_8"] = 96;
          lua["SDL_SCANCODE_KP_9"] = 97;
          lua["SDL_SCANCODE_KP_0"] = 98;
          lua["SDL_SCANCODE_KP_PERIOD"] = 99;

          lua["SDL_SCANCODE_NONUSBACKSLASH"] = 100; /**< This is the additional key that IS;
                                                     *   keyboards have over ANSI ones,
                                                     *   located between left shift and Y.
                                                     *   Produces GRAVE ACCENT and TILDE in a
                                                     *   US or UK Mac layout, REVERSE SOLIDUS
                                                     *   (backslash) and VERTICAL LINE in a
                                                     *   US or UK Windows layout, and
                                                     *   LESS-THAN SIGN and GREATER-THAN SIGN
                                                     *   in a Swiss German, German, or French
                                                     *   layout. */
          lua["SDL_SCANCODE_APPLICATION"] = 101;
          lua["SDL_SCANCODE_POWER"] = 102;

          lua["SDL_SCANCODE_KP_EQUALS"] = 103;
          lua["SDL_SCANCODE_F13"] = 104;
          lua["SDL_SCANCODE_F14"] = 105;
          lua["SDL_SCANCODE_F15"] = 106;
          lua["SDL_SCANCODE_F16"] = 107;
          lua["SDL_SCANCODE_F17"] = 108;
          lua["SDL_SCANCODE_F18"] = 109;
          lua["SDL_SCANCODE_F19"] = 110;
          lua["SDL_SCANCODE_F20"] = 111;
          lua["SDL_SCANCODE_F21"] = 112;
          lua["SDL_SCANCODE_F22"] = 113;
          lua["SDL_SCANCODE_F23"] = 114;
          lua["SDL_SCANCODE_F24"] = 115;
          lua["SDL_SCANCODE_EXECUTE"] = 116;
          lua["SDL_SCANCODE_HELP"] = 117, lua["SDL_SCANCODE_MENU"] = 118, lua["SDL_SCANCODE_SELECT"] = 119;
          lua["SDL_SCANCODE_STOP"] = 120, lua["SDL_SCANCODE_AGAIN"] = 121, lua["SDL_SCANCODE_UNDO"] = 122;
          lua["SDL_SCANCODE_CUT"] = 123;
          lua["SDL_SCANCODE_COPY"] = 124,   /**< AC Copy **/
           lua["SDL_SCANCODE_PASTE"] = 125; /**< AC Paste **/
          lua["SDL_SCANCODE_FIND"] = 126;   /**< AC Find **/
          lua["SDL_SCANCODE_MUTE"] = 127;
          lua["SDL_SCANCODE_VOLUMEUP"] = 128;
          lua["SDL_SCANCODE_VOLUMEDOWN"] = 129;
          /* not sure whether there's a reason to enable these */
          /*     SDL_SCANCODE_LOCKINGCAPSLOCK = 130,  */
          /*     SDL_SCANCODE_LOCKINGNUMLOCK = 131, */
          /*     SDL_SCANCODE_LOCKINGSCROLLLOCK = 132, */
          lua["SDL_SCANCODE_KP_COMMA"] = 133;
          lua["SDL_SCANCODE_KP_EQUALSAS400"] = 134;

          lua["SDL_SCANCODE_INTERNATIONAL1"] = 135;
          lua["SDL_SCANCODE_INTERNATIONAL2"] = 136;
          lua["SDL_SCANCODE_INTERNATIONAL3"] = 137;
          lua["SDL_SCANCODE_INTERNATIONAL4"] = 138;
          lua["SDL_SCANCODE_INTERNATIONAL5"] = 139;
          lua["SDL_SCANCODE_INTERNATIONAL6"] = 140;
          lua["SDL_SCANCODE_INTERNATIONAL7"] = 141;
          lua["SDL_SCANCODE_INTERNATIONAL8"] = 142;
          lua["SDL_SCANCODE_INTERNATIONAL9"] = 143;
          lua["SDL_SCANCODE_LANG1"] = 144;
          lua["SDL_SCANCODE_LANG2"] = 145;
          lua["SDL_SCANCODE_LANG3"] = 14;
          lua["SDL_SCANCODE_LANG4"] = 147;
          lua["SDL_SCANCODE_LANG5"] = 148;
          lua["SDL_SCANCODE_LANG6"] = 149;
          lua["SDL_SCANCODE_LANG7"] = 150;
          lua["SDL_SCANCODE_LANG8"] = 151;
          lua["SDL_SCANCODE_LANG9"] = 152;

          lua["SDL_SCANCODE_ALTERASE"] = 153;
          lua["SDL_SCANCODE_SYSREQ"] = 154;
          lua["SDL_SCANCODE_CANCEL"] = 155;
          lua["SDL_SCANCODE_CLEAR"] = 156;
          lua["SDL_SCANCODE_PRIOR"] = 157;
          lua["SDL_SCANCODE_RETURN2"] = 158;
          lua["SDL_SCANCODE_SEPARATOR"] = 159;
          lua["SDL_SCANCODE_OUT"] = 160;
          lua["SDL_SCANCODE_OPER"] = 161;
          lua["SDL_SCANCODE_CLEARAGAIN"] = 162;
          lua["SDL_SCANCODE_CRSEL"] = 163;
          lua["SDL_SCANCODE_EXSEL"] = 164;

          lua["SDL_SCANCODE_KP_00"] = 176;
          lua["SDL_SCANCODE_KP_000"] = 177;
          lua["SDL_SCANCODE_THOUSANDSSEPARATOR"] = 178;
          lua["SDL_SCANCODE_DECIMALSEPARATOR"] = 179;
          lua["SDL_SCANCODE_CURRENCYUNIT"] = 180;
          lua["SDL_SCANCODE_CURRENCYSUBUNIT"] = 181;
          lua["SDL_SCANCODE_KP_LEFTPAREN"] = 182;
          lua["SDL_SCANCODE_KP_RIGHTPAREN"] = 183;
          lua["SDL_SCANCODE_KP_LEFTBRACE"] = 184;
          lua["SDL_SCANCODE_KP_RIGHTBRACE"] = 185;
          lua["SDL_SCANCODE_KP_TAB"] = 186;
          lua["SDL_SCANCODE_KP_BACKSPACE"] = 187;
          lua["SDL_SCANCODE_KP_A"] = 188;
          lua["SDL_SCANCODE_KP_B"] = 189;
          lua["SDL_SCANCODE_KP_C"] = 190;
          lua["SDL_SCANCODE_KP_D"] = 191;
          lua["SDL_SCANCODE_KP_E"] = 192;
          lua["SDL_SCANCODE_KP_F"] = 193;
          lua["SDL_SCANCODE_KP_XOR"] = 194;
          lua["SDL_SCANCODE_KP_POWER"] = 195;
          lua["SDL_SCANCODE_KP_PERCENT"] = 196;
          lua["SDL_SCANCODE_KP_LESS"] = 197;
          lua["SDL_SCANCODE_KP_GREATER"] = 198;
          lua["SDL_SCANCODE_KP_AMPERSAND"] = 199;
          lua["SDL_SCANCODE_KP_DBLAMPERSAND"] = 200;
          lua["SDL_SCANCODE_KP_VERTICALBAR"] = 201;
          lua["SDL_SCANCODE_KP_DBLVERTICALBAR"] = 202;
          lua["SDL_SCANCODE_KP_COLON"] = 203;
          lua["SDL_SCANCODE_KP_HASH"] = 204;
          lua["SDL_SCANCODE_KP_SPACE"] = 205;
          lua["SDL_SCANCODE_KP_AT"] = 206;
          lua["SDL_SCANCODE_KP_EXCLAM"] = 207;
          lua["SDL_SCANCODE_KP_MEMSTORE"] = 208;
          lua["SDL_SCANCODE_KP_MEMRECALL"] = 209;
          lua["SDL_SCANCODE_KP_MEMCLEAR"] = 210;
          lua["SDL_SCANCODE_KP_MEMADD"] = 211;
          lua["SDL_SCANCODE_KP_MEMSUBTRACT"] = 212;
          lua["SDL_SCANCODE_KP_MEMMULTIPLY"] = 213;
          lua["SDL_SCANCODE_KP_MEMDIVIDE"] = 214;
          lua["SDL_SCANCODE_KP_PLUSMINUS"] = 215;
          lua["SDL_SCANCODE_KP_CLEAR"] = 216;
          lua["SDL_SCANCODE_KP_CLEARENTRY"] = 217;
          lua["SDL_SCANCODE_KP_BINARY"] = 218;
          lua["SDL_SCANCODE_KP_OCTAL"] = 219;
          lua["SDL_SCANCODE_KP_DECIMAL"] = 220;
          lua["SDL_SCANCODE_KP_HEXADECIMAL"] = 221;

          lua["SDL_SCANCODE_LCTRL"] = 224;
          lua["SDL_SCANCODE_LSHIFT"] = 225;
          lua["SDL_SCANCODE_LALT"] = 226;
          lua["SDL_SCANCODE_LGUI"] = 227;
          lua["SDL_SCANCODE_RCTRL"] = 228;
          lua["SDL_SCANCODE_RSHIFT"] = 229;
          lua["SDL_SCANCODE_RALT"] = 230;
          lua["SDL_SCANCODE_RGUI"] = 231;

          lua["SDL_SCANCODE_MODE"] = 257;

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

          lua["SDL_SCANCODE_AUDIONEXT"] = 258;
          lua["SDL_SCANCODE_AUDIOPREV"] = 259;
          lua["SDL_SCANCODE_AUDIOSTOP"] = 260;
          lua["SDL_SCANCODE_AUDIOPLAY"] = 261;
          lua["SDL_SCANCODE_AUDIOMUTE"] = 262;
          lua["SDL_SCANCODE_MEDIASELECT"] = 263;
          lua["SDL_SCANCODE_WWW"] = 264;
          lua["SDL_SCANCODE_MAIL"] = 265;
          lua["SDL_SCANCODE_CALCULATOR"] = 266;
          lua["SDL_SCANCODE_COMPUTER"] = 267;
          lua["SDL_SCANCODE_AC_SEARCH"] = 268;
          lua["SDL_SCANCODE_AC_HOME"] = 269;
          lua["SDL_SCANCODE_AC_BACK"] = 270;
          lua["SDL_SCANCODE_AC_FORWARD"] = 271;
          lua["SDL_SCANCODE_AC_STOP"] = 272;
          lua["SDL_SCANCODE_AC_REFRESH"] = 273;
          ;
          lua["SDL_SCANCODE_AC_BOOKMARKS"] = 274;
          lua["SDL_SCANCODE_BRIGHTNESSDOWN"] = 275;
          lua["SDL_SCANCODE_BRIGHTNESSUP"] = 276;
          lua["SDL_SCANCODE_DISPLAYSWITCH"] = 277;
          lua["SDL_SCANCODE_KBDILLUMTOGGLE"] = 278;
          lua["SDL_SCANCODE_KBDILLUMDOWN"] = 279;
          lua["SDL_SCANCODE_KBDILLUMUP"] = 280;
          lua["SDL_SCANCODE_EJECT"] = 281;
          lua["SDL_SCANCODE_SLEEP"] = 282;
          lua["SDL_SCANCODE_APP1"] = 283;
          lua["SDL_SCANCODE_APP2"] = 284;
          lua["SDL_SCANCODE_AUDIOREWIND"] = 285;
          lua["SDL_SCANCODE_AUDIOFASTFORWARD"] = 286;
          lua["SDL_SCANCODE_SOFTLEFT"] = 287;
          lua["SDL_SCANCODE_SOFTRIGHT"] = 288;
          lua["SDL_SCANCODE_CALL"] = 289;
          lua["SDL_SCANCODE_ENDCALL"] = 290;
          lua["SDL_NUM_SCANCODES"] = 512;
}

} // namespace JkrEXE