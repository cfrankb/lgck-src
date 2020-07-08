#include "infodock.h"
#include "ui_infodock.h"

CInfoDock::CInfoDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CInfoDock)
{
    ui->setupUi(this);
    this->setWidget(ui->plainTextEdit);
}

CInfoDock::~CInfoDock()
{
    delete ui;
}

void CInfoDock::appendText(const QString & text)
{
    ui->plainTextEdit->appendPlainText(text);
}
