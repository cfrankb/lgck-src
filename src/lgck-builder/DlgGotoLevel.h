/*
    LGCK Builder GUI
    Copyright (C) 1999, 2011  Francois Blanchette

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

#ifndef DLGGOTOLEVEL_H
#define DLGGOTOLEVEL_H

#include <QDialog>

namespace Ui {
    class CDlgGotoLevel;
}

class CDlgGotoLevel : public QDialog {
    Q_OBJECT
public:
    CDlgGotoLevel(QWidget *parent = 0);
    ~CDlgGotoLevel();

    void * m_gameFile;
    void setGameDB(void * p) {
        m_gameFile = p;
    }

    int getLevel();
    void setLevel(int level);

    void init();
    void initLayers(int levelId);
    void initSizes(int px);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CDlgGotoLevel *m_ui;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
};

#endif // DLGGOTOLEVEL_H
