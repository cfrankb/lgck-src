/*
    LGCK Builder GUI
    Copyright (C) 1999, 2011  Francois Blanchette

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

#ifndef DLGSOURCE_H
#define DLGSOURCE_H

#include <QDialog>

namespace Ui {
    class CDlgSource;
}

class CDlgSource : public QDialog {
    Q_OBJECT
public:
    CDlgSource(QWidget *parent = 0);
    ~CDlgSource();

    void setText(const QString s);
    const QString getText();
    void setReadOnly() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CDlgSource *m_ui;
};

#endif // DLGSOURCE_H
