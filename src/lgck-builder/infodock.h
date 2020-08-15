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
#ifndef INFODOCK_H
#define INFODOCK_H

#include <QDockWidget>

namespace Ui {
class CInfoDock;
}

class CInfoDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit CInfoDock(QWidget *parent = nullptr);
    ~CInfoDock();

public slots:
    void appendText(const QString & text);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::CInfoDock *ui;
};

#endif // INFODOCK_H
