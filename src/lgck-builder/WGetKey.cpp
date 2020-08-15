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
#include "WGetKey.h"
#include <QKeyEvent>
#include <QShortcut>
#include <QKeySequence>

CWGetKey::CWGetKey(QWidget *parent) :
    QLineEdit(parent)
{
    setReadOnly(true);
    setCursor(Qt::ArrowCursor);
}

void CWGetKey::keyPressEvent ( QKeyEvent * e )
{
    typedef struct {
        Qt::Key key;
        const char *text;
    } KEY;

    const KEY keys[] ={
        { Qt::Key_Space,         "Space" },
        { Qt::Key_Escape,        "Esc" },
        { Qt::Key_Tab,           "Tab" },
        { Qt::Key_Backtab,       "Backtab" },
        { Qt::Key_Backspace,     "Backspace" },
        { Qt::Key_Return,        "Return" },
        { Qt::Key_Enter,         "Enter" },
        { Qt::Key_Insert,        "Ins" },
        { Qt::Key_Delete,        "Del" },
        { Qt::Key_Pause,         "Pause" },
        { Qt::Key_Print,         "Print" },
        { Qt::Key_SysReq,        "SysReq" },
        { Qt::Key_Home,          "Home" },
        { Qt::Key_End,           "End" },
        { Qt::Key_Left,          "Left" },
        { Qt::Key_Up,            "Up" },
        { Qt::Key_Right,         "Right" },
        { Qt::Key_Down,          "Down" },
        { Qt::Key_PageUp,        "PgUp" },
        { Qt::Key_PageDown,      "PgDown" },
        { Qt::Key_CapsLock,      "CapsLock" },
        { Qt::Key_NumLock,       "NumLock" },
        { Qt::Key_ScrollLock,    "ScrollLock" },
        { Qt::Key_Menu,          "Menu" },
        { Qt::Key_Help,          "Help" },
        { Qt::Key_F1,            "F1"},
        { Qt::Key_F2,            "F2"},
        { Qt::Key_F3,            "F3"},
        { Qt::Key_F4,            "F4"},
        { Qt::Key_F5,            "F5"},
        { Qt::Key_F6,            "F6"},
        { Qt::Key_F7,            "F7"},
        { Qt::Key_F8,            "F8"},
        { Qt::Key_F9,            "F9"},
        { Qt::Key_F10,           "F10"},
        { Qt::Key_F11,           "F11"},
        { Qt::Key_F12,           "F12"},
    };

    Qt::KeyboardModifiers modifier = e->modifiers();
    int key = e->key();
    QString s;
    if (modifier & Qt::ControlModifier) {
        s = "Ctrl";
    }

    if (modifier & Qt::AltModifier) {
        if (!s.isEmpty()) {
            s += "+";
        }
        s = "Alt";
    }

    if (modifier & Qt::ShiftModifier
        && (key < Qt::Key_Exclam || key >Qt::Key_At)
         && (key < Qt::Key_BraceLeft || key>Qt::Key_AsciiTilde)) {
        if (!s.isEmpty()) {
            s += "+";
        }
        s = "Shift";
    }

    QString keyS = "";

    if (key>= Qt::Key_Exclam
            && key<= Qt::Key_AsciiTilde){
        keyS = QString::asprintf("%c", key).toUpper();
    }

    for (uint i=0; i < sizeof(keys)/sizeof(KEY);++i) {
        if (key == keys[i].key) {
            keyS = keys[i].text;
        }
    }

    if (!keyS.isEmpty()) {
        if (s.isEmpty()){
            s = keyS;
        }
        setText(s);
    }
    qDebug("%d", key);
}
