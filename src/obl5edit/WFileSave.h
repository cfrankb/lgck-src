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
#ifndef WFILESAVE_H
#define WFILESAVE_H

#include <QFileDialog>

class CWFileSave : public QFileDialog
{
    Q_OBJECT
public:
    //explicit CWFileSave(QObject *parent = 0);
    explicit CWFileSave(QWidget *parent = 0,
         const QString &caption = QString(),
         const QString &directory = QString(),
         const QString &filter = QString());
signals:
    
public slots:

private slots:
    void changeDefaultSuffix ( const QString & filter );
    
};

#endif // WFILESAVE_H
