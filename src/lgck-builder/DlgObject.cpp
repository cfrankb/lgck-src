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

#include "DlgObject.h"
#include "ui_DlgObject.h"
#include "DlgSource.h"
#include "DlgAnimation.h"
#include "DlgPath.h"
#include "DlgFrameSet.h"
#include <QMessageBox>
#include <QMenu>
#include "../shared/stdafx.h"
#include "../shared/GameFile.h"
#include "../shared/vlamits3.h"
#include "../shared/Animation.h"
#include "../shared/Snd.h"
#include "../shared/interfaces/IImageManager.h"
#include "../shared/qtgui/cheat.h"
#include "../shared/qtgui/qthelper.h"
#include "../shared/Frame.h"
#include "../shared/Path.h"

#define VALUES(x) x, sizeof(x)

CDlgObject::CDlgObject(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgObject)
{
    m_ui->setupUi(this);
    m_classIndex = 0;
    m_animations = (void*) new CAnimation [ CObject::getAnimationCount() ];
    m_imgPng = nullptr;
    m_imgSize = nullptr;
    m_imgCount = 0;
    m_events = new QString [ CProtoArray::getEventCount() ];
    m_paths = new CPath [ CObject::getPathCount() ];
    m_disableEvents = false;
}

void CDlgObject::freeImageSet()
{
    if (m_imgPng) {
        for (int i=0; i< m_imgCount; ++i) {
            delete [] ((uint8_t**) m_imgPng) [i];
        }
        delete [] (uint8_t**) m_imgPng;
        m_imgPng = nullptr;
    }

    if (m_imgSize) {
        delete [] m_imgSize;
        m_imgSize = nullptr;
    }
}

CDlgObject::~CDlgObject()
{
    delete m_ui;
    delete [] (CAnimation*) m_animations;
    delete [] (CPath*) m_paths;

    if (m_events) {
        delete [] m_events;
    }

    freeImageSet();
}

void CDlgObject::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CDlgObject::populateCombo(uint8_t *values, uint valueCount, uint8_t selected, QComboBox *combo, const QString & zero, const QString & format)
{
    bool first = true;
    for (uint32_t i=0; i < valueCount; ++i) {
        uint8_t c = values[i];
        if (selected < c && first) {
            combo->addItem(format.arg(selected));
            combo->setCurrentIndex(combo->count() - 1);
            first = false;
        }
        combo->addItem(c ? format.arg(c) : zero, c);
        if (c == selected) {
            combo->setCurrentIndex(combo->count() - 1);
            first = false;
        }
    }
}

