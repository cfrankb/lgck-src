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

#ifndef DLGOBJECT_H
#define DLGOBJECT_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QModelIndex>
#include "WAnimation.h"

class CPath;
class CGameFile;
class QComboBox;

namespace Ui {
    class CDlgObject;
}

class CDlgObject : public QDialog {
    Q_OBJECT
public:
    CDlgObject(QWidget *parent = 0);
    ~CDlgObject();

    void setGameDB(CGameFile * p) {
        m_gameFile = p;
    }

    void load(const int index);
    void save(const int index);

protected:
    void changeEvent(QEvent *e);
    void fillFrameCombo(int frameSet);    
    void setImage(int frameSet, int frameNo);
    QIcon makeIcon(void *png, int size);
    void updateAnimation(QTreeWidgetItem * item, int animationId);
    void freeImageSet();
    void clearAnimations();
    void reloadPaths();

    CGameFile * m_gameFile;
    int *m_classIndex;
    int m_objectIndex;

    // cache the animation sequences
    void *m_animations;

    // cache the path
    CPath * m_paths;

    // cache the imageSet
    void **m_imgPng;
    int *m_imgSize;
    int m_imgCount;

    // event cache
    QString *m_events;
    bool m_disableEvents;

    enum {
        MAX_DISPLAY_ICONS = 12
    };

    void setJumpText(int index);
    void populateCombo(uint8_t *values, uint valueCount, uint8_t selected, QComboBox *combo, const QString & zero, const QString & format);

private:
    Ui::CDlgObject *m_ui;

private slots:
    void on_treePaths_doubleClicked(QModelIndex index);
    void on_treeAnimations_doubleClicked(QModelIndex index);
    void on_btnClearAll_clicked();
    void on_treeEvents_doubleClicked(QModelIndex index);
    void on_cbClass_currentIndexChanged(int index);
    void on_cbBaseFrame_currentIndexChanged(int index);
    void on_cbFrameSet_currentIndexChanged(int index);
    void on_cNoChangeAtDeath_clicked();
    void on_btnAddObject_clicked();
    void on_cbJump_currentIndexChanged(int index);
    void on_cOverideSolid_clicked(bool checked);
    void on_eName_textChanged(const QString &arg1);
    void on_treePaths_customContextMenuRequested(const QPoint &pos);
    void restoreDefaultPaths();
    void clearPaths();
    void on_cInactive_clicked(bool checked);
    void editAnimation(int i);
    void clearAnimation(int i);
    void on_btnUuid_clicked();
    void on_cAutoTrigger_toggled(bool checked);
    void on_cTriggerCall_toggled(bool checked);
};

#endif // DLGOBJECT_H
