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
#ifndef WSLIDER_H
#define WSLIDER_H

#include <QSlider>
class QLineEdit;

class CWSlider : public QSlider
{
    Q_OBJECT
public:
    CWSlider(QWidget *parent);
    void setBuddy(QLineEdit *q);

signals:
    void valueChanged(const QString & s);

public slots:
    void setValue(int i);
    void setValue(const QString & s);

private:
    QLineEdit * m_buddy;
};

#endif // WSLIDER_H
