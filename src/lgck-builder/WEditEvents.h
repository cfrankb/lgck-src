/*
    LGCK Builder GUI
    Copyright (C) 1999, 2012  Francois Blanchette

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

#ifndef WEDITEVENTS_H
#define WEDITEVENTS_H

#include <QDockWidget>

namespace Ui {
    class CWEditEvents;
}

class CWEdit;
class QAbstractButton;
class CGameFile;

class CWEditEvents : public QDockWidget
{
    Q_OBJECT
public:
    explicit CWEditEvents(QWidget *parent = 0);
    ~CWEditEvents();
    void setGameFile(CGameFile *game);
protected:
    void resizeEvent (QResizeEvent *event);
    CGameFile * m_game;
    int m_proto;
    int m_event;
    QString *m_events;
    void reload();
private slots:
    void setProto(int proto, int event=-1);
    void setEvent(int event);
    void commitEvents();
    void deleteSprite(int sprite);
    void insertText(const char *text);
    void setFont(const QFont & font);
signals:
    void eventChanged(int proto, int event);
    void eventModified();
    void viewModeChanged();
private:
    Ui::CWEditEvents *ui;
};

#endif // WEDITEVENTS_H
