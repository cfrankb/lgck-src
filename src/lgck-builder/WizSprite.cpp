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

#include "WizSprite.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QTreeWidgetItem>
#include "ui_WizSprite.h"
#include "../shared/stdafx.h"
#include "../shared/GameFile.h"
#include "../shared/vlamits3.h"
#include "../shared/Snd.h"
#include "../shared/FileWrap.h"
#include "../shared/Frame.h"
#include "../shared/qtgui/cheat.h"
#include "../shared/Path.h"

CWizSprite::CWizSprite(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::CWizSprite)
{
    ui->setupUi(this);
    m_classIndex = nullptr;
    m_frameSet = new CFrameSet();
}

CWizSprite::~CWizSprite()
{
    delete ui;
    if (!m_classIndex) {
        delete [] m_classIndex;
    }
}

void CWizSprite::setGameFile(CGameFile *gf)
{
    m_gameFile = gf;
}

void CWizSprite::load(const int index)
{

    connect(this,
            SIGNAL(imageDeleted()),
            ui->treeWidget,
            SLOT(deleteImage()));

    connect(this,
            SIGNAL(imagesAdded()),
            ui->treeWidget,
            SLOT(addImages()));

    connect(this,
            SIGNAL(refill()),
            ui->treeWidget,
            SLOT(reload()));

    connect(ui->treeWidget,
            SIGNAL(clicked()),
            this,
            SLOT(selectRadioNew()));

#ifdef Q_WS_WIN
    int size = 10;
#else
    int size = 12;
#endif

    QString s = QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">" \
                       "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">" \
                       "p, li { white-space: pre-wrap; }" \
                       "</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:7.8pt; font-weight:400; font-style:normal;\">" \
                       "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:%1pt;\">%2</span></p></body></html>")
                        .arg(size).arg(tr("Welcome to the Sprite Wizard"));

    ui->sWelcome->setText(s);

    CGameFile & gf = *((CGameFile*)m_gameFile);

    CProto & proto = gf.m_arrProto[index];

    ui->treeWidget->setFrameSet(m_frameSet);
    //ui->treeWidget->reload();
    emit refill();

    // name

    ui->eName->setText(proto.m_szName);
    ui->eName2->setText(proto.m_szName);

    // classes

    m_classIndex = new int [ CGameFile::MAX_CLASSES ];
    bool classFound = false;
    int j = 0;
    for (int i=0; i < CGameFile::MAX_CLASSES; ++i) {
        if (!gf.m_className[i].empty()) {
            m_classIndex [ j ] = i;
            ui->cbClass->addItem(gf.m_className[i].c_str());
            if (proto.m_nClass == i) {
                ui->sClass->setText(gf.m_classInfo[i].c_str());
                ui->cbClass->setCurrentIndex( j );
                classFound =  true;
            }
            j++;
        }
    }

    if (!classFound) {
        QString s = QString(tr("Unknown class %1").arg(proto.m_nClass));
        ui->cbClass->addItem(s);
        m_classIndex[ j ] = proto.m_nClass;
        ui->cbClass->setCurrentIndex( j );
    }

    // imageSet
    // frame set

    for (int n=0; n < gf.frames().getSize(); ++n) {
        CFrameSet & frameSet = *gf.frames()[n];
        uint8_t *png;
        int size;
        frameSet[0]->toPng(png, size);

        QImage img;
        if (!img.loadFromData( png, size )) {
            qWarning("failed to load png (%d)\n", n);
        }
        delete [] png;

        QPixmap pm = QPixmap::fromImage(img);
        QIcon icon;
        icon.addPixmap(pm, QIcon::Normal, QIcon::On);
        ui->cbFrameSet->addItem(icon, frameSet.getName() );
    }

    ui->cbFrameSet->setCurrentIndex( 0 );
    ui->cbFallSpeed->setCurrentIndex(proto.m_nFallSpeed);

    // speeds

    for (int n = 0; n < 16; ++n) {
        if (n != 0) {
            QString s = QString(tr("Speed %1")).arg(n);
            ui->cbAnimateSpeed->addItem(s);
            ui->cbMoveSpeed->addItem(s);
            ui->cbFallSpeed->addItem(s);
        } else {
            ui->cbAnimateSpeed->addItem(tr("(fast)"));
            ui->cbMoveSpeed->addItem(tr("(fast)"));
            ui->cbFallSpeed->addItem(tr("(fast)"));
        }
    }

    for (int n = 0; n < 256; ++n) {
        if (n) {
            QString s = QString(tr("%1")).arg(n);
            ui->cbMaxFall->addItem(s);
            s = QString(tr("Speed %1")).arg(n);
            ui->cbFireRate->addItem(s);
        } else {
            ui->cbMaxFall->addItem(tr("n/a"));
            ui->cbFireRate->addItem(tr("(fast)"));
        }
    }

    ui->cbAnimateSpeed->setCurrentIndex(4);
    ui->cbMoveSpeed->setCurrentIndex(5);
    ui->cbFallSpeed->setCurrentIndex(0);
    ui->cbMaxFall->setCurrentIndex(0);
    ui->cbFireRate->setCurrentIndex(0);

    // damages

    for (int n = 0; n < 255; ++n) {
        if (n != 0) {
            ui->cbDamages->addItem(QString(tr("%1 pts")).arg(n));
        } else {
            ui->cbDamages->addItem(tr("(none)"));
        }
    }

    ui->cbDamages->setCurrentIndex(proto.m_nDamages);

    // BonusHP (hit points)

    for (int n = 0; n < 255; ++n) {
        if (n != 0) {
            ui->cbBonusHP->addItem(QString(tr("%1 pts")).arg(n));
            ui->cbHP->addItem(QString(tr("%1 pts")).arg(n));
        }
        else {
            ui->cbBonusHP->addItem(tr("(none)"));
            ui->cbHP->addItem(tr("(none)"));
        }
    }

    ui->cbBonusHP->setCurrentIndex(0);
    ui->cbHP->setCurrentIndex(0);

    // power level

    for (int n = 0; n < 255; ++n) {
        if (n != 0) {
            ui->cbPowerLevel->addItem(QString(tr("Level %1")).arg(n));
        } else {
            ui->cbPowerLevel->addItem(tr("(ignored)"));
        }
    }

    ui->cbPowerLevel->setCurrentIndex(0);

    // max bullets

    for (int n = 0; n <= 255; ++n) {
        if (n != 0) {
            ui->cbMaxBullets->addItem(QString("%1").arg(n));
        } else {
            ui->cbMaxBullets->addItem(tr("none"));
        }
    }

    ui->cbMaxBullets->setCurrentIndex(0);

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
        ui->cbPoints->addItem(QString("%1").arg(points[i]));
    }

    ui->cbPoints->setCurrentIndex(proto.m_nPoints);

    // sounds

    for (int i=0; i < gf.m_arrSounds.getSize(); ++i) {
        ui->cbPlaySound->addItem(gf.m_arrSounds[i]->getName());
        // page 4
        ui->cbAutoSound->addItem(gf.m_arrSounds[i]->getName());

        // sound combo for pageSounds
        ui->cbSound_death->addItem(gf.m_arrSounds[i]->getName());
        ui->cbSound_fall->addItem(gf.m_arrSounds[i]->getName());
        ui->cbSound_jumo->addItem(gf.m_arrSounds[i]->getName());
        ui->cbSound_move->addItem(gf.m_arrSounds[i]->getName());
        ui->cbSound_spawn->addItem(gf.m_arrSounds[i]->getName());
        ui->cbSound_bullet->addItem(gf.m_arrSounds[i]->getName());
    }

    ui->cbPlaySound->setCurrentIndex(proto.m_nChSound);
    ui->cbSound_death->setCurrentIndex(0);
    ui->cbSound_fall->setCurrentIndex(0);
    ui->cbSound_jumo->setCurrentIndex(0);
    ui->cbSound_move->setCurrentIndex(0);
    ui->cbSound_spawn->setCurrentIndex(0);
    ui->cbSound_bullet->setCurrentIndex(0);

    // proto (changeto)

    for (int i = 0; i < gf.m_arrProto.getSize(); ++i){
        CProto & proto = gf.m_arrProto[i];
        CFrameSet & frameSet = *gf.frames()[proto.m_nFrameSet];
        uint8_t *png;
        int size;
        frameSet[proto.m_nFrameNo]->toPng(png, size);

        QImage img;
        if (!img.loadFromData( png, size )) {
            qWarning("failed to load png $\n");
        }
        delete [] png;

        QPixmap pm = QPixmap::fromImage(img);
        QIcon icon;
        icon.addPixmap(pm, QIcon::Normal, QIcon::On);

        ui->cbChangeTo->addItem(icon, proto.getName());

        // from page 2
        ui->cbBuddy->addItem(icon, proto.getName());
        ui->cbPlayerBullet->addItem(icon, proto.getName());

        // from page 4

        ui->cbAutoBullet->addItem(icon, proto.getName());
        ui->cbAutoChangeTo->addItem(icon, proto.getName());
    }

    ui->cNoChangeAtDeath->setChecked( proto.getOption(CProto::OPTION_NO_SHIFT_AT_DEATH) );
    //ui->cNoSplat->setChecked( proto.getOption(CProto::OPTION_NO_SPLAT) );

    if (ui->cNoChangeAtDeath->isChecked()) {
        ui->cbChangeTo->setEnabled( false );
    }

    ui->cbChangeTo->setCurrentIndex( proto.m_nChProto );
    // from page 2
    ui->cbBuddy->setCurrentIndex(0);
    ui->cbPlayerBullet->setCurrentIndex(0);

    // nbr of rebirth

    for (int n = 0; n < 255; ++ n) {
        if (n == 1) {
            ui->cbRebirths->addItem(QString("%1 time").arg(n));
        } else if (n > 1) {
            ui->cbRebirths->addItem(QString("%1 times").arg(n));
        } else {
            ui->cbRebirths->addItem(QString(tr("(none)")));
        }
    }

    ui->cbRebirths->setCurrentIndex( proto.m_nRebirths );

    // rebirthdelay

    for (int n = 0; n < 255; ++n) {
        if (n != 0) {
            ui->cbRebirthDelay->addItem(QString(tr("Speed %1")).arg(n));
        } else {
            ui->cbRebirthDelay->addItem(QString(tr("(fast)")));
        }
    }

    ui->cbRebirthDelay->setCurrentIndex( proto.m_nRbDelay );

    const char *jumpModes [] = {
        "VLA3",
        "GIANA",
        "MIXED"
    };

    for (unsigned int n = 0; n < sizeof(jumpModes)/sizeof(const char*); ++n) {
        ui->cbJump->addItem(jumpModes[n]);
    }

    ui->cbJump->setCurrentIndex( proto.m_nJumpMode );

    setJumpText(0);

    // timers

    for (int n = 0; n < 256; ++n) {
        QString s = QString(tr("Speed %1").arg(n));

        if (n == 0) {
            s = tr("(never)");
        }

        ui->cbAutoSoundDelay->addItem(s);
        ui->cbAutoBulletDelay->addItem(s);
        ui->cbAutoChangeToDelay->addItem(s);
        ui->cbAutoTriggerDelay->addItem(s);
    }

    ui->cbAutoSoundDelay->setCurrentIndex(proto.m_nAutoSoundTime);
    ui->cbAutoBulletDelay->setCurrentIndex(proto.m_nAutoBulletTime);
    ui->cbAutoChangeToDelay->setCurrentIndex(proto.m_nAutoProtoTime);
    ui->cbAutoTriggerDelay->setCurrentIndex(proto.m_nAutoTriggerTime);

    // auto trigger checkbox
    ui->cAutoTrigger->setChecked(proto.m_bAutoTrigger != 0);

    // trigger

