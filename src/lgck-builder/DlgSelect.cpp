#include "DlgSelect.h"
#include "ui_DlgSelect.h"
#include <QDebug>
#include <QProcess>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include "launcher.h"

CDlgSelect::CDlgSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSelect)
{
    ui->setupUi(this);
    m_state = CLOSE;

    connect(ui->btnNew, SIGNAL(clicked()), this, SLOT(newFile()));
    connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->btnSkip, SIGNAL(clicked()), this, SLOT(skipBox()));

    ui->btnNew->setCursor(Qt::PointingHandCursor);
    ui->btnOpen->setCursor(Qt::PointingHandCursor);
    ui->btnSpriteEditor->setCursor(Qt::PointingHandCursor);
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
    Path outPath;
    if (!getCmd(SPRITE_EDITOR, outPath)) {
        QMessageBox::warning(this, "", tr("Couldn't find executable: %1").arg(SPRITE_EDITOR));
        return;
    }
    QString runtime = "\"" + outPath.path + "/" + outPath.cmd + "\"";
    bool result = launchProcess(outPath);
    if (!result) {
        QMessageBox::warning(this, "", tr("Running external editor failed: %1").arg(runtime));
    }
}

void CDlgSelect::on_btnTutorials_clicked()
{
     QDesktopServices::openUrl(QUrl("https://cfrankb.com/lgck/tutorials.html"));
}
