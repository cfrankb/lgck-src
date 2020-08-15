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
#ifndef WIZFONT_H
#define WIZFONT_H

#include <QWizard>

class QString;
class CFont;

namespace Ui {
class CWizFont;
}

class CWizFont : public QWizard
{
    Q_OBJECT

public:
    explicit CWizFont(QWidget *parent = 0);
    ~CWizFont();
    void importFont(CFont & font,QString &name);

private slots:
    void on_cbFonts_currentIndexChanged(const QString &arg1);

private:
    Ui::CWizFont *ui;
    enum {
        XBASE = 0,
        YBASE = 16,
        XSCALE = 20,
        YSCALE = 22,
        WIDTH = 256,
        FONTSIZE = 14
    };
    void updateFont(const QString &);
    void createBitmap(QImage & image, QString fontName, bool bold, bool grid, bool white);
    static char m_text[];
};

#endif // WIZFONT_H
