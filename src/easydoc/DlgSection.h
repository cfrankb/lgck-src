/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2009, 2010  Francois Blanchette

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


#ifndef DLGSECTION_H
#define DLGSECTION_H

#include <QDialog>
#include "Sections.h"

namespace Ui {
    class CDlgSection;
}

class CDlgSection : public QDialog {
    Q_OBJECT
public:
    CDlgSection(QWidget *parent = 0);
    ~CDlgSection();

    void load(Section & s);
    void save(Section & s);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CDlgSection *m_ui;
};

#endif // DLGSECTION_H
