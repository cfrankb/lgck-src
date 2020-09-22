/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

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
#include "DlgTestLevel.h"
#include "ui_DlgTestLevel.h"

#include "../shared/stdafx.h"
#include "Layer.h"
#include "Level.h"
#include "LevelEntry.h"
#include "Actor.h"

CDlgTestLevel::REZ CDlgTestLevel::m_rez[] {
    {640,480},
    {800,600},
    {1024,768},
    {1152,864},
    {1280,720},
    {1280,800},
    {1280,1024},
    {1440,900},
    {1920,1080}
};

CDlgTestLevel::CDlgTestLevel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgTestLevel)
{
    ui->setupUi(this);

    QString skillOptions[] = {
        tr("Normal - I'm just a kid"),
        tr("Nightmare - No sweat !"),
        tr("Hell - Bring it on !"),
        tr("Insane - Oh Yeah baby !!!")
    };

    this->setWindowTitle(tr("Test level"));
    for (unsigned int i=0; i< sizeof(skillOptions)/sizeof(QString); ++i) {
        ui->cbSkill->addItem(skillOptions[i], i);
    }

    ui->cbSkill->setFocus();

    for (int i=1; i< 255; ++i) {
        ui->cbLives->addItem(QVariant(i).toString(), i);
    }

    // TODO: make visible again when implemented
    ui->cInitGame->setVisible(false);

    for (unsigned int i=0; i < sizeof(m_rez)/sizeof(REZ);++i){
        ui->cbResolution->addItem(QString("%1 x %2").arg(m_rez[i].w).arg(m_rez[i].h));
    }
    ui->tabWidget->setCurrentIndex(0);

    QIcon iconBlank;
    iconBlank.addFile(":/images/blank.png");
    ui->sWarningText->setText("");
    ui->sWarningIcon->setPixmap(iconBlank.pixmap(16,16));
}

CDlgTestLevel::~CDlgTestLevel()
{
    delete ui;
}

void CDlgTestLevel::setSkill(int skill)
{
    ui->cbSkill->setCurrentIndex(skill);
}

int CDlgTestLevel::getSkill()
{
    return ui->cbSkill->currentData().toInt();
}

void CDlgTestLevel::setLives(int lives)
{
    lives = std::min(lives, 255);
    ui->cbLives->setCurrentIndex(lives - 1);
}

int CDlgTestLevel::getHP()
{
    return ui->eHitPoints->text().trimmed().toInt();
}

int CDlgTestLevel::getScore()
{
    return ui->eScore->text().trimmed().toInt();
}

int CDlgTestLevel::getLives()
{
    return ui->cbLives->currentData().toInt();
}

void CDlgTestLevel::setScore(int score)
{
    ui->eScore->setText(QVariant(score).toString());
}

void CDlgTestLevel::setHP(int hp)
{
    ui->eHitPoints->setText(QVariant(hp).toString());
}

bool CDlgTestLevel::getContinue()
{
    return ui->cContinue->isChecked();
}

void CDlgTestLevel::setContinue(bool c)
{
    return ui->cContinue->setChecked(c);
}

bool CDlgTestLevel::isExternal()
{
    return ui->cExternal->isChecked();
}

void CDlgTestLevel::setExternal(bool external)
{
    ui->cExternal->setChecked(external);
}

int CDlgTestLevel::getWidth()
{
    return m_rez[ui->cbResolution->currentIndex()].w;
}

int CDlgTestLevel::getHeight()
{
    return m_rez[ui->cbResolution->currentIndex()].h;
}

int CDlgTestLevel::getRez()
{
    return ui->cbResolution->currentIndex();
}

void CDlgTestLevel::setRez(int v)
{
    ui->cbResolution->setCurrentIndex(v);
}

void CDlgTestLevel::analyseLevel(CLevel *level)
{
    CLayer * layer = level->getMainLayer();
    if (!layer) {
        return;
    }
    bool hasPlayer = false;
    bool hasGoal = false;
    for (int i=0; i < layer->getSize(); ++i) {
        CLevelEntry e = (*layer)[i];
        CActor a = CActor(e);
        if (a.isPlayer()) {
            hasPlayer = true;
        }
        if (a.isGoal()) {
            hasGoal = true;
        }
    }

    if (!hasPlayer) {
        QIcon iconWarning;
        iconWarning.addFile(":/images/pd/warning-17_16x14.png");
        ui->sWarningIcon->setPixmap(iconWarning.pixmap(16,16));
        ui->sWarningText->setText(tr("No player detected. You must include one."));
        ui->sWarningIcon->setToolTip(tr("A player object is required for the level to execute properly. "\
                                        "This is mandatory and will lead to failure if absent."));
        return;
    }
    if (!hasGoal) {
        QIcon iconWarning;
        iconWarning.addFile(":/images/pd/warning-17_16x14.png");
        ui->sWarningIcon->setPixmap(iconWarning.pixmap(16,16));
        ui->sWarningText->setText(tr("No goal detected. You should include at least one."));
        ui->sWarningIcon->setToolTip(tr("Not including one goal on this level will lead to "\
                                        "automatically completing the level upon entry."\
                                        "This is likely not the intended result."));
        return;
    }
}
