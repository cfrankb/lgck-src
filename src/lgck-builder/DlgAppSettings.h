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

#ifndef DLGAPPSETTINGS_H
#define DLGAPPSETTINGS_H

#include <QDialog>
class QStringList;
class QSignalMapper;
class QTableWidgetItem;
class CWHotKey;

namespace Ui {
    class CDlgAppSettings;
}

class CDlgAppSettings : public QDialog {
    Q_OBJECT
public:
    CDlgAppSettings(QWidget *parent = 0);
    ~CDlgAppSettings();

    void init();
    void load(QStringList & listActions, QStringList & listShortcuts, QStringList & listDefaults);
    void save(QStringList & listShortcuts);
    int getGridSize();
    QString getGridColor();
    QString getTriggerKeyColor();
    bool getShowTriggerKey();
    void setGridSize(int gridSize);
    void setGridColor(const QString & color);
    void setTriggerKeyColor(const QString & color);
    void setShowTriggerKey(bool show);
    void setTriggerFontSize(int size);
    bool isShowGrid();
    void showGrid(bool show);
    void setSkill(int skill);
    int getSkill();
    int getLives();
    int getScore();
    int getHP();
    void setLives(int lives);
    void setScore(int score);
    void setHP(int hp);
    void setUpdater(bool checked, const QString & url);
    void getUpdater(bool & checked, QString & url);
    void setFontSize(int size);
    void getRuntime(QString & path, QString & args);
    void setRuntime(const QString path, const QString args);
    int getFontSize();
    void setFont(const QString & font);
    QString getFont();
    int getTriggerFontSize();
    bool getSkipSplashScreen();
    void setSkipSplashScreen(bool state);
    void setCurrentTab(int i);
    void enableWhiteSpace(bool state);
    void enableHighlight(bool state);
    void enableAutocomplete(bool state);
    void enableWordWrap(bool state);
    bool whiteSpace();
    bool highlight();
    bool autocomplete();
    bool wordWrap();
    void setLastProjects(int count);
    int lastProjects();
    static const char *defaultRuntimeArgs();

    enum {
        TAB_EDITOR,
        TAB_TEST,
        TAB_SHORTCUTS,
        TAB_DEFAULT=0
    };

protected:
    void changeEvent(QEvent *e);
    static int m_gridSizes [];
    int m_gridSize;
    void enableGridOptions();
    void enableTriggerKeyOptions();

private:
    Ui::CDlgAppSettings *m_ui;
    QSignalMapper *m_signalMapper;
    CWHotKey **m_hotkeys;
    QStringList *m_defaultShortcuts;
    int m_count;

signals:
    void versionCheck();

private slots:
    void on_cShowGrid_clicked();
    void buttonPushed();
    void on_cCheckUpdate_clicked(bool checked);
    void on_btnCheckUpdate_clicked();
    void on_btnRuntime_clicked();
    void on_btnRestore_clicked();
    void on_cbShowTriggerKey_toggled(bool checked);
};

#endif // DLGAPPSETTINGS_H
