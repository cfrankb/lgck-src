#include "DlgAlpha.h"
#include "ui_DlgAlpha.h"
#include "../shared/stdafx.h"
#include "../shared/FileWrap.h"
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
    CFileWrap file;
    if (file.open(":/images/pd/Rocket000_emblem-spades.png")) {
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
