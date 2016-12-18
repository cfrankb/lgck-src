#include "DlgSelect.h"
#include "ui_DlgSelect.h"

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