void CDlgObject::load(const int index)
{
    CGameFile & gf = *((CGameFile*)m_gameFile);

    m_disableEvents = true;

    // page 1 ****************************************************************

    // dialog box title

    m_objectIndex = index;
    CProto & proto = gf.m_arrProto[index];
    CObject & object = gf.m_arrProto.getObject( index );
    QString title = QString(tr("Properties for \"%1\" [%2]")).arg(proto.m_szName).arg(proto.m_uuid);
    setWindowTitle(title);

    // name

    m_ui->eName->setText(proto.m_szName);

    // image

    setImage(proto.m_nFrameSet, proto.m_nFrameNo);

    // frame set

    for (int n=0; n < gf.frames().getSize(); ++n) {
        CFrameSet & frameSet = gf.toFrameSet(n);
        QIcon icon = frame2icon(gf.toFrame(n, 0));
        m_ui->cbFrameSet->addItem(icon, frameSet.getName() );
    }

    m_ui->cbFrameSet->setCurrentIndex( proto.m_nFrameSet );

    // frame no

    fillFrameCombo( proto.m_nFrameSet );

    m_ui->cbBaseFrame->setCurrentIndex( proto.m_nFrameNo );

    // classes

    m_classIndex = new int [ CGameFile::MAX_CLASSES ];
    bool classFound = false;
    int j = 0;
    for (int i=0; i < CGameFile::MAX_CLASSES; ++i) {
        if (!gf.m_className[i].empty()) {
            m_classIndex [ j ] = i;
            m_ui->cbClass->addItem(gf.m_className[i].c_str());
            if (proto.m_nClass == i) {
                m_ui->sClass->setText(gf.m_classInfo[i].c_str());
                m_ui->cbClass->setCurrentIndex( j );
                classFound =  true;
            }
            j++;
        }
    }

    if (!classFound) {
        QString s = QString(tr("Unknown class %1").arg(proto.m_nClass));
        m_ui->cbClass->addItem(s);
        m_classIndex[ j ] = proto.m_nClass;
        m_ui->cbClass->setCurrentIndex( j );
    }

    // animate

    //m_ui->cAnimate->setChecked( proto.m_bAnimate != 0 );

    // no smart map

    m_ui->cNoSmartMap->setChecked( proto.m_bNoSmartMap != 0);

    // page 2 ****************************************************************

    // speeds

    for (int n = 0; n < 16; ++n) {
        QString s = n ? tr("Speed %1").arg(n) : tr("(fast)");
        m_ui->cbAnimateSpeed->addItem(s);
        m_ui->cbMoveSpeed->addItem(s);
        m_ui->cbFallSpeed->addItem(s);
    }

    for (int n = 0; n < 256; ++n) {
        if (n) {
            QString s = QString(tr("%1")).arg(n);
            m_ui->cbMaxFall->addItem(s);
            s = QString(tr("Speed %1")).arg(n);
            m_ui->cbFireRate->addItem(s);
        } else {
            m_ui->cbMaxFall->addItem(tr("n/a"));
            m_ui->cbFireRate->addItem(tr("(fast)"));
        }
    }

    m_ui->cbAnimateSpeed->setCurrentIndex(proto.m_nAniSpeed);
    m_ui->cbMoveSpeed->setCurrentIndex(proto.m_nMoveSpeed);
    m_ui->cbFallSpeed->setCurrentIndex(proto.m_nFallSpeed);
    m_ui->cbMaxFall->setCurrentIndex(proto.m_nMaxFall);
    m_ui->cbFireRate->setCurrentIndex(proto.m_nFireRate);

    // aims

    QString aims[] = {
        tr("UP"), tr("DOWN"), tr("LEFT"), tr("RIGHT")
    };
    for (int i = 0; i < 4; ++i) {
        m_ui->cbDefaultAim->addItem(aims[i], i);
    }
    m_ui->cbDefaultAim->setCurrentIndex(proto.m_nDefaultAim);

    // damages
    uint8_t damageOptions[] = {0,1,2,3,4,5,10,12,15,20,25,30,35,40,50,75,100,125,150,200,240,250,255};
    populateCombo(VALUES(damageOptions),proto.m_nDamages, m_ui->cbDamages, tr("(none)"), tr("%1 pts"));

    // BonusHP (hit points)
    uint8_t hpOptions[] = {0,1,2,3,4,5,10,12,15,20,25,30,35,40,50,75,100,125,150,200,240,250,255};
    populateCombo(VALUES(hpOptions),proto.m_nBonusHP, m_ui->cbBonusHP, "", tr("+%1 pts"));
    populateCombo(VALUES(hpOptions),proto.m_nHP, m_ui->cbHP, tr("(none)"), tr("%1 pts"));

    // bonus coins, lives and ammo
    uint8_t coinsOptions[] = {0, 1, 2, 3, 4, 5, 10, 20, 25, 50, 100, 200, 250};
    uint8_t livesOptions[] = {0, 1, 2, 3, 4, 5, 10, 20, 25, 50};
    uint8_t ammoOptions[] = {0, 1, 2, 3, 4, 5, 10, 15, 20, 25, 30, 40, 50, 100, 150, 200, 250, 255};
    populateCombo(VALUES(coinsOptions), proto.m_coinsBonus, m_ui->cbCoins, "", QString("+%1"));
    populateCombo(VALUES(livesOptions), proto.m_livesBonus, m_ui->cbLives, "", QString("+%1"));
    populateCombo(VALUES(ammoOptions), proto.m_ammoBonus, m_ui->cbAmmo, "", QString("+%1"));

    // power level
    for (int n = 0; n < 255; ++n) {
        if (n != 0) {
            m_ui->cbPowerLevel->addItem(tr("Level %1").arg(n));
        } else {
            m_ui->cbPowerLevel->addItem(tr("(ignored)"));
        }
    }
    m_ui->cbPowerLevel->setCurrentIndex(proto.m_nPowerLevel);

    // max bullets
    for (int n = 0; n <= 255; ++n) {
        if (n != 0) {
            m_ui->cbMaxBullets->addItem(QString("%1").arg(n));
        } else {
            m_ui->cbMaxBullets->addItem(tr("none"));
        }
    }

    m_ui->cbMaxBullets->setCurrentIndex(proto.m_nMaxBullets);

    // protoBuddy - implemented on page 3

    // page 3 ****************************************************************

    // no gravity
    m_ui->cNoGravity->setChecked( proto.getOption(CProto::OPTION_NO_GRAVITY) );

    // destroy on use
    m_ui->cDestroyUsed->setChecked( proto.getOption(CProto::OPTION_DESTROY_ON_USE) );

    // solid
    m_ui->cSolid->setChecked( proto.getOption(CProto::OPTION_SOLID) );

    // systemInvisible
    m_ui->cInvisible->setChecked( proto.getOption(CProto::OPTION_INVISIBLE) );

    // inactive
    m_ui->cInactive->setChecked( proto.getOption(CProto::OPTION_INACTIVE) );
    on_cInactive_clicked(proto.getOption(CProto::OPTION_INACTIVE));
    m_ui->cbInactive->addItem(tr("auto"));
    m_ui->cbInactive->addItem(tr("userdef"));
    if (proto.getOption(CProto::OPTION_INACTIVE_USERDEF)) {
        m_ui->cbInactive->setCurrentIndex(1);
    } else {
        m_ui->cbInactive->setCurrentIndex(0);
    }
    m_ui->cAdvancedHandling->setChecked( proto.getOption(CProto::OPTION_ADVANCED));

    // page 3 ****************************************************************

    // points

    int points[]= {
        0,
        10,
        25,
        50,
        100,
        200,
        300,
        400,
        500,
        1000,
        2000,
        5000,
        10000
    };   

    for (int i=0; i < (int) (sizeof(points) / sizeof(int)); ++i) {
        m_ui->cbPoints->addItem(QString("%1").arg(points[i]));
    }

    m_ui->cbPoints->setCurrentIndex(proto.m_nPoints);

    // sounds

    for (int i=0; i < gf.m_arrSounds.getSize(); ++i) {
        m_ui->cbPlaySound->addItem(gf.m_arrSounds[i]->getName());
        // page 4
        m_ui->cbAutoSound->addItem(gf.m_arrSounds[i]->getName());
        m_ui->cbSound_bullet->addItem(gf.m_arrSounds[i]->getName());
    }

    m_ui->cbPlaySound->setCurrentIndex(proto.m_nChSound);
    m_ui->cbSound_bullet->setCurrentIndex(proto.m_bulletSound);
    // proto (changeto)

    for (int i = 0; i < gf.m_arrProto.getSize(); ++i){
        CProto & protoSpr = gf.m_arrProto[i];
        QIcon icon = frame2icon(gf.toFrame(protoSpr.m_nFrameSet, protoSpr.m_nFrameNo));
        m_ui->cbChangeTo->addItem(icon, protoSpr.getName());

        // from page 2
        m_ui->cbBuddy->addItem(icon, protoSpr.getName(), i);
        if (i == 0 || protoSpr.m_nClass == CLASS_PLAYER_BULLET) {
            m_ui->cbBullet->addItem(icon, protoSpr.getName(), i);
            if (proto.m_nProtoBuddy == i) {
                m_ui->cbBullet->setCurrentIndex(m_ui->cbBullet->count() - 1);
            }
        }

        // from page 4
        m_ui->cbAutoBullet->addItem(icon, protoSpr.getName());
        m_ui->cbAutoChangeTo->addItem(icon, protoSpr.getName());
    }

    m_ui->cNoChangeAtDeath->setChecked( proto.getOption(CProto::OPTION_NO_SHIFT_AT_DEATH) );
    m_ui->cNoSplat->setChecked( proto.getOption(CProto::OPTION_NO_SPLAT) );
    m_ui->cAutomaticGoal->setChecked(proto.getOption(CProto::OPTION_AUTO_GOAL) );

    if (m_ui->cNoChangeAtDeath->isChecked()) {
        m_ui->cbChangeTo->setEnabled( false );
    }

    m_ui->cbChangeTo->setCurrentIndex( proto.m_nChProto );
    // from page 2
    m_ui->cbBuddy->setCurrentIndex(proto.m_nProtoBuddy);

    // nbr of rebirth

    for (int n = 0; n < 255; ++ n) {
        if (n == 1) {
            m_ui->cbRebirths->addItem(tr("%1 time").arg(n));
        } else if (n > 1) {
            m_ui->cbRebirths->addItem(tr("%1 times").arg(n));
        } else {
            m_ui->cbRebirths->addItem(tr("(none)"));
        }
    }

    m_ui->cbRebirths->setCurrentIndex( proto.m_nRebirths );

    // rebirthdelay

    for (int n = 0; n < 255; ++n) {
        if (n != 0) {
            m_ui->cbRebirthDelay->addItem(tr("Speed %1").arg(n));
        } else {
            m_ui->cbRebirthDelay->addItem(tr("(fast)"));
        }
    }

    m_ui->cbRebirthDelay->setCurrentIndex( proto.m_nRbDelay );

    // rebirth location
    const QString locations[] = {
        tr("Origin"),
        tr("Deathbed"),
        tr("Random")
    };

    for (unsigned int n = 0; n < sizeof(locations)/sizeof(QString); ++n) {
        m_ui->cbRebirthLocation->addItem(locations[n]);
    }

    // TODO: set the proper location here
    m_ui->cbRebirthLocation->setCurrentIndex(proto.m_rebirthLocation);

    const QString jumpModes [] = {
        tr("VLA3"),
        tr("GIANA"),
        tr("MIXED"),
    };

    for (unsigned int n = 0; n < sizeof(jumpModes) / sizeof(QString); ++n) {
        m_ui->cbJump->addItem(jumpModes[n]);
    }

    m_ui->cbJump->setCurrentIndex( proto.m_nJumpMode );
    setJumpText( proto.m_nJumpMode );

    // page 4 ****************************************************************

    // auto changeTo
    // auto bullet
    // auto sound
    // see page 3

    m_ui->cbAutoChangeTo->setCurrentIndex(proto.m_nAutoProto);
    m_ui->cbAutoBullet->setCurrentIndex(proto.m_nAutoBullet);
    m_ui->cbAutoSound->setCurrentIndex(proto.m_nAutoSound);

    // timers

    for (int n = 0; n < 256; ++n) {
        QString s = n ? tr("Speed %1").arg(n) : tr("(never)");
        m_ui->cbAutoSoundDelay->addItem(s);
        m_ui->cbAutoBulletDelay->addItem(s);
        m_ui->cbAutoChangeToDelay->addItem(s);
        m_ui->cbAutoTriggerDelay->addItem(s);
    }

    m_ui->cbAutoSoundDelay->setCurrentIndex(proto.m_nAutoSoundTime);
    m_ui->cbAutoBulletDelay->setCurrentIndex(proto.m_nAutoBulletTime);
    m_ui->cbAutoChangeToDelay->setCurrentIndex(proto.m_nAutoProtoTime);
    m_ui->cbAutoTriggerDelay->setCurrentIndex(proto.m_nAutoTriggerTime);

    // auto trigger checkbox
    m_ui->cAutoTrigger->setChecked(proto.m_bAutoTrigger != 0);

    // trigger
    m_ui->cTrigger->setChecked(proto.getOption(CProto::OPTION_NO_TRIGGER_FLIP));
    m_ui->cTriggerCall->setChecked(proto.getOption(CProto::OPTION_NO_TRIGGER_CALL));

    // page 5 ****************************************************************

    m_ui->treeEvents->setColumnCount(2);
    m_ui->treeEvents->setColumnWidth(0, 128);
    m_ui->treeEvents->setColumnWidth(1, 128);
    m_ui->treeEvents->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->treeEvents->setWordWrap(false);
    m_ui->treeEvents->setRootIsDecorated(false);
    m_ui->treeEvents->setAlternatingRowColors(true);

    for (int i = 0; i < object.getEventCount(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, CProtoArray::getEventName(i));
        // copy event to local copy
        m_events[i] = object.getEvent(i);
        if (object.getEvent(i)[0]) {
            item->setText(1, tr("[event procedure]"));
        }
        m_ui->treeEvents->addTopLevelItem(item);
    }

    // animation ad-hoc ******************************************************

    m_ui->treeAnimations->setColumnCount(MAX_DISPLAY_ICONS + 2);
    m_ui->treeAnimations->setColumnWidth(0, 128);
    for (int i = 1; i < MAX_DISPLAY_ICONS + 2; ++i) {
        m_ui->treeAnimations->setColumnWidth(i, 16);
    }
    m_ui->treeAnimations->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->treeAnimations->setWordWrap(false);
    m_ui->treeAnimations->setRootIsDecorated(false);
    m_ui->treeAnimations->setAlternatingRowColors(true);

    CAnimation * animations = (CAnimation*) m_animations;

    for (int i = 0; i < CObject::getAnimationCount(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, CObject::getAnimationName(i));
        animations [ i ] = object.getAnimation(i) ;
        updateAnimation(item, i);
        m_ui->treeAnimations->addTopLevelItem(item);
    }

    // paths ad-hoc **********************************************************

    // copy the path to Dlg local
    CPath * paths = (CPath*) m_paths;
    for (int i = 0; i < CObject::getPathCount(); ++i) {
        paths [ i ] = object.getPath(i) ;
    }

    // reload paths from local
    reloadPaths();

    // solidity ***************************************************************

    m_ui->cOverideSolid->setChecked( proto.m_solidState & CProto::SOLID_OVERIDE );
    m_ui->gSolid->setEnabled( (proto.m_solidState & CProto::SOLID_OVERIDE) != 0);

    m_ui->cSolidUP->setChecked( proto.m_solidState & CProto::SOLID_UP );
    m_ui->cSolidDOWN->setChecked( proto.m_solidState & CProto::SOLID_DOWN );
    m_ui->cSolidLEFT->setChecked( proto.m_solidState & CProto::SOLID_LEFT );
    m_ui->cSolidRIGHT->setChecked( proto.m_solidState & CProto::SOLID_RIGHT );

    m_disableEvents = false;

    // connect slots
    m_ui->treeAnimations->connectSlots(this);

    // bullet options
    m_ui->cBulletEnabled->setChecked(proto.m_bulletOptions & CProto::BULLET_ENABLED);
}

