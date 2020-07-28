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

#ifndef WIZGAME_H
#define WIZGAME_H

#include <QWizard>

class CGame;

namespace Ui {
class CWizGame;
}

class CWizGame : public QWizard
{
    Q_OBJECT
    
public:
    explicit CWizGame(QWidget *parent = 0);
    ~CWizGame();

    void init(CGame *game);
    void save();

private:
    void addSampleSprites();
    
private slots:
    void on_rSetByDifficulty_clicked();
    void on_rSetByLevel_clicked();

private:
    Ui::CWizGame *ui;
    CGame *m_game;
};

#endif // WIZGAME_H
