#include "../shared/stdafx.h"
#include "WizFrameSet.h"
#include "ui_WizFrameSet.h"
#include "../shared/FrameSet.h"

CWizFrameSet::CWizFrameSet(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::CWizFrameSet)
{
    ui->setupUi(this);
    m_frameSet = new CFrameSet;
}

CWizFrameSet::~CWizFrameSet()
{
    delete m_frameSet;
    delete ui;
}

void CWizFrameSet::init(int i)
{
    Q_UNUSED(i);
    connect(this,
            SIGNAL(imageDeleted()),
            ui->treeWidget,
            SLOT(deleteImage()));

    connect(this,
            SIGNAL(imagesAdded()),
            ui->treeWidget,
            SLOT(addImages()));

    connect(this,
            SIGNAL(refill()),
            ui->treeWidget,
            SLOT(reload()));

    //ui->pageGeneral->registerField("name*", ui->eName);
    ui->pageGeneral->registerField("tree", ui->treeWidget, "topLevelItemCount", SIGNAL(currentItemChanged()));

    connect(ui->treeWidget, SIGNAL(countChanged()),
            ui->pageGeneral, SIGNAL(completeChanged()));

    //QString s = QString( tr("imageSet_%1")).arg(i);
    //setField("name", s);
    ui->treeWidget->setFrameSet(m_frameSet);
    emit refill();
    ui->btnDeleteImage->setEnabled(false);
}

void CWizFrameSet::on_btnAddImages_clicked()
{
    emit imagesAdded();
}

void CWizFrameSet::on_btnDeleteImage_clicked()
{
    emit imageDeleted();
}

void CWizFrameSet::on_treeWidget_itemSelectionChanged()
{
    updateButtons();
}

void CWizFrameSet::updateButtons()
{
    QModelIndex index = ui->treeWidget->currentIndex();
    ui->btnDeleteImage->setEnabled( index.row() >= 0 );
}

CFrameSet * CWizFrameSet::getFrameSet()
{
    return m_frameSet;
}
