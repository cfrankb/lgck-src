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

#ifndef DLGFRAMESET_H
#define DLGFRAMESET_H

#include <QDialog>
#include <QTreeWidgetItem>

class CGameFile;
class CFrameSet;

namespace Ui {
    class CDlgFrameSet;
}

class CDlgFrameSet : public QDialog {
    Q_OBJECT
public:
    CDlgFrameSet(QWidget *parent = 0);
    ~CDlgFrameSet();

    void init(CFrameSet *);
    void save();

protected:
    void changeEvent(QEvent *e);
    //void updateButtons();

    CFrameSet *m_frameSet;

private:
    Ui::CDlgFrameSet *m_ui;

private slots:
    void on_treeWidget_itemSelectionChanged();
    void on_btnDeleteFrame_clicked();
    void on_btnAddFrames_clicked();
    void on_eName_textChanged(const QString &arg1);
    void updateButtons();

    void on_btnEdit_clicked();

signals:
    void imagesAdded();
    void imageDeleted();
    void refill();
};

#endif // DLGFRAMESET_H
