#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DlgSource.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionTestQcintilla2_triggered()
{
    CDlgSource dlg;
    dlg.exec();
}
