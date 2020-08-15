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
#ifndef DLGEDITFRAMES_H
#define DLGEDITFRAMES_H

#include <QDialog>

namespace Ui {
class CDlgEditFrames;
}

class CDlgEditFrames : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgEditFrames(QWidget *parent = 0);
    ~CDlgEditFrames();

private:
    Ui::CDlgEditFrames *ui;
};

#endif // DLGEDITFRAMES_H
