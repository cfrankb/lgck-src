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

#include "WizGame.h"
#include "ui_WizGame.h"
#include "../shared/stdafx.h"
#include "../shared/Game.h"
#include "../shared/qtgui/cheat.h"
#include <QStringList>
#include "../shared/Settings.h"
#include "../shared/GameEvents.h"

#define MIN_VALUE 1
#define MAX_VALUE 255
#define MIN_SPEED 40
#define MAX_SPEED 150

CWizGame::CWizGame(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::CWizGame)
{
    ui->setupUi(this);
}

CWizGame::~CWizGame()
{
    delete ui;
}

void CWizGame::init(CGame *game)
{
    QStringList list;
    m_game = game;

    for (int i = MIN_SPEED;i <= MAX_SPEED; ++i) {
        if (i == MIN_SPEED) {
            list.append(tr("(slow)"));
        } else {
            if (i == MAX_SPEED) {
                list.append(tr("(fast)"));
            } else {
                list.append(QString(tr("%1")).arg(i));
            }
        }
    }

    typedef struct {
        QComboBox *cb;
        int value;
    } COMBO;

    COMBO combos[]= {
        {ui->cbDefaultSpeed,50},
        {ui->cbSpeedNormal,50},
        {ui->cbSpeedNightmare,60},
        {ui->cbSpeedHell,65},
        {ui->cbSpeedInsane,80}
    };

    for (unsigned int i=0; i < sizeof(combos)/sizeof(COMBO);++i) {
        QComboBox *cb = combos[i].cb;
        cb->addItems(list);
        cb->setCurrentIndex(combos[i].value - MIN_SPEED);
    }

    // select default
    ui->rSetByLevel->setChecked(true);
    on_rSetByLevel_clicked();

    // clear the list and start over

    list.clear();
    for (int i = MIN_VALUE; i <= MAX_VALUE; ++i) {
        list.append(QString("%1").arg(i));
    }

    COMBO combos2[] = {
        {ui->cbHP, 5},
        {ui->cbHPMax, 255},
        {ui->cbLives, 5},
        {ui->cbLivesMax, 99},
        {ui->cbTimeout, 240}
    };

    for (unsigned int i=0; i < sizeof(combos2)/sizeof(COMBO);++i) {
        QComboBox *cb = combos2[i].cb;
        cb->addItems(list);
        cb->setCurrentIndex(combos2[i].value - MIN_VALUE);
    }

    typedef struct {
        QComboBox *cb;
        int value;
        int min;
        int max;
    } COMBO_SPECIAL;

    ui->eName->setText(tr("Your new game"));
}

void CWizGame::save()
{
    CGame & game = *m_game;
    game.restoreDefaults();

    // page1
    CSettings & settings = (* game.getSettings());
    CSettings::SETTING & author = settings[CGame::PARAM_AUTHOR];
    author.value = q2c(ui->eAuthor->text());

    CSettings::SETTING & desc = settings[CGame::PARAM_DESCRIPTION];
    desc.value = q2c(ui->eDescription->toPlainText());

    CSettings::SETTING & title = settings[CGame::PARAM_TITLE];
    title.value = q2c(ui->eName->text());

    typedef struct {
        QComboBox *cb;
        int value;
        int min;
    } COMBO_INT;

    COMBO_INT combos2[] = {
        {ui->cbHP, CGame::PARAM_HP, MIN_VALUE},
        {ui->cbHPMax, CGame::PARAM_HP_MAX, MIN_VALUE},
        {ui->cbLives, CGame::PARAM_LIVES, MIN_VALUE},
        {ui->cbLivesMax, CGame::PARAM_LIVES_MAX, MIN_VALUE},
        {ui->cbTimeout, CGame::PARAM_TIMEOUT, MIN_VALUE},
        {ui->cbDefaultSpeed, CGame::PARAM_DEFAULT_SPEED, MIN_SPEED}
    };

    for (unsigned int i=0; i < sizeof(combos2)/sizeof(COMBO_INT);++i) {
        COMBO_INT & c = combos2[i];
        QComboBox *cb = c.cb;
        CSettings::SETTING & e = settings[c.value];
        char value[10];
        sprintf(value, "%d",cb->currentIndex() + c.min);
        e.value = value;
        e.valueInt = cb->currentIndex() + c.min;
    }

    typedef struct {
        QComboBox *cb;
        const char *label;
    } COMBO_STRING;

    COMBO_STRING combos[]= {
        {ui->cbSpeedNormal,"SKILL_NORMAL"},
        {ui->cbSpeedNightmare, "SKILL_NIGHTMARE"},
        {ui->cbSpeedHell,"SKILL_HELL"},
        {ui->cbSpeedInsane,"SKILL_INSANE"}
    };

    if (ui->rSetByDifficulty->isChecked()) {
        QString sPrepareLevel = tr("-- set game speed based on difficulty level\n\n");
        for (unsigned int i=0; i < sizeof(combos)/sizeof(COMBO_STRING);++i) {
            int speed = combos[i].cb->currentIndex() + MIN_SPEED;
            sPrepareLevel += QString( \
                "    if getSkill() == %1 then\n" \
                "         setTickScale(1000 / %2);\n" \
                "    end\n\n").arg(combos[i].label).arg(speed);
        }
        game.getEvents()->setEvent(CGameEvents::EG_PREPARE_LEVEL, q2c(sPrepareLevel));
    }
}

void CWizGame::on_rSetByDifficulty_clicked()
{
    ui->gDifficulty->setEnabled(true);
    ui->cbDefaultSpeed->setEnabled(false);
}

void CWizGame::on_rSetByLevel_clicked()
{
    ui->gDifficulty->setEnabled(false);
    ui->cbDefaultSpeed->setEnabled(true);
}
