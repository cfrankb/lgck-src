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

#include "DlgClass.h"
#include "ui_DlgClass.h"
#include "DlgFunction.h"
#include "DlgParam.h"
#include "DlgSource.h"
#include "../shared/qtgui/cheat.h"
#include <QSettings>
#include "../shared/qtgui/qfilewrap.h"

CDlgClass::CDlgClass(QWidget *parent) : QDialog(parent),
                                        m_ui(new Ui::CDlgClass)
{
    m_ui->setupUi(this);
    m_db = NULL;
    m_cl = NULL;
    QSettings settings;
    bool largeFont = settings.value("largeFont", false).toBool();
    if (largeFont)
    {
        QFont font("Courier", 16);
        font.setBold(true);
        m_ui->eDescription->setFont(font);
        QFont font2("Courier", 12);
        font2.setBold(true);
        m_ui->eName->setFont(font2);
    }
}

CDlgClass::~CDlgClass()
{
    delete m_ui;
}

void CDlgClass::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CDlgClass::load(CDatabase *db, CClass *cl)
{
    m_db = db;
    m_cl = cl;

    QString images[] = {
        ":/images/blue.png",
        ":/images/cyan.png",
        ":/images/green.png",
        ":/images/grey.png",
        ":/images/red.png",
        ":/images/yellow.png",
        ":/images/na.png",
        ":/images/cpp.png",
        ":/images/lua.png"};

    for (unsigned int i = 0; i < sizeof(images) / sizeof(QString); ++i)
    {
        QFileWrap file;
        unsigned char *png = NULL;
        int size = 0;
        if (file.open(images[i]))
        {

            size = file.getSize();
            png = new unsigned char[size];
            file.read(png, size);
            file.close();

            QImage img;
            if (!img.loadFromData(png, size))
            {
                qDebug("failed to load png\n");
            }
            delete[] png;
            QPixmap pm = QPixmap::fromImage(img);

            m_icons[i].addPixmap(pm, QIcon::Normal, QIcon::On);
        }
        else
        {
            qDebug("failed to read ``%s``", q2c(images[i]));
        }
    }

    // functions (treeFn)

    m_ui->treeFn->setColumnCount(3);
    m_ui->treeFn->setColumnWidth(0, 16);
    m_ui->treeFn->setColumnWidth(1, 64);
    m_ui->treeFn->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->treeFn->setWordWrap(false);
    m_ui->treeFn->setRootIsDecorated(false);
    m_ui->treeFn->setAlternatingRowColors(true);

    for (int i = 0; i < cl->methods().getSize(); ++i)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        format(item, cl->methods()[i]);
        m_ui->treeFn->addTopLevelItem(item);
    }

    // variables (treeVar)

    m_ui->treeVar->setColumnCount(2);
    m_ui->treeVar->setColumnWidth(0, 64);
    m_ui->treeVar->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->treeVar->setWordWrap(false);
    m_ui->treeVar->setRootIsDecorated(false);
    m_ui->treeVar->setAlternatingRowColors(true);

    for (int i = 0; i < cl->variables().getSize(); ++i)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        format(item, cl->variables()[i]);
        m_ui->treeVar->addTopLevelItem(item);
    }

    // class name
    m_ui->eName->setText(cl->name());

    // class description
    m_ui->eDescription->setPlainText(cl->desc());
}

void CDlgClass::save(CClass *cl)
{
    // class name
    cl->name() = m_ui->eName->text().trimmed();

    // class description
    cl->desc() = m_ui->eDescription->toPlainText();
}

