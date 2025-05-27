/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2005, 2013  Francois Blanchette

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

#ifndef DLGSHORTCUTS_H
#define DLGSHORTCUTS_H

#include <QDialog>
class QSignalMapper;
class QTableWidgetItem;
class CWHotKey;

namespace Ui {
class CDlgShortCuts;
}

class CDlgShortCuts : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgShortCuts(QWidget *parent = 0);
    ~CDlgShortCuts();
    void load(QStringList & listActions, QStringList & listShortcuts, QStringList & listDefaults);
    void save(QStringList & listShortcuts);

private slots:
    void buttonPushed();
    void on_btnReset_clicked();

private:
    Ui::CDlgShortCuts *ui;
    QSignalMapper *m_signalMapper;
    CWHotKey **m_hotkeys;
    QStringList *m_defaultShortcuts;
    int m_count;
};

#endif // DLGSHORTCUTS_H
