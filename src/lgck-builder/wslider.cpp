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
#include "wslider.h"
#include <string>
#include <QLineEdit>

CWSlider::CWSlider(QWidget *parent):
    QSlider(parent)
{
    m_buddy = NULL;
}

void CWSlider::setBuddy(QLineEdit *q)
{
    connect(q, SIGNAL(textChanged(QString)), this, SLOT(setValue(QString)));
    connect(this, SIGNAL(valueChanged(QString)), q, SLOT(setText(QString)));
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
}

void CWSlider::setValue(int i)
{
    QString s = QString::asprintf("%d", i);
    emit valueChanged(s);
}

void CWSlider::setValue(const QString & s)
{
    int i = s.toInt();
    if (i < minimum()) {
        i = minimum();
        setValue(i);
    } else if (i > maximum()) {
       i = maximum();
       setValue(i);
    }
    setSliderPosition(i);
}
