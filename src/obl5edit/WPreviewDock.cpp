/*
    LGCK Builder GUI
    Copyright (C) 2011, 2013  Francois Blanchette

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

#include "WPreviewDock.h"
#include "ui_WPreviewDock.h"
#include <QResizeEvent>
#include <QSize>
#include <QGLWidget>
#include "previewwidget.h"
#include "previewscroll.h"

CWPreviewDock::CWPreviewDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CWPreviewDock)
{    
    m_titleTemplate = "noname - %d%%";
    m_isStatic = false;
    ui->setupUi(this);
    m_scroll = new CPreviewScroll(this);
    m_widget = static_cast<CPreviewWidget *>(m_scroll->viewport());
    ui->gridLayout->addWidget(m_scroll);
    connect(m_widget, SIGNAL(zoomChanged(int)),
            this, SLOT(updateTitle(int)));
    connect(m_scroll, SIGNAL(zoomChanged(int)),
            this, SLOT(updateTitle(int)));
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(newDockedLocation(Qt::DockWidgetArea)));
}

CWPreviewDock::~CWPreviewDock()
{
    delete ui;
}

void CWPreviewDock::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event);
    m_widget->update();
}

CPreviewWidget *CWPreviewDock::getView()
{
    return m_widget;
}

void CWPreviewDock::closeEvent ( QCloseEvent * event )
{
    Q_UNUSED(event);
    emit windowClosed();
    if (m_isStatic) {
        QDockWidget::closeEvent(event);
    }
}

void CWPreviewDock::setStatic()
{
    m_isStatic = true;
}

void CWPreviewDock::updateTitle(int zoom)
{
    QString s = QString(m_titleTemplate).arg(zoom * 100);
    setWindowTitle(s);
}

void CWPreviewDock::setTitleTemplate(const QString &s)
{
    m_titleTemplate = s;
}

void CWPreviewDock::newDockedLocation(Qt::DockWidgetArea area)
{
    Q_UNUSED(area);
    m_widget->update();
}
