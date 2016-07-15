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

#include "WEditEvents.h"
#include "ui_WEditEvents.h"
#include "WEdit.h"
#include <algorithm>
#include <QResizeEvent>
#include <QAbstractButton>
#include "../shared/stdafx.h"
#include "../shared/GameFile.h"
#include "../shared/qtgui/cheat.h"
#include "DlgSource.h"

CWEditEvents::CWEditEvents(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CWEditEvents)
{
    ui->setupUi(this);
    m_game = NULL;
    m_proto = -1;
    m_event = -1;    
    m_events = new QString[CProtoArray::getEventCount()];
    ui->eSource->setFocus();
}

CWEditEvents::~CWEditEvents()
{
    delete ui;
    delete [] m_events;
}

void CWEditEvents::resizeEvent (QResizeEvent *event)
{
    QSize sizeDlg = event->size();
    ui->eSource->resize(sizeDlg.width() - 4, sizeDlg.height() );
}

void CWEditEvents::setGameFile(CGameFile *game)
{
    m_game = game;
}

void CWEditEvents::setProto(int proto, int event)
{
    if (event == -1) {
        event = 0;
    }

    if (proto != -1) {
        if (proto != m_proto) {
            if (m_proto != -1) {
                commitEvents();
            }
            CObject & object = m_game->m_arrProto.getObject(proto);
            for (int i=0; i < CProtoArray::getEventCount(); ++i) {
                m_events[i] = QString(object.getEvent( i )).trimmed();
            }
        } else {
            if (event != m_event && m_event != -1) {
                m_events[ m_event ] = ui->eSource->text().trimmed();
                CObject & object = m_game->m_arrProto.getObject(proto);
                object.setEvent( event, q2c(m_events[event]));
                emit eventModified();
            }
        }
    }
    m_proto = proto;   
    m_event = event;
    reload();
}

void CWEditEvents::setEvent(int event)
{
    m_event = event;
    reload();
}

void CWEditEvents::reload()
{
    if (m_events [ m_event ].isEmpty()) {
        ui->eSource->setText( "" );
    } else {
        ui->eSource->setText( m_events [ m_event ] + "\n");
    }
    ui->eSource->setFocus();
}

void CWEditEvents::commitEvents()
{
    if (m_event != -1) {
        m_events[ m_event ] = ui->eSource->text().trimmed();
    }
    if (m_proto != -1) {
        CObject & object = m_game->m_arrProto.getObject(m_proto);
        for (int i=0; i < CProtoArray::getEventCount(); ++i) {
            if (m_events[i] != QString(object.getEvent( i )).trimmed()) {
                object.setEvent( i, q2c(m_events[i]));
                m_game->setDirty( true );
                emit eventModified();
            }
        }
    }
}

void CWEditEvents::deleteSprite(int sprite)
{
    if (sprite > m_proto) {
        -- m_proto;
    } else if (sprite == m_proto) {
        m_proto = -1;
        m_event = -1;
        emit viewModeChanged();
    }
}

void CWEditEvents::insertText(const char *text)
{
    int line;
    int index;
    ui->eSource->getCursorPosition(&line, &index);
    ui->eSource->insertAt(text, line, index);
    std::string s = text;
    size_t n = std::count(s.begin(), s.end(), '\n');
    ui->eSource->setCursorPosition(line + n, 0);
    emit eventModified();
}

void CWEditEvents::setFontSize(int size)
{
    ui->eSource->setFontSize(size);
    CDlgSource::setFontSize(size);
}
