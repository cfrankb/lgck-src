#include "WHotKey.h"
#include <QKeyEvent>
#include <QShortcut>
#include <QKeySequence>

CWHotKey::CWHotKey(QWidget *parent) :
    QLineEdit(parent)
{
    setReadOnly(true);
    setCursor(Qt::ArrowCursor);
}

void CWHotKey::keyPressEvent ( QKeyEvent * e )
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

    /*
Constant	Value	Description
Qt::Key_Escape	0x01000000
Qt::Key_Tab	0x01000001
Qt::Key_Backtab	0x01000002
Qt::Key_Backspace	0x01000003
Qt::Key_Return	0x01000004
Qt::Key_Enter	0x01000005	Typically located on the keypad.
Qt::Key_Insert	0x01000006
Qt::Key_Delete	0x01000007
Qt::Key_Pause	0x01000008	The Pause/Break key (Note: Not anything to do with pausing media)
Qt::Key_Print	0x01000009
Qt::Key_SysReq	0x0100000a
Qt::Key_Clear	0x0100000b
Qt::Key_Home	0x01000010
Qt::Key_End	0x01000011
Qt::Key_Left	0x01000012
Qt::Key_Up	0x01000013
Qt::Key_Right	0x01000014
Qt::Key_Down	0x01000015
Qt::Key_PageUp	0x01000016
Qt::Key_PageDown	0x01000017
Qt::Key_Shift	0x01000020
Qt::Key_Control	0x01000021	On Mac OS X, this corresponds to the Command keys.
Qt::Key_Meta	0x01000022	On Mac OS X, this corresponds to the Control keys. On Windows keyboards, this key is mapped to the Windows key.
Qt::Key_Alt	0x01000023
Qt::Key_AltGr	0x01001103	On Windows, when the KeyDown event for this key is sent, the Ctrl+Alt modifiers are also set.
Qt::Key_CapsLock	0x01000024
Qt::Key_NumLock	0x01000025
Qt::Key_ScrollLock	0x01000026
Qt::Key_F1	0x01000030
Qt::Key_F2	0x01000031
Qt::Key_F3	0x01000032
Qt::Key_F4	0x01000033
Qt::Key_F5	0x01000034
Qt::Key_F6	0x01000035
Qt::Key_F7	0x01000036
Qt::Key_F8	0x01000037
Qt::Key_F9	0x01000038
Qt::Key_F10	0x01000039
Qt::Key_F11	0x0100003a
Qt::Key_F12	0x0100003b
Qt::Key_F13	0x0100003c
Qt::Key_F14	0x0100003d
Qt::Key_F15	0x0100003e
Qt::Key_F16	0x0100003f
Qt::Key_F17	0x01000040
Qt::Key_F18	0x01000041
Qt::Key_F19	0x01000042
Qt::Key_F20	0x01000043
Qt::Key_F21	0x01000044
Qt::Key_F22	0x01000045
Qt::Key_F23	0x01000046
Qt::Key_F24	0x01000047
Qt::Key_F25	0x01000048
Qt::Key_F26	0x01000049
Qt::Key_F27	0x0100004a
Qt::Key_F28	0x0100004b
Qt::Key_F29	0x0100004c
Qt::Key_F30	0x0100004d
Qt::Key_F31	0x0100004e
Qt::Key_F32	0x0100004f
Qt::Key_F33	0x01000050
Qt::Key_F34	0x01000051
Qt::Key_F35	0x01000052
Qt::Key_Super_L	0x01000053
Qt::Key_Super_R	0x01000054
Qt::Key_Menu	0x01000055
Qt::Key_Hyper_L	0x01000056
Qt::Key_Hyper_R	0x01000057
Qt::Key_Help	0x01000058
Qt::Key_Direction_L	0x01000059
Qt::Key_Direction_R	0x01000060
Qt::Key_Space	0x20
Qt::Key_Any	Key_Space
Qt::Key_Exclam	0x21
Qt::Key_QuoteDbl	0x22
Qt::Key_NumberSign	0x23
Qt::Key_Dollar	0x24
Qt::Key_Percent	0x25
Qt::Key_Ampersand	0x26
Qt::Key_Apostrophe	0x27
Qt::Key_ParenLeft	0x28
Qt::Key_ParenRight	0x29
Qt::Key_Asterisk	0x2a
Qt::Key_Plus	0x2b
Qt::Key_Comma	0x2c
Qt::Key_Minus	0x2d
Qt::Key_Period	0x2e
Qt::Key_Slash	0x2f
Qt::Key_0	0x30
Qt::Key_1	0x31
Qt::Key_2	0x32
Qt::Key_3	0x33
Qt::Key_4	0x34
Qt::Key_5	0x35
Qt::Key_6	0x36
Qt::Key_7	0x37
Qt::Key_8	0x38
Qt::Key_9	0x39
Qt::Key_Colon	0x3a
Qt::Key_Semicolon	0x3b
Qt::Key_Less	0x3c
Qt::Key_Equal	0x3d
Qt::Key_Greater	0x3e
Qt::Key_Question	0x3f
Qt::Key_At	0x40
Qt::Key_A	0x41
Qt::Key_B	0x42
Qt::Key_C	0x43
Qt::Key_D	0x44
Qt::Key_E	0x45
Qt::Key_F	0x46
Qt::Key_G	0x47
Qt::Key_H	0x48
Qt::Key_I	0x49
Qt::Key_J	0x4a
Qt::Key_K	0x4b
Qt::Key_L	0x4c
Qt::Key_M	0x4d
Qt::Key_N	0x4e
Qt::Key_O	0x4f
Qt::Key_P	0x50
Qt::Key_Q	0x51
Qt::Key_R	0x52
Qt::Key_S	0x53
Qt::Key_T	0x54
Qt::Key_U	0x55
Qt::Key_V	0x56
Qt::Key_W	0x57
Qt::Key_X	0x58
Qt::Key_Y	0x59
Qt::Key_Z	0x5a
Qt::Key_BracketLeft	0x5b
Qt::Key_Backslash	0x5c
Qt::Key_BracketRight	0x5d
Qt::Key_AsciiCircum	0x5e
Qt::Key_Underscore	0x5f
Qt::Key_QuoteLeft	0x60
Qt::Key_BraceLeft	0x7b
Qt::Key_Bar	0x7c
Qt::Key_BraceRight	0x7d
Qt::Key_AsciiTilde	0x7e*/

    Qt::KeyboardModifiers modifier = e->modifiers();
    int key = e->key();
    QString s;
    if (modifier & Qt::ControlModifier) {
        s += "Ctrl";
    }

    if (modifier & Qt::AltModifier) {
        if (!s.isEmpty()) {
            s += "+";
        }
        s += "Alt";
    }

    if (modifier & Qt::ShiftModifier
        && (key < Qt::Key_Exclam || key >Qt::Key_At)
         && (key < Qt::Key_BraceLeft || key>Qt::Key_AsciiTilde)) {
        if (!s.isEmpty()) {
            s += "+";
        }
        s += "Shift";
    }

    QString keyS = "";

    if (key>= Qt::Key_Exclam
            && key<= Qt::Key_AsciiTilde){
        keyS = QString("").sprintf("%c", key).toUpper();
    }

    for (uint i=0; i < sizeof(keys)/sizeof(KEY);++i) {
        if (key == keys[i].key) {
            keyS = keys[i].text;
        }
    }

    if (!keyS.isEmpty()) {
        if (s.isEmpty()){
            s = keyS;
        } else {
            s += "+" + keyS;
        }
        setText(s);
    }
    qDebug("%d", key);

   // QKeySequence()
}
