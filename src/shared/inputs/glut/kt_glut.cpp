/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"
#include "Const.h"
#include "kt_glut.h"
#include <GL/glut.h>

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
    {"%c", 'A', lgck::Key::A,      lgck::Key::Z,     'a',  'z'   },
    {"%c", 'A', lgck::Key::A,      lgck::Key::Z,     'A',  'Z'   },
    {"%c", '0', lgck::Key::Num0,   lgck::Key::Num9,  '0',  '9'   },
    {"F%d", 1,  lgck::Key::F1,     lgck::Key::F12,    GLSKEY + GLUT_KEY_F1, GLSKEY + GLUT_KEY_F12 },
    // TODO: check num pad keys
    KEY ("ESCAPE", lgck::Key::Escape, 27)
    KEY ("CTRL", lgck::Key::LControl, GLSKEY + 114)
    KEY ("SHIFT", lgck::Key::LShift, GLSKEY + 112)
    KEY ("ALT", lgck::Key::LAlt, GLSKEY + 116 )
    KEY ("RCTRL", lgck::Key::RControl, GLSKEY + 113)
    KEY ("RSHIFT", lgck::Key::RShift, GLSKEY + 115)
    KEY ("RALT", lgck::Key::RAlt, GLSKEY + 117 )
    //   KEY ("MENU", lgck::Key::Menu, GLUT_KEY_MENU)
    KEY ("LBRACKET", lgck::Key::LBracket, '[')
    KEY ("RBRACKET", lgck::Key::RBracket, ']')
    KEY ("LBRACKET", lgck::Key::LBracket, '{')
    KEY ("RBRACKET", lgck::Key::RBracket, '}')
    KEY ("SEMICOLON", lgck::Key::SemiColon, ':')
    KEY ("COMMA", lgck::Key::Comma, ',') // ???
    KEY ("PERIOD", lgck::Key::Period, '.')
    KEY ("QUOTE", lgck::Key::Quote, 34) // ???
    KEY ("SLASH", lgck::Key::Slash, '/')
    KEY ("BACKSLASH", lgck::Key::BackSlash, '\\')
    KEY ("TILDE", lgck::Key::Tilde, '~') // ???
    KEY ("EQUAL", lgck::Key::Equal, '=')
    KEY ("MINUS", lgck::Key::Dash, '-')
    KEY ("SPACE", lgck::Key::Space, ' ' )
    KEY ("RETURN", lgck::Key::Return, '\r')
    KEY ("BACKSPACE", lgck::Key::Back, 8) //???
    KEY ("TAB", lgck::Key::Tab, '\t') //???
    KEY ("PAGEUP", lgck::Key::PageUp, GLSKEY + GLUT_KEY_PAGE_UP)
    KEY ("PAGEDOWN",lgck::Key::PageDown, GLSKEY + GLUT_KEY_PAGE_DOWN)
    KEY ("END", lgck::Key::End, GLSKEY + GLUT_KEY_END)
    KEY ("HOME", lgck::Key::Home, GLSKEY + GLUT_KEY_HOME)
    KEY ("INSERT", lgck::Key::Insert, GLSKEY + GLUT_KEY_INSERT)
    KEY ("DELETE", lgck::Key::Delete, 127)
    KEY ("PLUS", lgck::Key::Add, '+')
    KEY ("MINUS", lgck::Key::Subtract, '-')
    KEY ("MULTIPLY", lgck::Key::Multiply, '*')
    KEY ("DIVIDE", lgck::Key::Divide, '/')
    KEY ("LEFT", lgck::Key::Left, GLSKEY + GLUT_KEY_LEFT)
    KEY ("RIGHT", lgck::Key::Right, GLSKEY + GLUT_KEY_RIGHT)
    KEY ("UP", lgck::Key::Up, GLSKEY + GLUT_KEY_UP)
    KEY ("DOWN", lgck::Key::Down, GLSKEY + GLUT_KEY_DOWN)
    //KEY ("PAUSE", lgck::Key::Pause, GLUT_KEY_PAUSE)
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

