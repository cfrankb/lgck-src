#include "dlgdisplay.h"
#include "ui_dlgdisplay.h"

#include <QString>
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include "Display.h"

#include "../shared/stdafx.h"
#include "../shared/qtgui/cheat.h"
#include "GameFile.h"
#include "Frame.h"

#define STR(__INT__) QString().sprintf("%d", __INT__)
#define TEXT(__CX__) q2c(__CX__->text())
#define COLOR(RGB) (RGB & 0xff0000) >> 16, (RGB & 0xff00) >> 8, (RGB & 0xff)
#define TOINT(__E__) __E__->text().toInt()

CDlgDisplay::CDlgDisplay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgDisplay)
{
    ui->setupUi(this);
}

CDlgDisplay::~CDlgDisplay()
{
    delete ui;
}

void CDlgDisplay::load(CDisplay & d)
{
    CGameFile & gf = *(m_gameFile);

    // set current tab -> page 1
    ui->tabWidget->setCurrentIndex(0);

    ui->eName->setEnabled(!d.isProtected());

    const QString types[] = {
        tr("Time Left"),
        tr("Message"),
        tr("Score"),
        tr("Lives"),
        tr("Hit Points / Health"),
        tr("Debug"),
        tr("Image")
    };

    for (unsigned int i=0; i < sizeof(types)/sizeof(QString); ++i) {
        ui->cbType->addItem(types[i]);
    }

    // page 1
    ui->eName->setText(d.name());
    ui->cbType->setCurrentIndex(d.type());
    ui->cVisible->setChecked(d.visible());
    ui->btnColor->setBuddy(ui->eColor);
    ui->eColor->setText(QString().sprintf("%.2X%.2X%.2X", d.red(), d.green(), d.blue()));
    ui->eFontSize->setText(STR(d.size()));
    ui->eX->setText(STR(d.x()));
    ui->eY->setText(STR(d.y()));
    ui->sAlpha->setBuddy(ui->eAlpha);
    ui->sAlpha->setSliderPosition(d.alpha());

    // page 2
    ui->btnShadowColor->setBuddy(ui->eShadowColor);
    ui->eShadowColor->setText(QString().sprintf("%.2X%.2X%.2X", d.shadowR(), d.shadowG(), d.shadowB()));
    ui->sShadowAlpha->setBuddy(ui->eShadowAlpha);
    ui->sShadowAlpha->setSliderPosition(d.shadowA(
                                            ));
    ui->eShadowX->setText(STR(d.shadowX()));
    ui->eShadowY->setText(STR(d.shadowY()));
    ui->cShadowVisible->setChecked(d.shadow());

    // page 3
    ui->eTemplate->setText(d.templateStr());
    ui->eText->setText(d.text());
    setImage(d.imageSet(), d.imageNo());

    // frame set

    for (int n=0; n < gf.m_arrFrames.getSize(); ++n) {
        CFrameSet & frameSet = *gf.m_arrFrames[n];
        UINT8 *png;
        int size;
        frameSet[0]->toPng(png, size);

        QImage img;
        if (!img.loadFromData( png, size )) {
            qDebug("failed to load png (%d)\n", n);
        }
        delete [] png;

        QPixmap pm = QPixmap::fromImage(img);
        QIcon icon;
        icon.addPixmap(pm, QIcon::Normal, QIcon::On);
        ui->cbFrameSet->addItem(icon, frameSet.getName() );
    }
    ui->cbFrameSet->setCurrentIndex( d.imageSet() );

    // frame no

    fillFrameCombo( d.imageSet() );
    ui->cbBaseFrame->setCurrentIndex( d.imageNo() );

    enableType(d.type());
}

void CDlgDisplay::save(CDisplay & d)
{
    // page 1
    d.setName(TEXT(ui->eName));
    d.setText(TEXT(ui->eText));
    d.setType(ui->cbType->currentIndex());
    d.setVisible(ui->cVisible->isChecked());
    d.setColor(COLOR(ui->btnColor->colorBGR()), ui->sAlpha->sliderPosition());
    d.setFontSize(TOINT(ui->eFontSize));
    d.setXY(TOINT(ui->eX), TOINT(ui->eY));

    // page 2
    d.setShadowColor(COLOR(ui->btnShadowColor->colorBGR()), ui->sShadowAlpha->sliderPosition());
    d.setShadow(ui->cShadowVisible->isChecked(), TOINT(ui->eShadowX), TOINT(ui->eShadowY));

    // page 3
    d.setTemplate(TEXT(ui->eTemplate));
    if (d.type() == CDisplay::DISPLAY_IMAGE) {
        d.setImage(ui->cbFrameSet->currentIndex(), ui->cbBaseFrame->currentIndex());
    } else {
        d.setImage(0, 0);
    }
}

