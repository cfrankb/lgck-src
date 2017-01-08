#include "DlgStep.h"
#include "ui_DlgStep.h"
#include "pagecond.h"
#include "testcase.h"
#include <QSettings>
#include "const.h"

CDlgStep::CDlgStep(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgStep)
{
    ui->setupUi(this);
    QSettings settings(author, appName);
    bool largeFont = settings.value("largeFont", false).toBool();
    if (largeFont) {
        QFont font("Courier", 14);
        font.setBold(true);
        QFont font2("Courier", 12);
        font2.setBold(true);
        ui->eCode->setFont(font);
        ui->eName->setFont(font2);
    }
}

CDlgStep::~CDlgStep()
{
    delete ui;
}

void CDlgStep::on_btnAdd_clicked()
{
    int x = ui->tabWidget->count();
    if (x != CStep::MAX_CONDITIONS) {
        CPageCond *page = new CPageCond();
        ui->tabWidget->addTab(page, QString("%1").arg(x + 1));
        ui->tabWidget->setCurrentIndex(x);
        connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    }
}

void CDlgStep::closeTab(int index)
{
    QWidget *w = ui->tabWidget->widget(index);
    ui->tabWidget->removeTab(index);
    delete dynamic_cast<CPageCond*>(w);
}

void CDlgStep::on_btnDelete_clicked()
{
    int index = ui->tabWidget->currentIndex();
    if (index != -1) {
        closeTab(index);
        // Renumber tabs
        for (int x = 0; x < ui->tabWidget->count(); ++x) {
            ui->tabWidget->setTabText(x, QString("%1").arg(x + 1));
        }
    }
}

void CDlgStep::save(CStep & step)
{
    step.m_name = ui->eName->text();
    step.m_code = ui->eCode->toPlainText();
    int count = ui->tabWidget->count();
    step.m_conditionCount = count;
    for (int i=0; i < count; ++i) {
        QWidget *w = ui->tabWidget->widget(i);
        dynamic_cast<CPageCond*>(w)->save(step, i);
    }
}

void CDlgStep::load(CStep & step)
{
    ui->eName->setText(step.m_name);
    ui->eCode->setPlainText(step.m_code);
    int count = step.m_conditionCount;
    for (int i=0; i < count; ++i) {
        CPageCond *page = new CPageCond();
        ui->tabWidget->addTab(page, QString("%1").arg(i + 1));
        ui->tabWidget->setCurrentIndex(i);
        QWidget *w = ui->tabWidget->widget(i);
        dynamic_cast<CPageCond*>(w)->load(step, i);
    }
}

void CDlgStep::setName(const QString & name)
{
    ui->eName->setText(name);
}
