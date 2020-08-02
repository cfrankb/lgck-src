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

#include "WEdit.h"
#include <QCompleter>
#include <QStringListModel>
#include <QKeyEvent>
#include <QScrollBar>
#include <QDebug>
#include <QAbstractItemView>
#include <QTextEdit>
#include <QPainter>
#include "luahighlighter.h"
#include "../shared/stdafx.h"
#include "../shared/FileWrap.h"

const QColor highlightColor = QColor(Qt::yellow);
const QColor indentationBackgroundColor = QColor(Qt::lightGray);

CWEdit::CWEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setFontSize(10);

    m_highlighter = new CLuaHighlighter(document());
    lineNumberArea = new LineNumberArea(this);

    connect(this, &CWEdit::blockCountChanged, this, &CWEdit::updateLineNumberAreaWidth);
    connect(this, &CWEdit::updateRequest, this, &CWEdit::updateLineNumberArea);
    connect(this, &CWEdit::cursorPositionChanged, this, &CWEdit::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    m_enableHighlight = true;
    setFocus();

    setTabStopDistance(fontMetrics().horizontalAdvance(QLatin1Char('9')));
    enableWhiteSpace(true);
    enableWordWrap(false);

    // start autocompleter
    m_enableAutocomplete = true;
    m_words = words() + constants();
    m_completer = new QCompleter();
    m_listmodel = new QStringListModel(m_completer);
    m_listmodel->setStringList(m_words);
    m_completer->setModel(m_listmodel);
    m_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setWidget(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseSensitive);
    m_completer->setWrapAround(false);
    connect(m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

void CWEdit::enableAutocomplete(bool state)
{
    m_enableAutocomplete = state;
}

void CWEdit::enableHighlight(bool state)
{
    m_enableHighlight = state;
}

void CWEdit::enableWhiteSpace(bool state)
{
    QTextOption option;
    if (state) {
        option.setFlags(QTextOption::ShowLineAndParagraphSeparators | QTextOption::ShowTabsAndSpaces);
    } else {
        option.setFlags(option.flags() ^ (QTextOption::ShowLineAndParagraphSeparators | QTextOption::ShowTabsAndSpaces));
    }
    document()->setDefaultTextOption(option);
}

void CWEdit::enableWordWrap(bool state)
{
    if (state) {
        setLineWrapMode(WidgetWidth);
    } else {
        setLineWrapMode(NoWrap);
    }
}

QStringList CWEdit::fromFile(const char *fileName)
{
    QString fct;
    CFileWrap file;
    if (file.open(fileName)) {
        int size = file.getSize();
        char tmp[ size + 1];
        tmp[size]=0;
        file.read(tmp, size);
        file.close();
        fct = tmp;
    }
    return fct.split("\n");
}

QStringList CWEdit::words()
{
    return fromFile(":/res/functions.txt");
}

QStringList CWEdit::constants()
{
    return fromFile(":/res/const.txt");
}

QCompleter *CWEdit::completer() const
{
    return m_completer;
}

CWEdit::~CWEdit() {

}

///////////////////////////////////////////////////////////////////////////////
// autocomplete

void CWEdit::insertCompletion( QString completion )
{
    if (m_completer->widget() != this || !m_enableAutocomplete)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - m_completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString CWEdit::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CWEdit::focusInEvent(QFocusEvent *e)
{
    if (m_completer)
        m_completer->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void CWEdit::keyPressEvent(QKeyEvent *e)
{
    if (m_completer && m_completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!m_completer || !isShortcut) // do not process the shortcut when we have a completer
        QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!m_completer || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                      || eow.contains(e->text().right(1)))) {
        m_completer->popup()->hide();
        return;
    }

    if (completionPrefix != m_completer->completionPrefix()) {
        m_completer->setCompletionPrefix(completionPrefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                + m_completer->popup()->verticalScrollBar()->sizeHint().width());
    m_completer->complete(cr); // popup it up!
}

///////////////////////////////////////////////////////////////////////////////
// TODO: cleanup later
void CWEdit::setFontSize(int size)
{
    QPlainTextEdit::setFont(QFont("Courier", size, QFont::DemiBold));
}

void CWEdit::setFont(const QFont & font)
{
    QPlainTextEdit::setFont(font);
}

///////////////////////////////////////////////////////////////////////////////
// line numbers

int CWEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * std::max(digits, 4);
    return space;
}

void CWEdit::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CWEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CWEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

///////////////////////////////////////////////////////////////////////////////
// highlight

void CWEdit::highlightCurrentLine()
{
    if (!m_enableHighlight) {
        return;
    }

    QList<QTextEdit::ExtraSelection> extraSelections;
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = highlightColor.lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CWEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), indentationBackgroundColor);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
