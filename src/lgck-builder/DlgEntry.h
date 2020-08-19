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

#ifndef DLGENTRY_H
#define DLGENTRY_H

#include <QDialog>

class CGameFile;
class CPath;
class CSelection;

namespace Ui {
    class CDlgEntry;
}

class CDlgEntry : public QDialog {
    Q_OBJECT
public:
    CDlgEntry(QWidget *parent = 0);
    ~CDlgEntry();

    void setGameDB(CGameFile * p) {
        m_gameFile = p;
    }

    void load(const int entryPos);
    void save(const int entryPos, bool isMulti);
    void loadMultiSelection(CSelection & selection);

protected:
    void saveNonCombining(const int entryPos);
    void init();
    void changeEvent(QEvent *e);
    void updateText();
    void enableCheckboxes();
    void setImage(int frameSet, int frameNo);
    void enablePlaybackOptions(bool enable);

    CGameFile * m_gameFile;
    CPath *m_path;
    int m_proto;
    int m_entry;
    int m_currImage;
    int m_currSet;
    bool m_deletePath;

private:
    Ui::CDlgEntry *m_ui;

private slots:
    void on_btnObject_clicked();
    void on_btnNextImage_clicked();
    void on_btnPrevImage_clicked();
    void on_cPlayback_toggled(bool checked);
    void on_btnDeletePath_clicked();
    void on_cCustomHandler_clicked();
    void on_btnRawPath_clicked();
};

#endif // DLGENTRY_H