void CDlgObject::save(const int index)
{
    CGameFile & gf = *((CGameFile*)m_gameFile);

    // page 1 ****************************************************************

    // dialog box title

    CProto & proto = gf.m_arrProto[index];
    CObject & object = gf.m_arrProto.getObject( index );

    // name

    proto.m_szName[31] = 0;
    memccpy(proto.m_szName, q2c(m_ui->eName->text().trimmed()), 0, 31);

    // frame set

    proto.m_nFrameSet = m_ui->cbFrameSet->currentIndex();

    // frame no

    proto.m_nFrameNo = m_ui->cbBaseFrame->currentIndex();

    // classes

    proto.m_nClass = m_classIndex[ m_ui->cbClass->currentIndex() ];

    // noSmartMap

    proto.m_bNoSmartMap = (int) m_ui->cNoSmartMap->checkState () == Qt::Checked;

    // page 2 ****************************************************************

    // speeds

    proto.m_nAniSpeed = m_ui->cbAnimateSpeed->currentIndex();
    proto.m_nMoveSpeed = m_ui->cbMoveSpeed->currentIndex();
    proto.m_nFallSpeed = m_ui->cbFallSpeed->currentIndex();
    proto.m_nMaxFall = m_ui->cbMaxFall->currentIndex();
    proto.m_nMaxBullets = m_ui->cbMaxBullets->currentIndex();
    proto.m_nFireRate = m_ui->cbFireRate->currentIndex();

    // aims

    proto.m_nDefaultAim = m_ui->cbDefaultAim->currentIndex();

    // damages

    proto.m_nDamages = m_ui->cbDamages->currentData().toUInt();

    // hit points

    proto.m_nBonusHP = m_ui->cbBonusHP->currentData().toUInt();
    proto.m_nHP = m_ui->cbHP->currentData().toUInt();

    // coins, lives and ammo
    proto.m_coinsBonus = m_ui->cbCoins->currentData().toUInt();
    proto.m_livesBonus = m_ui->cbLives->currentData().toUInt();
    proto.m_ammoBonus = m_ui->cbAmmo->currentData().toUInt();

    // power level

    proto.m_nPowerLevel = m_ui->cbPowerLevel->currentIndex();

    // proto buddy

    if (proto.m_nClass==CLASS_PLAYER_OBJECT) {
        proto.m_nProtoBuddy = m_ui->cbBullet->currentData().toUInt();
    } else {
        proto.m_nProtoBuddy = m_ui->cbBuddy->currentIndex();
    }
    proto.m_bulletSound = m_ui->cbSound_bullet->currentIndex();

    // page 3 ****************************************************************

    // no gravity

    proto.setOption(CProto::OPTION_NO_GRAVITY, m_ui->cNoGravity->checkState () == Qt::Checked);

    // destroy on use

    proto.setOption(CProto::OPTION_DESTROY_ON_USE, m_ui->cDestroyUsed->checkState () == Qt::Checked);

    // solid

    proto.setOption(CProto::OPTION_SOLID, m_ui->cSolid->checkState () == Qt::Checked);

    // system Invisible

    proto.setOption(CProto::OPTION_INVISIBLE, m_ui->cInvisible->checkState () == Qt::Checked);

    // inactive

    proto.setOption(CProto::OPTION_INACTIVE, m_ui->cInactive->checkState () == Qt::Checked);
    proto.setOption(CProto::OPTION_INACTIVE_USERDEF, m_ui->cbInactive->currentIndex()!=0);

    // isChangeAtDeath disabled
    proto.setOption(CProto::OPTION_NO_SHIFT_AT_DEATH, m_ui->cNoChangeAtDeath->checkState () == Qt::Checked);

    // noSplat (crush by player)
    proto.setOption(CProto::OPTION_NO_SPLAT, m_ui->cNoSplat->checkState () == Qt::Checked);

    // advanced options
    proto.setOption(CProto::OPTION_ADVANCED, m_ui->cAdvancedHandling->isChecked());

    // jumpMode

    proto.m_nJumpMode = m_ui->cbJump->currentIndex();

    // page 4 ****************************************************************

    // points

    proto.m_nPoints = m_ui->cbPoints->currentIndex();

    // sounds

    proto.m_nChSound = m_ui->cbPlaySound->currentIndex();

    // proto (changeto)

    proto.m_nChProto = m_ui->cbChangeTo->currentIndex();

    // nbr of rebirth

    proto.m_nRebirths = m_ui->cbRebirths->currentIndex();

    // rebirthdelay

    proto.m_nRbDelay = m_ui->cbRebirthDelay->currentIndex();

    // rebirthLocation

    proto.m_rebirthLocation = m_ui->cbRebirthLocation->currentIndex();

    // page 5 ****************************************************************

    // auto changeTo
    // auto bullet
    // auto sound

    proto.m_nAutoBullet = m_ui->cbAutoBullet->currentIndex();
    proto.m_nAutoProto = m_ui->cbAutoChangeTo->currentIndex();
    proto.m_nAutoSound = m_ui->cbAutoSound->currentIndex();

    // timers

    proto.m_nAutoSoundTime = m_ui->cbAutoSoundDelay->currentIndex();
    proto.m_nAutoBulletTime = m_ui->cbAutoBulletDelay->currentIndex();
    proto.m_nAutoProtoTime = m_ui->cbAutoChangeToDelay->currentIndex();
    proto.m_nAutoTriggerTime = m_ui->cbAutoTriggerDelay->currentIndex();

    // auto trigger checkbox
    if (m_ui->cAutoTrigger->checkState() == Qt::Checked) {
        proto.m_bAutoTrigger = 1;
    } else {
        proto.m_bAutoTrigger = 0;
    }

    // trigger
    proto.setOption(CProto::OPTION_NO_TRIGGER_FLIP, m_ui->cTrigger->checkState () == Qt::Checked);
    proto.setOption(CProto::OPTION_NO_TRIGGER_CALL, m_ui->cTriggerCall->checkState () == Qt::Checked);

    // set autoGoal
    proto.setOption(CProto::OPTION_AUTO_GOAL, m_ui->cAutomaticGoal->checkState () == Qt::Checked);

    // page 6 ****************************************************************

    // events

    for (int i=0; i < object.getEventCount(); ++i) {
         object.setEvent(i, q2c(m_events[i]));
     }

    // animation ad-hoc (save)

    CAnimation * animations = (CAnimation*) m_animations;
    for (int i=0; i < CObject::getAnimationCount(); ++i) {
        object.setAnimation(i, animations [ i ]);
    }

    // path ad-hoc (save)

    CPath *paths = (CPath *) m_paths;
    for (int i=0; i < CObject::getPathCount(); ++i) {
        object.getPath(i) = paths [ i ];
    }

    // solidity ***************************************************************

    proto.m_solidState =  m_ui->cOverideSolid->isChecked() * CProto::SOLID_OVERIDE
            | m_ui->cSolidUP->isChecked() * CProto::SOLID_UP
            | m_ui->cSolidDOWN->isChecked() * CProto::SOLID_DOWN
            | m_ui->cSolidLEFT->isChecked() * CProto::SOLID_LEFT
            | m_ui->cSolidRIGHT->isChecked() * CProto::SOLID_RIGHT;

    // bullet options
    proto.m_bulletOptions = m_ui->cBulletEnabled->isChecked() ? CProto::BULLET_ENABLED : 0;

}

