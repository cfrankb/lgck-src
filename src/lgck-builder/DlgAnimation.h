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

#ifndef DLGANIMATION_H
#define DLGANIMATION_H

#include <QDialog>
#include <QModelIndex>
#include <QIcon>
class CAnimation;

namespace Ui {
    class CDlgAnimation;
}

class CDlgAnimation : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgAnimation(QWidget *parent = 0);
    ~CDlgAnimation();

    void addIcon(void * png, int size);
    void addToSeq(int iconId, bool visualOnly=false);
    void updateButtons();

    void load(CAnimation * a, int seqId);
    void save(CAnimation * a);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CDlgAnimation *ui;
    uint8_t **m_icons;
    int *m_iconSizes;
    int m_iconMax;
    int m_iconCount;
    CAnimation *m_animation;
    enum {
        GROWBY = 50
    };
    static QString m_sequences[];
    static QString m_sequenceGroups[];
    static QString m_aims[];

private slots:
    void on_treeIcons_itemSelectionChanged();
    void on_treeSequence_itemSelectionChanged();
    void on_btClearAll_clicked();
    void on_btRemove_clicked();
    void on_btAdd_clicked();
    void on_btCopyAll_clicked();
    void on_treeSequence_doubleClicked(QModelIndex index);
    void on_treeIcons_doubleClicked(QModelIndex index);
};

#endif // DLGANIMATION_H
