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

#include "DlgSkill.h"
#include "ui_DlgSkill.h"

CDlgSkill::CDlgSkill(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgSkill)
{
    m_ui->setupUi(this);
}

CDlgSkill::~CDlgSkill()
{
    delete m_ui;
}

void CDlgSkill::init()
{
    QString options[] = {
        tr("Normal - I'm just a kid"),
        tr("Nightmare - No sweat !"),
        tr("Hell - Bring it on !"),
        tr("Insane - Oh Yeah baby !!!")
    };

    this->setWindowTitle(tr("Test level"));
    for (unsigned int i=0; i< sizeof(options)/sizeof(QString); ++i) {
        m_ui->cbSkill->addItem(options[i]);
    }

    m_ui->cbSkill->setFocus();
}

void CDlgSkill::setSkill(int skill)
{
    m_ui->cbSkill->setCurrentIndex(skill);
}

int CDlgSkill::getSkill()
{
    return m_ui->cbSkill->currentIndex();
}

void CDlgSkill::changeEvent(QEvent *e)
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
