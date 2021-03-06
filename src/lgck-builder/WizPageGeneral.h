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

#ifndef WIZPAGEGENERAL_H
#define WIZPAGEGENERAL_H

#include <QWizardPage>
class CWizSprite;

class CWizPageGeneral : public QWizardPage
{
    Q_OBJECT
    
public:
    explicit CWizPageGeneral(QWidget *parent = 0);
    ~CWizPageGeneral();

    virtual bool isComplete () const;
    void initializePage();

private:

friend class CWizSprite;

};

#endif // WIZPAGEGENERAL_H
