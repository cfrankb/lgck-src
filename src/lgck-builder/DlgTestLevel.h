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
#ifndef DLGTESTLEVEL_H
#define DLGTESTLEVEL_H

#include <QDialog>
class CLevel;

namespace Ui {
    class CDlgTestLevel;
}

class CDlgTestLevel : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgTestLevel(QWidget *parent = 0);
    ~CDlgTestLevel();

    void setSkill(int skill);
    int getSkill();
    int getLives();
    int getScore();
    int getHP();
    bool getContinue();
    void setLives(int lives);
    void setScore(int score);
    void setHP(int hp);
    void setContinue(bool bContinue);
    bool isExternal();
    void setExternal(bool external);
    int getWidth();
    int getHeight();
    int getRez();
    void setRez(int v);
    void analyseLevel(CLevel *level);

private:
    Ui::CDlgTestLevel *ui;
    typedef struct {
        int w;
        int h;
    } REZ;
    static REZ m_rez[];

};

#endif // DLGTESTLEVEL_H
