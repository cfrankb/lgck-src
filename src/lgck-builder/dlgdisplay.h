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
#ifndef DLGDISPLAY_H
#define DLGDISPLAY_H

#include <QDialog>
class CDisplay;
class CGameFile;

namespace Ui {
class CDlgDisplay;
}

class CDlgDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgDisplay(QWidget *parent = 0);
    ~CDlgDisplay();

    void load(CDisplay & d);
    void save(CDisplay & d);
    void setGameFile(CGameFile *gf);
    void setDisplayID(int id);

private slots:
    void on_cbType_currentIndexChanged(int index);
    void on_cbFrameSet_currentIndexChanged(int index);
    void on_cbBaseFrame_currentIndexChanged(int index);
    void on_eName_textChanged(const QString &arg1);
    void on_cbFlagX_currentIndexChanged(int index);
    void on_cbFlagY_currentIndexChanged(int index);

private:
    void setImage(int frameSet, int frameNo);
    void fillFrameCombo(int frameSet);
    CGameFile *m_gameFile;
    Ui::CDlgDisplay *ui;
    int m_id;
    void enableType(int type);
};

#endif // DLGDISPLAY_H
