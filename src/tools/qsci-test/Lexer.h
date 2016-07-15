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

#ifndef LEXER_H
#define LEXER_H

#include <qobject.h>

#include <Qsci/qsciglobal.h>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexerlua.h>

class QSCINTILLA_EXPORT CLexer : public QsciLexerLua
{
    Q_OBJECT
public:
    explicit CLexer(QObject *parent = 0);


    //! Returns the set of keywords for the keyword set \a set recognised
    //! by the lexer as a space separated string.
    const char *keywords(int set) const;

signals:

public slots:

};

#endif // LEXER_H
