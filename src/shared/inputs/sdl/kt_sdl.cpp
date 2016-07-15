#include "stdafx.h"
#include "Const.h"
#include "kt_sdl.h"
#include <SDL2/SDL.h>

#define KEY(s, a, b) { s, 0, a, a, b, b },

typedef struct {
    const char *string;
    int base;
    int lgck_start;
    int lgck_end;
    int host_start;
    int host_end;
} key_match;

const key_match matches[] = {
    {"%c", 'A', lgck::Key::A,      lgck::Key::Z,     SDLK_a,  SDLK_z  },
    {"%c", '0', lgck::Key::Num0,   lgck::Key::Num9,  SDLK_0,  SDLK_9  },
    {"F%d", 1,  lgck::Key::F1,     lgck::Key::F12,   SDLK_F1, SDLK_F12 },
    // TODO: check num pad keys
    KEY ("ESCAPE", lgck::Key::Escape, SDLK_ESCAPE)
    KEY ("CTRL", lgck::Key::LControl, SDLK_LCTRL)
    KEY ("SHIFT", lgck::Key::LShift, SDLK_LSHIFT)
    KEY ("ALT", lgck::Key::LAlt, SDLK_LALT )
    KEY ("RCTRL", lgck::Key::RControl, SDLK_RCTRL)
    KEY ("RSHIFT", lgck::Key::RShift, SDLK_LSHIFT)
    KEY ("RALT", lgck::Key::RAlt, SDLK_LALT )
    KEY ("MENU", lgck::Key::Menu, SDLK_MENU)
    KEY ("LBRACKET", lgck::Key::LBracket, SDLK_LEFTBRACKET)
    KEY ("RBRACKET", lgck::Key::RBracket, SDLK_RIGHTBRACKET)
    KEY ("SEMICOLON", lgck::Key::SemiColon, SDLK_SEMICOLON)
    KEY ("COMMA", lgck::Key::Comma, SDLK_COMMA) // ???
    KEY ("PERIOD", lgck::Key::Period, SDLK_PERIOD)
    KEY ("QUOTE", lgck::Key::Quote, SDLK_QUOTEDBL) // ???
    KEY ("SLASH", lgck::Key::Slash, SDLK_SLASH)
    KEY ("BACKSLASH", lgck::Key::BackSlash, SDLK_BACKSLASH)
    //KEY ("TILDE", lgck::Key::Tilde, ) // ???
    KEY ("EQUAL", lgck::Key::Equal, SDLK_EQUALS)
    KEY ("MINUS", lgck::Key::Dash, SDLK_MINUS)
    KEY ("SPACE", lgck::Key::Space, SDLK_SPACE )
    KEY ("RETURN", lgck::Key::Return, SDLK_RETURN)
    KEY ("BACKSPACE", lgck::Key::Back, SDLK_BACKSPACE) //???
    KEY ("TAB", lgck::Key::Tab, SDLK_TAB) //???
    KEY ("PAGEUP", lgck::Key::PageUp, SDLK_PAGEUP)
    KEY ("PAGEDOWN",lgck::Key::PageDown, SDLK_PAGEDOWN)
    KEY ("END", lgck::Key::End, SDLK_END)
    KEY ("HOME", lgck::Key::Home, SDLK_HOME)
    KEY ("INSERT", lgck::Key::Insert, SDLK_INSERT)
    KEY ("DELETE", lgck::Key::Delete, SDLK_DELETE)
    KEY ("PLUS", lgck::Key::Add, SDLK_PLUS)
    KEY ("MINUS", lgck::Key::Subtract, SDLK_MINUS)
    KEY ("MULTIPLY", lgck::Key::Multiply, SDLK_ASTERISK)
    KEY ("DIVIDE", lgck::Key::Divide, SDLK_SLASH)
    KEY ("LEFT", lgck::Key::Left, SDLK_LEFT)
    KEY ("RIGHT", lgck::Key::Right, SDLK_RIGHT)
    KEY ("UP", lgck::Key::Up, SDLK_UP)
    KEY ("DOWN", lgck::Key::Down, SDLK_DOWN)
    KEY ("PAUSE", lgck::Key::Pause, SDLK_PAUSE)
    {"", 0, -1, -1, -1, -1}
};

CKeyTranslator::CKeyTranslator()
{

}

CKeyTranslator::~CKeyTranslator()
{

}

int CKeyTranslator::translate(int key)
{
    for (int i = 0; matches[i].host_start != -1; ++i) {
        if (key >= matches[i].host_start &&
                key <= matches[i].host_end) {
            return matches[i].lgck_start + key - matches[i].host_start;
        }
    }
    return -1;
}
