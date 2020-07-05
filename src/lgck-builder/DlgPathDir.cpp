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
#include "../shared/stdafx.h"
#include "../shared/GameFile.h"
#include "../shared/FileWrap.h"
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

        CFileWrap file;
        if (file.open(q2c(QString(":/images/%1").arg(m_iconNames[i])))) {

            int size = file.getSize();
            uint8_t *png = new uint8_t [ size ];
            file.read(png, size);
            file.close();

            QImage img;
            if (!img.loadFromData( png, size )) {
                qDebug("failed to load png\n");
            }

            QPixmap pm = QPixmap::fromImage(img);
            QIcon icon;
            icon.addPixmap(pm, QIcon::Normal, QIcon::On);
            ui->cbAim->addItem(icon, m_iconText[i]);

            delete [] png;

        } else {
            ui->cbAim->addItem(QIcon(), m_iconText[i]);
            qDebug("failed to load icon: %s\n", q2c(m_iconNames[i]));
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

void CDlgPathDir::load(const char aim)
{
    for (int i=0; i < MAX_ICONS; ++i) {
        if (m_iconValues[i] == aim) {
            ui->cbAim->setCurrentIndex( i );
        }
    }
}

void CDlgPathDir::save(char & aim)
{
    if (ui->cbAim->currentIndex() != -1) {
        aim = m_iconValues[ui->cbAim->currentIndex()];
    } else {
        aim = -1;
    }
}

