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

#ifndef DLGLAYER_H
#define DLGLAYER_H

#include <QDialog>

namespace Ui {
    class CDlgLayer;
}

class CDlgLayer : public QDialog {
    Q_OBJECT
public:
    CDlgLayer(QWidget *parent = 0);
    ~CDlgLayer();

    QString getName();
    void setName(const QString s);
    void init();
    void setParams(const QString s, int h, int v);
    void getParams(QString & s, int & h, int & v);
    void setType(int type);
    int getType();
    void lockType();

protected:
    void changeEvent(QEvent *e);
    static QString m_ratios [];

private slots:
    void on_cbType_currentIndexChanged(int index);
    const char *getLayerDesc(int index);

    void on_cCustom_toggled(bool checked);

private:
    Ui::CDlgLayer *m_ui;
};

#endif // DLGLAYER_H
