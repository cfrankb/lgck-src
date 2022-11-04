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

#ifndef DLGPATH_H
#define DLGPATH_H

#include <QDialog>
#include <QIcon>
#include <QModelIndex>
#include <QTreeWidgetItem>

class CPath;

namespace Ui {
    class CDlgPath;
}

class CDlgPath : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgPath(QWidget *parent = 0);
    ~CDlgPath();

    void load(void *p);
    void save(void *p);
    void updateButtons();

protected:
    void changeEvent(QEvent *e);

//    void addToPath(char aim, bool visualOnly);

    void updateIcon(QTreeWidgetItem *item, int aim);

    CPath *m_path;

    enum {
        MAX_ICONS = 5
    };
    //QIcon m_icons[MAX_ICONS];
    static QString m_iconNames[];
    static QString m_iconText[];
    static int m_iconValues[];


private:
    Ui::CDlgPath *ui;

private slots:
    void on_btnDuplicate_clicked();
    void on_btnRaw_clicked();
    void on_treeWidget_itemSelectionChanged();
    void on_treeWidget_doubleClicked(QModelIndex index);
    void on_btnDelete_clicked();
    void on_btnAdd_clicked();
};

#endif // DLGPATH_H
