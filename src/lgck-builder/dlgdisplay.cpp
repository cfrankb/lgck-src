#include "dlgdisplay.h"
#include "ui_dlgdisplay.h"

#include <QString>
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include "Display.h"
#include "displayconfig.h"
#include "fontmanager.h"

#include "../shared/stdafx.h"
#include "../shared/qtgui/cheat.h"
#include "GameFile.h"
#include "Frame.h"

#define STR(__INT__) QString::asprintf("%d", __INT__)
#define TEXT1(__CX__) q2c(__CX__->text().trimmed())
#define COLOR(RGB) (RGB & 0xff0000) >> 16, (RGB & 0xff00) >> 8, (RGB & 0xff)
#define TOINT(__E__) __E__->text().toInt()

CDlgDisplay::CDlgDisplay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgDisplay)
{
    ui->setupUi(this);
    m_id = CDisplayConfig::NOT_FOUND;
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
    ui->cbType->setEnabled(!d.isProtected());
  //  ui->eName->setFocus();

    const QString types[] = {
        tr("Time Left"),
        tr("Message"),
        tr("Score"),
        tr("Lives"),
        tr("Hit Points / Health"),
        tr("Debug"),
        tr("Image"),
        tr("Healthbar")
    };

    for (unsigned int i=0; i < sizeof(types)/sizeof(QString); ++i) {
        ui->cbType->addItem(types[i]);
    }

    const QString flagX[]= {
        tr("manual"),
        tr("left"),
        tr("right"),
        tr("center")
    };

    const QString flagY[]= {
        tr("manual"),
        tr("top"),
        tr("bottom"),
        tr("center")
    };

    for (unsigned int i=0; i < sizeof(flagX)/sizeof(QString); ++i) {
        ui->cbFlagX->addItem(flagX[i]);
    }

    for (unsigned int i=0; i < sizeof(flagY)/sizeof(QString); ++i) {
        ui->cbFlagY->addItem(flagY[i]);
    }

    // page 1
    ui->eName->setText(d.name());
    ui->cbType->setCurrentIndex(d.type());
    ui->cVisible->setChecked(d.visible());
    ui->btnColor->setBuddy(ui->eColor);
    ui->eColor->setText(QString::asprintf("%.2X%.2X%.2X", d.red(), d.green(), d.blue()));
    ui->eFontSize->setText(STR(d.size()));
    ui->eX->setText(STR(d.x()));
    ui->eY->setText(STR(d.y()));
    ui->sAlpha->setBuddy(ui->eAlpha);
    ui->sAlpha->setSliderPosition(d.alpha());
    ui->cbFlagX->setCurrentIndex(d.flagX());
    ui->cbFlagY->setCurrentIndex(d.flagY());

    // page 2
    ui->btnShadowColor->setBuddy(ui->eShadowColor);
    ui->eShadowColor->setText(QString::asprintf("%.2X%.2X%.2X", d.shadowR(), d.shadowG(), d.shadowB()));
    ui->sShadowAlpha->setBuddy(ui->eShadowAlpha);
    ui->sShadowAlpha->setSliderPosition(d.shadowA(
                                            ));
    ui->eShadowX->setText(STR(d.shadowX()));
    ui->eShadowY->setText(STR(d.shadowY()));
    ui->cShadowVisible->setChecked(d.shadow());

    // page 3
    ui->eTemplate->setText(d.templateStr());
    ui->eText->setPlainText(d.text());
    CFontManager & fonts = *(gf.getFonts());
    for (int i=0; i < fonts.getSize(); ++i) {
        ui->cbFont->addItem(fonts.nameAt(i));
    }
    ui->cbFont->setCurrentIndex(d.font());

    // page 4
    // frame set
    setImage(d.imageSet(), d.imageNo());
    for (int n=0; n < gf.frames().getSize(); ++n) {
        CFrameSet & frameSet = *gf.frames()[n];
        uint8_t *png;
        int size;
        frameSet[0]->toPng(png, size);

        QImage img;
        if (!img.loadFromData( png, size )) {
            qWarning("failed to load png (%d)\n", n);
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
    ui->eX->setEnabled(d.flagX() == 0);
    ui->eY->setEnabled(d.flagY() == 0);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(strlen(d.name()) > 0);
}

void CDlgDisplay::save(CDisplay & d)
{
    // page 1
    d.setName(TEXT1(ui->eName));
    d.setText(q2c(ui->eText->toPlainText()));
    d.setType(ui->cbType->currentIndex());
    d.setVisible(ui->cVisible->isChecked());
    d.setColor(COLOR(ui->btnColor->colorBGR()), ui->sAlpha->sliderPosition());
    d.setFontSize(TOINT(ui->eFontSize));
    d.setXY(TOINT(ui->eX), TOINT(ui->eY));
    d.setFlagXY(ui->cbFlagX->currentIndex(), ui->cbFlagY->currentIndex());

    // page 2
    d.setShadowColor(COLOR(ui->btnShadowColor->colorBGR()), ui->sShadowAlpha->sliderPosition());
    d.setShadow(ui->cShadowVisible->isChecked(), TOINT(ui->eShadowX), TOINT(ui->eShadowY));

    // page 3
    d.setTemplate(TEXT1(ui->eTemplate));
    d.setFont(ui->cbFont->currentIndex());

    // page 4
    if (d.type() == CDisplay::DISPLAY_IMAGE) {
        d.setImage(ui->cbFrameSet->currentIndex(), ui->cbBaseFrame->currentIndex());
    } else {
        d.setImage(0, 0, false);
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
    if (!frameSet || frameSet < 0) {
        qDebug("frameset: 0");
        ui->sImage->clear();
        return;
    }
    CGameFile & gf = *m_gameFile;
    CFrameSet & fs = *gf.frames()[frameSet];

    uint8_t *png;
    int size;
    fs[frameNo]->toPng(png, size);

    QImage img;
    if (!img.loadFromData( png, size )) {
        qWarning("failed to load png $$\n");
    }
    delete [] png;

    QPixmap pm = QPixmap::fromImage(img);
    ui->sImage->setPixmap(pm);
}

void CDlgDisplay::fillFrameCombo(int frameSet)
{
    CGameFile & gf = *(m_gameFile);
    CFrameSet & fs = *gf.frames()[frameSet];

    ui->cbBaseFrame->clear();
    if (!frameSet) {
        return;
    }

    int imgCount = fs.getSize();
    unsigned char **imgPng = new uint8_t *[imgCount];
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
        imgPng[i] = nullptr;
    }
    delete [] imgPng;
}

QIcon CDlgDisplay::makeIcon(void *png, int size)
{
    QImage img;
    if (!img.loadFromData( (uint8_t*)png, size )) {
        qWarning("failed to load png");
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

void CDlgDisplay::on_eName_textChanged(const QString &arg1)
{
    bool enabled = false;
    if (arg1.length() > 0) {
        QString name = arg1.trimmed();
        CDisplayConfig * conf = m_gameFile->getDisplayConfig();
        int i = conf->indexOf(q2c(name));
        enabled = i == m_id || i == CDisplayConfig::NOT_FOUND;
    }
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enabled);
}

void CDlgDisplay::setDisplayID(int id)
{
    m_id = id;
}

void CDlgDisplay::on_cbFlagX_currentIndexChanged(int index)
{
    ui->eX->setEnabled(index == 0);
}

void CDlgDisplay::on_cbFlagY_currentIndexChanged(int index)
{
    ui->eY->setEnabled(index == 0);
}
