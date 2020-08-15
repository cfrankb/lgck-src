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
#ifndef CGAMEFIXER_H
#define CGAMEFIXER_H

#include <QObject>

class CGame;

enum Severity:uint8_t{
    Okay,
    Warning,
    Error,
   // Fatal
};

typedef struct{
    Severity severity;
    QString message;
    QString tip;
    QString url;
} DesignError;

class CGameFixer: QObject
{
public:
    CGameFixer();
    void setGame(CGame *game);
    void addSampleSprites();
    void troubleshoot();
    Severity severity() {
        return m_severity;
    }
    const char *getIcon(Severity severity, bool flip=true);
    const char *getIcon();
    QString getTooltip();
    QString getStatus();
    const std::vector<DesignError> errors();
    QString getText();
    bool ready() {
        return m_ready;
    }

protected:
    CGame *m_game;
    bool m_flip;
    bool m_ready;
    Severity m_severity;
    std::vector<DesignError> m_errors;
};

#endif // CGAMEFIXER_H
