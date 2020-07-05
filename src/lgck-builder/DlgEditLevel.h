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

#ifndef DLGEDITLEVEL_H
#define DLGEDITLEVEL_H

#include <QDialog>
#include <QModelIndex>
class CGameFile;
class QLineEdit;
class CLevel;

namespace Ui {
    class CDlgEditLevel;
}

class CDlgEditLevel : public QDialog {
    Q_OBJECT
public:
    CDlgEditLevel(QWidget *parent = 0);
    ~CDlgEditLevel();
    void setGameDB(CGameFile * p) {
        m_gameFile = p;
    }
    void setNewLevel();
    void load(CLevel *);
    void save(CLevel *);
protected:
    void changeEvent(QEvent *e);
    void setTitle(const QString title);
    void setHint(const QString hint);
    void setColor(QLineEdit *edit, const QString color);
private:
    Ui::CDlgEditLevel *m_ui;
    CLevel *m_level;
    CGameFile *m_gameFile;
    QString getTitle();
    QString getHint();
    QString getColor(QLineEdit *edit);
    bool m_bNewLevel;
private slots:
    void on_treeEvents_doubleClicked(QModelIndex index);
    void on_cCanvas_toggled(bool checked);
    void on_btnWorldSize_clicked();
    void on_btnColorMod_clicked();
    void on_pushButton_clicked();
    void on_btnMusic_clicked();
};

#endif // DLGEDITLEVEL_H
