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
#include "DlgAlpha.h"
#include "ui_DlgAlpha.h"
#include "../shared/qtgui/qfilewrap.h"
#include "../shared/Frame.h"
#include "../shared/FrameSet.h"
#include <QImage>
#include <QPixmap>

CDlgAlpha::CDlgAlpha(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgAlpha)
{
    ui->setupUi(this);
    m_alpha =0;

    CFrame *frame = new CFrame(256, 4);
    for (int i=0; i < 256; ++i) {
        frame->at(i,0) = 0xff000000 + i + (i <<8) + (i <<16);
        frame->at(i,1) = 0xff000000 + i + (i <<8) + (i <<16);
        frame->at(i,2) = 0xff000000 + i + (i <<8) + (i <<16);
        frame->at(i,3) = 0xff000000 + i + (i <<8) + (i <<16);
    }

    int pngSize;
    uint8_t *png;
    frame->toPng(png, pngSize);
    QImage img;
    if (!img.loadFromData( png, pngSize )) {
        qDebug("failed to load png\n");
    }
    delete [] png;
    delete frame;
    QPixmap pm = QPixmap::fromImage(img);
    ui->sImgBar->setPixmap(pm);
    ui->eAlpha->setText(QString("%1").arg(m_alpha));
    updateAlpha();
}

CDlgAlpha::~CDlgAlpha()
{
    delete ui;
}

void CDlgAlpha::on_sldAlpha_sliderMoved(int position)
{
    m_alpha = position;
    ui->eAlpha->setText(QString("%1").arg(position));
    updateAlpha();
}

void CDlgAlpha::updateAlpha()
{
    QFileWrap file;
    const char *filename = ":/images/pd/Rocket000_emblem-spades.png";
    if (file.open(filename)) {
    //if (file.open(":/images/pd/pierig_four-leaf_clover_1.png")) {
        CFrameSet set;
        if (set.extract(file)) {
            uchar *png;
            int pngSize;
            set[0]->fade(m_alpha);
            set[0]->toPng(png, pngSize);
            QImage img;
            if (!img.loadFromData( png, pngSize )) {
                qDebug("failed to load png\n");
            }
            delete [] png;
            QPixmap pm = QPixmap::fromImage(img);

            ui->sImgAlpha->setPixmap(pm);
            //m_actionAlpha->setIcon(QIcon(pm));
        }
        file.close();
    } else {
        qDebug("can't open %s", filename);
    }
}

void CDlgAlpha::on_eAlpha_textChanged(const QString &arg1)
{
    m_alpha = arg1.toInt();
    ui->sldAlpha->setValue(m_alpha);
    updateAlpha();
}

int CDlgAlpha::alpha()
{
   return m_alpha;
}

void CDlgAlpha::setAlpha(int alpha)
{
    m_alpha = alpha;
    ui->sldAlpha->setValue(m_alpha);
    ui->eAlpha->setText(QString("%1").arg(m_alpha));
    updateAlpha();
}

void CDlgAlpha::on_sldAlpha_valueChanged(int value)
{
    m_alpha = value;
    ui->eAlpha->setText(QString("%1").arg(value));
    updateAlpha();
}
