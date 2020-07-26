#include "DlgDistributeGame.h"
#include <QFileDialog>
#include "ui_DlgDistributeGame.h"
#include <QFileInfo>

bool fileExists(QString path) {
    QFileInfo checkFile(path);
    if (checkFile.exists() && checkFile.isFile()) {
        return true;
    } else {
        return false;
    }
}

CDlgDistributeGame::CDlgDistributeGame(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgDistributeGame)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    QString appDir = QCoreApplication::applicationDirPath();
    QFile f;
#ifdef Q_OS_WIN32
    QString cmd_runtime = "lgck-runtime-sdl.exe";
    QString stub = "game.exe";
    QString cmd7z = "\"" + appDir + "/7z.exe\"";
    ui->rZip->setChecked(true);
#else
    QString cmd_runtime = "lgck-runtime-sdl.exe";
    QString stub = "game.exe";
    QString cmd7z = "7z";
    ui->r7z->setChecked(true);
    ui->rSFX->setDisabled(true);
#endif

    QString runtimeSource = appDir + "/" + cmd_runtime;
    if (fileExists(runtimeSource)) {
        ui->eRuntime->setText(runtimeSource);
    }
    QString sfx = appDir + "/7z.sfx";
    if (fileExists(sfx)) {
        ui->eSFX->setText(sfx);
    }
    updateButtons();
}

CDlgDistributeGame::~CDlgDistributeGame()
{
    delete ui;
}

void CDlgDistributeGame::on_btnRuntime_clicked()
{
#ifdef Q_OS_WIN32
    const QString fileFilter = tr("Runtime executables (lgck-runtime*.exe)");
#else
    const QString fileFilter = tr("Runtime executables (lgck-runtime*)");
#endif
    QString runtime;
    getRuntime(runtime);
    runtime = QFileDialog::getOpenFileName(this, tr("Select LGCK Runtime"), runtime, fileFilter);
    if (!runtime.isEmpty()) {
        ui->eRuntime->setText(runtime);
    }
}

void CDlgDistributeGame::on_btnSFX_clicked()
{
    const QString fileFilter = tr("7z SFX modules (*.sfx)");
    QString sfx;
    getSFX(sfx);
    sfx = QFileDialog::getOpenFileName(this, tr("Select 7z SFX module"), sfx, fileFilter);
    if (!sfx.isEmpty()) {
        ui->eSFX->setText(sfx);
    }
}

int CDlgDistributeGame::outType()
{
    int out = OUT_NONE;
    if (ui->r7z->isChecked()) {
        out = OUT_7ZIP;
    }
    if (ui->rZip->isChecked()) {
        out = OUT_ZIP;
    }
    if (ui->rSFX->isChecked()) {
        out = OUT_SFX;
    }
    return out;
}

void CDlgDistributeGame::updateButtons()
{
    int out = outType();
    if (out == OUT_SFX) {
        ui->eSFX->setDisabled(false);
        ui->btnSFX->setDisabled(false);
    } else {
        ui->eSFX->setDisabled(true);
        ui->btnSFX->setDisabled(true);
    }
    QString runtime;
    getRuntime(runtime);
    QString sfx;
    getSFX(sfx);
    bool canExport = (!runtime.isEmpty() && fileExists(runtime))
        && (out != OUT_SFX || (!sfx.isEmpty() && fileExists(sfx)));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(canExport);
}

void CDlgDistributeGame::getRuntime(QString & runtime)
{
    runtime = ui->eRuntime->text();
}

void CDlgDistributeGame::getSFX(QString & sfx)
{
    sfx = ui->eSFX->text();
}

void CDlgDistributeGame::on_rZip_clicked()
{
    updateButtons();
}

void CDlgDistributeGame::on_r7z_clicked()
{
    updateButtons();
}

void CDlgDistributeGame::on_rSFX_clicked()
{
    updateButtons();
}

void CDlgDistributeGame::on_eRuntime_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    updateButtons();
}

void CDlgDistributeGame::on_eSFX_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    updateButtons();
}
