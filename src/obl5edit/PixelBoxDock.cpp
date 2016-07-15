/*
    LGCK Builder GUI
    Copyright (C) 2005, 2012  Francois Blanchette

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

#include "PixelBoxDock.h"
#include "ui_PixelBoxDock.h"
#include <QColorDialog>
#include "../shared/stdafx.h"
#include "../shared/Frame.h"


CPixelBoxDock::CPixelBoxDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CPixelBoxDock)
{
    ui->setupUi(this);
    ui->sPalette->init( (uint*)CFrame::dosPal());
    ui->sFavPalette->init(NULL, 0);
    connect(ui->sPalette, SIGNAL(colorChanged(QColor)),
        ui->wiColorWheel, SLOT(setColor(QColor))) ;

    setObjectName("pixelBox");
    setWindowTitle(tr("Toolbox"));
    setMaximumWidth(PIXELBOX_WIDTH);
    setMinimumHeight( 408 -50);
    setMaximumHeight( 408 -50);
    setMinimumWidth(PIXELBOX_WIDTH);
}

CPixelBoxDock::~CPixelBoxDock()
{
    delete ui;
}

void CPixelBoxDock::on_btnBkColor_clicked()
{
    QColorDialog *d = new QColorDialog(this);
    if (d->exec() == QDialog::Accepted) {
        QString s;
        QColor color = d->currentColor();
        uint iColor = color.red() +
                (color.green() << 8) +
                (color.blue() << 16);

        setBkColor(iColor);
        emit bkColorChanged(iColor);
    }

    delete d;
}

void CPixelBoxDock::on_btnGridColor_clicked()
{
    QColorDialog *d = new QColorDialog(this);
    if (d->exec() == QDialog::Accepted) {
        QString s;
        QColor color = d->currentColor();
        uint iColor = color.red() +
                (color.green() << 8) +
                (color.blue() << 16);

        setGridColor(iColor);
        emit gridColorChanged(iColor);
    }

    delete d;
}

void CPixelBoxDock::on_btnBorderColor_clicked()
{
    QColorDialog *d = new QColorDialog(this);
    if (d->exec() == QDialog::Accepted) {
        QString s;
        QColor color = d->currentColor();
        uint iColor = color.red() +
                (color.green() << 8) +
                (color.blue() << 16);

        setBorderColor(iColor);
        emit borderColorChanged(iColor);
    }

    delete d;
}

QString CPixelBoxDock::formatCSS(uint color)
{
    int red = color & 0xff;
    int green = (color >> 8) & 0xff;
    int blue = (color >> 16) & 0xff;
    return QString("* { background-color: rgb(%1,%2,%3) }")
        .arg(red).arg(green).arg(blue);
}

void CPixelBoxDock::setBkColor(uint color)
{
    ui->btnBkColor->setStyleSheet(formatCSS(color));
}

void CPixelBoxDock::setGridColor(uint color)
{
    ui->btnGridColor->setStyleSheet(formatCSS(color));
}

void CPixelBoxDock::setBorderColor(uint color)
{
    ui->btnBorderColor->setStyleSheet(formatCSS(color));
}

const CWPalette *CPixelBoxDock::getFavPalette() const
{
    return ui->sFavPalette;
}

const CWPalette *CPixelBoxDock::getPalette() const
{
    return ui->sPalette;
}

const ColorPickerWidget *CPixelBoxDock::getColorWheel() const
{
    return ui->wiColorWheel;
}

void CPixelBoxDock::closeEvent ( QCloseEvent * event )
{
    Q_UNUSED(event);
    emit windowClosed();
}
