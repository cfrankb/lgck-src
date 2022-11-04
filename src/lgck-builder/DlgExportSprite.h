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
#ifndef DLGEXPORTSPRITE_H
#define DLGEXPORTSPRITE_H

#include <QDialog>

class CGameFile;
class QTreeWidgetItem;

namespace Ui {
class CDlgExportSprite;
}

class CDlgExportSprite : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgExportSprite(QWidget *parent, CGameFile *gf);
    ~CDlgExportSprite();

private slots:
    void on_btnExport_clicked();
    void updateBtnExport();

private:
    Ui::CDlgExportSprite *ui;
    CGameFile *m_gameFile;
    void reloadSprites();
    void updateIcon(QTreeWidgetItem * item, int protoId);
};

#endif // DLGEXPORTSPRITE_H
