/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2005, 2013  Francois Blanchette

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
#include "DlgShortcuts.h"
#include "ui_DlgShortcuts.h"

#include <QComboBox>
#include <QSignalMapper>
#include <QLabel>
#include <QTableWidgetItem>
#include <QMessageBox>
#include "WHotKey.h"

CDlgShortCuts::CDlgShortCuts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgShortCuts)
{
    ui->setupUi(this);
    m_count = 0;
    m_hotkeys = NULL;
    m_defaultShortcuts = new QStringList();
}

CDlgShortCuts::~CDlgShortCuts()
{
    delete ui;
    if (m_hotkeys) {
        delete [] m_hotkeys;
    }
    if (m_defaultShortcuts) {
        delete m_defaultShortcuts;
    }
}

void CDlgShortCuts::load(QStringList &listActions, QStringList &listShortcuts, QStringList &defaults)
{
    // save defaults
    (*m_defaultShortcuts) = defaults;
    setWindowTitle(tr("Edit Shortcuts"));
    QStringList labels;
    labels.append(tr("Action"));
    labels.append(tr("Shortcut"));
    labels.append(tr(""));
    setLayout(ui->verticalLayout);
    m_signalMapper = new QSignalMapper(this);
    QTableWidget *widget = ui->tableWidget;
    widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    widget->setSelectionMode(QAbstractItemView::NoSelection);
    widget->setColumnCount(3);
    widget->setRowCount(listActions.count());
    widget->setHorizontalHeaderLabels(labels);
    widget->setColumnWidth(0, 120);
    widget->setColumnWidth(1, 208-20);
    widget->setColumnWidth(2, 16);
    widget->verticalHeader()->setVisible(false);
    //widget->horizontalHeader()->setMovable(false);
    //:setVerticalHeaderLabels
    //connect(widget, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(selectCell(int,int,int,int)));
    m_count = listActions.count();
    m_hotkeys = new CWHotKey *[m_count];
    QPixmap pixmap(":/images/pd/dagobert83_cancelx16.png");
    QIcon icon;
    icon.addPixmap(pixmap);
    for (int i=0; i < listActions.count(); ++i){
        m_hotkeys[i] = new CWHotKey();
        m_hotkeys[i]->setText(listShortcuts[i]);
        QPushButton *button = new QPushButton(icon,"", this);
        button->setToolTip(tr("Clear"));
        button->resize(16,16);
        button->setProperty("id", i);
        QLabel *label = new QLabel();
        label->setBuddy(m_hotkeys[i]);
        label->setText(listActions[i]);
        widget->setCellWidget(i,0,label);
        widget->setCellWidget(i,1,m_hotkeys[i]);
        widget->setCellWidget(i,2,button);
        connect(button, SIGNAL(clicked()), this, SLOT(buttonPushed()));
        //QTableWidgetItem *item = (QTableWidgetItem *)ui->tableWidget->cellWidget(i,0);
        //item->setText(listActions[i]);
        //item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        //connect(cb, SIGNAL(currentIndexChanged(int)), m_signalMapper, SLOT(map()));
        //connect(ui->tableWidget, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*))currentChanged(int, int)),
          //      this, SLOT(recordAction(int, int)));
    }
    // connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(demoSlot(int)));
}

void CDlgShortCuts::save(QStringList &listShortcuts)
{
    listShortcuts.clear();
    for (int i=0; i < m_count; ++i){
        listShortcuts.append(m_hotkeys[i]->text());
    }
}

void CDlgShortCuts::buttonPushed()
{
    QPushButton *button = qobject_cast<QPushButton *>(QObject::sender());
    if (button) {
        int id = button->property("id").toInt();
        m_hotkeys[id]->setText("");
    }
}

void CDlgShortCuts::on_btnReset_clicked()
{
    QMessageBox::StandardButton
            ret = QMessageBox::warning(this, "",
                   tr("This action cannot be undone. Are you sure that\n" \
                      "you want to reset all the shortcuts?"),
                   QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {
        for (int i=0; i < m_count; ++i){
            m_hotkeys[i]->setText((*m_defaultShortcuts)[i]);
        }
    }
}
