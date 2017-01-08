/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2009, 2017  Francois Blanchette

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


#include "DlgFunction.h"
#include "ui_DlgFunction.h"
#include "DlgParam.h"
#include <QMessageBox>
#include <QInputDialog>
#include "const.h"
#include <QSettings>
#include "DlgStep.h"
#include <QDebug>

CDlgFunction::CDlgFunction(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgFunction)
{
    m_ui->setupUi(this);
    m_fn = NULL;
    m_inSet = 0;
    QSettings settings(author, appName);
    bool largeFont = settings.value("largeFont", false).toBool();
    if (largeFont) {
        QFont font("Courier", 14);
        font.setBold(true);
        QFont font2("Courier", 12);
        font2.setBold(true);
        m_ui->eComment->setFont(font);
        m_ui->eName->setFont(font2);
    }
    m_testcase = 0;
}

CDlgFunction::~CDlgFunction()
{
    delete m_ui;
}

void CDlgFunction::changeEvent(QEvent *e)
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

const QString CDlgFunction::formatParam(Param & param)
{
    QString s;
    char o[2];
    o[0] = 0;
    o[1] = 0;
    if (param.flags & CFunction::FLAG_OPTIONAL) {
        o[0] = '*';
    }

    if (param.name.isEmpty()) {
        s = param.type;
    } else {
        s = QString("%1 %2").arg(param.type).arg(param.name);
    }

    if (param.flags & CFunction::FLAG_MORE) {
        s += "...";
    }
    return s + o;
}

void CDlgFunction::load(CFunction *fn)
{
    m_fn = fn;
    m_ui->eComment->setPlainText(fn->desc);
    m_ui->eName->setText(fn->name);

    // IN
    m_ui->treeIn->setColumnCount(2);
    m_ui->treeIn->setEditTriggers(0);
    m_ui->treeIn->setWordWrap(false);
    m_ui->treeIn->setRootIsDecorated(false);
    updateInTree();
    updateInSetCombo();

    // OUT
    m_ui->treeOut->setColumnCount(2);
    m_ui->treeOut->setEditTriggers(0);
    m_ui->treeOut->setWordWrap(false);
    m_ui->treeOut->setRootIsDecorated(false);
    for (int i = 0; i < fn->Out().getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, formatParam(fn->Out()[i]));
        m_ui->treeOut->addTopLevelItem(item);
    }

    QString state_list [] = {
        tr("unknown"),
        tr("finished"),
        tr("tba"),
        tr("not tested")
    };

    QString lang_list [] = {
        tr("unknown"),
        tr("cpp"),
        tr("lua"),
    };

    for (unsigned int i = 0; i < sizeof(state_list) / sizeof(QString); ++i) {
        m_ui->cbState->addItem(state_list[i]);
    }

    for (unsigned int i = 0; i < sizeof(lang_list) / sizeof(QString); ++i) {
        m_ui->cbLang->addItem(lang_list[i]);
    }
    m_ui->cbState->setCurrentIndex( fn->state );
    m_ui->cbLang->setCurrentIndex( fn->lang );
    QListIterator<QString> itr (fn->m_alias);
    while (itr.hasNext()) {
        QString current = itr.next();
        m_ui->listAlias->addItem(current);
    }

    // Test Case
    m_ui->treeTestCase->setColumnCount(2);
    m_ui->treeTestCase->setEditTriggers(0);
    m_ui->treeTestCase->setWordWrap(false);
    m_ui->treeTestCase->setRootIsDecorated(false);
    updateTestCaseTree();
    updateTestCaseCombo();
}

void CDlgFunction::save(CFunction *fn)
{
    fn->desc = m_ui->eComment->toPlainText().trimmed();
    fn->name = m_ui->eName->text().trimmed();
    fn->state = m_ui->cbState->currentIndex();
    fn->lang = m_ui->cbLang->currentIndex();
    fn->m_alias.clear();
    for(int i = 0; i < m_ui->listAlias->count(); ++i) {
        QListWidgetItem* item = m_ui->listAlias->item(i);
        fn->m_alias.append(item->text());
    }
}

/////////////////////////////////////////////////////////////////////////////
/// in

void CDlgFunction::on_treeIn_doubleClicked(QModelIndex index)
{
    CDlgParam *d = new CDlgParam ( (QWidget*) parent() );
    d->setWindowTitle(tr("edit IN param"));
    int j = m_inSet;
    d->load(& m_fn->In(j)[index.row()]);
    if (d->exec() == QDialog::Accepted) {
        d->save(& m_fn->In(j)[index.row()]);
        QTreeWidgetItem * item = m_ui->treeIn->topLevelItem( index.row() );
        item->setText(0, formatParam(m_fn->In(j)[index.row()]));
    }

    delete d;
}

