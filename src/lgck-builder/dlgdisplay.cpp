#include "dlgdisplay.h"
#include "ui_dlgdisplay.h"

#include <QString>
#include "Display.h"
#include "../shared/qtgui/cheat.h"

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
}

void CDlgDisplay::enableType(int type)
{
    ui->eTemplate->setEnabled(true);
    ui->eText->setEnabled(true);
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
        break;
    }
}

void CDlgDisplay::on_cbType_currentIndexChanged(int index)
{
    enableType(index);
}