void CDlgDisplay::enableType(int type)
{
    ui->eTemplate->setEnabled(true);
    ui->eText->setEnabled(true);
    ui->cbBaseFrame->setEnabled(false);
    ui->cbFrameSet->setEnabled(false);
    bool keepIcon = false;

    switch(type) {
    case CDisplay::DISPLAY_TIME_LEFT:
        ui->eText->setEnabled(false);
        break;
    case CDisplay::DISPLAY_MESSAGE:
        ui->eTemplate->setEnabled(true);
        break;
    case CDisplay::DISPLAY_LIVES:
        ui->eText->setEnabled(false);
        break;
    case CDisplay::DISPLAY_SCORE:
        ui->eText->setEnabled(false);
        break;
    case CDisplay::DISPLAY_DEBUGX:
        ui->eText->setEnabled(false);
        ui->eTemplate->setEnabled(false);
        break;
    case CDisplay::DISPLAY_HP:
        ui->eText->setEnabled(false);
        ui->eTemplate->setEnabled(false);
        break;
    case CDisplay::DISPLAY_IMAGE:
        ui->eText->setEnabled(false);
        ui->eTemplate->setEnabled(false);
        ui->cbBaseFrame->setEnabled(true);
        ui->cbFrameSet->setEnabled(true);
        keepIcon = true;
        int frameSet = ui->cbFrameSet->currentIndex();
        int frameNo = ui->cbBaseFrame->currentIndex();
        setImage(frameSet, frameNo);
        break;
    }

    if (!keepIcon) {
        ui->sImage->clear();
    }
}

void CDlgDisplay::on_cbType_currentIndexChanged(int index)
{
    enableType(index);
}

void CDlgDisplay::setImage(int frameSet, int frameNo)
{
    if (!frameSet) {
        ui->sImage->clear();
        return;
    }
    CGameFile & gf = *m_gameFile;
    CFrameSet & fs = *gf.m_arrFrames[frameSet];

    UINT8 *png;
    int size;
    fs[frameNo]->toPng(png, size);

    QImage img;
    if (!img.loadFromData( png, size )) {
        qDebug("failed to load png $$\n");
    }
    delete [] png;

    QPixmap pm = QPixmap::fromImage(img);
    ui->sImage->setPixmap(pm);
}

void CDlgDisplay::fillFrameCombo(int frameSet)
{
    CGameFile & gf = *(m_gameFile);
    CFrameSet & fs = *gf.m_arrFrames[frameSet];

    ui->cbBaseFrame->clear();
    if (!frameSet) {
        return;
    }

    int imgCount = fs.getSize();
    unsigned char **imgPng = new UINT8 *[imgCount];
    int *imgSize = new int [imgCount];

    for (int i=0; i < imgCount; ++i) {
        fs[i]->toPng((imgPng)[i], imgSize[i]);
        QIcon icon = makeIcon(imgPng[i], imgSize[i]);
        ui->cbBaseFrame->addItem(icon, QString("%1").arg(i + 1));
    }

    // free imageSet
    delete [] imgSize;
    for (int i=0; i < imgCount; ++i) {
        delete [] imgPng[i];
        imgPng[i] = NULL;
    }
    delete [] imgPng;
}

QIcon CDlgDisplay::makeIcon(void *png, int size)
{
    QImage img;
    if (!img.loadFromData( (UINT8*)png, size )) {
        qDebug("failed to load png");
    }

    QPixmap pm = QPixmap::fromImage(img);
    QIcon icon;
    icon.addPixmap(pm, QIcon::Normal, QIcon::On);
    return icon;
}

void CDlgDisplay::setGameFile(CGameFile *gf)
{
    m_gameFile = gf;
}

void CDlgDisplay::on_cbFrameSet_currentIndexChanged(int index)
{
    fillFrameCombo(index);
    setImage(index, 0);
}

void CDlgDisplay::on_cbBaseFrame_currentIndexChanged(int index)
{
    int frameSet = ui->cbFrameSet->currentIndex();
    int frameNo = index;
    setImage(frameSet, frameNo);
}
