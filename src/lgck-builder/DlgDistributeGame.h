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
#ifndef DLGDISTRIBUTEGAME_H
#define DLGDISTRIBUTEGAME_H

#include <QDialog>

namespace Ui {
class CDlgDistributeGame;
}

class CDlgDistributeGame : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgDistributeGame(QWidget *parent = 0);
    enum {
        OUT_ZIP,
        OUT_7ZIP,
        OUT_SFX,
        OUT_NONE=-1
    };
    int outType();
    void getRuntime(QString & runtime);
    void getSFX(QString & sfx);
    ~CDlgDistributeGame();

private slots:
    void on_btnRuntime_clicked();
    void on_btnSFX_clicked();
    void on_rZip_clicked();
    void on_r7z_clicked();
    void on_rSFX_clicked();

    void on_eRuntime_textChanged(const QString &arg1);

    void on_eSFX_textChanged(const QString &arg1);

private:
    Ui::CDlgDistributeGame *ui;
    void updateButtons();
};

#endif // DLGDISTRIBUTEGAME_H
