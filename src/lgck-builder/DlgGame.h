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

#ifndef DLGGAME_H
#define DLGGAME_H

#include <QModelIndex>
#include <QDialog>
class QTreeWidgetItem;
class CGameFile;

namespace Ui {
    class CDlgGame;
}

class CDlgGame : public QDialog {
    Q_OBJECT
public:
    CDlgGame(QWidget *parent = 0);
    ~CDlgGame();

    void setGameDB(CGameFile * p) {
        m_gameFile = p;
    }

    void init();

protected:
    int *m_indexSettings;
    //QString *m_events;

    void changeEvent(QEvent *e);
    void updateIcon(QTreeWidgetItem * item, int protoId);
    void updateIconFrameSet(QTreeWidgetItem * item, int fs);
    void updateObjects (int OldframeSet, int newFrameSet);
    void updateButtons();
    void fillTreeSettings();

private:
    Ui::CDlgGame *m_ui;
    CGameFile * m_gameFile;

private slots:
    void on_btnDefaults_clicked();
    //void on_btnDebug_clicked();
    void on_treeFrameSets_itemSelectionChanged();
    void on_treeSounds_itemSelectionChanged();
    void on_treeObjects_itemSelectionChanged();
    void on_btnDeleteLevel_clicked();
    void on_btnAddLevel_clicked();
    void on_treeLevels_itemSelectionChanged();
    void on_treeLevels_doubleClicked(QModelIndex index);
    void on_treeSettings_doubleClicked(QModelIndex index);
    void on_treeFrameSets_doubleClicked(QModelIndex index);
    void on_btnDeleteFrameSet_clicked();
    void on_btnAddFrameSet_clicked();
//    void on_treeFrameSets_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
//    void on_treeSounds_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
//    void on_treeObjects_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void on_treeSounds_doubleClicked(QModelIndex index);
    void on_btnDeleteSound_clicked();
    void on_btnAddSound_clicked();
    void on_btnDeleteObject_clicked();
    void on_btnAddObject_clicked();
    void on_treeObjects_doubleClicked(QModelIndex index);
    void on_treeEvents_doubleClicked(QModelIndex index);
    void on_btnClearAll_clicked();
    //void on_btnExportSprite_clicked();
};

#endif // DLGGAME_H
