#include "DlgSelect.h"
#include "ui_DlgSelect.h"
#include <QDebug>
#include <QProcess>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

CDlgSelect::CDlgSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSelect)
{
    ui->setupUi(this);

    m_state = CLOSE;

    connect(ui->btnNew, SIGNAL(clicked()), this, SLOT(newFile()));
    connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->btnSkip, SIGNAL(clicked()), this, SLOT(skipBox()));

    int red=0;
    int green=0xf0;
    int blue=0xe0;
    ui->btnNew->setStyleSheet( QString("* { background-color: rgb(%1,%2,%3) }")
        .arg(red).arg(green).arg(blue));
    ui->btnNew->setCursor(Qt::PointingHandCursor);
    ui->btnOpen->setStyleSheet( QString("* { background-color: rgb(%1,%2,%3) }")
        .arg(red).arg(green).arg(blue));
    ui->btnOpen->setCursor(Qt::PointingHandCursor);
    ui->btnSpriteEditor->setStyleSheet( QString("* { background-color: rgb(%1,%2,%3) }")
        .arg(red).arg(green).arg(blue));
    ui->btnSpriteEditor->setCursor(Qt::PointingHandCursor);
    ui->btnTutorials->setStyleSheet( QString("* { background-color: rgb(%1,%2,%3) }")
        .arg(red).arg(green).arg(blue));
    ui->btnTutorials->setCursor(Qt::PointingHandCursor);

}

CDlgSelect::~CDlgSelect()
{
    delete ui;
}

void CDlgSelect::openFile()
{
    m_state = OPEN;
    accept();
}

void CDlgSelect::newFile()
{
    m_state = NEW;
    accept();
}

void CDlgSelect::skipBox()
{
    m_state = SKIP;
    accept();
}


void CDlgSelect::on_btnNoShow_clicked()
{
    m_state = NO_SHOW;
    accept();
}

void CDlgSelect::on_btnSpriteEditor_clicked()
{
    QString appDir = QCoreApplication::applicationDirPath();
    qDebug() << appDir;
#ifdef Q_OS_WIN32
    QString cmd = "obl5edit.exe";
#else
    QString cmd = "obl5edit";
#endif
    QString runtime = "\"" + appDir + "/" + cmd + "\"";
    bool result = QProcess::startDetached(runtime, QStringList());
    if (!result) {
        QString errMsg = tr("Running external editor failed: %1").arg(runtime);
        QMessageBox::warning(this, "", errMsg);
    }
}

void CDlgSelect::on_btnTutorials_clicked()
{
     QDesktopServices::openUrl(QUrl("https://cfrankb.com/lgck/tutorials.html"));
}