//    ui->cTrigger->setChecked( proto.m_bNoTrigger != 0 );
    ui->cTrigger->setChecked( proto.getOption(CProto::OPTION_NO_TRIGGER_FLIP) );

    updateButtons();

    updateMonsterDesc(GENERIC_MONSTER);

    // register fields
    // http://doc.qt.digia.com/qq/qq22-qwizard.html#initializingapage

    ui->pageEasy->registerField("name2*", (QWidget*)ui->eName2);
    ui->pageGeneral->registerField("name*", ui->eName);
    ui->pageImages->registerField("frameSet", ui->cbFrameSet);
    ui->pageImages->registerField("tree", ui->treeWidget, "topLevelItemCount", SIGNAL(currentItemChanged()));
    ui->pageImages->registerField("old", ui->rbFrameSetOld);

    connect(ui->rbFrameSetOld, SIGNAL(toggled(bool)),
             ui->pageImages, SIGNAL(completeChanged()));

    connect(ui->cbFrameSet, SIGNAL(currentIndexChanged(int)),
            ui->pageImages, SIGNAL(completeChanged()));

    connect(ui->treeWidget, SIGNAL(countChanged()),
            ui->pageImages, SIGNAL(completeChanged()));

    on_cPlayerFire_clicked(false);
}

void CWizSprite::save(const int index)
{
    CGameFile & gf = *m_gameFile;

    CProto & proto = gf.m_arrProto[index];
    proto.m_nClass = getClass();//m_classIndex[ ui->cbClass->currentIndex() ];
    proto.m_szName[31] = 0;  

    if (isExpert()) {
        memccpy(proto.m_szName, q2c( (ui->eName->text()).trimmed()), 0, 31);

        // system Invisible

        proto.setOption(CProto::OPTION_INVISIBLE, ui->cInvisible->checkState () == Qt::Checked);

        // noSmartMap

        proto.m_bNoSmartMap = (int) ui->cNoSmartMap->checkState () == Qt::Checked;

    } else {
        memccpy(proto.m_szName, q2c( (ui->eName2->text()).trimmed()), 0, 31);
        if (proto.m_nClass == CLASS_STOP) {
            proto.setOption(CProto::OPTION_INVISIBLE, true);
        }
    }

    if (!proto.m_szName[0]) {
        sprintf(proto.m_szName, q2c(tr("sprite_%d")), index);
    }

    // images
    proto.m_nFrameNo = 0;

    if (ui->rbFrameSetOld->isChecked()) {
        proto.m_nFrameSet = ui->cbFrameSet->currentIndex();
    } else {
        // check if the new frameset has images
        // if not then assign to null set
        CFrameSet *frameSet = getFrameSet();

        qDebug("FrameSet size: %d", frameSet->getSize());

        if (frameSet->getSize()) {
            proto.m_nFrameSet = gf.frames().getSize();
        } else {
            proto.m_nFrameSet = 0;
        }

        qDebug("proto.m_nFrameSet: %d", proto.m_nFrameSet);
    }

    //

    proto.m_nAniSpeed = ui->cbAnimateSpeed->currentIndex();
    proto.m_nMoveSpeed = ui->cbMoveSpeed->currentIndex();
    proto.m_nFallSpeed = ui->cbFallSpeed->currentIndex();
    proto.m_nMaxFall = ui->cbMaxFall->currentIndex();
    proto.m_nMaxBullets = ui->cbMaxBullets->currentIndex();
    proto.m_nFireRate = ui->cbFireRate->currentIndex();

    // aims

//    proto.m_nDefaultAim = ui->cbDefaultAim->currentIndex();

    // damages

    proto.m_nDamages = ui->cbDamages->currentIndex();

    // hit points

    proto.m_nBonusHP = ui->cbBonusHP->currentIndex();
    proto.m_nHP = ui->cbHP->currentIndex();

    // power level

    proto.m_nPowerLevel = ui->cbPowerLevel->currentIndex();

    // proto buddy

    proto.m_nProtoBuddy = ui->cbBuddy->currentIndex();

    // page 3 ****************************************************************

    // no gravity

    proto.setOption(CProto::OPTION_NO_GRAVITY, ui->cNoGravity->checkState () == Qt::Checked);

    // destroy on use

    proto.setOption(CProto::OPTION_DESTROY_ON_USE, ui->cDestroyUsed->checkState () == Qt::Checked);

    // solid

    proto.setOption(CProto::OPTION_SOLID, ui->cSolid->checkState () == Qt::Checked);

    // system Invisible

    proto.setOption(CProto::OPTION_INVISIBLE, ui->cInvisible->checkState () == Qt::Checked);

    // isChangeAtDeath disabled
    proto.setOption(CProto::OPTION_NO_SHIFT_AT_DEATH, ui->cNoChangeAtDeath->checkState () == Qt::Checked);

    // noSplat (crush by player)
    proto.setOption(CProto::OPTION_NO_SPLAT, ui->cNoSplat->checkState () == Qt::Checked);

    // set autoGoal
    proto.setOption(CProto::OPTION_AUTO_GOAL, ui->cAutomaticGoal->checkState () == Qt::Checked);

    // jumpMode

    proto.m_nJumpMode = ui->cbJump->currentIndex();

    // page 4 ****************************************************************

    // points

    proto.m_nPoints = ui->cbPoints->currentIndex();

    // sounds

    proto.m_nChSound = ui->cbPlaySound->currentIndex();

    // proto (changeto)

    proto.m_nChProto = ui->cbChangeTo->currentIndex();

    // nbr of rebirth

    proto.m_nRebirths = ui->cbRebirths->currentIndex();

    // rebirthdelay

    proto.m_nRbDelay = ui->cbRebirthDelay->currentIndex();

    // page 5 ****************************************************************

    // auto changeTo
    // auto bullet
    // auto sound

    proto.m_nAutoBullet = ui->cbAutoBullet->currentIndex();
    proto.m_nAutoProto = ui->cbAutoChangeTo->currentIndex();
    proto.m_nAutoSound = ui->cbAutoSound->currentIndex();

    // timers

    proto.m_nAutoSoundTime = ui->cbAutoSoundDelay->currentIndex();
    proto.m_nAutoBulletTime = ui->cbAutoBulletDelay->currentIndex();
    proto.m_nAutoProtoTime = ui->cbAutoChangeToDelay->currentIndex();
    proto.m_nAutoTriggerTime = ui->cbAutoTriggerDelay->currentIndex();

    // auto trigger checkbox
    if (ui->cAutoTrigger->checkState() == Qt::Checked) {
        proto.m_bAutoTrigger = 1;
    } else {
        proto.m_bAutoTrigger = 0;
    }

    proto.setOption(CProto::OPTION_NO_TRIGGER_FLIP, ui->cTrigger->checkState () == Qt::Checked);

    CObject & object = gf.m_arrProto.getObject( index );
    if (ui->cKillOffScreen->checkState() == Qt::Checked) {
        object.setEvent(CObject::EO_LAND, "local s = getSprite(self);\n"  \
            "local sx, sy = getImageSize(s.imageSet, s.imageNo);\n" \
            "\n" \
            "if s.y + sy >= BUFFERHEI then\n" \
            "  s:setState(STATE_DEAD);\n" \
            "  s:hide();\n" \
            "end\n");
    }

    if (ui->cKillPlayerOnTouch->checkState() == Qt::Checked) {
        object.setEvent(CObject::EO_TOUCH, "killPlayer();");
    }

    // add sounds through scripting

    typedef struct {
        QComboBox * cbSound;
        int event;
        QString desc;
    } SOUND;

    SOUND sounds[] = {
        { ui->cbSound_death, CObject::EO_DEATH, tr("play sound when the sprite dies") },
        { ui->cbSound_fall, CObject::EO_FALL, tr("play sound when the sprite falls") },
        { ui->cbSound_jumo, CObject::EO_JUMP, tr("play sound when the sprite jumps") },
        { ui->cbSound_move, CObject::EO_MOVE, tr("play sound when the sprite moves") },
        { ui->cbSound_spawn, CObject::EO_SPAWN, tr("play sound when the sprite is created") }
    };

    for (unsigned int i=0; i < sizeof(sounds) / sizeof(SOUND); ++i) {
        int sound = 0;

        if (sounds[i].cbSound) {
            sound = sounds[i].cbSound->currentIndex();
        }

        QString soundName = "";
        if (sound) {
            soundName = gf.m_arrSounds[sound]->getName();
        }

        if (sound || ui->cPlaceholderSounds->checkState () == Qt::Checked ) {
            QString eventCode = "-- " + sounds[i].desc + "\n\n";

            QString prefix = "";
            if (!sound) {
                prefix = "-- ";
            }

            if (proto.m_nClass == CLASS_PLAYER_OBJECT) {
                eventCode += QString("%2playSound(\"%1\");\n")
                        .arg(soundName)
                        .arg(prefix);
            } else {
                eventCode += QString("%2if isVisible(self) then\n"\
                                    "%2   playSound(\"%1\");\n" \
                                    "%2end\n")
                        .arg(soundName)
                        .arg(prefix);
            }

            object.setEvent(sounds[i].event, q2c(eventCode));
        }
    }

    // if defaultPath is checked
    if (ui->cDefaultPaths->checkState() == Qt::Checked) {

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

        for (unsigned i=0; i < sizeof(pathVLA3)/ sizeof(JUMP_SEQ); ++i) {
            JUMP_SEQ & js = pathVLA3[i];
            CPath & path = object.getPath(js.id_seq);
            QString sq = QString(js.path);

            // decode the string
            for (unsigned int k=0; k < strlen(js.path);k +=3) {
                char aim = strtol(q2c(sq.mid(k, 2)), nullptr, 16);
                path.add(aim);
            }
        }
    }

    // if player firing is enable
    // we assign bullet to buddy slot
    proto.m_bulletOptions = 0;
    if (ui->cPlayerFire->isChecked()) {
        proto.m_nProtoBuddy = ui->cbPlayerBullet->currentIndex();
        proto.m_bulletSound = ui->cbSound_bullet->currentIndex();
        proto.m_bulletOptions |= CProto::BULLET_ENABLED;
    }

    if (getClass() != CLASS_PLAYER_OBJECT &&
            ui->cWaypoint->isChecked()) {
        int event = CObject::EO_TOUCH;
        if (getClass() == CLASS_PICKUP_TRIGGERS) {
            event = CObject::EO_DEATH;
        } else if (getClass() >= CLASS_GENERIC_COS) {
            event = CObject::EO_DEATH;
        }
        CFileWrap file;
        if (file.open(":/scripts/templates/waypoint.lua")) {
            int size=file.getSize();
            char *buf = new char[size+1];
            buf[size]=0;
            file.read(buf,size);
            file.close();
            qDebug("event:%s", buf);
            object.setEvent(event, buf);
            delete[]buf;
        } else {
            qWarning("can't open");
        }
    }
}

