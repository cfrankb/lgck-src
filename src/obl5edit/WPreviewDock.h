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

#ifndef WIMAGEPRIEVIEW_H
#define WIMAGEPRIEVIEW_H

#include <QDockWidget>
class CWPreview;
class QString;

namespace Ui {
    class CWPreviewDock;
}

class CWPreviewDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit CWPreviewDock(QWidget *parent = 0);
    ~CWPreviewDock();

    CWPreview * getView();
    void setStatic();
    void setTitleTemplate(const QString & s);
private:
    Ui::CWPreviewDock *ui;
    bool m_isStatic;
    QString m_titleTemplate;

protected:
    void resizeEvent ( QResizeEvent * event );
    virtual void closeEvent ( QCloseEvent * event );

protected slots:
    void updateTitle(int zoom);

signals:
    void windowClosed();
};

#endif // WIMAGEPRIEVIEW_H
