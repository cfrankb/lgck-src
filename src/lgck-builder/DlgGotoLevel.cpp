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

#include "DlgGotoLevel.h"
#include "ui_DlgGotoLevel.h"
#include "../shared/GameFile.h"
#include "../shared/Level.h"
#include "../shared/Layer.h"
#include "../shared/qtgui/cheat.h"

CDlgGotoLevel::CDlgGotoLevel(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgGotoLevel)
{
    m_ui->setupUi(this);
}

CDlgGotoLevel::~CDlgGotoLevel()
{
    delete m_ui;
}

void CDlgGotoLevel::changeEvent(QEvent *e)
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

void CDlgGotoLevel::init()
{
    CGameFile &gf = *((CGameFile*)m_gameFile);

    for (int i = 0; i < gf.getSize(); ++i) {
        QString s = QString::asprintf("%.3d - %s", i + 1, gf[i]->getSetting("title"));
        m_ui->cbLevel->addItem(s);
    }
}

void CDlgGotoLevel::initLayers(int levelId)
{
    CGameFile &gf = *((CGameFile*)m_gameFile);
    CLevel *level = gf[levelId];

    for (int i = 0; i < level->getSize(); ++i) {
        CLayer & layer = (*level) [i];
        QString s = QString::asprintf("%.3d - %s", i + 1, layer.getName());
        m_ui->cbLevel->addItem(s);
    }
}

void CDlgGotoLevel::initSizes(int px)
{
    int sizes[] = {
        16, 24, 32, 48, 64, 96, 128, 256
    };

    for (unsigned int i = 0; i < sizeof(sizes)/sizeof(int)
         && sizes[i] < px; ++i) {
        QString s = QString::asprintf(q2c(tr("%d x %d pixels")), sizes[i], sizes[i]);
        m_ui->cbLevel->addItem(s);
    }
}

int CDlgGotoLevel::getLevelId()
{
    return m_ui->cbLevel->currentIndex();
}

void CDlgGotoLevel::setLevelId(int levelId)
{
    m_ui->cbLevel->setCurrentIndex( levelId );
}

void CDlgGotoLevel::on_buttonBox_accepted()
{
    CDlgGotoLevel::accept();
}

void CDlgGotoLevel::on_buttonBox_rejected()
{
    CDlgGotoLevel::reject();
}
