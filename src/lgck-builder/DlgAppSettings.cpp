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
#include <QDebug>
#include <cstring>
#include "WHotKey.h"
#include "WGetKey.h"
#include "DlgAppSettings.h"
#include "ui_DlgAppSettings.h"
#include "../shared/qtgui/cheat.h"
#include "../shared/Const.h"
#include "../shared/inputs/qt/kt_qt.h"

const int CDlgAppSettings::m_gridSizes [] = {
    16,
    32,
    64,
    128,
    256
};

constexpr int triggerFontSizes[] = {
    16, 18, 20, 22, 24, 26, 28, 30, 32, 40
};

constexpr int lastProjectCount[] = {
    4, 8, 12, 16
};

constexpr int tickMaxRateOptions[]= {
    10, 25, 50, 100, 200, 250, 400
};

CDlgAppSettings::CDlgAppSettings(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgAppSettings)
{
    m_ui->setupUi(this);
    m_gridSize = 16;
    m_count = 0;
    m_hotkeys = nullptr;
    m_cbButtons = nullptr;
    m_keys = nullptr;
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
        m_ui->cbLives->addItem(QVariant(i).toString());
    }

    m_ui->tabWidget->setCurrentIndex(0);

    // editor
    for (int i=10; i< 50; ++i) {
        m_ui->cbFontSize->addItem(QVariant(i).toString());
    }
    m_ui->cbFontSize->setCurrentIndex(0);

    for (unsigned int i=0; i< sizeof(triggerFontSizes)/sizeof(int); ++i) {
        m_ui->cbTriggerKeyFontSize->addItem(
                    QVariant(triggerFontSizes[i]).toString(), triggerFontSizes[i]);
    }

    for (unsigned int i=0; i< sizeof(lastProjectCount)/sizeof(int); ++i) {
        m_ui->cbProjects->addItem(QVariant(lastProjectCount[i]).toString(), lastProjectCount[i]);
    }

    for (unsigned int i=0; i< sizeof(tickMaxRateOptions)/sizeof(int); ++i) {
        m_ui->cbTickMaxRate->addItem(QVariant(tickMaxRateOptions[i]).toString(), tickMaxRateOptions[i]);
    }

    m_ui->sArgsHelp->setText(tr("%1 lgckdb filename\n" \
                                "%2 level\n" \
                                "%3 skill\n" \
                                "%4 width\n" \
                                "%5 height"));

    m_ui->btnGridColor->setBuddy(m_ui->eGridColor);
    m_ui->btnTriggerKeyColor->setBuddy(m_ui->eTriggerKeyColor);

#ifndef LGCK_RUNTIME
    m_ui->eRuntime->setDisabled(true);
    m_ui->eRuntimeArgs->setDisabled(true);
    m_ui->btnRestore->setDisabled(true);
    m_ui->btnRuntime->setDisabled(true);
#endif

    initButtonTable();
}

