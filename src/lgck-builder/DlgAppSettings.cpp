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

#include <QColorDialog>
#include <QComboBox>
#include <QSignalMapper>
#include <QLabel>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include "WHotKey.h"
#include "DlgAppSettings.h"
#include "ui_DlgAppSettings.h"
#include "../shared/qtgui/cheat.h"

CDlgAppSettings::CDlgAppSettings(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgAppSettings)
{
    m_ui->setupUi(this);
    m_gridSize = 16;
    m_gridColor = "a0b0c0";
    m_showGrid = true;
    m_count = 0;
    m_hotkeys = NULL;
    m_defaultShortcuts = new QStringList();

    QString options[] = {
        tr("Normal - I'm just a kid"),
        tr("Nightmare - No sweat !"),
        tr("Hell - Bring it on !"),
        tr("Insane - Oh Yeah baby !!!")
    };

    for (unsigned int i=0; i< sizeof(options)/sizeof(QString); ++i) {
        m_ui->cbSkill->addItem(options[i]);
    }

    m_ui->cbSkill->setFocus();
    for (int i=1; i< 255; ++i) {
        QString s = QString("%1").arg(i);
        m_ui->cbLives->addItem(s);
    }

    m_ui->tabWidget->setCurrentIndex(0);

    // editor

    for (int i=10; i< 50; ++i) {
        QString s = QString("%1").arg(i);
        m_ui->cbFontSize->addItem(s);
    }
    m_ui->cbFontSize->setCurrentIndex(0);
    m_ui->sArgsHelp->setText(tr("%1 lgckdb filename\n" \
                                "%2 level\n" \
                                "%3 skill\n" \
                                "%4 width\n" \
                                "%5 height"));
}

CDlgAppSettings::~CDlgAppSettings()
{
    delete m_ui;
    if (m_hotkeys) {
        delete [] m_hotkeys;
    }
    if (m_defaultShortcuts) {
        delete m_defaultShortcuts;
    }
}

int CDlgAppSettings::m_gridSizes [] = {
    16,
    32,
    64,
    128,
    256
};

void CDlgAppSettings::changeEvent(QEvent *e)
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

void CDlgAppSettings::on_btnGridColor_clicked()
{
    QString str = m_ui->eGridColor->text();
    bool ok;
    uint color = str.toUInt(&ok,16);
    int red = color >> 16 & 0xff;
    int green = color >> 8 & 0xff;
    int blue = color & 0xff;
    QColorDialog *d = new QColorDialog(QColor(red,green,blue), this);
    int result = d->exec();
    if (result == QDialog::Accepted) {
        QString s;
        QColor color = d->currentColor();
        s.sprintf("%.2x%.2x%.2x", color.red(), color.green(), color.blue() );
        m_ui->eGridColor->setText(s);
    }
    delete d;
}

void CDlgAppSettings::setGridSize(int gridSize)
{
    m_gridSize = gridSize;
}

void CDlgAppSettings::setGridColor(QString gridColor)
{
    m_gridColor = gridColor;
}

bool CDlgAppSettings::isShowGrid()
{
    return m_showGrid;
}

void CDlgAppSettings::showGrid(bool show)
{
    m_showGrid = show;
}

void CDlgAppSettings::init()
{
    m_ui->cShowGrid->setChecked(m_showGrid);
    m_ui->eGridColor->setText(m_gridColor);
    setBtnColor(m_gridColor);
    m_ui->eGridColor->setEnabled( m_showGrid );
    m_ui->cbGridSize->setEnabled( m_showGrid );
    m_ui->btnGridColor->setEnabled( m_showGrid );
    for (unsigned int i = 0; i < sizeof(m_gridSizes) / sizeof(int); ++i) {
        QString s = QString("%1").arg(m_gridSizes[i]);
        m_ui->cbGridSize->addItem(s);
        if (m_gridSize == m_gridSizes[i]) {
            m_ui->cbGridSize->setCurrentIndex( i );
        }
    }

    // TODO:update checkbox state
    m_ui->eApiURL->setEnabled(m_ui->cCheckUpdate->isChecked());
}

int CDlgAppSettings::getGridSize()
{
    return m_gridSizes[ m_ui->cbGridSize->currentIndex() ];
}

void CDlgAppSettings::on_cShowGrid_clicked()
{
    m_showGrid = m_ui->cShowGrid->isChecked();
    m_ui->eGridColor->setEnabled( m_showGrid );
    m_ui->cbGridSize->setEnabled( m_showGrid );
    m_ui->btnGridColor->setEnabled( m_showGrid );
}

QString CDlgAppSettings::getGridColor()
{
    return m_ui->eGridColor->text();
}