void CWizSprite::on_cbClass_currentIndexChanged(int index)
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    int classId = m_classIndex [ index ];
    if (!gf.m_className[ classId ].empty()) {
        ui->sClass->setText(gf.m_classInfo[ classId ].c_str());
    } else {
        ui->sClass->setText(tr("This could be a custom class."));
    }    
}

void CWizSprite::on_btnAddImages_clicked()
{
    emit imagesAdded();
}

void CWizSprite::on_btnDeleteImage_clicked()
{
    emit imageDeleted();
}

void CWizSprite::on_treeWidget_itemSelectionChanged()
{
    updateButtons();
}

void CWizSprite::updateButtons()
{
    if (ui->rbFrameSetNew->isChecked()) {
        QModelIndex index = ui->treeWidget->currentIndex();
        ui->btnDeleteImage->setEnabled( index.row() >= 0 );
        ui->btnAddImages->setEnabled( true );
    } else {
        ui->btnAddImages->setEnabled( false );
        ui->btnDeleteImage->setEnabled( false );
    }
}

CFrameSet * CWizSprite::getFrameSet()
{
    return m_frameSet;
}

bool CWizSprite::isExpert() const
{
    return ui->cExpert->checkState() == Qt::Checked;
}

int CWizSprite::nextId() const
{
    int classId = getClass();

    switch (currentId()) {

    case page_intro:
        if (isExpert()) {
            return page_class;
        } else {
            return page_easy;
        }

    case page_class:
        return page_obl5;

    case page_easy:

        switch (getOption())
        {
        case option_Monster:
            return page_monsters;
        case option_Animated:
        case option_Door:
        case option_Key:
        case option_Object:
        case option_Player:
        case option_Switch:
            return page_obl5;
        default:
        case option_Ladder:
        case option_Lava:
        case option_Background:
        case option_Brick:
            return page_obl5;
        }


    case page_obl5:
        if (classId < 0x10) {
            return page_finish;
        } else {
            return page_features;
        }

    case page_features:
        return page_death;

    case page_death:
        return page_auto;

    case page_auto:
        return page_sounds;

    case page_sounds:
        switch (classId) {
        case CLASS_PLAYER_OBJECT:
            return page_player;
        case CLASS_OPEN_TO_OWNER:
            return page_door;
        default:
            return page_finish;
        }                

    case page_monsters:
         return page_obl5;

    case page_door:

    case page_player:
        return page_finish;

    case page_finish:
    default:
        return -1;
    }
 }

 bool CWizSprite::validateCurrentPage ()
 {
     return QWizard::validateCurrentPage();
 }

 int CWizSprite::getOption() const
 {
     QRadioButton * buttons [] = {
         ui->rAnimated,
         ui->rBackground,
         ui->rBrick,
         ui->rDoor,
         ui->rKey,
         ui->rLadder,
         ui->rLava,
         ui->rMonster,
         ui->rObject,
         ui->rPlayer,
         ui->rSwitch
     };

     for (unsigned int i=0; i< sizeof(buttons) / sizeof(QRadioButton*); ++i) {
         if (buttons[i]->isChecked()) {
            return i;
         }
     }

     return -1;
 }

 int CWizSprite::getClass() const
 {
     if (isExpert()) {
         int index = ui->cbClass->currentIndex();
         if (index != -1) {
             return m_classIndex [ index ];
         } else {
             return 0;
         }
     } else {

         switch (getOption())
         {
         case option_Monster:
             return getMonsterClass();
         case option_Animated:
             return CLASS_ANIMATED_THINGS;
         case option_Door:
             return CLASS_OPEN_TO_OWNER;
         case option_Key:
             return CLASS_INVENTORY_ITEM;
         case option_Object:
             return CLASS_PICKUP_TRIGGERS;
         case option_Player:
             return CLASS_PLAYER_OBJECT;

         case option_Switch:
             return CLASS_Z_KEY_SWITCH;

         case option_Ladder:
             return CLASS_METAL_LADDERS;
         case option_Lava:
             return CLASS_IMMEDIATE_DEADLY;

         case option_Brick:
             return CLASS_SOLID_BACKGROUND;

         case option_Background:
         default:
             return CLASS_BACKGROUND;

         }
     }
 }

 int CWizSprite::getMonsterClass() const
 {
     QRadioButton * monsters [] = {
         ui->rbGenericMonster,
         ui->rbDroneLeftRight,
         ui->rbDroneUpDown,
         ui->rbPlatformLeftRight,
         ui->rbPlatformUpDown,
         ui->rbWackerLeftRight,
         ui->rbWackerUoDown,
         ui->rbVampirePlant
     };

     int classes[] = {
         CLASS_GENERIC_COS,
         CLASS_DRONE_LEFT_RIGHT,
         CLASS_DRONE_UP_DOWN,
         CLASS_PLATFORM_LEFT_RIGHT,
         CLASS_PLATFORM_UP_DOWN,
         CLASS_WACKER_LEFT_RIGHT,
         CLASS_WACKER_UP_DOWN,
         CLASS_VAMPIRE_PLANT_SOLID
     };

     for (unsigned int i=0; i< sizeof(monsters) / sizeof(QRadioButton*); ++i) {
         if (monsters[i]->isChecked()) {
            return classes[i];
         }
     }

     return -1;
 }

