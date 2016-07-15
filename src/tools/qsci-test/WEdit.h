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

#include <qt4/Qsci/qsciscintilla.h>
#include <qstringlist.h>

class CLexer;
class QsciAPIs;
class QCompleter;
class QStringListModel;

class CWEdit : public QsciScintilla
{
    Q_OBJECT
public:
    explicit CWEdit(QWidget *parent = 0);
    ~CWEdit();
protected:
    // reimplemented from private
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    char getCharacter(int &pos) const;
    QString getWord(int &pos) const;

    bool canImproveWord();

    QStringList m_words;
    CLexer *m_lexer;
    QsciAPIs *m_api;
    QCompleter * m_completer;
    QStringListModel *m_listmodel;
    int m_pos;
    int m_line;
    int m_posX;
    int m_posY;
    QString m_currentWord;
    bool m_ready;

signals:

public slots:
    void CursorAtIndex(int line ,int pos);
    void insertCompletion( QString completion );
};

#endif // WEDIT_H