void CDlgClass::format(QTreeWidgetItem *item, CFunction &fn)
{
    int icon_states[] = {
        ICON_YELLOW,
        ICON_BLUE,
        ICON_RED,
        ICON_GREY};

    int icon_langs[] = {
        ICON_NA,
        ICON_CPP,
        ICON_LUA};

    QString ret;
    switch (fn.Out().getSize())
    {
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

void CDlgClass::format(QTreeWidgetItem *item, Param &var)
{
    item->setIcon(0, m_icons[0]);
    item->setText(0, var.type);
    item->setText(1, var.name);
}

void CDlgClass::initFn(CFunction &fn)
{
    fn.In(0).forget();
    fn.Out().forget();
    fn.state = 0;
    fn.lang = 0;
}

void CDlgClass::on_btnAdd_clicked()
{
    CDlgFunction *d = new CDlgFunction((QWidget *)parent());
    CFunction fn;
    // initFn(fn);
    d->load(&fn);
    d->setWindowTitle(tr("new method"));

    if (d->exec() == QDialog::Accepted)
    {
        d->save(&fn);
        int pos = m_cl->methods().add(fn, true);

        QAbstractItemModel *model = m_ui->treeFn->model();
        if (!model)
        {
            qDebug("model is null\n");
        }

        model->insertRow(pos);

        QTreeWidgetItem *item = m_ui->treeFn->topLevelItem(pos);
        m_ui->treeFn->setCurrentItem(item);

        format(item, fn);
        m_db->setDirty(true);
    }

    delete d;
}

void CDlgClass::on_btnDelete_clicked()
{
    QModelIndex index = m_ui->treeFn->currentIndex();
    if (index.row() != -1)
    {
        m_cl->methods().removeAt(index.row());
        QAbstractItemModel *model = m_ui->treeFn->model();
        model->removeRow(index.row());
        m_db->setDirty(true);
    }
}

void CDlgClass::on_treeFn_doubleClicked(QModelIndex index)
{
    CFunction fn;
    fn.copy(m_cl->methods()[index.row()]);
    CDlgFunction *d = new CDlgFunction((QWidget *)parent());
    d->load(&fn);
    d->setWindowTitle(tr("edit function"));

    if (d->exec() == QDialog::Accepted)
    {
        d->save(&fn);

        m_cl->methods().removeAt(index.row());
        int pos = m_cl->methods().add(fn, true);

        QAbstractItemModel *model = m_ui->treeFn->model();
        if (!model)
        {
            qDebug("model is null\n");
        }

        if (pos != index.row())
        {
            model->removeRow(index.row());
            model->insertRow(pos);
        }

        QTreeWidgetItem *item = m_ui->treeFn->topLevelItem(pos);
        m_ui->treeFn->setCurrentItem(item);

        format(item, fn);
        m_db->setDirty(true);
    }

    delete d;
}

/////////////////////////////////////////////////////////////////////
// treeVar

void CDlgClass::on_treeVar_doubleClicked(QModelIndex index)
{
    Param &var = m_cl->variables()[index.row()];
    CDlgParam *d = new CDlgParam((QWidget *)parent());
    d->load(&var);
    d->setWindowTitle(tr("edit variable"));

    if (d->exec() == QDialog::Accepted)
    {
        d->save(&var);

        m_cl->variables()[index.row()] = var;

        QTreeWidgetItem *item = m_ui->treeVar->topLevelItem(index.row());
        format(item, var);
        m_db->setDirty(true);
    }

    delete d;
}

void CDlgClass::on_btnAddVar_clicked()
{
    CDlgParam *d = new CDlgParam((QWidget *)parent());
    Param var;
    d->load(&var);
    d->setWindowTitle(tr("new variable"));

    if (d->exec() == QDialog::Accepted)
    {
        d->save(&var);
        m_cl->variables().add(var);

        QAbstractItemModel *model = m_ui->treeVar->model();
        if (!model)
        {
            qDebug("model is null\n");
        }

        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        format(item, var);
        m_ui->treeVar->addTopLevelItem(item);
        m_ui->treeVar->setCurrentItem(item);
        m_db->setDirty(true);
    }

    delete d;
}

void CDlgClass::on_btnDeleteVar_clicked()
{
    QModelIndex index = m_ui->treeVar->currentIndex();
    if (index.row() != -1)
    {
        m_cl->variables().removeAt(index.row());
        QAbstractItemModel *model = m_ui->treeVar->model();
        model->removeRow(index.row());
        m_db->setDirty(true);
    }
}

void CDlgClass::on_btnAddVarM_clicked()
{
    CDlgSource *d = new CDlgSource((QWidget *)this);
    d->setWindowTitle(tr("Add Variable Batch"));
    if (d->exec())
    {
        QString s = d->getText();
        char *buf = new char[strlen(q2c(s)) + 1];
        strcpy(buf, q2c(s));
        char *p = buf;

        while (*p)
        {
            int sx = 0;
            for (char *pe = p; *pe && *pe != '\n'; ++pe, ++sx)
                ;
            auto tmp = new char[sx + 1];
            memcpy(tmp, p, sx);
            tmp[sx] = 0;
            p += sx;
            if (*p)
            {
                ++p;
            }

            // qDebug("line `%s`\n", tmp);

            char *ps = tmp;
            while (*ps && *ps == ' ')
            {
                ++ps;
            }

            char *pe = ps;
            while (*pe && *pe != ' ')
            {
                ++pe;
            }

            char c = *pe;
            *pe = 0;

            Param var;
            var.type = ps;
            if (c)
            {
                var.name = QString(pe + 1).trimmed();
            }

            m_cl->variables().add(var);
            QAbstractItemModel *model = m_ui->treeVar->model();
            if (!model)
            {
                qDebug("model is null\n");
            }

            QTreeWidgetItem *item = new QTreeWidgetItem(0);
            format(item, var);
            m_ui->treeVar->addTopLevelItem(item);
            m_ui->treeVar->setCurrentItem(item);
            m_db->setDirty(true);

            delete[] tmp;
        }

        delete[] buf;
    }
}