void CDlgAppSettings::load(QStringList &listActions, QStringList &listShortcuts, QStringList &defaults)
{
    // save defaults
    (*m_defaultShortcuts) = defaults;
    QStringList labels;
    labels.append(tr("Action"));
    labels.append(tr("Shortcut"));
    labels.append(tr(""));
    m_signalMapper = new QSignalMapper(this);
    QTableWidget *widget = m_ui->tableWidget;
    widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    widget->setSelectionMode(QAbstractItemView::NoSelection);
    widget->setColumnCount(3);
    widget->setRowCount(listActions.count());
    widget->setHorizontalHeaderLabels(labels);
    widget->setColumnWidth(0, 120+35);
    widget->setColumnWidth(1, 208-20);
    widget->setColumnWidth(2, 16);
    widget->verticalHeader()->setVisible(false);
    //widget->horizontalHeader()->setMovable(false);
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
    }
}

void CDlgAppSettings::save(QStringList &listShortcuts)
{
    listShortcuts.clear();
    for (int i=0; i < m_count; ++i){
        listShortcuts.append(m_hotkeys[i]->text());
    }
}

void CDlgAppSettings::buttonPushed()
{
    QPushButton *button = qobject_cast<QPushButton *>(QObject::sender());
    if (button) {
        int id = button->property("id").toInt();
        m_hotkeys[id]->setText("");
    }
}

void CDlgAppSettings::setSkill(int skill)
{
    m_ui->cbSkill->setCurrentIndex(skill);
}

int CDlgAppSettings::getSkill()
{
    return m_ui->cbSkill->currentIndex();
}

void CDlgAppSettings::setLives(int lives)
{
    lives = std::min(lives, 255);
    m_ui->cbLives->setCurrentIndex(lives - 1);
}

int CDlgAppSettings::getHP()
{
    QString s = m_ui->eHitPoints->text().trimmed();
    bool ok;
    return s.toInt( &ok, 10 );
}

int CDlgAppSettings::getScore()
{
    QString s = m_ui->eScore->text().trimmed();
    bool ok;
    return s.toInt( &ok, 10 );
}

int CDlgAppSettings::getLives()
{
    return m_ui->cbLives->currentIndex()+1;
}

void CDlgAppSettings::setScore(int score)
{
    QString s = QString("%1").arg(score);
    m_ui->eScore->setText(s);
}

void CDlgAppSettings::setHP(int hp)
{
    QString s = QString("%1").arg(hp);
    m_ui->eHitPoints->setText(s);
}

void CDlgAppSettings::setBtnColor(const QString & str)
{
    bool ok;
    uint color = str.toUInt(&ok,16);
    int red = color >> 16 & 0xff;
    int green = color >> 8 & 0xff;
    int blue = color & 0xff;
    m_ui->btnGridColor->setStyleSheet(
        QString("* { background-color: rgb(%1,%2,%3) }")
        .arg(red).arg(green).arg(blue));
}

void CDlgAppSettings::on_eGridColor_textChanged(const QString &arg1)
{
    setBtnColor(arg1);
}

void CDlgAppSettings::on_cCheckUpdate_clicked(bool checked)
{
    m_ui->eApiURL->setEnabled(checked);
}

void CDlgAppSettings::setUpdater(bool checked, const QString & url)
{
    m_ui->eApiURL->setText(url);
    m_ui->cCheckUpdate->setChecked(checked);
}

void CDlgAppSettings::getUpdater(bool & checked, QString & url)
{
    url = m_ui->eApiURL->text();
    checked = m_ui->cCheckUpdate->isChecked();
}

void CDlgAppSettings::on_btnCheckUpdate_clicked()
{
    m_ui->btnCheckUpdate->setEnabled(false);
    emit versionCheck();
}

void CDlgAppSettings::setFontSize(int size)
{
    m_ui->cbFontSize->setCurrentIndex(std::max(size - 10, 0));
}

int CDlgAppSettings::getFontSize()
{
    return m_ui->cbFontSize->currentIndex() + 10;
}

void CDlgAppSettings::on_btnRuntime_clicked()
{
#ifdef Q_WS_WIN
    QString fileFilter = tr("Executables (*.exe)");
#else
    QString fileFilter = tr("Executables (lgck-runtime*)");
#endif
    QString fileName = m_ui->eRuntime->text();
    fileName = QFileDialog::getOpenFileName(this, tr("Runtime executable"), fileName, tr(q2c(fileFilter)));
    if (!fileName.isEmpty()) {
        m_ui->eRuntime->setText(fileName);
    }
}

void CDlgAppSettings::on_btnRestore_clicked()
{
    //m_ui->eRuntime->setText("");
    m_ui->eRuntimeArgs->setText(defaultRuntimeArgs());
}

void CDlgAppSettings::getRuntime(QString & path, QString & args)
{
    path = m_ui->eRuntime->text().trimmed();
    args = m_ui->eRuntimeArgs->text().trimmed();
}

void CDlgAppSettings::setRuntime(const QString path, const QString args)
{
    m_ui->eRuntime->setText(path);
    m_ui->eRuntimeArgs->setText(args);
}

void CDlgAppSettings::setCurrentTab(int i)
{
    m_ui->tabWidget->setCurrentIndex(i);
}

const char *CDlgAppSettings::defaultRuntimeArgs()
{
    return "%1 --level=%2 --skill=%3 --size=%4x%5";
}