void CDlgObject::setImage(int frameSet, int frameNo)
{
    m_ui->sImage->setPixmap(frame2pixmap(m_gameFile->toFrame(frameSet, frameNo)));
}

void CDlgObject::fillFrameCombo(int frameSet)
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    CFrameSet & fs = gf.toFrameSet(frameSet);

    m_ui->cbBaseFrame->clear();

    freeImageSet();

    m_imgCount = fs.getSize();
    m_imgPng = (void**)new  uint8_t ** [m_imgCount];
    m_imgSize = new int [m_imgCount];

    for (int i=0; i < m_imgCount; ++i) {
        fs[i]->toPng( ((uint8_t**)m_imgPng)[i], m_imgSize[i]);
        QIcon icon = makeIcon(m_imgPng[i], m_imgSize[i]);
        m_ui->cbBaseFrame->addItem(icon, QString("%1").arg(i + 1));
    }
}

void CDlgObject::on_cbFrameSet_currentIndexChanged(int index)
{
    if (!m_disableEvents) {
        fillFrameCombo( index );
        m_ui->cbBaseFrame->setCurrentIndex(0);
        setImage(index, 0);
        clearAnimations();
    }
}

void CDlgObject::on_cbBaseFrame_currentIndexChanged(int index)
{
    if (!m_disableEvents) {
        int frameSet = m_ui->cbFrameSet->currentIndex();
        setImage(frameSet, index);
    }
}

