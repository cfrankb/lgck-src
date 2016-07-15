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

#include "WizPageImages.h"
#include <QVariant>
#include <QString>

CWizPageImages::CWizPageImages(QWidget *parent) :
    QWizardPage(parent)
{
}


bool CWizPageImages::isComplete () const
{
    bool result = false;
    if (field("old").toString() == "true") {
        // old set
        result = field("frameSet").toInt() != 0;
    } else {
        // new set
        result = field("tree").toInt() != 0;
    }

    return result;
}

void CWizPageImages::initializePage()
{
}
