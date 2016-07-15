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

#include "DlgAbout.h"
#include "ui_DlgAbout.h"
#include "FileWrap.h"

CDlgAbout::CDlgAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgAbout)
{
    ui->setupUi(this);

    CFileWrap file;
    if (file.open(":/res/gpl.txt")) {
        int size = file.getSize();
        char *buf = new char[ size + 1 ];
        buf [size ] = 0;
        file.read(buf, size);
        file.close();
        ui->eLicense->setPlainText( buf );
        delete [] buf;
    }

}

CDlgAbout::~CDlgAbout()
{
    delete ui;
}

void CDlgAbout::changeEvent(QEvent *e)
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
