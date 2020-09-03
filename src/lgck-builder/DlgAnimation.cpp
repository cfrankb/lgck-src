/*
    LGCK Builder GUI
    Copyright (C) 1999, 2011  Francois Blanchette

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

#include "../shared/stdafx.h"
#include "DlgAnimation.h"
#include "ui_DlgAnimation.h"
#include "../shared/Animation.h"
#include "../shared/Object.h"
#include "../shared/FileWrap.h"
#include "../shared/qtgui/qthelper.h"

QString CDlgAnimation::m_sequences[] =
{
// AS_DEFAULT      = 0x00,
    tr("This is the default animation that will run when this object is first spawned."),
// AS_IDLE         = 0x01,
    tr("This is the idle animation that will switch on when this object has been idle for some time."),
// AS_CUSTOM1      = 0x02,
    tr("This is a custom animation reserved for user extensions."),
// AS_CUSTOM2      = 0x03,
    tr("This is a custom animation reserved for user extensions.")
};

QString CDlgAnimation::m_sequenceGroups[] =
{
    // AS_DEFAULT      = 0x00,
    tr("Default group"),
    // AS_STAND        = 0x04,
    tr("The engine will switch to running this animation when the object has stopped motion after moving %1."),
    // AS_MOVE         = 0x08,
    tr("The engine will switch to running this animation when the object is moving %1."),
    // AS_JUMP         = 0x0c,
    tr("The engine will switch to running this animation when the object is jumping %1."),
    // AS_HURT         = 0x10,
    tr("The engine will switch to running this animation when the object is hurt %1."),
    // AS_ATTACK       = 0x14,
    tr("The engine will switch to running this animation when the object is attacked %1."),
    // AS_RUN          = 0x18,
    tr("The engine will switch to running this animation when the object is running %1."),
    // AS_DEAD         = 0x1c,
    tr("The engine will switch to running this animation when the object is death %1."),
};

QString CDlgAnimation::m_aims[] =
{
    tr("up"),
    tr("down"),
    tr("left"),
    tr("right")
};

CDlgAnimation::CDlgAnimation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgAnimation)
{
    ui->setupUi(this);

    ui->treeIcons->setColumnCount(1);
    ui->treeIcons->setColumnWidth(0, 64);
    ui->treeIcons->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeIcons->setWordWrap(false);
    ui->treeIcons->setRootIsDecorated(false);
    ui->treeIcons->setAlternatingRowColors(true);

    ui->treeSequence->setColumnCount(1);
    ui->treeSequence->setColumnWidth(0, 64);
    ui->treeSequence->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeSequence->setWordWrap(false);
    ui->treeSequence->setRootIsDecorated(false);
    ui->treeSequence->setAlternatingRowColors(true);

    m_iconMax = GROWBY;
    m_iconCount = 0;
    m_icons = new uint8_t * [m_iconMax];
    m_iconSizes = new int[m_iconMax];
    m_animation = new CAnimation;
}

CDlgAnimation::~CDlgAnimation()
{
    delete ui;
    delete m_animation;
    delete [] m_icons;
    delete [] m_iconSizes;
}

void CDlgAnimation::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CDlgAnimation::addIcon(void *png, int size)
{
    QImage img;
    if (!img.loadFromData( (uint8_t*)png, size )) {
        qWarning("failed to load png\n");
    }

    QPixmap pm = QPixmap::fromImage(img);
    QIcon icon;
    icon.addPixmap(pm, QIcon::Normal, QIcon::On);

    QTreeWidgetItem *item = new QTreeWidgetItem(0);
    item->setIcon(0, icon);
    ui->treeIcons->addTopLevelItem(item);

    m_icons[m_iconCount] = (uint8_t*)png;
    m_iconSizes[m_iconCount] = size;
    ++m_iconCount;

    if (m_iconCount == m_iconMax) {
        m_iconMax += GROWBY;
        uint8_t ** t = new uint8_t* [ m_iconMax ];
        int * tt = new int [ m_iconMax];
        for (int i=0; i < m_iconCount; ++i) {
            t [ i ] = (uint8_t*)m_icons[i];
            tt [ i ] = m_iconSizes [ i ];
        }

        delete [] m_icons;
        delete [] m_iconSizes;
        m_icons = t;
        m_iconSizes = tt;
    }
}

void CDlgAnimation::addToSeq(int iconId, bool visualOnly)
{
    uint8_t * png = (uint8_t*) m_icons[iconId];

    QImage img;
    if (!img.loadFromData( png, m_iconSizes[iconId] )) {
        qWarning("failed to load png\n");
    }

    QPixmap pm = QPixmap::fromImage(img);
    QIcon icon;
    icon.addPixmap(pm, QIcon::Normal, QIcon::On);

    QTreeWidgetItem *item = new QTreeWidgetItem(0);
    item->setIcon(0, icon);
    ui->treeSequence->addTopLevelItem(item);

    if (!visualOnly) {
        CAnimation & t = * m_animation;
        t.add(iconId);
    }
}

void CDlgAnimation::updateButtons()
{
    QList<QTreeWidgetItem *> listIcons =ui->treeIcons->selectedItems();
    QList<QTreeWidgetItem *> listSeq =ui->treeSequence->selectedItems();

    ui->btAdd->setEnabled( listIcons.count() != 0 );
    ui->btClearAll->setEnabled( ui->treeSequence->model()->rowCount() != 0);
    ui->btCopyAll->setEnabled( m_iconCount != 0);
    ui->btRemove->setEnabled( listSeq.count() != 0);
}

void CDlgAnimation::on_treeIcons_doubleClicked(QModelIndex index)
{
    int i =  index.row();

    if (i != -1) {
        addToSeq( i );
    }

    updateButtons();
}

void CDlgAnimation::on_treeSequence_doubleClicked(QModelIndex index)
{   
    int i =  index.row();

    if (i != -1) {
        ui->treeSequence->model()->removeRow( i );
    }

    CAnimation & as = * m_animation;
    as.removeAt(i);

    updateButtons();
}

void CDlgAnimation::on_btCopyAll_clicked()
{
    for (int i=0; i < m_iconCount; ++i) {
        addToSeq( i );
    }

    updateButtons();
}

void CDlgAnimation::on_btAdd_clicked()
{
    QModelIndexList iconIndex = ui->treeIcons->selectionModel()->selectedIndexes();

    for (int i=0; i < iconIndex.count(); ++i) {
        addToSeq( iconIndex.at(i).row() );
    }

    updateButtons();
}

void CDlgAnimation::on_btRemove_clicked()
{
    CAnimation & as = * m_animation;

    QModelIndexList seqIndex = ui->treeSequence->selectionModel()->selectedIndexes();

    for (int i=seqIndex.count() - 1; i >= 0; --i) {
        int j = seqIndex.at(i).row();
        ui->treeSequence->model()->removeRow( j );
        as.removeAt(j);
    }

    updateButtons();
}

void CDlgAnimation::on_btClearAll_clicked()
{
    // TODO: add warning box

    CAnimation & as = * m_animation;

    int count = ui->treeSequence->model()->rowCount();
    for (int i = count - 1; i >= 0; --i) {
        ui->treeSequence->model()->removeRow( i );
    }

    as.forget();
    updateButtons();
}

void CDlgAnimation::load(CAnimation *a, int seqId)
{
    CAnimation & s = * a;
    CAnimation & t = * m_animation;

    for (int i=0; i< s.getSize(); ++i) {
        addToSeq(s[i], true);
    }

    t = s;    

    if (t.getOptions() & CObject::ASO_REPETE) {
        ui->cRepete->setChecked( true );
    }

    QString str;

    if (seqId < 4) {
        str = m_sequences[seqId];
    } else {
        str = QString(m_sequenceGroups[seqId / 4]).arg(m_aims[seqId % 4]);
    }

    str += "\n\n" + tr("If you select repeate this animation will loop infinitely. By default this sequence will run at the object's default animation speed.");
    ui->sDescription->setText(str);
}

void CDlgAnimation::save(CAnimation *a)
{
    CAnimation & t = * a;
    CAnimation & s = * m_animation;

    t = s;

    int options = (int) ui->cRepete->checkState() == Qt::Checked;

    t.setOptions(options);
}

void CDlgAnimation::on_treeSequence_itemSelectionChanged()
{
    updateButtons();
}

void CDlgAnimation::on_treeIcons_itemSelectionChanged()
{
    updateButtons();
}
