#include "DlgTestLevel.h"
#include "ui_DlgTestLevel.h"

#include "../shared/stdafx.h"

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
