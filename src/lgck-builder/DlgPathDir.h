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

#ifndef DLGPATHDIR_H
#define DLGPATHDIR_H

#include <QDialog>
#include <QIcon>

namespace Ui {
    class CDlgPathDir;
}

class CDlgPathDir : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgPathDir(QWidget *parent = 0);
    ~CDlgPathDir();

    void load(const uint8_t aim);
    void save(uint8_t &aim);

protected:
    void changeEvent(QEvent *e);

    enum {
        MAX_ICONS = 5
    };
   // QIcon m_icons[MAX_ICONS];
    static QString m_iconNames[];
    static QString m_iconText[];
    static int m_iconValues[];

    //bool loadIcon(int i);

private:
    Ui::CDlgPathDir *ui;
};

#endif // DLGPATHDIR_H