void CDlgObject::on_cbClass_currentIndexChanged(int index)
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    int classId = m_classIndex [ index ];
    if (!gf.m_className[ classId ].empty()) {
        m_ui->sClass->setText(gf.m_classInfo[ classId ].c_str());
    } else {
        m_ui->sClass->setText(tr("This could be a custom class."));
    }

    int tabCount = m_ui->tab->count();
    for (int i=2; i <= tabCount; ++i) {
        m_ui->tab->removeTab(1);
    }

    if (classId >= 0x10) {
        m_ui->tab->addTab(m_ui->tab_2, QIcon(), tr("Features"));
        m_ui->tab->addTab(m_ui->tab_3, QIcon(), tr("Animation"));

        if (classId == CLASS_PLAYER_OBJECT) {
            m_ui->tab->addTab(m_ui->tab_7, QIcon(), tr("Misc"));
        } else {
            if (classId < CLASS_GENERIC_COS) {
                m_ui->tab->addTab(m_ui->tab_9, QIcon(), tr("Solidity"));
            }
        }

        if (classId == CLASS_OPEN_TO_OWNER) {
            m_ui->tab->addTab(m_ui->tab_8, QIcon(), tr("Door"));
        }

        m_ui->tab->addTab(m_ui->tab_4, QIcon(), tr("Death"));
        m_ui->tab->addTab(m_ui->tab_5, QIcon(), tr("Auto"));
        m_ui->tab->addTab(m_ui->tab_6, QIcon(), tr("Events"));
    }
}

