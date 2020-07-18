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
#include <QPushButton>
#include "WizScript.h"

int CDlgSource::m_fontSize = 10;

CDlgSource::CDlgSource(QWidget *parent) :
        QDialog(parent),
        m_ui(new Ui::CDlgSource)
{
    m_ui->setupUi(this);
    QIcon icon(":/images/pd/small_chemistry.png");
    m_btn = new QPushButton(icon, "");
    m_btn->setStatusTip(tr("scriptWiz"));
    m_ui->buttonBox->addButton(m_btn, QDialogButtonBox::ActionRole);
    m_gameFile = NULL;
    connect(m_btn, SIGNAL(pressed()), this, SLOT(wizButton()));
    connect(this, SIGNAL(textInserted(const char*)), m_ui->eSource, SLOT(insertText(const char*)));
    connect(this,SIGNAL(fontSizeChanged(int)), m_ui->eSource, SLOT(setFontSize(int)));
    emit fontSizeChanged(m_fontSize);
}

CDlgSource::~CDlgSource()
{
    delete m_ui;
}

void CDlgSource::init(CGameFile *gf)
{
    if (!gf) {
        qCritical("gf is NULL");
    }
    m_gameFile = gf;
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
    m_ui->eSource->setText(s.trimmed());
}

const QString CDlgSource::getText()
{
    return m_ui->eSource->text().trimmed();
}

void CDlgSource::setReadOnly() const
{
    m_ui->eSource->setReadOnly(true);
    m_btn->hide();
}

void CDlgSource::wizButton()
{
    CWizScript *wiz = new CWizScript(this);
    wiz->init(m_gameFile);
    if (wiz->exec()) {
        emit textInserted(wiz->getScript().c_str());
    }
    delete wiz;
    m_ui->eSource->setFocus();
}

void CDlgSource::setFontSize(int size)
{
    m_fontSize = size;
}