void CDlgFunction::on_btnAdd_IN_clicked()
{
    Param param;
    CDlgParam *d = new CDlgParam ( (QWidget*) parent() );
    d->setWindowTitle(tr("add new IN param"));
    if (d->exec() == QDialog::Accepted) {
        d->save(&param);
        int j = m_inSet;
        m_fn->In(j).add(param);
        int i = m_fn->In(j).getSize() - 1;
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, formatParam(m_fn->In(j)[i]));
        m_ui->treeIn->addTopLevelItem(item);
    }

    delete d;
}

void CDlgFunction::on_btnDelete_IN_clicked()
{
    QModelIndex index = m_ui->treeIn->currentIndex();
    if (index.row() != -1) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, "",  tr("Delete the current param?"),
                                                               QMessageBox::Ok | QMessageBox::Cancel);
        int j = m_inSet;
        if (ret == QMessageBox::Ok) {
            m_fn->In(j).removeAt(index.row());
            QAbstractItemModel * model =  m_ui->treeIn->model();
            model->removeRow( index.row() );
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
/// out

void CDlgFunction::on_treeOut_doubleClicked(QModelIndex index)
{
    CDlgParam *d = new CDlgParam ( (QWidget*) parent() );
    d->setWindowTitle(tr("edit Out param"));
    d->load(& m_fn->Out()[index.row()]);
    if (d->exec() == QDialog::Accepted) {
        d->save(& m_fn->Out()[index.row()]);
        QTreeWidgetItem * item = m_ui->treeOut->topLevelItem( index.row() );
        item->setText(0, formatParam(m_fn->Out()[index.row()]));
    }
    delete d;
}

void CDlgFunction::on_btnAdd_OUT_clicked()
{
    Param param;
    CDlgParam *d = new CDlgParam ( (QWidget*) parent() );
    d->setWindowTitle(tr("add new OUT param"));
    if (d->exec() == QDialog::Accepted) {
        d->save(&param);
        m_fn->Out().add( param );
        int i = m_fn->Out().getSize() -1 ;
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, formatParam(m_fn->Out()[i]));
        m_ui->treeOut->addTopLevelItem(item);
    }
    delete d;
}

void CDlgFunction::on_btnDelete_OUT_clicked()
{
    QModelIndex index = m_ui->treeOut->currentIndex();
    if (index.row() != -1) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, "",  tr("Delete the current param?"),
                                                               QMessageBox::Ok | QMessageBox::Cancel);
        if (ret == QMessageBox::Ok) {
            m_fn->Out().removeAt(index.row());
            QAbstractItemModel * model =  m_ui->treeOut->model();
            model->removeRow( index.row() );
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
/// alias

void CDlgFunction::on_btnAdd_Alias_clicked()
{
    QString str = QInputDialog::getText(this,tr("alias"), tr("function"));
    if (!str.isEmpty()) {
        m_ui->listAlias->addItem(str.trimmed());
    }
}

void CDlgFunction::on_btnDelete_Alias_clicked()
{
    QModelIndex index = m_ui->listAlias->currentIndex();
    if (index.row() != -1) {
        QListWidgetItem * item = m_ui->listAlias->takeItem(index.row());
        delete item;
    }
}

void CDlgFunction::on_cbInSet_currentIndexChanged(int index)
{
    qDebug("on_cbInSet_currentIndexChanged");
    m_inSet = std::max(index,0);
    updateInTree();
}

void CDlgFunction::on_btnAdd_IN_SET_clicked()
{
    if (m_ui->cbInSet->count() < CFunction::MAX_IN_COUNT) {
        m_inSet = m_ui->cbInSet->count();
        ++m_fn->m_inCount;
        m_ui->cbInSet->addItem(QString("set %1").arg(m_fn->m_inCount));
        m_ui->cbInSet->setCurrentIndex(m_inSet);
        updateInTree();
    }
}

void CDlgFunction::on_btnDelete_IN_SET_clicked()
{
    if (m_ui->cbInSet->count() > 1) {
        // TODO remove InParams(x)
        m_fn->removeInSet(m_inSet);
        if (m_inSet == m_ui->cbInSet->count() -1) {
            --m_inSet;
        }
        updateInSetCombo();
        updateInTree();
        m_ui->cbInSet->setCurrentIndex(m_inSet);
    }
}

void CDlgFunction::updateInSetCombo()
{
    qDebug("updateInSetCombo()");
    m_ui->cbInSet->clear();
    for (int i=0; i < m_fn->m_inCount; ++i) {
        QString str = QString("set %1").arg(i+1);
        m_ui->cbInSet->addItem(str);
    }
}

void CDlgFunction::updateInTree()
{
    qDebug("updateInTree() in");
    // clear tree
    int count = m_ui->treeIn->model()->rowCount();
    m_ui->treeIn->model()->removeRows(0, count);
    // grow new leafs
    int j = m_inSet;
    qDebug("m_inSet: %d", m_inSet);
    for (int i = 0; i < m_fn->In(j).getSize(); ++i) {
        qDebug("%d",i);
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, formatParam(m_fn->In(j)[i]));
        m_ui->treeIn->addTopLevelItem(item);
    }
    qDebug("updateInTree() out");
}