void CWizSprite::on_rbFrameSetOld_toggled(bool checked)
{
    ui->cbFrameSet->setEnabled( checked );
    ui->treeWidget->setEnabled( !checked);
    updateButtons();
}

void CWizSprite::on_rbFrameSetNew_toggled(bool checked)
{
    ui->cbFrameSet->setEnabled( !checked );
    ui->treeWidget->setEnabled( checked);
    updateButtons();
}

void CWizSprite::on_cNoChangeAtDeath_clicked(bool checked)
{
    ui->cbChangeTo->setEnabled(!checked);
}

void CWizSprite::on_cbJump_currentIndexChanged(int index)
{
    setJumpText(index);
}

void CWizSprite::setJumpText(int index)
{
    const QString jumpMode[] = {
        tr("This is the engine default; you need to press JUMP + AIM. This mode allows you to climp ladders."),
        tr("Press UP to jump and move LEFT/RIGHT. You cannot climb ladders."),
        tr("Press JUMP and move LEFT/RIGHT. And still get to climb ladders. :)")
    };

    ui->sJumo->setText(jumpMode[index]);
}


void CWizSprite::on_rbGenericMonster_clicked()
{
    updateMonsterDesc(GENERIC_MONSTER);
}

void CWizSprite::on_rbVampirePlant_clicked()
{
    updateMonsterDesc(VAMPIRE_PLANT);
}

