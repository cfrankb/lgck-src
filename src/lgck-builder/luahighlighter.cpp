/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "luahighlighter.h"
#include "FileWrap.h"

CLuaHighlighter::CLuaHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat numberFormat;

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    applyRule(keywords(), keywordFormat);

    keywordFormat.setForeground(Qt::magenta); // purple
    keywordFormat.setFontWeight(QFont::Bold);
    applyRule(constants(), keywordFormat);

    numberFormat.setForeground(Qt::darkCyan);
    rule.pattern = QRegularExpression(QStringLiteral("(\\b0x[0-9a-fA-F]+|\\b[0-9]+)\\b"));
    rule.format = numberFormat;
    highlightingRules.append(rule);

    /*classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
    rule.format = classFormat;
    highlightingRules.append(rule);*/

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("--[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::darkBlue);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+[\\s]*(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    QBrush orange = QBrush(QColor(255,165,0));
    quotationFormat.setForeground(Qt::darkMagenta); // dark purple
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression(QStringLiteral("[--]{0,1}\\[\\["));
    commentEndExpression = QRegularExpression(QStringLiteral("[--]{0,1}\\]\\]"));
}

void CLuaHighlighter::applyRule(const QStringList & words, const QTextCharFormat & format)
{
    for (const QString &pattern : words) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
        rule.format = format;
        highlightingRules.append(rule);
    }
}

void CLuaHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

QStringList CLuaHighlighter::constants()
{
    return QString("KEY_A KEY_B KEY_C KEY_D KEY_E KEY_F KEY_G KEY_H KEY_I KEY_J KEY_K KEY_L " \
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
            "CLASS_MONSTER_BULLET").split(" ");
}

QStringList CLuaHighlighter::keywords()
{
    return QString("and break do else elseif end false for function if "\
            "in local nil not or repeat return then true until while setmetatable").split(" ");
}

QStringList CLuaHighlighter::words()
{
    QString fct;
    CFileWrap file;
    if (file.open(":/res/functions.txt")) {
        int size = file.getSize();
        char tmp[ size + 1];
        tmp[size]=0;
        file.read(tmp, size);
        file.close();
        fct = tmp;
    }
    return fct.split("\n");
}