void CDlgObject::on_treeEvents_doubleClicked(QModelIndex index)
{
    CDlgSource *d = new CDlgSource ( (QWidget*) parent());
    d->init(m_gameFile);
    d->setWindowTitle(tr("Edit Event `%1`").arg(CProtoArray::getEventName(index.row())));
    CGameFile & gf = *((CGameFile*)m_gameFile);
    d->setText(m_events[index.row()]);
    if (d->exec()) {
        m_events[ index.row() ] = d->getText();
        QTreeWidgetItem * item = m_ui->treeEvents->topLevelItem( index.row() );
        if (d->getText().isEmpty()) {
            item->setText(1, "");
        } else {
            item->setText(1, tr("[event procedure]"));
        }
        gf.setDirty( true );
    }
    delete d;
}

void CDlgObject::on_btnClearAll_clicked()
{
    // TODO: implement this

    if (QMessageBox::warning(this, "", tr("Are you sure that you want to clear all the event handler for this sprite?"),
                             QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {

        CGameFile & gf = *((CGameFile*)m_gameFile);
        for (int i=0; i < CProtoArray::getEventCount(); ++i) {
            if (m_events[i] != "") {
                m_events[i] = "";
                gf.setDirty( true );
                QTreeWidgetItem *item = m_ui->treeEvents->topLevelItem(i);
                item->setText(1, tr(""));
            }
        }
    }
}

void CDlgObject::on_treeAnimations_doubleClicked(QModelIndex index)
{
    editAnimation(index.row());
}

void CDlgObject::editAnimation(int idx)
{
    if (idx != -1) {
        CAnimation * animations = (CAnimation*) m_animations;
        CAnimation & animation = animations [ idx ];

        CDlgAnimation *dlg = new CDlgAnimation( (QWidget*) parent());
        QString s = QString(tr("Edit Animation `%1`")).arg(CObject::getAnimationName(idx));
        dlg->setWindowTitle( s );

        int frameSet = m_ui->cbFrameSet->currentIndex();

        CGameFile & gf = *((CGameFile*)m_gameFile);
        CFrameSet & fs = gf.toFrameSet(frameSet);

        int count = fs.getSize();
        for (int i=0; i < count; ++i) {
            dlg->addIcon((void*)m_imgPng[i], m_imgSize[i]);
        }

        dlg->load(&animation, idx);
        dlg->updateButtons();

        if (dlg->exec()) {
            dlg->save(&animation);
            QTreeWidgetItem * item = m_ui->treeAnimations->topLevelItem( idx );
            updateAnimation(item, idx);
            gf.setDirty( true );
        }
        delete dlg;
    }
}

void CDlgObject::clearAnimation(int i)
{
    CAnimation * animations = (CAnimation*) m_animations;
    CGameFile & gf = *((CGameFile*)m_gameFile);
    QTreeWidgetItem *item = m_ui->treeAnimations->topLevelItem(i);
    animations [ i ].forget();
    updateAnimation(item, i);
    gf.setDirty( true );
}

void CDlgObject::updateAnimation(QTreeWidgetItem * item, int animationId)
{
    CAnimation * animations = (CAnimation*) m_animations;
    CAnimation & animation = animations [ animationId ];

    for (int j=0; j < MAX_DISPLAY_ICONS; ++j) {
        if (j < animation.getSize()) {
            int k = animation[j];
            if (k < m_imgCount && k >= 0) {
                QIcon icon = makeIcon(m_imgPng[k], m_imgSize[k]);
                item->setIcon(j + 1, icon);
                item->setText(j + 1, "");
            } else {
                QIcon icon;
                item->setIcon(j + 1, icon);
                item->setText(j + 1, "?");
            }
        } else {
            QIcon icon;
            item->setIcon(j + 1, icon);
            item->setText(j + 1, "");
        }
    }

    if (animation.getSize()>MAX_DISPLAY_ICONS) {
        item->setText(MAX_DISPLAY_ICONS + 1, "...");
    } else {
        item->setText(MAX_DISPLAY_ICONS + 1, "");
    }
}

QIcon CDlgObject::makeIcon(void *png, int size)
{
    QImage img;
    if (!img.loadFromData( (uint8_t*)png, size )) {
        qWarning("failed to load png");
    }

    QPixmap pm = QPixmap::fromImage(img);
    QIcon icon;
    icon.addPixmap(pm, QIcon::Normal, QIcon::On);
    return icon;
}

void CDlgObject::on_treePaths_doubleClicked(QModelIndex index)
{
    CPath *paths = (CPath*) m_paths;
    int idx = index.row();
    if (idx != -1) {
        CPath & path = paths[idx];
        CDlgPath *dlg = new CDlgPath( (QWidget*) parent());
        QString s = QString(tr("Edit Path `%1`")).arg(CObject::getPathName(idx));
        dlg->setWindowTitle( s );
        dlg->load(&path);

        if (dlg->exec()) {
            dlg->save(&path);
            QTreeWidgetItem * item = m_ui->treePaths->topLevelItem( idx );
            QString s;
            if (paths[idx].getSize() != 0) {
              s = QString(tr("%1 move(s)")).arg(paths[idx].getSize());
            }
            item->setText(1, s);
            CGameFile & gf = *((CGameFile*)m_gameFile);
            gf.setDirty( true );
        }

        delete dlg;
    }
}

void CDlgObject::on_cNoChangeAtDeath_clicked()
{
    m_ui->cbChangeTo->setEnabled( !m_ui->cNoChangeAtDeath->isChecked() );
}

void CDlgObject::on_btnAddObject_clicked()
{
    CDlgFrameSet * d = new CDlgFrameSet (this);
    d->setWindowTitle ( tr("New Image Set") );
    CFrameSet fs;
    d->init(& fs);
    if (d->exec() == QDialog::Accepted) {
        d->save();
        qDebug("on_btnAddFrameSet_clicked() -> save\n");

        // add new FrameSet
        CGameFile & gf = *m_gameFile;

        int frameSet = gf.frames().getSize();
        gf.frames().add(new CFrameSet (&fs));

        // add this new imageSet to the cache
        gf.cache()->add( &fs);

        // add new ImageSet UI
        QIcon icon = frame2icon(* fs[0]);
        m_ui->cbFrameSet->addItem(icon, fs.getName() );
        m_ui->cbFrameSet->setCurrentIndex(frameSet);

        fillFrameCombo (frameSet);
        setImage(frameSet, 0);
        clearAnimations();

        gf.setDirty( true );
    }

    delete d;
}

void CDlgObject::clearAnimations()
{
    CAnimation * animations = (CAnimation*) m_animations;
    CGameFile & gf = *((CGameFile*)m_gameFile);
    for (int i = 0; i < CObject::getAnimationCount(); ++i) {
        QTreeWidgetItem *item = m_ui->treeAnimations->topLevelItem(i);
        animations [ i ].forget();
        updateAnimation(item, i);
        gf.setDirty( true );
    }
}


void CDlgObject::on_cbJump_currentIndexChanged(int index)
{
    setJumpText(index);
}

void CDlgObject::setJumpText(int index)
{
    QString jumpMode[] = {
        tr("This is the engine default; you need to press JUMP + AIM. This mode allows you to climb ladders."),
        tr("Press UP to jump and move LEFT/RIGHT. You cannot climb ladders."),
        tr("Press JUMP and move LEFT/RIGHT. And still get to climb ladders. :)")
    };

    m_ui->sJumo->setText(jumpMode[index]);
}

void CDlgObject::on_cOverideSolid_clicked(bool checked)
{
    m_ui->gSolid->setEnabled( checked );
}

void CDlgObject::on_eName_textChanged(const QString &arg1)
{
    QPushButton *button=m_ui->buttonBox->button(QDialogButtonBox::Save);
    button->setEnabled(!arg1.trimmed().isEmpty());
}

void CDlgObject::on_treePaths_customContextMenuRequested(const QPoint &pos){
    // create pop-up
    QMenu menu(this);

    QAction *actionDefaultPaths = new QAction(tr("Restore default paths"), &menu);
    menu.addAction(actionDefaultPaths);

    QAction *actionClearPaths = new QAction(tr("Clear All"), &menu);
    menu.addAction(actionClearPaths);

    connect(actionDefaultPaths, SIGNAL(triggered()),
            this, SLOT(restoreDefaultPaths()));

    connect(actionClearPaths, SIGNAL(triggered()),
            this, SLOT(clearPaths()));

    menu.exec(m_ui->treePaths->mapToGlobal(pos));
}

void CDlgObject::clearPaths()
{
    if (QMessageBox::warning(this, "", tr("Are you sure that you want to clear all the paths for this sprite?"),
                             QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {
        // FIXME: assume doc is dirty
        m_gameFile->setDirty( true );
        m_paths->forget();
        reloadPaths();
    }
}

void CDlgObject::restoreDefaultPaths()
{
    if (QMessageBox::warning(this, "", tr("Are you sure that you want to restore the default paths for this sprite?"),
                             QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {
        // FIXME: assume doc is dirty
        m_gameFile->setDirty( true );

        // add jump sequences

        typedef struct {
            int id_seq;
            const char *path;
        } JUMP_SEQ;

        JUMP_SEQ pathVLA3 [] =  {
            { CObject::PS_JUMP_UP,      "00 00 00 00 00 ff 00 00 00 00 00 ff 01 01 01 01 01 ff 01 01 01 01 01" },
            { CObject::PS_JUMP_DOWN,    ""},
            { CObject::PS_JUMP_LEFT,    "00 00 02 00 00 ff 00 02 02 02 00 ff 01 02 02 02 01 ff 01 01 02 01 01" },
            { CObject::PS_JUMP_RIGHT,   "00 00 03 00 00 ff 00 03 03 03 03 00 ff 01 03 03 03 01 ff 01 01 03 01 01" },
            { CObject::PS_JUMP_UP_LEFT, "00 00 00 00 00 ff 00 00 02 02 02 ff 01 01 02 02 02 02 ff 01 01 01 01 01" },
            { CObject::PS_JUMP_UP_RIGHT,"00 00 00 00 00 ff 00 00 03 03 03 ff 01 01 03 03 03 03 ff 01 01 01 01 01" },
            { CObject::PS_JUMP_DN_LEFT, "00 00 02 02 02 ff 00 00 02 02 02 ff 02 02 02 02 02 ff 02 01 01 01 01" },
            { CObject::PS_JUMP_DN_RIGHT,"00 00 03 03 03 ff 00 00 03 03 03 ff 03 03 03 03 03 ff 03 01 01 01 01" },
        };

        m_paths->forget();

        for (unsigned i=0; i < sizeof(pathVLA3)/ sizeof(JUMP_SEQ); ++i) {
            JUMP_SEQ & js = pathVLA3[i];
            QString sq = QString(js.path);

            // decode the string
            for (unsigned int k=0; k < strlen(js.path);k +=3) {
                char aim = strtol(q2c(sq.mid(k, 2)), nullptr, 16);
                m_paths[js.id_seq].add(aim);
            }
        }

        reloadPaths();
    }
}

void CDlgObject::reloadPaths()
{
    m_ui->treePaths->clear();
    m_ui->treePaths->setColumnCount( 2);
    m_ui->treePaths->setColumnWidth(0, 100);
    m_ui->treePaths->setColumnWidth(1, 70);
    m_ui->treePaths->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->treePaths->setWordWrap(false);
    m_ui->treePaths->setRootIsDecorated(false);
    m_ui->treePaths->setAlternatingRowColors(true);

    CPath * paths = m_paths;
    for (int i = 0; i < CObject::getPathCount(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, CObject::getPathName(i));
        QString s;
        if (paths[i].getSize() != 0) {
          s = QString(tr("%1 move(s)")).arg(paths[i].getSize());
          item->setText(1, s);
        }
        m_ui->treePaths->addTopLevelItem(item);
    }
}

void CDlgObject::on_cInactive_clicked(bool checked)
{
    m_ui->cbInactive->setEnabled(checked);
}

void CDlgObject::on_btnUuid_clicked()
{
    CProto & proto = m_gameFile->m_arrProto[m_objectIndex];
    QClipboard *clip = QApplication::clipboard();
    clip->setText(proto.m_uuid);
}

void CDlgObject::on_cAutoTrigger_toggled(bool checked)
{
    m_ui->cTriggerCall->setChecked(!checked);
}

void CDlgObject::on_cTriggerCall_toggled(bool checked)
{
    m_ui->cAutoTrigger->setChecked(!checked);
}
