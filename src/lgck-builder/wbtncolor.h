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
#ifndef WBTNCOLOR_H
#define WBTNCOLOR_H

#include <QPushButton>
class QLineEdit;

class CWBtnColor : public QPushButton
{
    Q_OBJECT
public:
    explicit CWBtnColor(QWidget *parent = 0);
    void setBuddy(QLineEdit *q);

signals:
    void colorChanged(int &red, int &green, int &blue);
    void colorChanged(const QString & color);

public slots:
    void setColor(int red, int green, int blue);
    void setColor(const QString & color);
    void click();
    unsigned int color();
    unsigned int colorBGR();

protected:
    void style();
    int m_red;
    int m_green;
    int m_blue;
    QLineEdit *m_buddy;
};

#endif // WBTNCOLOR_H
