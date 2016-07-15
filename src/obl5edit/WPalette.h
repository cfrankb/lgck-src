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

#ifndef WPALETTE_H
#define WPALETTE_H

#include <QLabel>

class CWPalette : public QLabel
{
    Q_OBJECT

public:
    explicit CWPalette(QWidget *parent = 0);
    ~CWPalette();
    void init(uint *colors, int count=MAX_COLORS);
    const uint* palette() const;

protected:

    typedef struct {
        int x;
        int y;
        bool lButton;
        bool rButton;
        bool mButton;
    } MOUSE;

    MOUSE m_mouse;

    void mousePressEvent ( QMouseEvent * event );
    void mouseReleaseEvent ( QMouseEvent * event );
    void mouseMoveEvent ( QMouseEvent * event );

    uint *m_palette;
    int m_count;
    void updateView();

Q_SIGNALS:
    void colorChanged(const QColor &col);
    void colorPicked(uint color);
    void bkColorPicked(uint color);

protected slots:
    void addColor(const uint color);
    void palChanged(uint *colors, int count=MAX_COLORS);

private:
    enum {
        GRID_SIZE = 12,
        ROW_SIZE  = 16,
        MAX_COLORS = 256
    };
};

#endif // WPALETTE_H
