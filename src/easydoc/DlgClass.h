/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2009, 2010  Francois Blanchette

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

#ifndef DLGCLASS_H
#define DLGCLASS_H

#include <QDialog>
#include <QModelIndex>
#include <QTreeWidgetItem>
#include "Database.h"

namespace Ui {
    class CDlgClass;
}

class CDlgClass : public QDialog {
    Q_OBJECT
public:
    CDlgClass(QWidget *parent = 0);
    ~CDlgClass();

    void load(CDatabase *db, CClass *cl);
    void save(CClass *cl);

protected:
    CDatabase *m_db;
    CClass *m_cl;
    void changeEvent(QEvent *e);
    void format (QTreeWidgetItem * item, CFunction & fn);
    void format (QTreeWidgetItem * item, Param & var);
    void initFn (CFunction & fn);

    enum {
        ICON_BLUE,
        ICON_CYAN,
        ICON_GREEN,
        ICON_GREY,
        ICON_RED,
        ICON_YELLOW,
        ICON_NA,
        ICON_CPP,
        ICON_LUA,
        ICON_COUNT,
    };

    QIcon m_icons[ICON_COUNT];

private:
    Ui::CDlgClass *m_ui;

private slots:
    void on_btnDeleteVar_clicked();
    void on_btnAddVar_clicked();
    void on_treeVar_doubleClicked(QModelIndex index);
    void on_treeFn_doubleClicked(QModelIndex index);
    void on_btnDelete_clicked();
    void on_btnAdd_clicked();
//    void on_btnAddVars_clicked();
    void on_btnAddVarM_clicked();
};

#endif // DLGFUNCTIONS_H
