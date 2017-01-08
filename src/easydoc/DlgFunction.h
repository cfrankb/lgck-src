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

#ifndef DLGFUNCTION_H
#define DLGFUNCTION_H

#include <QDialog>
#include <QModelIndex>
#include "Database.h"

namespace Ui {
    class CDlgFunction;
}

class CDlgFunction : public QDialog {
    Q_OBJECT
public:
    CDlgFunction(QWidget *parent = 0);
    ~CDlgFunction();

    void load(CFunction *fn);
    void save(CFunction *fn);

protected:
    void changeEvent(QEvent *e);
    CFunction * m_fn;
    const QString formatParam(Param & param);
    int m_inSet;
    void updateInTree();
    void updateInSetCombo();
    int m_testcase;
    void updateTestCaseTree();
    void updateTestCaseCombo();

private:
    Ui::CDlgFunction *m_ui;

private slots:
    void on_treeOut_doubleClicked(QModelIndex index);
    void on_btnDelete_OUT_clicked();
    void on_btnAdd_OUT_clicked();
    void on_btnDelete_IN_clicked();
    void on_btnAdd_IN_clicked();
    void on_treeIn_doubleClicked(QModelIndex index);
    void on_btnAdd_Alias_clicked();
    void on_btnDelete_Alias_clicked();
    void on_cbInSet_currentIndexChanged(int index);
    void on_btnAdd_IN_SET_clicked();
    void on_btnDelete_IN_SET_clicked();
    void on_treeTestCase_clicked(const QModelIndex &index);
    void on_btnAdd_Step_clicked();
    void on_btnDelete_Step_clicked();
    void on_treeTestCase_doubleClicked(const QModelIndex &index);
    void on_btnAdd_TestCase_clicked();
    void on_btnDelete_TestCase_clicked();
    void on_cbTestCase_currentIndexChanged(int index);
};

#endif // DLGFUNCTION_H