void CDlgAppSettings::initButtonTable()
{
    const QStringList listActions = {
        tr("Up"),
        tr("Down"),
        tr("Left"),
        tr("Right"),
        tr("Jump"),
        tr("Fire"),
        tr("Action"),
        tr("Special_1"),
        tr("Special_2")
    };

    const QStringList buttonText = {
        QString(""),
        tr("A"),
        tr("B"),
        tr("X"),
        tr("Y"),
        tr("L1"),
        tr("L3"),
        tr("R1"),
        tr("R3"),
        tr("Select"),
        tr("Start"),
        tr("Up"),
        tr("Down"),
        tr("Left"),
        tr("Right"),
        tr("Center"),
        tr("Guide")
    };

    QTableWidget *widget = m_ui->tableButtons;
    widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    widget->setSelectionMode(QAbstractItemView::NoSelection);
    widget->setColumnCount(4);
    widget->setRowCount(listActions.count());
    widget->setHorizontalHeaderLabels({tr("Actions"), tr("Keys"), tr("Gamepad"), ""});
    widget->setColumnWidth(0, 165);
    widget->setColumnWidth(1, 120);
    widget->setColumnWidth(2, 120);
    widget->setColumnWidth(3, 16);
    widget->verticalHeader()->setVisible(false);
    QIcon icon = QIcon(QPixmap(":/images/pd/dagobert83_cancelx16.png"));
    m_keys = new CWGetKey *[lgck::Input::Count];
    m_cbButtons = new QComboBox *[lgck::Input::Count];
    for (int i=0; i < listActions.count(); ++i){
        m_keys[i] = new CWGetKey(widget);
        QPushButton *button = new QPushButton(icon,"", this);
        m_cbButtons[i] = new QComboBox(widget);
#ifndef LGCK_GAMEPAD
        m_cbButtons[i]->setDisabled(true);
#endif
        for (int j=0; j < buttonText.size(); ++j) {
            m_cbButtons[i]->addItem(buttonText[j]);
        }
        button->setToolTip(tr("Clear"));
        button->resize(16,16);
        button->setProperty("id", i);
        QLabel *label = new QLabel();
        label->setText(listActions[i]);
        widget->setCellWidget(i,0,label);
        widget->setCellWidget(i,1,m_keys[i]);
        widget->setCellWidget(i,2,m_cbButtons[i]);
        widget->setCellWidget(i,3,button);
        connect(button, SIGNAL(clicked()), this, SLOT(clearKey()));
    }
}

CDlgAppSettings::~CDlgAppSettings()
{
    delete m_ui;
    if (m_hotkeys) {
        delete [] m_hotkeys;
    }
    if (m_keys) {
        delete [] m_keys;
    }
    if (m_cbButtons) {
        delete [] m_cbButtons;
    }
    if (m_defaultShortcuts) {
        delete m_defaultShortcuts;
    }
}

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

void CDlgAppSettings::setGridSize(int gridSize)
{
    m_gridSize = gridSize;
}

void CDlgAppSettings::setGridColor(const QString &color)
{
    m_ui->eGridColor->setText(color);
}

void CDlgAppSettings::setTriggerKeyColor(const QString & color)
{
    m_ui->eTriggerKeyColor->setText(color);
}

bool CDlgAppSettings::isShowGrid()
{
    return m_ui->cShowGrid->isChecked();
}

void CDlgAppSettings::showGrid(bool show)
{
    m_ui->cShowGrid->setChecked(show);
}

void CDlgAppSettings::enableGridOptions()
{
    bool showGrid = m_ui->cShowGrid->isChecked();
    m_ui->eGridColor->setEnabled(showGrid);
    m_ui->cbGridSize->setEnabled(showGrid);
    m_ui->btnGridColor->setEnabled(showGrid);
}

void CDlgAppSettings::init()
{
    enableGridOptions();
    enableTriggerKeyOptions();
    for (unsigned int i = 0; i < sizeof(m_gridSizes) / sizeof(int); ++i) {
        QString s = QString("%1").arg(m_gridSizes[i]);
        m_ui->cbGridSize->addItem(s);
        if (m_gridSize == m_gridSizes[i]) {
            m_ui->cbGridSize->setCurrentIndex( i );
        }
    }
    m_ui->eApiURL->setEnabled(m_ui->cCheckUpdate->isChecked());
}

int CDlgAppSettings::getGridSize()
{
    return m_gridSizes[ m_ui->cbGridSize->currentIndex() ];
}

void CDlgAppSettings::on_cShowGrid_clicked()
{
    enableGridOptions();
}

QString CDlgAppSettings::getGridColor()
{
    return m_ui->eGridColor->text();
}

QString CDlgAppSettings::getTriggerKeyColor()
{
    return m_ui->eTriggerKeyColor->text();
}

