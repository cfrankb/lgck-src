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

#include "DlgLayer.h"
#include "ui_DlgLayer.h"
#include "../shared/Layer.h"

QString CDlgLayer::m_ratios [] =
{
    tr("no scroll"),
    tr("1:1"),
    tr("1/2"),
    tr("1/4"),
    tr("1/8"),
    tr("1/16"),
    tr("1/32"),
    tr("1/64"),
    tr("1/128")
};

CDlgLayer::CDlgLayer(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgLayer)
{
    m_ui->setupUi(this);
    init();
}

CDlgLayer::~CDlgLayer()
{
    delete m_ui;
}

void CDlgLayer::changeEvent(QEvent *e)
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

QString CDlgLayer::getName()
{
    return m_ui->eName->text();
}

void CDlgLayer::setName(const QString s)
{
    m_ui->eName->setText(s);
}

void CDlgLayer::init()
{
    for (unsigned int i=0; i < sizeof(m_ratios)/sizeof(QString); ++i) {
        m_ui->cbHSpeed->addItem(m_ratios[i]);
        m_ui->cbVSpeed->addItem(m_ratios[i]);
    }

    m_ui->cbType->addItem(tr("Background"));
    m_ui->cbType->addItem(tr("Foreground"));
    m_ui->sType->setText(getLayerDesc(0));
}

void CDlgLayer::setParams(const QString s, int h, int v)
{
    setName(s);
    m_ui->eName->setFocus();
    if (h==CLayer::SPEED_CUSTOM) {
        m_ui->cCustom->setChecked(true);
        m_ui->cbHSpeed->setEnabled(false);
        m_ui->cbVSpeed->setEnabled(false);
    } else {
        m_ui->cbHSpeed->setCurrentIndex(h);
        m_ui->cbVSpeed->setCurrentIndex(v);
    }
}

void CDlgLayer::getParams(QString & s, int & h, int & v)
{
    s = getName();
    if (m_ui->cCustom->isChecked()) {
        h = v = CLayer::SPEED_CUSTOM;
    } else {
        h = m_ui->cbHSpeed->currentIndex();
        v = m_ui->cbVSpeed->currentIndex();
    }
}

void CDlgLayer::setType(int type)
{
    m_ui->cbType->setCurrentIndex(type / 2);
    m_ui->sType->setText(getLayerDesc(type / 2));
}

int CDlgLayer::getType()
{
    return m_ui->cbType->currentIndex() * 2;
}

void CDlgLayer::lockType()
{
    return m_ui->cbType->setDisabled( true );
}

const char* CDlgLayer::getLayerDesc(int index)
{
    const char *text[] = {
        "Background layer sits below the main layer where all the action takes place.",
        "Foreground layer is drawn on top of the main layer where all the action takes place."
    };
    return text[index];
}

void CDlgLayer::on_cbType_currentIndexChanged(int index)
{
    m_ui->sType->setText(tr(getLayerDesc(index)));
}

void CDlgLayer::on_cCustom_toggled(bool checked)
{
    m_ui->cbHSpeed->setEnabled(!checked);
    m_ui->cbVSpeed->setEnabled(!checked);
}
