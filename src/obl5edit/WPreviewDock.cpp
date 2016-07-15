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

CWPreviewDock::CWPreviewDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CWPreviewDock)
{    
    m_titleTemplate = "noname - %d%%";
    m_isStatic = false;
    ui->setupUi(this);        
    connect(ui->graphicsView, SIGNAL(zoomChanged(int)),
            this, SLOT(updateTitle(int)));
}

CWPreviewDock::~CWPreviewDock()
{
    delete ui;
}

void CWPreviewDock::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event);
}

CWPreview * CWPreviewDock::getView()
{
    return ui->graphicsView;
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
    /*
    QString s;
    if (m_isStatic) {
        s.sprintf("Static %d%%", zoom * 100);
    } else {
        s.sprintf("Liveview %d%%", zoom * 100);
    }*/

    QString s = QString(m_titleTemplate).arg(zoom * 100);
    setWindowTitle(s);
}

void CWPreviewDock::setTitleTemplate(const QString &s)
{
    m_titleTemplate = s;
}
