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

#include "DlgAbout.h"
#include "ui_DlgAbout.h"
#include <QDesktopServices>
#include <QUrl>
#include "FileWrap.h"
#include "../shared/Credits.h"
#include "../shared/ss_version.h"

CDlgAbout::CDlgAbout(QWidget *parent) : QDialog(parent),
                                        m_ui(new Ui::CDlgAbout)
{
    m_ui->setupUi(this);

    // About
    QString s;
    QString ver;
    int version = SS_LGCK_VERSION;
    for (int i = 0; i < 4; ++i)
    {
        s = QString("%1").arg(version % 256);
        version /= 256;
        if (i)
        {
            ver = s + "." + ver;
        }
        else
        {
            ver = s + ver;
        }
    }
    ver = "<b>" + tr("Version ") + ver + "</b>";

    m_ui->sVersion->setText(ver);

    // license GPL
    CFileWrap file;
    if (file.open(":/res/gpl.txt"))
    {
        int size = file.getSize();
        char *buf = new char[size + 1];
        buf[size] = 0;
        file.read(buf, size);
        file.close();
        m_ui->eLicense->setPlainText(buf);
        delete[] buf;
    }
    m_ui->eLicense->setReadOnly(true);

    // legal text

    if (file.open(":/res/license.txt"))
    {
        int size = file.getSize();
        char *buf = new char[size + 1];
        buf[size] = 0;
        file.read(buf, size);
        file.close();
        m_ui->eLegal->setPlainText(buf);
        delete[] buf;
    }
    m_ui->eLegal->setReadOnly(true);

    // team
    m_ui->treeTeam->setColumnCount(2);
    m_ui->treeTeam->setColumnWidth(0, 128);
    m_ui->treeTeam->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->treeTeam->setWordWrap(false);
    m_ui->treeTeam->setRootIsDecorated(false);
    m_ui->treeTeam->setAlternatingRowColors(true);

    for (int i = 0; team[i].name; ++i)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, team[i].name);
        item->setText(1, team[i].job);
        m_ui->treeTeam->addTopLevelItem(item);
    }
}

CDlgAbout::~CDlgAbout()
{
    delete m_ui;
}

void CDlgAbout::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CDlgAbout::setGLInfo(const QString &vendor,
                          const QString &renderer,
                          const QString &version,
                          const QString &extensions)
{
    m_ui->sOpenGLRenderer->setText(renderer);
    m_ui->sOpenGLVendor->setText(vendor);
    m_ui->sOpenGLVersion->setText(version);
    QRegExp rx("(\\ )");
    QStringList extList = extensions.split(rx);
    for (QStringList::iterator it = extList.begin();
         it != extList.end(); ++it)
    {
        QString current = *it;
        m_ui->cbExtensions->addItem(current);
    }
}
