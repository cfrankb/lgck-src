/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2009, 2010  Francois Blanchette

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

#include "tabwidget.h"
#include "ui_tabwidget.h"
#include <QMessageBox>
#include "stdafx.h"
#include "DlgFunction.h"
#include "DlgClass.h"
#include "DlgSection.h"
#include "../shared/qtgui/cheat.h"

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent),
    m_ui(new Ui::TabWidget)
{
    m_ui->setupUi(this);
    m_db = NULL;

    m_ui->treeFn->setColumnCount(3);
    m_ui->treeFn->setColumnWidth(0, 16);
    m_ui->treeFn->setColumnWidth(1, 128);
    m_ui->treeFn->setEditTriggers(0);
    m_ui->treeFn->setWordWrap(false);
    m_ui->treeFn->setRootIsDecorated(false);
    m_ui->treeFn->setAlternatingRowColors(true);
    m_ui->treeClass->setRootIsDecorated(false);
    m_ui->treeSection->setRootIsDecorated(false);

    QString images[] = {
        ":/images/blue.png",
        ":/images/cyan.png",
        ":/images/green.png",
        ":/images/grey.png",
        ":/images/red.png",
        ":/images/yellow.png",
        ":/images/na.png",
        ":/images/cpp.png",
        ":/images/lua.png"
    };

    for (unsigned int i=0; i < sizeof(images)/sizeof(QString); ++i) {
        CFileWrap file;
        unsigned char *png = NULL;
        int size = 0;
        if (file.open(images[i])) {

            size = file.getSize();
            png = new unsigned char[size];
            file.read(png, size);
            file.close();

            QImage img;
            if (!img.loadFromData( png, size )) {
                qDebug("failed to load png\n");
            }
            delete [] png;
            QPixmap pm = QPixmap::fromImage(img);

            m_icons[i].addPixmap( pm, QIcon::Normal, QIcon::On);

        } else {
            qDebug("failed to read ``%s``", q2c(images[i]));
        }
    }
}

TabWidget::~TabWidget()
{
    delete m_ui;
}