/////////////////////////////////////////////////////////////////////////////
/// test case

void CDlgFunction::updateTestCaseTree()
{
    bool disabled = m_fn->m_testCaseCount == 0;
    m_ui->treeTestCase->setDisabled(disabled);
    m_ui->btnAdd_Step->setDisabled(disabled);
    m_ui->btnDelete_Step->setDisabled(disabled);
    m_ui->cbTestCase->setDisabled(disabled);
    qDebug("updateTestCaseTree() in");
    // clear tree
    int count = m_ui->treeTestCase->model()->rowCount();
    m_ui->treeTestCase->model()->removeRows(0, count);
    // grow new leafs
    CTestCase & testcase = m_fn->TestCase(m_testcase);
    qDebug("m_testcase: %d", m_testcase);
    for (int i = 0; i < testcase.getSize(); ++i) {
        qDebug("%d",i);
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        CStep & step = testcase.m_steps[i];
        item->setText(0, step.m_name);
        m_ui->treeTestCase->addTopLevelItem(item);
    }
    qDebug("updateTestCaseTree() out");
}

void CDlgFunction::updateTestCaseCombo()
{
    bool disabled = m_fn->m_testCaseCount == 0;
    m_ui->cbTestCase->setDisabled(disabled);
    m_ui->cbTestCase->clear();
    for (int i=0; i < m_fn->m_testCaseCount; ++i) {
        QString str = QString("test %1").arg(i + 1);
        m_ui->cbTestCase->addItem(str);
    }
}

void CDlgFunction::on_treeTestCase_clicked(const QModelIndex &index)
{

}

void CDlgFunction::on_btnAdd_Step_clicked()
{
    CTestCase & testcase = m_fn->TestCase(m_testcase);
    int i = testcase.m_count;
    CDlgStep *d = new CDlgStep(this);
    d->setWindowTitle("New Step");
    d->setName(QString("Step %1").arg(i + 1));
    if (d->exec() == QDialog::Accepted) {
        qDebug("m_testcase=%d - testcase.m_count=%d", m_testcase, testcase.m_count);
        CStep & step = testcase.m_steps[i];
        ++testcase.m_count;
        d->save(step);
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, step.m_name);
        m_ui->treeTestCase->addTopLevelItem(item);
        m_ui->treeTestCase->setCurrentItem(item);
    }
    delete d;
}

void CDlgFunction::on_btnDelete_Step_clicked()
{    
    QModelIndex index = m_ui->treeTestCase->currentIndex();
    int i = index.row();
    if (i != -1) {
        CTestCase & testcase = m_fn->TestCase(m_testcase);
        testcase.removeAt(i);
        QAbstractItemModel * model =  m_ui->treeTestCase->model();
        model->removeRow( i );
    }
}

void CDlgFunction::on_treeTestCase_doubleClicked(const QModelIndex &index)
{
    int i = index.row();
    if (i != -1) {
        CTestCase & testcase = m_fn->TestCase(m_testcase);
        CDlgStep *d = new CDlgStep(this);
        d->setWindowTitle("Edit Step");
        CStep & step = testcase.m_steps[i];
        d->load(step);
        if (d->exec() == QDialog::Accepted) {
            d->save(step);
            QTreeWidgetItem *item = m_ui->treeTestCase->topLevelItem(i);
            item->setText(0, step.m_name);
        }
        delete d;
    }
}

void CDlgFunction::on_btnAdd_TestCase_clicked()
{
    if (m_ui->cbTestCase->count() < CFunction::MAX_TEST_CASE) {
        m_testcase = m_ui->cbTestCase->count();
        qDebug("m_ui->cbTestCase->count(): %d", m_ui->cbTestCase->count());
        ++m_fn->m_testCaseCount;
        m_ui->cbTestCase->addItem(QString("test %1").arg(m_fn->m_testCaseCount));
        m_ui->cbTestCase->setCurrentIndex(m_testcase);
        updateTestCaseTree();
    }
}

void CDlgFunction::on_btnDelete_TestCase_clicked()
{
    if (m_ui->cbTestCase->count() > 0) {
        m_fn->removeTestCase(m_testcase);
        if (m_testcase == m_ui->cbTestCase->count() -1) {
            --m_testcase;
        }
        updateTestCaseCombo();
        updateTestCaseTree();
        m_ui->cbTestCase->setCurrentIndex(m_testcase);
    }
}

void CDlgFunction::on_cbTestCase_currentIndexChanged(int index)
{
    m_testcase = std::max(index,0);
    updateTestCaseTree();
}
