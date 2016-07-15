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

#include "DlgSource.h"
#include "ui_DlgSource.h"

CDlgSource::CDlgSource(QWidget *parent) :
        QDialog(parent),
        m_ui(new Ui::CDlgSource)
{
    m_ui->setupUi(this);
}

CDlgSource::~CDlgSource()
{
    delete m_ui;
}

void CDlgSource::changeEvent(QEvent *e)
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

void CDlgSource::setText(const QString s)
{
    m_ui->eSource->setPlainText(s);
}

const QString CDlgSource::getText()
{
    return m_ui->eSource->toPlainText();
}

void CDlgSource::setReadOnly() const
{
    m_ui->eSource->setReadOnly(true);
}
