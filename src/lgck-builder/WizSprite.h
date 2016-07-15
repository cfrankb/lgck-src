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

#ifndef WIZSPRITE_H
#define WIZSPRITE_H

#include <QWizard>

class CGameFile;
class CFrameSet;
class CWizPageEasy;
class QLineEdit;

namespace Ui {
    class CWizSprite;
}

class CWizSprite : public QWizard
{
    Q_OBJECT

public:

    explicit CWizSprite(QWidget *parent = 0);
    ~CWizSprite();

    void setGameFile(CGameFile *gf);
    void load(const int index);
    void save(const int index);
    CFrameSet * getFrameSet();
    virtual int nextId () const;
    virtual bool validateCurrentPage ();

private slots:

    void selectRadioNew();
    void on_cbClass_currentIndexChanged(int index);
    void on_btnAddImages_clicked();
    void on_btnDeleteImage_clicked();
    void on_treeWidget_itemSelectionChanged();

    void on_rbFrameSetOld_toggled(bool checked);
    void on_rbFrameSetNew_toggled(bool checked);
    void on_cNoChangeAtDeath_clicked(bool checked);
    void on_cbJump_currentIndexChanged(int index);
    void on_rbGenericMonster_clicked();
    void on_rbVampirePlant_clicked();
    void on_rbPlatformLeftRight_clicked();
    void on_rbPlatformUpDown_clicked();
    void on_rbWackerLeftRight_clicked();
    void on_rbWackerUoDown_clicked();
    void on_rbDroneLeftRight_clicked();
    void on_rbDroneUpDown_clicked();

    void on_cPlayerFire_clicked(bool checked);

signals:

    void imagesAdded();
    void imageDeleted();
    void refill();

private:

    void updateButtons();
    int getOption() const;
    bool isExpert() const;
    int getClass() const;
    int getMonsterClass() const;
    void setJumpText(int index);
    void updateMonsterDesc(int i);

    Ui::CWizSprite *ui;
    CGameFile *m_gameFile;
    int *m_classIndex;
    CFrameSet * m_frameSet;

    enum {
        page_intro,
        page_easy,
        page_class,
        page_obl5,
        page_player,
        page_door,
        page_features,
        page_death,
        page_auto,
        page_sounds,
        page_monsters,
        page_finish
    };

    enum {
        option_Animated,
        option_Background,
        option_Brick,
        option_Door,
        option_Key,
        option_Ladder,
        option_Lava,
        option_Monster,
        option_Object,
        option_Player,
        option_Switch
    };

    enum {
        GENERIC_MONSTER,
        VAMPIRE_PLANT,
        PLATFORM_LEFT,
        PLATFORM_UP,
        WACKER_LEFT,
        WACKER_UP,
        DRONE_LEFT,
        DRONE_UP
    };
};

#endif // WIZSPRITE_H