void CWizSprite::on_rbPlatformLeftRight_clicked()
{
    updateMonsterDesc(PLATFORM_LEFT);
}

void CWizSprite::on_rbPlatformUpDown_clicked()
{
    updateMonsterDesc(PLATFORM_UP);
}

void CWizSprite::on_rbWackerLeftRight_clicked()
{
    updateMonsterDesc(WACKER_LEFT);
}

void CWizSprite::on_rbWackerUoDown_clicked()
{
    updateMonsterDesc(WACKER_UP);
}

void CWizSprite::on_rbDroneLeftRight_clicked()
{
    updateMonsterDesc(DRONE_LEFT);
}

void CWizSprite::on_rbDroneUpDown_clicked()
{
    updateMonsterDesc(DRONE_UP);
}

void CWizSprite::updateMonsterDesc(int i)
{
    const QString strings [] ={
        tr("Generic monsters can move up, down, left and right."),
        tr("Vampire plants cannot move but will do damage to nearby player"),
        tr("Platform that will move the player UP or DOWN when he is standing on top amd crush him against solid objects."),
        tr("Platform that will move the player LEFT or RIGHT when he is standing on top amd crush him against solid objects."),
        tr("Wacker are similar to platforms but will not carry the player."),
        tr("Wacker are similar to platforms but will not carry the player."),
        tr("This Drone will move LEFT and RIGHT and attack the player if he is in front."),
        tr("This Drone will move UP and DOWN and attack the player if he is in front.")
   };

    ui->sMonsterDesc->setText(strings[i]);
}

void CWizSprite::selectRadioNew()
{
    ui->rbFrameSetNew->setChecked(true);
}

void CWizSprite::on_cPlayerFire_clicked(bool checked)
{
    ui->cbFireRate->setEnabled(checked);
    ui->cbMaxBullets->setEnabled(checked);
    ui->cbPlayerBullet->setEnabled(checked);
    ui->cbSound_bullet->setEnabled(checked);
}
