/*
    LGCK Builder GUI
    Copyright (C) 1999, 2012  Francois Blanchette

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

#include "Lexer.h"

CLexer::CLexer(QObject *parent) :
    QsciLexerLua(parent)
{
}

const char *CLexer::keywords(int set) const
{
    // constants
    if (set == 5) {
        return "KEY_A KEY_B KEY_C KEY_D KEY_E KEY_F KEY_G KEY_H KEY_I KEY_J KEY_K KEY_L " \
                "KEY_M KEY_N KEY_O KEY_P KEY_Q KEY_R KEY_S KEY_T KEY_U KEY_V KEY_W KEY_X " \
                "KEY_Y KEY_Z KEY_0 KEY_1 KEY_2 KEY_3 KEY_4 KEY_5 KEY_6 KEY_7 KEY_8 KEY_9 " \
                "KEY_F1 KEY_F2 KEY_F3 KEY_F4 KEY_F5 KEY_F6 KEY_F7 KEY_F8 KEY_F9 KEY_F10 " \
                "KEY_F11 KEY_F12 KEY_F13 KEY_F14 KEY_F15 KEY_ESCAPE KEY_CTRL KEY_SHIFT " \
                "KEY_ALT KEY_MENU KEY_LBRACKET KEY_RBRACKET KEY_SEMICOLON KEY_COMA " \
                "KEY_PERIOD KEY_QUOTE KEY_SLASH KEY_BACKSLASH KEY_TILDE KEY_EQUAL " \
                "KEY_MINUS KEY_SPACE KEY_RETURN KEY_BACKSPACE KEY_TAB KEY_PAGEUP " \
                "KEY_PAGEDOWN KEY_END KEY_HOME KEY_INSERT KEY_DELETE KEY_PLUS KEY_MINUS " \
                "KEY_MULTIPLY KEY_DIVIDE KEY_LEFT KEY_RIGHT KEY_UP KEY_DOWN KEY_PAUSE " \
                "TRUE FALSE NULL INVALID " \
                "EO_SPAWN EO_ACTIVATE EO_DEATH EO_HURT EO_TOUCH EO_TRIGGER EO_HANDLER " \
                "EO_SPLAT EO_HIT_TEST EO_ZKEY EO_AUTO EO_JUMP EO_FALL EO_LAND EO_LEAP EO_MOVE " \
                "EO_FIRE EO_LEFTCLICK EO_RIGHTCLICK EO_NOTIFYCLOSURE " \
                "EL_CREATE EL_DRAW EL_RESTART EL_GOAL_COLLECTED EL_LEVEL_COMPLETED EL_TIMEOUT " \
                "EL_KEY_PRESSED EL_KEY_UP EL_GOAL_KILLED EL_HANDLER " \
                "EG_INIT_GAME EG_PREPARE_LEVEL EG_COMPLETE_LEVEL EG_DEATH EG_GOAL_COLLECTED EG_GOAL_KILLED " \
                "DI_NONE DI_ANIMATION DI_REMOVAL " \
                "EVENT_NO_EVENT EVENT_QUIT EVENT_LEVEL_COMPLETED EVENT_TIMEOUT EVENT_PLAYER_DIED EVENT_NO_PLAYER " \
                "UP DOWN LEFT RIGHT AIM_MASK LEFT_UP LEFT_DOWN RIGHT_UP RIGHT_DOWN LEAP LEAP_LEFT LEAP_RIGHT " \
                "PAUSE SEPARATOR HERE " \
                "JOY_UP JOY_DOWN JOY_LEFT JOY_RIGHT JOY_FIRE JOY_JUMP JOY_ZKEY " \
                "FLAG_NONE FLAG_METAL FLAG_NOSOUND FLAG_WATER FLAG_DEATH FLAG_HIT FLAG_TELEPORT " \
                "STATE_NONE STATE_FALL STATE_DEAD STATE_HIT STATE_JUMP STATE_BEGINNING STATE_LOOKUP STATE_FIRING " \
                "PROTO_DEAD_PLAYER PROTO_POINTS " \
                "WRAP_UP WRAP_DOWN WRAP_LEFT WRAP_RIGHT " \
                "GOAL_ONLY GOAL_UP GOAL_DOWN GOAL_LEFT GOAL_RIGHT GOAL_NO_COMPLETE " \
                "DISPLAY_TIME_LEFT DISPLAY_MESSAGE DISPLAY_SCORE DISPLAY_LIVES DISPLAY_HP DISPLAY_DEBUG DISPLAY_IMAGE " \
                "SKILL_NORMAL SKILL_NIGHTMARE SKILL_HELL SKILL_INSANE " \
                "PS_DEFAULT PS_IDLE PS_CUSTOM1 PS_CUSTOM2 PS_ATTACK PS_ATTACK_UP PS_ATTACK_DOWN PS_ATTACK_LEFT " \
                "PS_ATTACK_RIGHT PS_JUMP PS_JUMP_UP PS_JUMP_DOWN PS_JUMP_LEFT PS_JUMP_RIGHT PS_JUMP_UP_LEFT " \
                "PS_JUMP_UP_RIGHT PS_JUMP_DN_LEFT PS_JUMP_DN_RIGHT PS_MOVE PS_MOVE_UP PS_MOVE_DOWN PS_MOVE_LEFT " \
                "PS_MOVE_RIGHT PS_MOVE_UP_LEFT PS_MOVE_UP_RIGHT PS_MOVE_DN_LEFT PS_MOVE_DN_RIGHT PS_SPRITE_CUSTOM " \
                "SPEED_NOSCROLL SPEED_WHOLE SPEED_1_2 SPEED_1_4 SPEED_1_8 SPEED_1_16 SPEED_1_32 SPEED_1_64 SPEED_1_128 " \
                "LAYER_BK LAYER_FW LAYER_MAIN " \
                "AS_DEFAULT AS_IDLE AS_CUSTOM1 AS_CUSTOM2 " \
                "AS_STAND AS_STAND_UP AS_STAND_DOWN AS_STAND_LEFT AS_STAND_RIGHT " \
                "AS_MOVE AS_MOVE_UP AS_MOVE_DOWN AS_MOVE_LEFT AS_MOVE_RIGHT " \
                "AS_JUMP AS_JUMP_UP AS_JUMP_DOWN AS_JUMP_LEFT AS_JUMP_RIGHT " \
                "AS_HURT AS_HURT_UP AS_HURT_DOWN AS_HURT_LEFT AS_HURT_RIGHT " \
                "AS_ATTACK AS_ATTACK_UP AS_ATTACK_DOWN AS_ATTACK_LEFT AS_ATTACK_RIGHT " \
                "AS_RUN AS_RUN_UP AS_RUN_DOWN AS_RUN_LEFT AS_RUN_RIGHT " \
                "AS_DEAD AS_DEAD_UP AS_DEAD_DOWN AS_DEAD_LEFT AS_DEAD_RIGHT AS_MAX_VALUE " \
                "CLASS_BACKGROUND_DECORATION " \
                "CLASS_ANTIGRAVITY " \
                "CLASS_METAL_LADDER " \
                "CLASS_SOLID_DOWN_ONLY " \
                "CLASS_REVERT_TO_BACKGROUND " \
                "CLASS_IMMEDIATELY_DEADLY " \
                "CLASS_SOLID_BACKGROUND " \
                "CLASS_STOP " \
                "CLASS_ANIMATED_THING " \
                "CLASS_PICKUP_TRIGGER " \
                "CLASS_INVENTORY_ITEM " \
                "CLASS_OPEN_TO_OWNER " \
                "CLASS_TELEPORTATION_PAD " \
                "CLASS_TELEPORTATION_DESTINATION " \
                "CLASS_Z_KEY_SWITCH " \
                "CLASS_ANIMATE_ONCE " \
                "CLASS_SOLID_ANIMATED " \
                "CLASS_PLAYER_OBJECT " \
                "CLASS_GENERIC_MONSTER " \
                "CLASS_USER_DEFINED_MONSTER " \
                "CLASS_DRONE_UP_DOWN " \
                "CLASS_DRONE_LEFT_RIGHT " \
                "CLASS_ATTACKER " \
                "CLASS_VAMPIRE_PLANT " \
                "CLASS_WHACKER_UP_DOWN " \
                "CLASS_WHACKER_LEFT_RIGHT " \
                "CLASS_PLATFORM_UP_DOWN " \
                "CLASS_PLATFORM_LEFT_RIGHT " \
                "CLASS_PLAYER_BULLER " \
                "CLASS_MONSTER_BULLET";

        // TODO: add the other constant starting with SPEED
    }

    // let the luaLexer take care of the rest
    return QsciLexerLua::keywords(set);
}
