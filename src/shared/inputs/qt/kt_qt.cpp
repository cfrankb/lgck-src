#include "stdafx.h"
#include "Const.h"
#include "kt_qt.h"

#define KEY(s, a, b) { s, 0, a, a, b, b },

typedef struct {
    const char *string;
    int base;
    int sf_start;
    int sf_end;
    int qt_start;
    int qt_end;
} key_match;

const key_match matches[] = {
  {"%c", 'A', lgck::Key::A,      lgck::Key::Z,     Qt::Key_A,  Qt::Key_Z   },
  {"%c", '0', lgck::Key::Num0,   lgck::Key::Num9,  Qt::Key_0,  Qt::Key_9   },
  {"F%d", 1, lgck::Key::F1,     lgck::Key::F15,    Qt::Key_F1, Qt::Key_F15  },
  // num pad no matched on qt side
  KEY ("ESCAPE", lgck::Key::Escape, Qt::Key_Escape)
  KEY ("CTRL", lgck::Key::LControl, Qt::Key_Control)
  KEY ("SHIFT", lgck::Key::LShift, Qt::Key_Shift)
  KEY ("ALT", lgck::Key::LAlt, Qt::Key_Alt )
  KEY ("MENU", lgck::Key::Menu, Qt::Key_Menu)
  KEY ("LBRACKET", lgck::Key::LBracket, Qt::Key_BracketLeft)
  KEY ("RBRACKET", lgck::Key::RBracket, Qt::Key_BracketRight)
  KEY ("SEMICOLON", lgck::Key::SemiColon, Qt::Key_Semicolon)
  KEY ("COMMA", lgck::Key::Comma, Qt::Key_Comma) // ???
  KEY ("PERIOD", lgck::Key::Period, Qt::Key_Period)
  KEY ("QUOTE", lgck::Key::Quote, Qt::Key_QuoteLeft) // ???
  KEY ("SLASH", lgck::Key::Slash, Qt::Key_Slash)
  KEY ("BACKSLASH", lgck::Key::BackSlash, Qt::Key_Backslash)
  KEY ("TILDE", lgck::Key::Tilde, Qt::Key_AsciiTilde) // ???
  KEY ("EQUAL", lgck::Key::Equal, Qt::Key_Equal)
  KEY ("MINUS", lgck::Key::Dash, Qt::Key_Minus)
  KEY ("SPACE", lgck::Key::Space, Qt::Key_Space)
  KEY ("RETURN", lgck::Key::Return, Qt::Key_Return)
  KEY ("BACKSPACE", lgck::Key::Back, Qt::Key_Backspace) //???
  KEY ("TAB", lgck::Key::Tab, Qt::Key_Tab) //???
  KEY ("PAGEUP", lgck::Key::PageUp, Qt::Key_PageUp)
  KEY ("PAGEDOWN",lgck::Key::PageDown, Qt::Key_PageDown)
  KEY ("END", lgck::Key::End, Qt::Key_End)
  KEY ("HOME", lgck::Key::Home, Qt::Key_Home)
  KEY ("INSERT", lgck::Key::Insert, Qt::Key_Insert)
  KEY ("DELETE", lgck::Key::Delete, Qt::Key_Delete)
  KEY ("PLUS", lgck::Key::Add, Qt::Key_Plus)
  KEY ("MINUS", lgck::Key::Subtract, Qt::Key_Minus)
  KEY ("MULTIPLY", lgck::Key::Multiply, Qt::Key_multiply)
  KEY ("DIVIDE", lgck::Key::Divide, Qt::Key_division)
  KEY ("LEFT", lgck::Key::Left, Qt::Key_Left)
  KEY ("RIGHT", lgck::Key::Right, Qt::Key_Right)
  KEY ("UP", lgck::Key::Up,  Qt::Key_Up)
  KEY ("DOWN", lgck::Key::Down, Qt::Key_Down)
  KEY ("PAUSE", lgck::Key::Pause, Qt::Key_Pause)
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
    for (int i = 0; matches[i].qt_start != -1; ++i) {
        if (key >= matches[i].qt_start &&
                key <= matches[i].qt_end) {
            return matches[i].sf_start + key - matches[i].qt_start;
        }
    }
    return -1;
}

