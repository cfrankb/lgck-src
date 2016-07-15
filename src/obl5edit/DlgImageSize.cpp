/*
    LGCK Builder GUI
    Copyright (C) 2012, 2013  Francois Blanchette

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

#include "DlgImageSize.h"
#include "ui_DlgImageSize.h"

#include <QPushButton>

CDlgImageSize::CDlgImageSize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgImageSize)
{
    ui->setupUi(this);
    ui->spHeight->setValue(64);
    ui->spWidth->setValue(64);
}

CDlgImageSize::~CDlgImageSize()
{
    delete ui;
}

int CDlgImageSize::getWidth()
{
    return ui->spWidth->value();
}

int CDlgImageSize::getHeight()
{
    return ui->spHeight->value();
}

void CDlgImageSize::setSize(int width, int height)
{
    ui->spHeight->setValue(height);
    ui->spWidth->setValue(width);
}

void CDlgImageSize::on_spWidth_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    int height = ui->spHeight->value();
    int width = ui->spWidth->value();
    /*if (width % 8 != 0) {
        width = width & 0xfffffff8;
        ui->spWidth->setValue(width);
    }*/

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(width % 8 == 0 && height % 8 == 0);
}

void CDlgImageSize::on_spHeight_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    int height = ui->spHeight->value();
    int width = ui->spWidth->value();
    /*if (height % 8 != 0) {
        height = height & 0xfffffff8;
        ui->spHeight->setValue(height);
    }*/
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(width % 8 == 0 && height % 8 == 0);
}