void CDlgAppSettings::load(QStringList &listActions, QStringList &listShortcuts, QStringList &defaults)
{
    // save defaults
    (*m_defaultShortcuts) = defaults;
    m_signalMapper = new QSignalMapper(this);
    QTableWidget *widget = m_ui->tableWidget;
    widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    widget->setSelectionMode(QAbstractItemView::NoSelection);
    widget->setColumnCount(3);
    widget->setRowCount(listActions.count());
    widget->setHorizontalHeaderLabels({tr("Actions"), tr("Shortcuts"), ""});
    widget->setColumnWidth(0, 200);
    widget->setColumnWidth(1, 228);
    widget->setColumnWidth(2, 16);
    widget->verticalHeader()->setVisible(false);
    m_count = listActions.count();
    m_hotkeys = new CWHotKey *[m_count];
    QIcon icon = QIcon(QPixmap(":/images/pd/dagobert83_cancelx16.png"));
    for (int i=0; i < listActions.count(); ++i){
        m_hotkeys[i] = new CWHotKey();
        m_hotkeys[i]->setText(listShortcuts[i]);
        QPushButton *button = new QPushButton(icon,"", this);
        button->setToolTip(tr("Clear"));
        button->resize(16,16);
        button->setProperty("id", i);
        QLabel *label = new QLabel();
        label->setText(listActions[i]);
        widget->setCellWidget(i,0,label);
        widget->setCellWidget(i,1,m_hotkeys[i]);
        widget->setCellWidget(i,2,button);
        connect(button, SIGNAL(clicked()), this, SLOT(clearHotkey()));
    }
}

void CDlgAppSettings::save(QStringList &listShortcuts)
{
    listShortcuts.clear();
    for (int i=0; i < m_count; ++i){
        listShortcuts.append(m_hotkeys[i]->text());
    }
}

void CDlgAppSettings::clearHotkey()
{
    QPushButton *button = qobject_cast<QPushButton *>(QObject::sender());
    if (button) {
        int id = button->property("id").toInt();
        m_hotkeys[id]->setText("");
    }
}