void TabWidget::changeEvent(QEvent *e)
{
    QTabWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TabWidget::init(CDatabase *db)
{
    m_db = db;

    int count;
    count = m_ui->treeFn->model()->rowCount();
    m_ui->treeFn->model()->removeRows(0, count);
    for (int i = 0; i < db->m_functions.getSize() ; ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        format(item, db->m_functions[i]);
        m_ui->treeFn->addTopLevelItem(item);
    }

    count = m_ui->treeClass->model()->rowCount();
    m_ui->treeClass->model()->removeRows(0, count);
    for (int i = 0; i < db->m_classes.getSize() ; ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setIcon(0, m_icons[1]);
        item->setText(0, db->m_classes[i]->name());
        m_ui->treeClass->addTopLevelItem(item);
    }

    count = m_ui->treeSection->model()->rowCount();
    m_ui->treeSection->model()->removeRows(0, count);
    for (int i = 0; i < db->m_sections.getSize() ; ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setIcon(0, m_icons[1]);
        item->setText(0, db->m_sections[i].name);
        m_ui->treeSection->addTopLevelItem(item);
    }
}

///////////////////////////////////////////////////////////////////
// tab1 - functions

void TabWidget::format (QTreeWidgetItem * item, CFunction & fn)
{
    int icon_states[] = {
      ICON_YELLOW,
      ICON_BLUE,
      ICON_RED,
      ICON_GREY
    };

    int icon_langs[] = {
      ICON_NA,
      ICON_CPP,
      ICON_LUA
    };

  //  QString name;
    QString ret;
    switch (fn.Out().getSize()) {
    case 0:
        ret = "void";
        break;

    case 1:
        ret = fn.Out()[0].type;
        break;

    default:
        ret = "mixed";
        break;
    }

    item->setIcon(0, m_icons[icon_langs[fn.lang]]);
    item->setIcon(1, m_icons[icon_states[fn.state]]);
    item->setText(1, ret);
    item->setText(2, fn.name);
}



void TabWidget::on_btnAdd_clicked()
{
    CDlgFunction *d = new CDlgFunction ( (QWidget*) parent());
    CFunction fn;
    d->load(&fn);
    d->setWindowTitle(tr("new function"));

    if (d->exec() == QDialog::Accepted) {
        d->save(&fn);
        int pos = m_db->m_functions.add( fn, true );

        QAbstractItemModel * model =  m_ui->treeFn->model();
        if (!model) {
            qDebug("model is null\n");
        }

        model->insertRow(pos);

        QTreeWidgetItem * item = m_ui->treeFn->topLevelItem( pos );
        m_ui->treeFn->setCurrentItem( item );

        format(item, fn);

        m_db->setDirty(true);
    }

    delete d;
}

void TabWidget::on_btnDelete_clicked()
{
    QMessageBox::StandardButton ret = QMessageBox::warning(this, "",  tr("Delete the current function?"),
                                                           QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {

        QModelIndex index = m_ui->treeFn->currentIndex();
        m_db->m_functions.removeAt( index.row() );
        QAbstractItemModel * model =  m_ui->treeFn->model();
        model->removeRow( index.row() );
        m_db->setDirty(true);
    }
}

void TabWidget::on_treeFn_doubleClicked(QModelIndex index)
{
    CFunction fn;
    fn.copy(m_db->m_functions[index.row()]);
    CDlgFunction *d = new CDlgFunction ( (QWidget*) parent());
    d->load( &fn );
    d->setWindowTitle(tr("edit function"));

    if (d->exec() == QDialog::Accepted) {
        d->save(& fn);

        m_db->m_functions.removeAt( index.row() );
        int pos = m_db->m_functions.add(fn, true);

        QAbstractItemModel * model =  m_ui->treeFn->model();
        if (!model) {
            qDebug("model is null\n");
        }

        if (pos != index.row()) {
            model->removeRow( index.row() );
            model->insertRow(pos);
        }

        QTreeWidgetItem * item = m_ui->treeFn->topLevelItem( pos );
        m_ui->treeFn->setCurrentItem( item );

        format(item, fn);
        m_db->setDirty(true);
    }

    delete d;
}

////////////////////////////////////////////////////////////////
// tab2 - classes

void TabWidget::on_btnAddClass_clicked()
{
    CDlgClass *d = new CDlgClass((QWidget*)parent());
    CClass * cl = new CClass;
    d->load(m_db, cl);
    d->setWindowTitle(tr("new class"));        
    if (d->exec() == QDialog::Accepted) {
        d->save( cl);
        int pos = m_db->m_classes.add( cl, true );

        QAbstractItemModel * model =  m_ui->treeClass->model();
        if (!model) {
            qDebug("model is null\n");
        }

        model->insertRow(pos);

        QTreeWidgetItem * item = m_ui->treeClass->topLevelItem( pos );
        m_ui->treeClass->setCurrentItem( item );
        item->setText(0, cl->name());

        m_db->setDirty(true);
    } else {
        delete cl;
    }

    delete d;
}

void TabWidget::on_btnDeleteClass_clicked()
{    
    QModelIndex index = m_ui->treeClass->currentIndex();
    if (index.row() != -1) {
        QMessageBox::StandardButton ret =
                QMessageBox::warning(this, "",  tr("Delete the current class?"),
                                     QMessageBox::Ok | QMessageBox::Cancel);
        if (ret == QMessageBox::Ok) {
            CClass * cl = m_db->m_classes.removeAt( index.row() );
            delete cl;
            QAbstractItemModel * model =  m_ui->treeClass->model();
            model->removeRow( index.row() );
            m_db->setDirty(true);
        }
    }
}

void TabWidget::on_treeClass_doubleClicked(QModelIndex index)
{
    CDlgClass *d = new CDlgClass ( (QWidget*) parent());
    d->load( m_db, m_db->m_classes[index.row()] );
    d->setWindowTitle(tr("edit class"));

    if (d->exec() == QDialog::Accepted) {

        CClass * cl = m_db->m_classes[index.row()];
        d->save( m_db->m_classes[index.row()]);

        cl = m_db->m_classes.removeAt( index.row() );
        int pos = m_db->m_classes.add(cl, true);

        QAbstractItemModel * model =  m_ui->treeClass->model();
        if (!model) {
            qDebug("model is null\n");
        }

        if (pos != index.row()) {
            model->removeRow( index.row() );
            model->insertRow(pos);
        }

        QTreeWidgetItem * item = m_ui->treeClass->topLevelItem( pos );
        m_ui->treeClass->setCurrentItem( item );
        item->setText( 0, cl->name() );

        m_db->setDirty(true);
    }
}

////////////////////////////////////////////////////////////////
// tab3 - sections

void TabWidget::on_treeSection_doubleClicked(QModelIndex index)
{
    CDlgSection *d = new CDlgSection ( (QWidget*) parent());
    d->setWindowTitle(tr("Edit Section"));
    d->load(m_db->m_sections[ index.row() ]);
    if (d->exec() == QDialog::Accepted) {
        d->save(m_db->m_sections[ index.row() ]);

        QTreeWidgetItem * item = m_ui->treeSection->topLevelItem( index.row() );
        m_ui->treeSection->setCurrentItem( item );
        item->setText( 0, m_db->m_sections[ index.row() ].name );

        m_db->setDirty(true);
    }

    delete d;
}

void TabWidget::on_btnAddSection_clicked()
{
    CDlgSection *d = new CDlgSection ( (QWidget*) parent());
    d->setWindowTitle(tr("Create New Section"));
    if (d->exec() == QDialog::Accepted) {
        Section s;
        d->save(s);

        m_db->m_sections.add(s);

        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        m_ui->treeSection->addTopLevelItem(item);
        m_ui->treeSection->setCurrentItem( item );
        item->setText( 0, s.name );

        m_db->setDirty(true);
    }
    delete d;
}

void TabWidget::on_btnDeleteSection_clicked()
{
    QModelIndex index = m_ui->treeSection->currentIndex();    
    if (index.row() != -1) {
        QMessageBox::StandardButton ret =
                QMessageBox::warning(this, "",  tr("Delete the current section?"),
                                     QMessageBox::Ok | QMessageBox::Cancel);
        if (ret == QMessageBox::Ok) {
            m_db->m_sections.removeAt( index.row() );
            QAbstractItemModel * model =  m_ui->treeSection->model();
            model->removeRow( index.row() );
            m_db->setDirty(true);
        }
    }
}
