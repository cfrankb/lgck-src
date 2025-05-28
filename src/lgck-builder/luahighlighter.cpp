/*
    LGCK Builder GUI
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
#include "../shared/qtgui/qfilewrap.h"

const QBrush orange = QBrush(QColor(255, 165, 0));
const QBrush darkOrange = QBrush(QColor(0xa3, 0x5a, 0x00));

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

    classFormat.setForeground(darkOrange);
    applyRule(classes(), classFormat);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::darkBlue);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+[\\s]*(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkMagenta); // dark purple
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("--[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);

    commentStartExpression = QRegularExpression(QStringLiteral("[--]{0,1}\\[\\["));
    commentEndExpression = QRegularExpression(QStringLiteral("[--]{0,1}\\]\\]"));
}

void CLuaHighlighter::applyRule(const QStringList &words, const QTextCharFormat &format)
{
    for (const QString &pattern : words)
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
        rule.format = format;
        highlightingRules.append(rule);
    }
}

void CLuaHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : std::as_const(highlightingRules))
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0)
    {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

QStringList CLuaHighlighter::fromFile(const char *fileName)
{
    QString fct;
    QFileWrap file;
    if (file.open(fileName))
    {
        int size = file.getSize();
        auto tmp = new char[size + 1];
        tmp[size] = 0;
        file.read(tmp, size);
        file.close();
        fct = tmp;
        delete[] tmp;
    }
    return fct.split("\n");
}

QStringList CLuaHighlighter::constants()
{
    return fromFile(":/res/const.txt");
}

QStringList CLuaHighlighter::keywords()
{
    return QString("and break do else elseif end false for function if "
                   "in local nil not or repeat return then true until while setmetatable")
        .split(" ");
}

QStringList CLuaHighlighter::classes()
{
    return QString("Counters Debug Display Extra Layer Element HitTest Inventory Proto Sprite").split(" ");
}

QStringList CLuaHighlighter::words()
{
    return fromFile(":/res/functions.txt");
}
