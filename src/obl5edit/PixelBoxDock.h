/*
    LGCK Builder GUI
    Copyright (C) 2005, 2012  Francois Blanchette

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

#ifndef PIXELBOXDOCK_H
#define PIXELBOXDOCK_H

#include "QtWidgets/qmainwindow.h"
#include <QDockWidget>

namespace Ui {
    class CPixelBoxDock;
}

class CWPalette;
class ColorPickerWidget;

class CPixelBoxDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit CPixelBoxDock(QWidget *parent = 0);
    ~CPixelBoxDock();

    const CWPalette *getPalette() const;
    const ColorPickerWidget * getColorWheel() const;
    const CWPalette *getFavPalette() const;

signals:
    void bkColorChanged(uint color);
    void gridColorChanged(uint color);
    void borderColorChanged(uint color);
    void windowClosed();

private slots:
    void on_btnBkColor_clicked();
    void on_btnGridColor_clicked();
    void on_btnBorderColor_clicked();

    void setBkColor(uint color);
    void setGridColor(uint color);
    void setBorderColor(uint color);

protected:
    virtual void closeEvent ( QCloseEvent * event );
    enum {
         PIXELBOX_WIDTH   = 250
    };

private:
    Ui::CPixelBoxDock *ui;    
    QString formatCSS(uint color);
};

#endif // PIXELBOXDOCK_H
