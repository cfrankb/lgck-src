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

    QString options[] = {
        tr("Normal - I'm just a kid"),
        tr("Nightmare - No sweat !"),
        tr("Hell - Bring it on !"),
        tr("Insane - Oh Yeah baby !!!")
    };

    this->setWindowTitle(tr("Test level"));
    for (unsigned int i=0; i< sizeof(options)/sizeof(QString); ++i) {
        ui->cbSkill->addItem(options[i]);
    }

    ui->cbSkill->setFocus();

    for (int i=1; i< 255; ++i) {
        QString s = QString("%1").arg(i);
        ui->cbLives->addItem(s);
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
    return ui->cbSkill->currentIndex();
}

void CDlgTestLevel::setLives(int lives)
{
    lives = std::min(lives, 255);
    ui->cbLives->setCurrentIndex(lives - 1);
}

int CDlgTestLevel::getHP()
{
    QString s = ui->eHitPoints->text().trimmed();
    bool ok;
    return s.toInt( &ok, 10 );
}

int CDlgTestLevel::getScore()
{
    QString s = ui->eScore->text().trimmed();
    bool ok;
    return s.toInt( &ok, 10 );
}

int CDlgTestLevel::getLives()
{
    return ui->cbLives->currentIndex()+1;
}

void CDlgTestLevel::setScore(int score)
{
    QString s = QString("%1").arg(score);
    ui->eScore->setText(s);
}

void CDlgTestLevel::setHP(int hp)
{
    QString s = QString("%1").arg(hp);
    ui->eHitPoints->setText(s);
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
        return;
    }
    if (!hasGoal) {
        QIcon iconWarning;
        iconWarning.addFile(":/images/pd/warning-17_16x14.png");
        ui->sWarningIcon->setPixmap(iconWarning.pixmap(16,16));
        ui->sWarningText->setText(tr("No goal detected. You should include at least one."));
        return;
    }
}
