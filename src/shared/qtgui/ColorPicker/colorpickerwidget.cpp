/*

Copyright (C) 2009, Etienne Moutot <e.moutot@gmail.com>

This file is part of colorPickerWidget.

    colorPickerWidget is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    colorPickerWidget is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "colorpickerwidget.h"
#include <QScreen>
#include <QGuiApplication>

ColorPickerWidget::ColorPickerWidget(QWidget *parent)
    : QWidget(parent)
{
    ecran=0;
    colorTriangle = new QtColorTriangle(this);

    connect(colorTriangle, SIGNAL(colorChanged(QColor)), this, SLOT(colorChgd()));

    colorView = new ColorViewer(this);
    colorView->setColor(colorTriangle->color());
    connect(colorTriangle, SIGNAL(colorChanged(QColor)), colorView, SLOT(changeColor(QColor)));
    connect(colorTriangle, SIGNAL(colorChanged(QColor)), this, SLOT(changeColor(QColor)));

    pickColor = new QPushButton(tr("Pick a color"), this);
    connect(pickColor, SIGNAL(released()), this, SLOT(pickMode()));

    layout = new QGridLayout;
    layout->addWidget(  colorTriangle,  0, 0, 3, 2);
    layout->addWidget(  colorView,      0, 2, 2, 1);
    layout->addWidget(  pickColor,      2, 2, 1, 1);

    setLayout(layout);
}

ColorPickerWidget::~ColorPickerWidget()
{
}

void ColorPickerWidget::pickMode()
{
    if(ecran!=0) {
        delete ecran;
    }
    ecran = new CScreen();
    connect( ecran, SIGNAL(colorSelected(QColor)), colorTriangle, SLOT(setColor(QColor)) );
}

void ColorPickerWidget::setColor(const QColor &col)
{
    colorView->setColor(col);
    colorTriangle->setColor(col);
    colorView->repaint();
    colorTriangle->repaint();
    uint color = col.red() + (col.green() << 8) + (col.blue() << 16);
    emit colorChanged(color);
}

void ColorPickerWidget::setColor(uint color)
{
    QColor col(color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff );
    colorView->setColor(col);
    colorTriangle->setColor(col);
    colorView->repaint();
    colorTriangle->repaint();
}

void ColorPickerWidget::colorChgd()
{
    emit colorChanged(colorView->color());
    QColor col = colorView->color();
    uint color = col.red() + (col.green() << 8) + (col.blue() << 16);
    emit colorChanged(color);
}

void ColorPickerWidget::changeColor(const QColor &col)
{
    uint color = col.red() + (col.green() << 8) + (col.blue() << 16);
    emit colorChanged(color);
}
