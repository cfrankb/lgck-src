/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2014  Francois Blanchette

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
#ifndef _CONST_H
#define _CONST_H

namespace lgck {
    struct Player {
        enum Action{
            Up,
            Down,
            Left,
            Right,
            Jump,
            Fire,
            Action,
            Special_1,
            Special_2,
            Count // this is not an action
        };
    };

    struct Button {
        enum JoyButton {
            A,
            B,
            X,
            Y,
            L1,
            L3,
            R1,
            R3,
            Select,
            Start,
            Up,
            Down,
            Left,
            Right,
            Center,
            Guide,
            Count, // this is not a button
            Invalid = -1,
        };
    };

    struct Key{
        enum  	Code { 
            A = 'a', 
            B = 'b', 
            C = 'c', 
            D = 'd', 
            E = 'e', 
            F = 'f', 
            G = 'g', 
            H = 'h', 
            I = 'i', 
            J = 'j', 
            K = 'k', 
            L = 'l', 
            M = 'm', 
            N = 'n', 
            O = 'o', 
            P = 'p', 
            Q = 'q', 
            R = 'r', 
            S = 's', 
            T = 't', 
            U = 'u', 
            V = 'v', 
            W = 'w', 
            X = 'x', 
            Y = 'y', 
            Z = 'z', 
            Num0 = '0', 
            Num1 = '1', 
            Num2 = '2', 
            Num3 = '3', 
            Num4 = '4', 
            Num5 = '5', 
            Num6 = '6', 
            Num7 = '7', 
            Num8 = '8', 
            Num9 = '9', 
            Escape = 256, 
            LControl, 
            LShift, 
            LAlt, 
            LSystem, 
            RControl, 
            RShift, 
            RAlt, 
            RSystem, 
            Menu, 
            LBracket, 
            RBracket, 
            SemiColon, 
            Comma, 
            Period, 
            Quote, 
            Slash, 
            BackSlash, 
            Tilde, 
            Equal, 
            Dash, 
            Space, 
            Return, 
            Back, 
            Tab, 
            PageUp, 
            PageDown, 
            End, 
            Home, 
            Insert, 
            Delete, 
            Add, 
            Subtract, 
            Multiply, 
            Divide, 
            Left, 
            Right, 
            Up, 
            Down, 
            Numpad0, 
            Numpad1, 
            Numpad2, 
            Numpad3, 
            Numpad4, 
            Numpad5, 
            Numpad6, 
            Numpad7, 
            Numpad8, 
            Numpad9, 
            F1, 
            F2, 
            F3, 
            F4, 
            F5, 
            F6, 
            F7, 
            F8, 
            F9, 
            F10, 
            F11, 
            F12, 
            F13, 
            F14, 
            F15, 
            Pause,
            Count,
            Invalid = -1
        };
    };
}

#endif
