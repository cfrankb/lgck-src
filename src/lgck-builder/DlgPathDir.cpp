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

#include "DlgPathDir.h"
#include "ui_DlgPathDir.h"
#include "../shared/GameFile.h"
#include "../shared/qtgui/cheat.h"

QString CDlgPathDir::m_iconNames[] = {
  "aim-up.png",
  "aim-down.png",
  "aim-left.png",
  "aim-right.png",
  "aim-separator.png"
};

QString CDlgPathDir::m_iconText[] = {
    tr("UP"),
    tr("DOWN"),
    tr("LEFT"),
    tr("RIGHT"),
    tr("SEPARATOR")
};

int CDlgPathDir::m_iconValues[] = {
    CGameFile::UP,
    CGameFile::DOWN,
    CGameFile::LEFT,
    CGameFile::RIGHT,
    CGameFile::SEPARATOR
};

CDlgPathDir::CDlgPathDir(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgPathDir)
{
    ui->setupUi(this);

    // cache all the icons
    for (int i=0; i < MAX_ICONS; ++i) {
        QIcon icon = QPixmap(QString(":/images/%1").arg(m_iconNames[i]));
        if (icon.isNull()) {
            ui->cbAim->addItem(QIcon(), m_iconText[i]);
            qWarning("failed to load icon: %s\n", q2c(m_iconNames[i]));
        } else {
            ui->cbAim->addItem(icon, m_iconText[i]);
        }
    }
}

CDlgPathDir::~CDlgPathDir()
{
    delete ui;
}

void CDlgPathDir::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CDlgPathDir::load(const uint8_t aim)
{
    for (int i=0; i < MAX_ICONS; ++i) {
        if (m_iconValues[i] == aim) {
            ui->cbAim->setCurrentIndex( i );
        }
    }
}

void CDlgPathDir::save(uint8_t & aim)
{
    if (ui->cbAim->currentIndex() != -1) {
        aim = m_iconValues[ui->cbAim->currentIndex()];
    } else {
        aim = -1;
    }
}

