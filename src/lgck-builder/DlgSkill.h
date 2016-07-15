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

#ifndef DLGSKILL_H
#define DLGSKILL_H

#include <QDialog>

namespace Ui {
    class CDlgSkill;
}

class CDlgSkill : public QDialog {
    Q_OBJECT
public:
    CDlgSkill(QWidget *parent = 0);
    ~CDlgSkill();

    void init();
    void setSkill(int skill);
    int getSkill();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CDlgSkill *m_ui;
};

#endif // DLGSKILL_H
