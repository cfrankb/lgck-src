#include "stdafx.h"
#include "Const.h"
#include "kt_sfml.h"
#include <SFML/Window.hpp>

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
    {"%c", 'A', lgck::Key::A,      lgck::Key::Z,     sf::Keyboard::A,  sf::Keyboard::Z  },
    {"%c", '0', lgck::Key::Num0,   lgck::Key::Num9,  sf::Keyboard::Num0,  sf::Keyboard::Num9  },
    {"F%d", 1,  lgck::Key::F1,     lgck::Key::F12,   sf::Keyboard::F1, sf::Keyboard::F12 },
    // TODO: check num pad keys
    KEY ("ESCAPE", lgck::Key::Escape, sf::Keyboard::Escape)
    KEY ("CTRL", lgck::Key::LControl, sf::Keyboard::LControl)
    KEY ("SHIFT", lgck::Key::LShift, sf::Keyboard::LShift)
    KEY ("ALT", lgck::Key::LAlt, sf::Keyboard::LAlt )
    KEY ("RCTRL", lgck::Key::RControl, sf::Keyboard::RControl)
    KEY ("RSHIFT", lgck::Key::RShift, sf::Keyboard::RShift)
    KEY ("RALT", lgck::Key::RAlt, sf::Keyboard::LAlt )
    KEY ("MENU", lgck::Key::Menu, sf::Keyboard::Menu)
    KEY ("LBRACKET", lgck::Key::LBracket, sf::Keyboard::LBracket)
    KEY ("RBRACKET", lgck::Key::RBracket, sf::Keyboard::RBracket)
    KEY ("SEMICOLON", lgck::Key::SemiColon, sf::Keyboard::SemiColon)
    KEY ("COMMA", lgck::Key::Comma, sf::Keyboard::Comma) // ???
    KEY ("PERIOD", lgck::Key::Period, sf::Keyboard::Period)
    KEY ("QUOTE", lgck::Key::Quote, sf::Keyboard::Quote) // ???
    KEY ("SLASH", lgck::Key::Slash, sf::Keyboard::Slash)
    KEY ("BACKSLASH", lgck::Key::BackSlash, sf::Keyboard::BackSlash)
    KEY ("TILDE", lgck::Key::Tilde, sf::Keyboard::Tilde) // ???
    KEY ("EQUAL", lgck::Key::Equal, sf::Keyboard::Equal)
    KEY ("MINUS", lgck::Key::Dash, sf::Keyboard::Dash)
    KEY ("SPACE", lgck::Key::Space, sf::Keyboard::Space )
    KEY ("RETURN", lgck::Key::Return, sf::Keyboard::Return)
    KEY ("BACKSPACE", lgck::Key::Back, sf::Keyboard::BackSpace) //???
    KEY ("TAB", lgck::Key::Tab, sf::Keyboard::Tab) //???
    KEY ("PAGEUP", lgck::Key::PageUp, sf::Keyboard::PageUp)
    KEY ("PAGEDOWN",lgck::Key::PageDown, sf::Keyboard::PageDown)
    KEY ("END", lgck::Key::End, sf::Keyboard::End)
    KEY ("HOME", lgck::Key::Home, sf::Keyboard::Home)
    KEY ("INSERT", lgck::Key::Insert, sf::Keyboard::Insert)
    KEY ("DELETE", lgck::Key::Delete, sf::Keyboard::Delete)
    KEY ("PLUS", lgck::Key::Add, sf::Keyboard::Add)
    KEY ("MINUS", lgck::Key::Subtract, sf::Keyboard::Subtract)
    KEY ("MULTIPLY", lgck::Key::Multiply, sf::Keyboard::Multiply)
    KEY ("DIVIDE", lgck::Key::Divide, sf::Keyboard::Divide)
    KEY ("LEFT", lgck::Key::Left, sf::Keyboard::Left)
    KEY ("RIGHT", lgck::Key::Right, sf::Keyboard::Right)
    KEY ("UP", lgck::Key::Up, sf::Keyboard::Up)
    KEY ("DOWN", lgck::Key::Down, sf::Keyboard::Down)
    KEY ("PAUSE", lgck::Key::Pause, sf::Keyboard::Pause)
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
