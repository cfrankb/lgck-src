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


#include "DlgFunctions.h"
#include "ui_DlgFunctions.h"
#include "stdafx.h"
#include "DlgFunction.h"

CDlgFunctions::CDlgFunctions(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgFunctions)
{
    m_ui->setupUi(this);
    m_db = NULL;
}

CDlgFunctions::~CDlgFunctions()
{
    delete m_ui;
}

void CDlgFunctions::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void CDlgFunctions::init(CDatabase *db)
{
    m_db = db;

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

    m_ui->treeFn->setColumnCount(3);
    m_ui->treeFn->setColumnWidth(0, 16);
    m_ui->treeFn->setColumnWidth(1, 256);
    m_ui->treeFn->setColumnWidth(2, 64);
    m_ui->treeFn->setEditTriggers(0);
    m_ui->treeFn->setWordWrap(false);
    m_ui->treeFn->setRootIsDecorated(false);
    m_ui->treeFn->setAlternatingRowColors(true);


    for (int i = 0; i < db->m_functions.getSize() ; ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        format(item, db->m_functions[i]);
        m_ui->treeFn->addTopLevelItem(item);
    }
}

void CDlgFunctions::format (QTreeWidgetItem * item, Function & fn)
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

    char states[] = "?+- ";
    QString name;

    QString ret;
    switch (fn.pCountOut) {
    case 0:
        ret = "void";
        break;

    case 1:
        ret = fn.paramsOut[0].type;
        break;

    default:
        ret = "mixed";
        break;
    }

    name.sprintf("%c%s %s", states[fn.state], q2c(ret), q2c(fn.name));
    item->setIcon(0, m_icons[icon_langs[fn.lang]]);
    item->setText(1, name);
    item->setIcon(1, m_icons[icon_states[fn.state]]);
}

void CDlgFunctions::initFn(Function & fn)
{
    fn.pCountIn = 0;
    fn.pCountOut = 0;
    fn.state = 0;
    fn.lang = 0;
    for (int i=0; i < MAX_PARAMS; i++) {
        fn.paramsIn[i].flags = 0;
        fn.paramsOut[i].flags = 0;
    }
}



void CDlgFunctions::on_btnAdd_clicked()
{
    CDlgFunction *d = new CDlgFunction ( (QWidget*) parent());
    Function fn;
    initFn(fn);
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

        //QTreeWidgetItem *item = new QTreeWidgetItem(0);
        //item->setText(0, fn.name);
        format(item, fn);

        //m_ui->treeFn->addTopLevelItem(item);
        m_db->setDirty(true);
    }

    delete d;
}

void CDlgFunctions::on_btnDelete_clicked()
{
    QModelIndex index = m_ui->treeFn->currentIndex();
    m_db->m_functions.removeAt( index.row() );
    QAbstractItemModel * model =  m_ui->treeFn->model();
    model->removeRow( index.row() );
}

void CDlgFunctions::on_treeFn_doubleClicked(QModelIndex index)
{    
    Function fn = m_db->m_functions[index.row()];
    CDlgFunction *d = new CDlgFunction ( (QWidget*) parent());
    d->load( &fn );
    d->setWindowTitle(tr("edit function"));

    if (d->exec() == QDialog::Accepted) {
        d->save(& fn);

        m_db->m_functions.removeAt( index.row() );
        int pos = m_db->m_functions.add(fn, true);

        //m_db->m_functions[ index.row() ] = fn;
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

//        QTreeWidgetItem * item = m_ui->treeFn->topLevelItem( index.row() );
//        item->setText(0, fn.name);
        format(item, fn);
        m_db->setDirty(true);
    }

    delete d;
}
