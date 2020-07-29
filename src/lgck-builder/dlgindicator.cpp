#include "dlgindicator.h"
#include "ui_dlgindicator.h"

CDlgIndicator::CDlgIndicator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgIndicator)
{
    // https://doc.qt.io/qt-5/qtextbrowser.html
    // https://stackoverflow.com/questions/54501745/performantly-appending-rich-text-into-qtextedit-or-qtextbrowser-in-qt
    ui->setupUi(this);

}

CDlgIndicator::~CDlgIndicator()
{
    delete ui;
}

void CDlgIndicator::setText(QString text)
{
    QString html = ui->eText->toHtml();
    QStringList list = html.split("***");
    ui->eText->setHtml(list[0] + text + list[1]);
}
