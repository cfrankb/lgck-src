/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2009, 2010  Francois Blanchette

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

#include "DlgSection.h"
#include "Sections.h"
#include "ui_DlgSection.h"

CDlgSection::CDlgSection(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgSection)
{
    m_ui->setupUi(this);
}

CDlgSection::~CDlgSection()
{
    delete m_ui;
}

void CDlgSection::changeEvent(QEvent *e)
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

void CDlgSection::load(Section & s)
{
    m_ui->eName->setText(s.name);
    m_ui->eContent->setPlainText(s.content);
}

void CDlgSection::save(Section & s)
{
    s.name = m_ui->eName->text().trimmed();
    s.content = m_ui->eContent->toPlainText().trimmed();
}
