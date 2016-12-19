#include "DlgSelect.h"
#include "ui_DlgSelect.h"
#include <QDebug>
#include <QProcess>
#include <QMessageBox>

CDlgSelect::CDlgSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSelect)
{
    ui->setupUi(this);

    m_state = CLOSE;

    connect(ui->btnNew, SIGNAL(clicked()), this, SLOT(newFile()));
    connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->btnSkip, SIGNAL(clicked()), this, SLOT(skipBox()));
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
    QString runtime = appDir + "/" + cmd;
    bool result = QProcess::startDetached(runtime);
    if (!result) {
        QString errMsg = tr("Running external editor failed");
        QMessageBox msgBox(QMessageBox::Warning, "m_appName", errMsg, 0, this);
        msgBox.exec();
    }
}
