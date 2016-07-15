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

#include "DlgParam.h"
#include "Params.h"
#include "Functions.h"
#include "ui_DlgParam.h"

CDlgParam::CDlgParam(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgParam)
{
    m_ui->setupUi(this);
}

CDlgParam::~CDlgParam()
{
    delete m_ui;
}

void CDlgParam::changeEvent(QEvent *e)
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

void CDlgParam::load( Param * param)
{
    m_ui->eComment->setPlainText(param->desc);
    m_ui->eName->setText(param->name);
    m_ui->eType->setText(param->type);

    if (param->flags & CFunction::FLAG_OPTIONAL) {
        m_ui->cOptional->setChecked( true );
    }

    if (param->flags & CFunction::FLAG_MORE) {
        m_ui->cMore->setChecked( true );
    }
}

void CDlgParam::save( Param * param)
{
    param->desc = m_ui->eComment->toPlainText().trimmed();
    param->name = m_ui->eName->text().trimmed();
    param->type = m_ui->eType->text().trimmed();
    param->flags = 0 ;

    if (m_ui->cOptional->isChecked()) {
        param->flags |= CFunction::FLAG_OPTIONAL;
    }

    if (m_ui->cMore->isChecked()) {
        param->flags |= CFunction::FLAG_MORE;
    }
}
