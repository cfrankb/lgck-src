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
#ifndef WIZFRAMESET_H
#define WIZFRAMESET_H

#include <QWizard>
class CFrameSet;

namespace Ui {
class CWizFrameSet;
}

class CWizFrameSet : public QWizard
{
    Q_OBJECT
    
public:
    explicit CWizFrameSet(QWidget *parent = 0);
    ~CWizFrameSet();
    
    void init(int i);
    const char * getName();
    CFrameSet * getFrameSet();

signals:

    void imagesAdded();
    void imageDeleted();
    void refill();

private slots:
    void on_btnAddImages_clicked();
    void on_btnDeleteImage_clicked();

    void on_treeWidget_itemSelectionChanged();
    void updateButtons();

private:
    Ui::CWizFrameSet *ui;
    CFrameSet *m_frameSet;
};

#endif // WIZFRAMESET_H
