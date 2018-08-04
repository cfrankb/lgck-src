/*
    LGCK Builder GUI
    Copyright (C) 1999, 2012  Francois Blanchette

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

#ifndef TOOLBARDOCK_H
#define TOOLBARDOCK_H

#include <QDockWidget>
#include <QModelIndex>
#include <QDialog>
#include <QTreeWidgetItem>
#include <QMouseEvent>

class CGameFile;
class CProtoIndex;

namespace Ui {
    class CToolBoxDock;
}

class CToolBoxDock : public QDockWidget {
    Q_OBJECT
public:
    CToolBoxDock(QWidget *parent = 0);
    ~CToolBoxDock();

    void setGameDB(CGameFile * p) {
        m_gameFile = p;
    }

    void init();
    void reload();

protected:
    Ui::CToolBoxDock *m_ui;
    CProtoIndex *m_index;
    CGameFile *m_gameFile;
    int *m_indexSettings;

    void updateIcon(void * item, int protoId);
    void updateIconFrameSet(void * item, int fs);
    void updateObjects (int OldframeSet, int newFrameSet);
    void updateButtons();
    void clearSprites();
    void reloadSprites();
    void reloadSounds();
    void reloadLevels();
    void reloadEvents();
    void reloadDisplays();
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
    void checkFrameSetUses(int frameSet);

private slots:
    void on_btnDeleteSprite_clicked();
    void on_btnAddSprite_clicked();
    void on_treeObjects_doubleClicked(QModelIndex index);
    void on_treeObjects_customContextMenuRequested(QPoint pos);
    void on_btnCopySprite_clicked();
    void on_cbFilterSprites_currentIndexChanged(int index);
    void on_btnAddLevel_clicked();
    void on_btnDeleteLevel_clicked();
    void on_btnAddSound_clicked();
    void on_btnDeleteSound_clicked();
    void on_treeLevels_doubleClicked(QModelIndex index);
    void on_treeEvents_doubleClicked(QModelIndex index);
    void on_btnClearAll_clicked();
    void on_treeLevels_itemSelectionChanged();
    void on_treeSounds_itemSelectionChanged();
    void on_treeSounds_doubleClicked(QModelIndex index);
    void on_treeLevels_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void on_treeLevels_customContextMenuRequested(QPoint pos);
    void on_treeSounds_customContextMenuRequested(QPoint pos);
    void on_treeObjects_itemSelectionChanged();
    void editSprite();
    void copySprite();
    void editLevel(int index);
    void selectLevel(int index);
    void deleteLevel(int index);
    void addLevel();
    void moveLevel(int fromIndex, int toIndex);
    void editSound();
    void playSound();
    void renameSound();
    void editEvents();
    void createSprite();
    void editFrames();
    void exportSprite();
    void updateFrameSet(int frameSet);
    void on_treeDisplays_doubleClicked(const QModelIndex &index);
    void on_btnDeleteDisplay_clicked();
    void on_btnAddDisplay_clicked();
    void on_treeDisplays_clicked(const QModelIndex &index);

signals:
    void levelSelection(int index);
    void levelDeleted(int index);
    void levelEdited(int index);
    void levelAdded();
    void menuSeekingItems(QMenu *, int);
    void spriteDeleted(int sprite);
    void spriteChanged(int sprite);

};

#endif // TOOLBARDOCK_H
