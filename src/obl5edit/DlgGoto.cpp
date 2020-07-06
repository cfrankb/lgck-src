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

#include "DlgGoto.h"
#include "ui_DlgGoto.h"
#include "../shared/stdafx.h"
#include "../shared/qtgui/cheat.h"
#include <QTreeWidgetItem>
//#define q2c(__qstring__) (const char*) __qstring__.toAscii().constData()

CDlgGoto::CDlgGoto(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgGoto)
{
    m_ui->setupUi(this);
}

CDlgGoto::~CDlgGoto()
{
    delete m_ui;
}

void CDlgGoto::changeEvent(QEvent *e)
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

void CDlgGoto::initSizes(int px)
{
    const int sizes[] = {
        16, 24, 32, 48, 64, 96, 128, 256
    };

    for (unsigned int i = 0; i < sizeof(sizes)/sizeof(int)
         && sizes[i] < px; ++i) {
        const QString s = QString::asprintf(q2c(tr("%d x %d pixels")), sizes[i], sizes[i]);
        m_ui->cbLevel->addItem(s);
    }
}

void CDlgGoto::initMoveTo(CFrame **frames, int size)
{
    Q_UNUSED(frames);
    for (int i = 0; i < size; ++i) {
        QString s = QString(tr("Position %1")).arg(i + 1);
        m_ui->cbLevel->addItem(s);
    }
    m_ui->cbLevel->setCurrentIndex(0);
}

int CDlgGoto::getIndex()
{
    return m_ui->cbLevel->currentIndex();
}

void CDlgGoto::select(int index)
{
    m_ui->cbLevel->setCurrentIndex( index );
}

void CDlgGoto::on_buttonBox_accepted()
{
    CDlgGoto::accept();
}

void CDlgGoto::on_buttonBox_rejected()
{
    CDlgGoto::reject();
}

void CDlgGoto::initAlpha(int alpha)
{
    for (int i = 0; i < 256; ++i) {
        const QString s = QString::asprintf("0x%.2x - %d%%", i, 100*i/255);
        m_ui->cbLevel->addItem(s);
    }
    m_ui->cbLevel->setCurrentIndex(alpha);
}

void CDlgGoto::initSpeed(int speed)
{
    for (int i = 1; i < 200; ++i) {
        QString s;
        if (i != 1) {
            s = tr("%1 frames per second (fps)").arg(i);
        } else {
            s = tr("1 frame per second (fps)");
        }
        m_ui->cbLevel->addItem(s);
    }
    m_ui->cbLevel->setCurrentIndex(speed-1);
}
