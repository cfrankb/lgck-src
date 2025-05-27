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

#include <QtGui>
#include <QApplication>
#include <QScreen>
#include "screen.h"

CScreen::CScreen()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    label = new QLabel(this);
    label->setPixmap(screen->grabWindow());
    label->move(0,0);
    showFullScreen();
}

void CScreen::mousePressEvent(QMouseEvent *event)
{
    emit colorSelected( label->pixmap().toImage().pixel( event->pos() ) );
    close();
}
