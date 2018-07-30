/*
    LGCK Builder package
    Copyright (C) 2005, 2011  Francois Blanchette

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "OBL5File.h"

class QComboBox;
class QLabel;
class CPixelBoxDock;
class QActionGroup;
class CWImagePrieview;
class CEditView;
class QTreeWidgetItem;
class QPushButton;
class CFrameScroll;
class CFrameWidget;
class CWPreviewDock;
class QToolbar;
class QAction;
class QToolButton;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void	setVisible ( bool visible );

private:
    Ui::MainWindow *ui;
    bool updateTitle();
    void updateIcon(QTreeWidgetItem *item, const QString source, int j);
    void initFileMenu();
    void updateMenus();
    void reloadSettings();
    bool maybeSave();
    void setDocument(const QString);
    void warningMessage(const QString message);
    bool saveAs(char *outFormat);
    bool save();
    void open(QString);
    void updateRecentFileActions();
    void reloadRecentFileActions();
    void initImageList();
    void setStatus(const QString msg);
    void updateStatus();
    void initStatusBar();
    void reloadScene();
    void initToolbar();
    void initActionGroups();
    void setPenSize(int size);
    void updateColorButton(const UINT32 color);
    QAction** actionShortcuts();
    QStringList & defaultShortcuts();

    enum {
        MaxRecentFiles = 4
    };

    static char m_appName[];
    static char m_author[];
    static char m_allFilter[];
    static char m_oblFilter[];
    static char m_pngFilter[];    
    QString m_lastOpenFilter;

    QAction *m_recentFileActs[MaxRecentFiles];
    COBL5File m_doc;
    CFrameScroll *m_scrollArea;
    CFrameWidget *m_view;
    QLabel **m_labels;
    QToolButton *m_button;
    QAction *m_actionAlpha;
    QActionGroup *groupTools;
    bool m_saveSettings;
    CPixelBoxDock *m_pixelBox;
    CWPreviewDock *m_previewDock;
    int m_lastWidth;
    int m_lastHeight;
    int m_fadeFactor;
    QToolBar *m_toolbar1;
    QToolBar *m_toolbar2;
    QToolBar *m_toolbar3;
    bool m_showMain;
    bool m_showTB1;
    bool m_showTB2;
    bool m_showTB3;
    QAction *m_actionFavColor;
    CWPreviewDock *m_animate;

private slots:
    void on_actionNext_triggered();
    void on_actionPrevious_triggered();
    void on_actionRevert_triggered();
    void on_actionAbout_triggered();
    void on_actionQuit_triggered();
    void on_actionImport_triggered();
    void on_actionSave_As_triggered();
    void on_actionSave_triggered();
    void on_actionOpen_triggered();
    void on_actionNew_triggered();
    void openRecentFile();   
    void setStatus(int, const QString msg);
    void colorButtonPressed();
    void on_actionTransparency_toggled(bool );
    void on_actionInsert_image_triggered();
    void on_actionDelete_image_triggered();
    void on_actionDuplicate_triggered();
    void on_actionFlip_Horizontal_triggered();
    void on_actionFlip_Vertical_triggered();
    void on_actionExport_triggered();
    void on_actionRotate_triggered();
    void on_actionPen_triggered();
    void on_actionFlood_fill_triggered();
    void on_actionLine_triggered();
    void on_actionCircle_triggered();
    void on_actionRectangle_triggered();
    void on_actionColor_Select_triggered();
    void on_actionMap_triggered();
    void setPenColor(uint penColor);
    void changeZoom(int zoom);
    void imageChanged();
    void uncheckAnimate();
    void uncheckPreview();
    void uncheckToolbox();
    void on_action1_toggled(bool );
    void on_action2_toggled(bool );
    void on_action3_toggled(bool );
    void on_action4_toggled(bool );
    void on_action5_toggled(bool );
    void on_action6_toggled(bool );
    void on_action7_toggled(bool );
    void on_action8_toggled(bool );
    void on_action9_toggled(bool );
    void on_actionSelect_triggered();
    void on_action100_triggered();
    void on_action400_triggered();
    void on_action800_triggered();
    void on_action1600_triggered();
    void on_actionEraser_triggered();
    void on_actionClear_triggered();
    void on_actionZoom_triggered();
    void on_actionShowGrid_triggered(bool checked);
    void on_actionTapestry_triggered();
    void on_actionSpread_Vertical_triggered();
    void on_actionSpread_Horizontal_triggered();
    void on_actionShrink_triggered();
    void on_actionInverse_triggered();
    void on_actionEnlarge_triggered();
    void on_actionMove_triggered();
    void on_actionShadow_triggered(bool checked);
    void on_actionResize_triggered();
    void on_actionShift_UP_triggered();
    void on_actionShift_Down_triggered();
    void on_actionShift_Left_triggered();
    void on_actionShift_Right_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionAppend_image_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionSave_Settings_triggered(bool checked);
    void on_actionSplit_triggered();
    void on_actionPreview_toggled(bool arg1);
    void on_actionToolbox_toggled(bool arg1);
    void on_actionAdd_static_view_triggered();
    void setAlpha(int alpha);
    void setAlphaU(uchar alpha);
    void on_actionFilp_Horizontal_All_triggered();
    void on_actionFlip_Vertical_All_triggered();
    void on_actionRotate_All_triggered();
    void on_actionInverse_All_triggered();
    void on_actionEnlarge_All_triggered();
    void on_actionShrink_All_triggered();
    void on_actionResize_All_triggered();
    void on_actionDelete_All_triggered();
    void on_actionShift_Up_All_triggered();
    void on_actionShift_Down_All_triggered();
    void on_actionShift_Left_All_triggered();
    void on_actionShift_Right_All_triggered();
    void on_actionConfigure_Shortcuts_triggered();
    void on_actionMain_toggled(bool arg1);
    void on_actionColor_toggled(bool arg1);
    void on_actionTools_toggled(bool arg1);
    void on_actionGo_to_First_triggered();
    void on_actionGo_to_Last_triggered();
    void addFavColor();
    void alphaPressed();
    void queryNextFrame(int currFrame);
    void on_actionAnimated_view_toggled(bool arg1);
    void on_actionSwap_Green_Blue_triggered();
    void on_actionSwap_Red_Blue_triggered();
    void on_actionAnimation_Speed_triggered();
    void on_actionPaintNormal_triggered();
    void on_actionPaintColorOnly_triggered();
    void on_actionPaintAlphaOnly_triggered();
    void on_actionEdit_Mode_toggled(bool arg1);
    void on_actionFade_triggered();
    void loadFile(const QString & filename);

signals:
    void penColorChanged(uint penColor);
    void penColorChanged(const QColor & color);
    void penSizeChanged(int penSze);
    void toolChanged(int tool);
    void zoomChanged(int);
    void zoomLVChanged(int);
    void zoomAChanged(int);
    void gridEnabled(bool enabled);
    void tapestryEnabled ( bool );
    void transparencyToggled ( bool );
    void shadowSet(CFrame *);
    void bkColorSet(uint color);
    void gridColorSet(uint color);
    void borderColorSet(uint color);
    void frameChanged(CFrame*);
    void alphaChanged(uchar);
    void favColorAdded(uint color);
    void changePal(uint *colors, int count);
    void nextFrame(int currFrame, CFrame *);
    void viewModeChanged(int mode);
    void openFile(const QString & filename);
};

#endif // MAINWINDOW_H
