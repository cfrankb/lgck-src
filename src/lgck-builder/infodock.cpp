/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

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
#include "infodock.h"
#include "ui_infodock.h"

CInfoDock::CInfoDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CInfoDock)
{
    ui->setupUi(this);
    this->setWidget(ui->plainTextEdit);
}

CInfoDock::~CInfoDock()
{
    delete ui;
}

void CInfoDock::appendText(const QString & text)
{
    ui->plainTextEdit->appendPlainText(text);
}

void CInfoDock::appendError(const QString & text)
{
    QString error = "<font color=red>" + text + "</font>";
    ui->plainTextEdit->appendHtml(error);
}

void CInfoDock::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
    emit visibilityChanged(false);
}
