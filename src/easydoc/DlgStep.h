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
#ifndef DLGSTEP_H
#define DLGSTEP_H

#include <QDialog>

namespace Ui {
class CDlgStep;
}

class CStep;

class CDlgStep : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgStep(QWidget *parent = 0);
    ~CDlgStep();
    void save(CStep & step);
    void load(CStep & step);
    void setName(const QString &name);

private slots:
    void on_btnAdd_clicked();
    void closeTab(int index);
    void on_btnDelete_clicked();

private:
    Ui::CDlgStep *ui;
};

#endif // DLGSTEP_H