void CDlgAppSettings::clearKey()
{
    QPushButton *button = qobject_cast<QPushButton *>(QObject::sender());
    if (button) {
        int id = button->property("id").toInt();
        m_cbButtons[id]->setCurrentIndex(0);
        m_keys[id]->setText("");
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

void CDlgAppSettings::setTriggerFontSize(int size) {
    for (unsigned int i=0; i< sizeof(triggerFontSizes)/sizeof(int); ++i) {
        if (triggerFontSizes[i] == size) {
            m_ui->cbTriggerKeyFontSize->setCurrentIndex(i);
            return;
        }
    }
    m_ui->cbTriggerKeyFontSize->setCurrentIndex(0);
}

int CDlgAppSettings::getTriggerFontSize()
{
    return triggerFontSizes[m_ui->cbTriggerKeyFontSize->currentIndex()];
}

int CDlgAppSettings::getFontSize()
{
    return m_ui->cbFontSize->currentIndex() + 10;
}

void CDlgAppSettings::on_btnRuntime_clicked()
{
#ifdef Q_OS_WIN32
    QString fileFilter = tr("Runtime executables (lgck-runtime*.exe)");
#else
    QString fileFilter = tr("Executables (lgck-runtime*)");
#endif
    QString fileName = m_ui->eRuntime->text();
    fileName = QFileDialog::getOpenFileName(this, tr("Select runtime executable"), fileName, tr(q2c(fileFilter)));
    if (!fileName.isEmpty()) {
        m_ui->eRuntime->setText(fileName);
    }
}

void CDlgAppSettings::on_btnRestore_clicked()
{
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

bool CDlgAppSettings::getSkipSplashScreen()
{
    return m_ui->cSkipSplashScreen->isChecked();
}

void CDlgAppSettings::setSkipSplashScreen(bool state)
{
    m_ui->cSkipSplashScreen->setChecked(state);
}

void CDlgAppSettings::setShowTriggerKey(bool show)
{
    m_ui->cbShowTriggerKey->setChecked(show);
    enableTriggerKeyOptions();
}

bool CDlgAppSettings::getShowTriggerKey()
{
    return m_ui->cbShowTriggerKey->isChecked();
}

void CDlgAppSettings::enableTriggerKeyOptions()
{
    bool checked = m_ui->cbShowTriggerKey->isChecked();
    m_ui->eTriggerKeyColor->setEnabled(checked);
    m_ui->btnTriggerKeyColor->setEnabled(checked);
    m_ui->cbTriggerKeyFontSize->setEnabled(checked);
}

void CDlgAppSettings::on_cbShowTriggerKey_toggled(bool checked)
{
    Q_UNUSED(checked);
    enableTriggerKeyOptions();
}

void CDlgAppSettings::setFont(const QString & font)
{
    QFont f = QFont();
    f.setFamily(font);
    m_ui->cbFontFamily->setCurrentFont(f);
}

QString CDlgAppSettings::getFont()
{
     return m_ui->cbFontFamily->currentFont().family();
}

void CDlgAppSettings::enableWhiteSpace(bool state)
{
    m_ui->cWhiteSpace->setChecked(state);
}
void CDlgAppSettings::enableHighlight(bool state)
{
    m_ui->cHighlight->setChecked(state);
}
void CDlgAppSettings::enableAutocomplete(bool state)
{
    m_ui->cAutoComplete->setChecked(state);
}

void CDlgAppSettings::enableWordWrap(bool state)
{
    m_ui->cWordWrap->setChecked(state);
}

bool CDlgAppSettings::whiteSpace()
{
    return m_ui->cWhiteSpace->isChecked();
}

bool CDlgAppSettings::highlight()
{
    return m_ui->cHighlight->isChecked();
}

bool CDlgAppSettings::autocomplete()
{
    return m_ui->cAutoComplete->isChecked();
}

bool CDlgAppSettings::wordWrap()
{
    return m_ui->cWordWrap->isChecked();
}

void CDlgAppSettings::setLastProjects(int count)
{
    for (unsigned int i=0; i< sizeof(lastProjectCount)/sizeof(int); ++i) {
        if (lastProjectCount[i] == count) {
            m_ui->cbProjects->setCurrentIndex(i);
            return;
        }
    }
    m_ui->cbProjects->setCurrentIndex(0);
}

int CDlgAppSettings::lastProjects()
{
    return lastProjectCount[m_ui->cbProjects->currentIndex()];
}

void CDlgAppSettings::setTickMaxRate(int scale)
{
    for (unsigned int i=0; i< sizeof(tickMaxRateOptions)/sizeof(int); ++i) {
        if (tickMaxRateOptions[i] == scale) {
            m_ui->cbTickMaxRate->setCurrentIndex(i);
            return;
        }
    }
    m_ui->cbTickMaxRate->setCurrentIndex(2);
}

int CDlgAppSettings::tickMaxRate()
{
    return tickMaxRateOptions[m_ui->cbTickMaxRate->currentIndex()];
}

void CDlgAppSettings::setLastFolder(bool set)
{
    m_ui->cLastFolder->setChecked(set);
}

bool CDlgAppSettings::lastFolder()
{
    return m_ui->cLastFolder->isChecked();
}

bool CDlgAppSettings::getContinue()
{
    return m_ui->cContinue->isChecked();
}

void CDlgAppSettings::setContinue(bool c)
{
    return m_ui->cContinue->setChecked(c);
}

void CDlgAppSettings::setJoyButtons(const CGame::JoyStateEntry *map)
{
    for (int i=0; i < lgck::Input::Count; ++i) {
        m_joyStates[i] = map[i];
        m_cbButtons[i]->setCurrentIndex(map[i].button + 1);
        QString qtKeyText;
        int qtKeyCode = CKeyTranslator::translateLgck2Text(map[i].keyCode, qtKeyText);
        Q_UNUSED(qtKeyCode);
        m_keys[i]->setText(qtKeyText);
    }
}

void CDlgAppSettings::getJoyButtons(CGame::JoyStateEntry *map)
{
    // https://stackoverflow.com/questions/14034209/convert-string-representation-of-keycode-to-qtkey-or-any-int-and-back
    for (int i=0; i < lgck::Input::Count; ++i) {
        map[i].button = static_cast<lgck::Button::JoyButton>(m_cbButtons[i]->currentIndex() - 1);
        map[i].keyCode = static_cast<lgck::Key::Code>(CKeyTranslator::translateText2Lgck(m_keys[i]->text()));
    }
}
