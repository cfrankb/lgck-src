/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "WizFrameSet.h"
#include "ui_WizFrameSet.h"
#include "stdafx.h"
#include "FrameSet.h"
#include "../shared/qtgui/cheat.h"

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

    ui->pageGeneral->registerField("name*", ui->eName);
    ui->pageGeneral->registerField("tree", ui->treeWidget, "topLevelItemCount", SIGNAL(currentItemChanged()));

    connect(ui->treeWidget, SIGNAL(countChanged()),
            ui->pageGeneral, SIGNAL(completeChanged()));

    QString s = QString( tr("imageSet_%1")).arg(i);
    setField("name", s);
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

const char *CWizFrameSet::getName()
{
    qDebug("%s", q2c(ui->eName->text()));
    return q2c(ui->eName->text());
}
