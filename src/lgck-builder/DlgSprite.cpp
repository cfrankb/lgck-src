#include "DlgSprite.h"
#include "ui_DlgSprite.h"

CDlgSprite::CDlgSprite(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSprite)
{
    ui->setupUi(this);
}

CDlgSprite::~CDlgSprite()
{
    delete ui;
}
