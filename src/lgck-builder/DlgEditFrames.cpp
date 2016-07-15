#include "DlgEditFrames.h"
#include "ui_DlgEditFrames.h"

CDlgEditFrames::CDlgEditFrames(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgEditFrames)
{
    ui->setupUi(this);
}

CDlgEditFrames::~CDlgEditFrames()
{
    delete ui;
}
