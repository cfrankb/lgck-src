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

#ifndef WEDIT_H
#define WEDIT_H
#include <QPlainTextEdit>
#include <qstringlist.h>

class QCompleter;
class QStringListModel;
class CLuaHighlighter;

class CWEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CWEdit(QWidget *parent = 0);
    ~CWEdit();

public:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    QCompleter *completer() const;

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
    QString textUnderCursor() const;

    QStringList m_words;
    QCompleter * m_completer;
    QStringListModel *m_listmodel;
    int m_pos;
    int m_line;
    int m_posX;
    int m_posY;
    QString m_currentWord;
    bool m_ready;
    CLuaHighlighter *m_highlighter;
    QStringList words();

signals:

public slots:
    void insertCompletion( QString completion );
    void setFontSize(int size);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CWEdit *editor) : QWidget(editor), codeEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CWEdit *codeEditor;
};

#endif // WEDIT_H
