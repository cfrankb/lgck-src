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

#include <QApplication>
#include <string.h>
#include <QMessageBox>
#include <QGraphicsView>
#include <QActionGroup>
#include <QColor>
#include <QLabel>
#include <QFileDialog>
#include <QToolBar>
#include <QtOpenGL>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../shared/Tools.h"
#include "../shared/qtgui/ColorPicker/colorpickerwidget.h"
#include "PixelBoxDock.h"
#include "framescroll.h"
#include "framewidget.h"
#include "WizFrameSet.h"
#include "WPalette.h"
#include "previewwidget.h"
#include "WFileSave.h"
#include "WPreviewDock.h"
#include "DlgAbout.h"
#include "DlgGoto.h"
#include "DlgImageSize.h"
#include "DlgShortcuts.h"
#include "DlgAlpha.h"
#include "../shared/Frame.h"
#include "../shared/ss_version.h"
#include "../shared/qtgui/qfilewrap.h"

//http://stackoverflow.com/questions/3542718/installing-opengl-for-qt
char MainWindow::m_appName[] = "Object Block Editor";
char MainWindow::m_author[] = "cfrankb";
char MainWindow::m_allFilter[]= "All Supported Formats (*.obl *.obl5 *.png)";
char MainWindow::m_oblFilter[] = "Object Blocks (*.obl *.obl5)";
char MainWindow::m_pngFilter[] = "PNG Images (*.png)";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{

    QString s;
    QString appVersion;
    int version = SS_LGCK_VERSION;
    for (int i=0; i < 4; ++i) {
        s = QString("%1").arg(version % 256);
        version /= 256;
        if (i) {
            appVersion = s + "." + appVersion  ;
        } else {
            appVersion = s + appVersion ;
        }
    }

    QCoreApplication::setOrganizationDomain("");
    QCoreApplication::setOrganizationName(m_author);
    QCoreApplication::setApplicationName(m_appName);
    QCoreApplication::setApplicationVersion(appVersion);

    ui->setupUi(this);
    //ui->centralWidget->hide();
    m_scrollArea = new CFrameScroll(this);
    m_scrollArea->viewport()->update();
    //m_view->update();
    setCentralWidget(m_scrollArea);
    m_view = static_cast<CFrameWidget*>(m_scrollArea->viewport());
    m_fadeFactor = 255;

    updateTitle();
    initFileMenu();
    updateMenus();
    initStatusBar();
    initActionGroups();
    initToolbar();
    updateStatus();

    m_pixelBox = new CPixelBoxDock(this);
    this->addDockWidget(Qt::LeftDockWidgetArea, m_pixelBox);

    m_previewDock = new CWPreviewDock( this );
    m_previewDock->setObjectName("liveView");
    m_previewDock->setTitleTemplate(tr("Liveview %1%"));
    this->addDockWidget(Qt::LeftDockWidgetArea, m_previewDock);
    CPreviewWidget *preview = m_previewDock->getView();
    connect(m_previewDock, SIGNAL(windowClosed()),
            this, SLOT(uncheckPreview()));
    connect(this, SIGNAL(zoomLVChanged(int)),
            preview, SLOT(setZoom(int)));

    m_animate = new CWPreviewDock( this );
    m_animate->setObjectName("animated");
    m_animate->setTitleTemplate(tr("Animated %1%"));
    this->addDockWidget(Qt::RightDockWidgetArea, m_animate);
    CPreviewWidget *prAnime = m_animate->getView();
    connect(prAnime, SIGNAL(nextFrame(int)),
            this, SLOT(queryNextFrame(int)));
    connect(this, SIGNAL(nextFrame(int,CFrame*)),
            prAnime, SLOT(setNextFrame(int,CFrame*)));
    connect(this, SIGNAL(bkColorSet(uint)),
            prAnime, SLOT(setBkColor(uint)));
    connect(m_animate, SIGNAL(windowClosed()),
            this, SLOT(uncheckAnimate()));

    connect(this, SIGNAL(bkColorSet(uint)),
            m_pixelBox, SLOT(setBkColor(uint)));
    connect(this, SIGNAL(gridColorSet(uint)),
            m_pixelBox, SLOT(setGridColor(uint)));
    connect(this, SIGNAL(borderColorSet(uint)),
            m_pixelBox, SLOT(setBorderColor(uint)));
    connect(m_pixelBox, SIGNAL(windowClosed()),
            this, SLOT(uncheckToolbox()));

    connect(m_actionFavColor, SIGNAL(triggered()),
            this, SLOT(addFavColor()));
    connect(this, SIGNAL(favColorAdded(uint)),
            m_pixelBox->getFavPalette(), SLOT(addColor(uint)));

    connect(this, SIGNAL(bkColorSet(uint)),
            m_view, SLOT(setBkColor(uint)));
    connect(this, SIGNAL(gridColorSet(uint)),
            m_view, SLOT(setGridColor(uint)));
    connect(this, SIGNAL(borderColorSet(uint)),
            m_view, SLOT(setBorderColor(uint)));

    connect(this, SIGNAL(bkColorSet(uint)),
            preview, SLOT(setBkColor(uint)));
    connect(this, SIGNAL(frameChanged(CFrame*)),
            preview, SLOT(changeFrame(CFrame*)));

    connect(m_pixelBox, SIGNAL(bkColorChanged(uint)),
            m_view, SLOT(setBkColor(uint)));
    connect(m_pixelBox, SIGNAL(bkColorChanged(uint)),
            preview, SLOT(setBkColor(uint)));
    connect(m_pixelBox, SIGNAL(bkColorChanged(uint)),
            prAnime, SLOT(setBkColor(uint)));
    connect(m_pixelBox, SIGNAL(gridColorChanged(uint)),
            m_view, SLOT(setGridColor(uint)));
    connect(m_pixelBox, SIGNAL(borderColorChanged(uint)),
            m_view, SLOT(setBorderColor(uint)));

    const CWPalette *pal = m_pixelBox->getPalette();
    const ColorPickerWidget *colorWheel = m_pixelBox->getColorWheel();
    connect(m_button, SIGNAL(clicked()),
            this, SLOT(colorButtonPressed()));
    connect(colorWheel, SIGNAL(colorChanged(uint)),
            this, SLOT(setPenColor(uint)));
    connect(colorWheel, SIGNAL(colorChanged(uint)),
            m_view, SLOT(setPenColor(uint)));
    connect( this, SIGNAL(penColorChanged(uint)),
            colorWheel, SLOT(setColor(uint)));
    connect(pal, SIGNAL(bkColorPicked( uint )),
            prAnime, SLOT( setBkColor( uint ))) ;

    // set ColorButton
    connect(pal, SIGNAL(colorPicked( uint )),
            this, SLOT( setPenColor( uint ))) ;
    connect(m_pixelBox->getFavPalette(), SIGNAL(colorPicked( uint )),
            this, SLOT( setPenColor( uint ))) ;

    // set pencolor in view
    connect(pal, SIGNAL(colorPicked( uint )),
            m_view, SLOT( setPenColor( uint ))) ;
    connect(m_pixelBox->getFavPalette(), SIGNAL(colorPicked( uint )),
            m_view, SLOT( setPenColor( uint ))) ;
    connect(m_pixelBox->getFavPalette(), SIGNAL(colorPicked( uint )),
            colorWheel, SLOT(setColor(uint)));

    // set penColor in view
    connect(this, SIGNAL(penColorChanged(uint)),
            m_view, SLOT(setPenColor(uint)));
    // set Color Button
    connect(this, SIGNAL(penColorChanged(uint)),
            this, SLOT(setPenColor(uint)));
    // set Color Button
    connect(m_view, SIGNAL(penColorChanged(uint)),
            this, SLOT(setPenColor(uint)));
    connect(m_view, SIGNAL(penColorChanged(uint)),
            colorWheel, SLOT(setColor(uint)));

    connect(pal, SIGNAL(bkColorPicked( uint )),
            m_view, SLOT( setBkColor( uint ))) ;
    connect(pal, SIGNAL(bkColorPicked( uint )),
            prAnime, SLOT( setBkColor( uint ))) ;
    connect(m_pixelBox->getFavPalette(),  SIGNAL(bkColorPicked( uint )),
            m_view, SLOT( setBkColor( uint ))) ;
    connect(pal, SIGNAL(bkColorPicked( uint )),
            m_pixelBox, SLOT(setBkColor(uint)));
    connect(m_pixelBox->getFavPalette(),  SIGNAL(bkColorPicked( uint )),
            m_pixelBox, SLOT(setBkColor(uint)));
    connect(pal, SIGNAL(bkColorPicked(uint)),
            preview, SLOT(setBkColor(uint)));
    connect(m_pixelBox->getFavPalette(),  SIGNAL(bkColorPicked( uint )),
            preview, SLOT(setBkColor(uint)));
    connect(m_pixelBox->getFavPalette(),  SIGNAL(bkColorPicked( uint )),
            prAnime, SLOT(setBkColor(uint)));

    connect(this, SIGNAL(penSizeChanged(int)),
            m_view, SLOT(setPenSize(int)));
    connect(this, SIGNAL(toolChanged(int)),
            m_view, SLOT(setTool(int)));
    connect(this, SIGNAL(zoomChanged(int)),
            m_view, SLOT(changeZoom(int)));
    connect(m_scrollArea, SIGNAL(statusUpdate(int,QString)),
            this, SLOT(setStatus(int,QString)));
    connect(m_view, SIGNAL(zoomChanged(int)),
            this, SLOT(changeZoom(int)));
    connect(m_scrollArea, SIGNAL(modified()),
            this, SLOT(imageChanged()));
    connect(m_scrollArea, SIGNAL(modified()),
            preview, SLOT(imageChanged()));
    connect(m_scrollArea, SIGNAL(mouseUp()),
            preview, SLOT(imageChanged()));
    connect(this, SIGNAL(gridEnabled(bool)),
            m_view, SLOT(enableGrid(bool)));
    connect(this, SIGNAL(shadowSet(CFrame*)),
            m_view, SLOT(setShadow(CFrame*)));
    connect(this, SIGNAL(frameChanged(CFrame*)),
            m_view, SLOT(changeFrame(CFrame*)));
    connect(this, SIGNAL(transparencyToggled(bool)),
            m_view, SLOT(setTransparency(bool)));
    connect(m_actionAlpha, SIGNAL(triggered()),
            this, SLOT(alphaPressed()));
    connect(this, SIGNAL(alphaChanged(uchar)),
            m_view,SLOT(setAlpha(uchar)));
    connect(m_view, SIGNAL(alphaChanged(uchar)),
            this,SLOT(setAlphaU(uchar)));
    connect(this, SIGNAL(viewModeChanged(int)),
            m_view, SLOT(setViewMode(int)));

   // ui->centralWidget->show();

    connect(this, SIGNAL(openFile(QString)),
            this, SLOT(loadFile(QString)));

    reloadSettings();
}

void MainWindow::setVisible ( bool visible )
{
    QWidget::setVisible(visible);
    if (visible) {
        QSettings settings;
        settings.beginGroup("UI_Components");
            restoreGeometry(settings.value("mainWindow:geometry").toByteArray());
            restoreState(settings.value("mainWindow:state").toByteArray());
        settings.endGroup();
    }
}

void MainWindow::reloadSettings()
{
    QSettings settings;
    m_saveSettings = settings.value("saveSettings", false).toBool();
    ui->actionSave_Settings->setChecked(m_saveSettings);

    int zoom = settings.value("zoom", 8).toInt();
    if (zoom != 1 && zoom != 4 && zoom != 8 && zoom != 16) {
        zoom = 8;
    }
    emit zoomChanged ( zoom );
    changeZoom(zoom);

    bool showGrid = settings.value("showGrid", true).toBool();
    ui->actionShowGrid->setChecked(showGrid);
    emit gridEnabled( showGrid );

    bool showTransparency = settings.value("showTransparency", false).toBool();
    ui->actionTransparency->setChecked(showTransparency);
    emit transparencyToggled(showTransparency);

    QAction *tools[] = {
    // TOOL_PEN        0
        ui->actionPen,
    // TOOL_FLOODFILL  1
        ui->actionFlood_fill,
    // TOOL_LINE       2
        ui->actionLine,
    // TOOL_CIRCLE     3
        ui->actionCircle,
    // TOOL_RECT       4
        ui->actionRectangle,
    // TOOL_COLORSELECT 5
        ui->actionColor_Select,
    // TOOL_SELECT     6
        ui->actionSelect,
    // TOOL_MAP        7
        ui->actionMap,
    // TOOL_ERASER     8
        ui->actionEraser,
    // TOOL_ZOOM       9
        ui->actionZoom
    };

    int tool = settings.value("tool", TOOL_PEN).toInt();
    if (tool < 0 || tool >= (int)(sizeof(tools)/sizeof(QAction*))) {
        tool = TOOL_PEN;
    }
    emit toolChanged(tool);
    tools[tool]->setChecked(true);

    QAction *penSizes[] = {
        ui->action1,
        ui->action2,
        ui->action3,
        ui->action4,
        ui->action5,
        ui->action6,
        ui->action7,
        ui->action8,
        ui->action9
    };
    int penSize = settings.value("penSize", 1).toInt();
    if (penSize < 1 || penSize >= (int)(sizeof(penSizes)/sizeof(QAction*)) -1) {
        penSize = 1;
    }
    emit penSizeChanged(penSize);
    penSizes[penSize-1]->setChecked(true);
    bool ok;
    QString bkColor = settings.value("bkColor", "ff8040").toString();
    emit bkColorSet(bkColor.toUInt(&ok, 16));
    QString gridColor = settings.value("gridColor", "201008").toString();
    emit gridColorSet(gridColor.toUInt(&ok,16));
    QString borderColor = settings.value("borderColor", "ffffff").toString();
    emit borderColorSet(borderColor.toUInt(&ok,16));
    QString penColor = settings.value("penColor", "ffc0d0").toString();
    emit penColorChanged(penColor.toUInt(&ok,16));
    m_lastOpenFilter = settings.value("lastOpenFilter", m_pngFilter).toString();
    uchar alpha = settings.value("alpha", 0xff).toUInt();
    setAlpha(alpha);
    emit alphaChanged(alpha);
    m_lastWidth = settings.value("lastWidth",64).toInt();
    m_lastHeight = settings.value("lastHeight",64).toInt();
    int viewMode = settings.value("viewMode",CFrameWidget::MODE_NORMAL).toInt();
    emit viewModeChanged(viewMode);
    QAction *actionViewModes[]= {
        ui->actionPaintNormal,
        ui->actionPaintColorOnly,
        ui->actionPaintAlphaOnly,
        ui->actionTapestry
    };
    actionViewModes[viewMode % 4]->setChecked(true);
    m_fadeFactor = settings.value("fadeFactor",255).toInt();
    // save default shortcuts
    defaultShortcuts();
    // restore settings from config
    settings.beginGroup("Shortcuts");
    QAction **actions = actionShortcuts();
    for (uint i=0; actions[i];++i) {
        QAction *a = actions[i];
        QString s = settings.value(a->text(),"n/a").toString();
        if (s!="n/a") {
            a->setShortcut(QKeySequence(s));
        }
    }
    settings.endGroup();
    // Toolbars
    settings.beginGroup("Toolbars");
    m_showMain = settings.value("main", true).toBool();
    ui->actionMain->setChecked(m_showMain);
    if (!m_showMain){
       ui->mainToolBar->hide();
    }
    m_showTB1 = settings.value("toolbar1", true).toBool();
    if (!m_showTB1) {
        m_toolbar1->hide();
    }
    ui->actionColor->setChecked(m_showTB1);
    m_showTB2 = settings.value("toolbar2", true).toBool();
    if (!m_showTB2) {
        m_toolbar2->hide();
    }
    ui->actionTools->setChecked(m_showTB2);
    m_showTB3 = settings.value("toolbar3", true).toBool();
    if (!m_showTB3) {
        m_toolbar3->hide();
    }
    ui->actionTools->setChecked(m_showTB3);
    settings.endGroup();

    uint colors[256];
    settings.beginGroup("FavColors");
    const CWPalette * wpal = m_pixelBox->getFavPalette();
    for (int i=0; i < 256; ++i) {
        const QString name = QString::asprintf("color_%d", i);
        QString col = settings.value(name, "0").toString();
        colors[i] = col.toUInt(&ok, 16) | 0xff000000;
    }
    settings.endGroup();
    connect(this, SIGNAL(changePal(uint*,int)), wpal, SLOT(palChanged(uint*,int)));
    emit changePal(colors, 256);

    settings.beginGroup("UI_Components");
        bool showPreview = settings.value("showPreview", true).toBool();
        bool showAnimated = settings.value("showAnimated", false).toBool();
        int animateSpeed = settings.value("animateSpeed", 1000/12).toInt();
        int animateZoom = settings.value("animateZoom", 1).toInt();
    settings.endGroup();

    // restore liveView zoom
    int lv_zoom = settings.value("liveViewZoom", 1).toInt();
    emit zoomLVChanged(lv_zoom);
    QString s = tr("Liveview %1%").arg(lv_zoom*100);
    m_previewDock->setWindowTitle(s);
    ui->actionPreview->setChecked(showPreview);
    if (showPreview) {
        m_previewDock->show();
    } else {
        m_previewDock->hide();
    }
    // restore animated view
    s = tr("Animated %1%").arg(animateZoom*100);
    m_animate->setWindowTitle(s);
    CPreviewWidget *prAnime = m_animate->getView();
    prAnime->animate(animateSpeed);
    prAnime->setZoom(animateZoom);
    ui->actionAnimated_view->setChecked(showAnimated);
    if (showAnimated) {
        m_animate->show();
    } else {
        m_animate->hide();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
        return;
    }
    QSettings settings;
    settings.setValue("saveSettings", m_saveSettings);
    if (m_saveSettings) {
        settings.setValue("zoom", m_view->zoom());
        settings.setValue("showGrid", m_view->showGrid());
        settings.setValue("showTransparency", m_view->showTransparency());
        settings.setValue("tool", m_view->tool());
        settings.setValue("penSize", m_view->penSize());
        const QString bkColor = QString::asprintf("%.6x", m_view->bkColor());
        settings.setValue("bkColor", bkColor);
        const QString gridColor = QString::asprintf("%.6x", m_view->gridColor());
        settings.setValue("gridColor", gridColor);
        const QString borderColor = QString::asprintf("%.6x", m_view->borderColor());
        settings.setValue("borderColor", borderColor);
        const QString penColor = QString::asprintf("%.6x", m_view->penColor());
        settings.setValue("penColor", penColor);
        settings.setValue("lastOpenFilter", m_lastOpenFilter);
        settings.setValue("alpha", m_view->alpha());
        settings.setValue("lastWidth", m_lastWidth);
        settings.setValue("lastHeight", m_lastHeight);
        settings.setValue("liveViewZoom", m_previewDock->getView()->zoom());
        settings.setValue("viewMode", m_view->viewMode());
        settings.setValue("fadeFactor", m_fadeFactor);
        settings.beginGroup("Shortcuts");
            QAction **actions = actionShortcuts();
            for (uint i=0; actions[i];++i) {
                QAction *a = actions[i];
                QKeySequence s = a->shortcut();
                settings.setValue(a->text(), s.toString());
            }
        settings.endGroup();
        settings.beginGroup("Toolbars");
            settings.setValue("main", m_showMain);
            settings.setValue("toolbar1", m_showTB1);
            settings.setValue("toolbar2", m_showTB2);
            settings.setValue("toolbar3", m_showTB3);
            settings.endGroup();
            settings.beginGroup("FavColors");
            const CWPalette * wpal = m_pixelBox->getFavPalette();
            const uint* pal = wpal->palette();
            for (int i=0; i < 256; ++i) {
                const QString name = QString::asprintf("color_%d", i);
                const QString value = QString::asprintf("%.6x", pal[i] & 0xffffff);
                settings.setValue(name, value);
            }
        settings.endGroup();
        settings.beginGroup("UI_Components");
            settings.setValue("mainWindow:geometry",
                              this->saveGeometry());
            settings.setValue("mainWindow:state",
                              this->saveState());
            CPreviewWidget *preview = m_previewDock->getView();
            settings.setValue("showPreview",
                              preview->isVisible());
            settings.setValue("previewIsFloating",
                              m_previewDock->isFloating());
            CPreviewWidget *prAnime = m_animate->getView();
            settings.setValue("showAnimated",
                              prAnime->isVisible());
            settings.setValue("animateSpeed",
                              prAnime->speed());
            settings.setValue("animateZoom",
                              prAnime->zoom());
            settings.setValue("animateIsFloating",
                              m_animate->isFloating());
        settings.endGroup();
        settings.sync();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::maybeSave()
{
    if (m_doc.isDirty()) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, tr(m_appName),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard
                     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::open(QString fileName)
{    
    QString selected = m_lastOpenFilter;
    if (maybeSave()) {
        if (fileName.isEmpty()) {
            QStringList filters;
            filters.append(m_allFilter);
            filters.append(m_oblFilter);
            filters.append(m_pngFilter);
            //if (selected != tr(m_pngFilter)) {
            //    selected += "\n" + tr(m_pngFilter);
            //} else {
            //    selected += "\n" + tr(m_oblFilter);
            //}
            QFileDialog * dlg = new QFileDialog(this,tr("Open"),"",selected);
            //dlg->setFilter(filters);
            //dlg->seselectFilter(selected);
            dlg->setAcceptMode(QFileDialog::AcceptOpen);
            dlg->setFileMode(QFileDialog::ExistingFile);
            dlg->selectFile(m_doc.getFileName());
            dlg->setNameFilters(filters);
            if (dlg->exec()) {
                m_lastOpenFilter = dlg->selectedNameFilter();
                QStringList fileNames = dlg->selectedFiles();
                if (fileNames.count()>0) {
                    fileName = fileNames[0];
                }
            }
            delete dlg;
        }

        loadFile(fileName);
    }
    updateMenus();
    updateStatus();
}

void MainWindow::loadFile(const QString &fileName)
{
    if (!fileName.isEmpty()) {
        QString oldFileName = m_doc.getFileName();
        m_doc.setFileName(fileName);
        if (m_doc.read())  {
            qDebug("size: %d", m_doc.getSize());
            m_doc.setCurrentIndex(0);
            CFrame * frame = m_doc.getCurrent();
            emit frameChanged(frame);
        } else {
            warningMessage(tr("error:\n") + m_doc.getLastError());
            m_doc.setFileName(oldFileName);
            // update fileList
            QSettings settings;
            QStringList files = settings.value("recentFileList").toStringList();
            files.removeAll(fileName);
            settings.setValue("recentFileList", files);
        }

        updateTitle();
        updateRecentFileActions();
        reloadRecentFileActions();
    }
    updateMenus();
    updateStatus();
}

bool MainWindow::save()
{
    QString oldFileName = m_doc.getFileName();
    if (m_doc.isUntitled()) {
        char format[7];
        if (!saveAs(format))
            return false;        
        m_doc.setFormat(format);
    }

    if (!m_doc.write() || !updateTitle())  {
        warningMessage(tr("Can't write file"));
        m_doc.setFileName(oldFileName);
        return false;
    }

    updateRecentFileActions();
    reloadRecentFileActions();
    updateStatus();
    return true;
}

bool MainWindow::saveAs(char *outFormat)
{
    bool result = false;
    QStringList filters;
    QString selected = tr(m_oblFilter) + "\n" + tr(m_pngFilter);
    QString suffix = "obl";
    QString fileName = "";
    if (!CFrameSet::isFriendFormat(m_doc.getFormat())) {
        selected = tr(m_pngFilter) + "\n" + tr(m_oblFilter);
        suffix = "png";
        filters.append(tr(m_pngFilter));
        filters.append(tr(m_oblFilter));
    } else {
        filters.append(tr(m_oblFilter));
        filters.append(tr(m_pngFilter));
    }

    CWFileSave * dlg = new CWFileSave(this,tr("Save As"),"",selected);
    //dlg->setFilters(filters);
    //dlg->selectedNameFilter(selected);
    dlg->setNameFilters(filters);
    dlg->setAcceptMode(QFileDialog::AcceptSave);
    dlg->setDefaultSuffix(suffix);
    dlg->selectFile(m_doc.getFileName());
    connect(dlg, SIGNAL(filterSelected(QString)), dlg, SLOT(changeDefaultSuffix(QString)));
    if (dlg->exec()) {
        QStringList fileNames = dlg->selectedFiles();
        if (fileNames.count()>0) {
            fileName = fileNames[0];
        }
    }

    if (!fileName.isEmpty()) {
        selected = dlg->selectedNameFilter();
        if (selected == tr(m_oblFilter)) {
            strcpy(outFormat, "OBL5");
        } else {
            strcpy(outFormat, "PNG");
        }
        m_doc.setFileName(fileName);
        result = true;
    }

    delete dlg;
    return result;
}

void MainWindow::warningMessage(const QString message)
{
    QMessageBox::warning(this, m_appName, message);
}

void MainWindow::setDocument(const QString fileName)
{
    m_doc.setFileName(fileName);
    m_doc.read();
}

bool MainWindow::updateTitle()
{
    QString file;
    if (m_doc.getFileName().isEmpty()) {
        file = tr("untitled");
    } else {
        file = QFileInfo(m_doc.getFileName()).fileName();
    }

    m_doc.setDirty(false);
    setWindowTitle(tr("%1[*] - %2").arg( file  ).arg(tr(m_appName)));
    return true;
}

void MainWindow::on_actionOpen_triggered()
{
    open("");
    updateMenus();
}

void MainWindow::on_actionSave_triggered()
{    
    save();
}

void MainWindow::on_actionSave_As_triggered()
{
    char format[7];
    if (saveAs(format)) {
        m_doc.setFormat(format);
        save();
    }
}

void MainWindow::on_actionRevert_triggered()
{
    if (m_doc.isDirty()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr(m_appName),
                     tr("The document has been modified.\n"
                        "If you revert it back, all changes will be lost."),
                     QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Discard) {
            m_doc.setDirty(false);
            open(m_doc.getFileName());
        }
    }
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        open(action->data().toString());
    }
    updateMenus();
}

void MainWindow::initFileMenu()
{
    // gray out the open recent `nothin' yet`
    ui->action_nothin_yet->setEnabled(false);

    for (int i = 0; i < MaxRecentFiles; i++) {
        m_recentFileActs[i] = new QAction(this);
        m_recentFileActs[i]->setVisible(false);
        ui->menuOpen_recent->addAction(m_recentFileActs[i]);
        connect(m_recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
    reloadRecentFileActions();

    // connect the File->Quit to the close app event
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    ui->actionQuit->setMenuRole(QAction::QuitRole);
}

void MainWindow::reloadRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        m_recentFileActs[i]->setText(text);
        m_recentFileActs[i]->setData(files[i]);
        m_recentFileActs[i]->setVisible(true);
        m_recentFileActs[i]->setStatusTip(files[i]);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        m_recentFileActs[j]->setVisible(false);

    ui->action_nothin_yet->setVisible(numRecentFiles == 0);
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    QString fileName = m_doc.getFileName();
    files.removeAll(fileName);
    if (!fileName.isEmpty()) {
        files.prepend(fileName);
        while (files.size() > MaxRecentFiles)
            files.removeLast();
    }
    settings.setValue("recentFileList", files);
}

void MainWindow::updateMenus()
{
    CFrame * frame = m_doc.getCurrent();

    QAction * actions[] = {
        ui->actionFlip_Horizontal,
        ui->actionFlip_Vertical,
        ui->actionCut,
        ui->actionCopy,
        ui->actionClear,
        //ui->actionPaste,
        ui->actionInverse,
        ui->actionMove,
        ui->actionPrevious,
        ui->actionNext,
        ui->actionRotate,
        ui->actionShrink,
        ui->actionSpread_Horizontal,
        ui->actionSpread_Vertical,
        ui->actionDelete_image,
        ui->actionEnlarge,
        ui->actionShrink,
        ui->actionDuplicate,
        ui->actionResize,
        ui->actionShift_UP,
        ui->actionShift_Down,
        ui->actionShift_Left,
        ui->actionShift_Right,
        ui->actionShadow,
        ui->actionSplit,
        ui->actionAdd_static_view,
        ui->actionSave,
        ui->actionSave_As,
        ui->actionRotate_All,
        ui->actionShrink_All,
        ui->actionDelete_All,
        ui->actionEnlarge_All,
        ui->actionShrink_All,
        ui->actionDuplicate,
        ui->actionResize_All,
        ui->actionShift_Up_All,
        ui->actionShift_Down_All,
        ui->actionShift_Left_All,
        ui->actionShift_Right_All,
        ui->actionFilp_Horizontal_All,
        ui->actionFlip_Vertical_All,
        ui->actionInverse_All,
        ui->actionGo_to_First,
        ui->actionGo_to_Last,
        ui->actionSwap_Green_Blue,
        ui->actionSwap_Red_Blue,
        ui->actionFade
    };

    for (unsigned int i=0; i < sizeof(actions)/sizeof(QAction*); ++i) {
        actions[i]->setEnabled( frame != NULL );
    }

    ui->actionUndo->setEnabled( frame != NULL && frame->canUndo());
    ui->actionRedo->setEnabled( frame != NULL && frame->canRedo());

    if (frame != NULL) {
        int i = m_doc.getCurrentIndex();
        ui->actionNext->setEnabled(i < m_doc.getSize() -1 );
        ui->actionGo_to_Last->setEnabled(i < m_doc.getSize() -1 );
        ui->actionPrevious->setEnabled(i > 0);
        ui->actionGo_to_First->setEnabled(i > 0);
    }

    ui->actionExport->setEnabled(false);
    //ui->actionCircle->setEnabled(false);
    ui->actionSelect->setEnabled(false);
    ui->actionMap->setEnabled(false);
}

void MainWindow::on_actionAbout_triggered()
{
    QString vendor;
    QString renderer;
    QString version;
    QString extensions;
    m_view->getGLInfo(vendor, renderer, version, extensions);
    CDlgAbout *d = new CDlgAbout (this);
    d->setGLInfo(vendor, renderer, version, extensions);
    d->exec();
    delete d;
}

void MainWindow::on_actionQuit_triggered()
{

}

void MainWindow::on_actionImport_triggered()
{
    CWizFrameSet *wiz = new CWizFrameSet( (QWidget*)parent() );
    wiz->init(m_doc.getSize());
    if (wiz->exec()) {
        CFrameSet & fs = *(wiz->getFrameSet());
        if (fs.getSize()) {
            m_doc.setDirty( true );
        }
        for (int i = 0; i < fs.getSize(); ++i) {
            m_doc.add(new CFrame(fs[i]));
        }

        m_doc.setCurrentIndex(m_doc.getSize() -1);
        CFrame * frame = m_doc.getCurrent();
        emit frameChanged(frame);
    }
    delete wiz;
    updateStatus();
    updateMenus();
}

void MainWindow::on_actionNew_triggered()
{
    if (maybeSave()) {
        emit frameChanged(NULL);
        m_doc.forget();
        m_doc.setFileName("");
        updateTitle();
        updateRecentFileActions();
        reloadRecentFileActions();
    }
    updateMenus();
    updateStatus();
}

void MainWindow::on_actionMove_triggered()
{
    CFrame **frames = new CFrame* [m_doc.getSize()];
    for (int i=0; i < m_doc.getSize(); ++i) {
        frames[i] = m_doc[i];
    }

    CDlgGoto * dlg = new CDlgGoto();
    dlg->setWindowTitle(tr("Move Image To"));
    dlg->initMoveTo(frames, m_doc.getSize());

    if (dlg->exec() == QDialog::Accepted) {
        int target = dlg->getIndex();
        int current = m_doc.getCurrentIndex();
        if (target != current) {
            m_doc.setDirty(true);
            CFrameSet & set = m_doc.getImageSet();
            CFrame *source = set.removeAt(current);
            set.insertAt(target, source);
            m_doc.setCurrentIndex(dlg->getIndex());
            emit frameChanged(source);
        }
    }
    updateMenus();
    updateStatus();
    delete dlg;
    delete [] frames;
}

void MainWindow::on_actionPrevious_triggered()
{
    int i = m_doc.getCurrentIndex();
    if (i > 0 && m_doc.getSize()) {
        m_doc.setCurrentIndex(--i);
        emit frameChanged(m_doc.getCurrent());
    }
    updateMenus();
    updateStatus();
}

void MainWindow::on_actionNext_triggered()
{
    int i = m_doc.getCurrentIndex();
    if (i < m_doc.getSize() -1 && m_doc.getSize()) {
        m_doc.setCurrentIndex(++i);
        emit frameChanged(m_doc.getCurrent());
    }
    updateStatus();
    updateMenus();
}

void MainWindow::on_actionGo_to_First_triggered()
{
    int i = m_doc.getCurrentIndex();
    if (i > 0 && m_doc.getSize()) {
        m_doc.setCurrentIndex(0);
        emit frameChanged(m_doc.getCurrent());
        updateMenus();
        updateStatus();
    }
}

void MainWindow::on_actionGo_to_Last_triggered()
{
    int i = m_doc.getCurrentIndex();
    if (i < m_doc.getSize() - 1 && m_doc.getSize()) {
        m_doc.setCurrentIndex(m_doc.getSize() - 1);
        emit frameChanged(m_doc.getCurrent());
        updateMenus();
        updateStatus();
    }
}

void MainWindow::initToolbar()
{
    //void QMainWindow::insertToolBarBreak ( QToolBar * before )
    ui->mainToolBar->setWindowTitle(tr("Main"));
    ui->mainToolBar->setObjectName("mainToolbar");
    ui->mainToolBar->setIconSize( QSize(16,16) );
    ui->mainToolBar->addAction(ui->actionNew);
    ui->mainToolBar->addAction(ui->actionOpen);
    ui->mainToolBar->addAction(ui->actionSave);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionGo_to_First);
    ui->mainToolBar->addAction(ui->actionPrevious);
    ui->mainToolBar->addAction(ui->actionNext);
    ui->mainToolBar->addAction(ui->actionGo_to_Last);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionInsert_image);
    ui->mainToolBar->addAction(ui->actionDelete_image);

    connect(ui->mainToolBar, SIGNAL(visibilityChanged(bool)),
            ui->actionMain, SLOT(setChecked(bool)));

    m_toolbar3 = new QToolBar(this);
    m_toolbar3->setWindowTitle(tr("Edit Mode"));
    m_toolbar3->setObjectName("toolbar3");
    m_toolbar3->setIconSize( QSize(16,16) );
    addToolBar(m_toolbar3);
    connect(m_toolbar3, SIGNAL(visibilityChanged(bool)),
            ui->actionEdit_Mode, SLOT(setChecked(bool)));

    m_toolbar3->addAction(ui->actionPaintNormal);
    m_toolbar3->addAction(ui->actionPaintColorOnly);
    m_toolbar3->addAction(ui->actionPaintAlphaOnly);
    m_toolbar3->addAction(ui->actionTapestry);
    //m_toolbar1->addSeparator();

    m_toolbar1 = new QToolBar(this);
    m_toolbar1->setWindowTitle(tr("Pen Color"));
    m_toolbar1->setObjectName("toolbar1");
    m_toolbar1->setIconSize( QSize(16,16) );

    m_button =  new QToolButton(this);
    m_button->resize(12,16);
    m_button->setToolTip(tr("Pen color"));
    m_button->setStatusTip(tr("Set the current pen color."));
    m_toolbar1->addWidget(m_button);
    addToolBar(m_toolbar1);

    QPixmap spadePixmap(":/images/pd/Rocket000_emblem-spades.png");
    m_actionAlpha = new QAction(this);//QComboBox(this);
    m_actionAlpha->setIcon(QIcon(spadePixmap));
    m_actionAlpha->setToolTip(tr("Alpha Channel"));
    m_actionAlpha->setStatusTip(tr("Set the Alpha Channel Transparency for the current tool."));
    m_actionAlpha->setIconText(tr("Set Alpha Channel"));
    m_toolbar1->addAction(m_actionAlpha);

    QPixmap starPixmap(":/images/pd/Rocket000_emblem-star.png");
    m_actionFavColor = new QAction(QIcon(starPixmap), tr("Add Favorite Color"), this);
    m_actionFavColor->setToolTip(tr("Add Favorite Color"));
    m_actionFavColor->setStatusTip(tr("Add the current pen color to the custom palette."));
    m_toolbar1->addAction(m_actionFavColor);

    m_toolbar2 = new QToolBar(this);
    m_toolbar2->setWindowTitle(tr("Tools"));
    m_toolbar2->setObjectName("toolbar2");
    m_toolbar2->setIconSize( QSize(16,16) );
    addToolBar(m_toolbar2);
    connect(m_toolbar2, SIGNAL(visibilityChanged(bool)),
            ui->actionTools, SLOT(setChecked(bool)));

    m_toolbar2->addAction(ui->actionPen);
    m_toolbar2->addAction(ui->actionEraser);
    m_toolbar2->addAction(ui->actionFlood_fill);
    m_toolbar2->addAction(ui->actionLine);
    m_toolbar2->addAction(ui->actionRectangle);
    m_toolbar2->addAction(ui->actionCircle);
    m_toolbar2->addAction(ui->actionColor_Select);
    m_toolbar2->addAction(ui->actionSelect);
    m_toolbar2->addAction(ui->actionMap);
    m_toolbar2->addAction(ui->actionZoom);
}

void MainWindow::addFavColor()
{
    emit favColorAdded(m_view->penColor() | 0xff000000);
}

void MainWindow::setStatus(const QString msg)
{
    ui->statusBar->showMessage(msg);
}

void MainWindow::setStatus(int i, const QString message)
{
    m_labels[i]->setText(message);
}

void MainWindow::updateStatus()
{
    QString s;
    if (m_doc.getSize()) {
        s = QString(tr("%1 of %2")).arg(m_doc.getCurrentIndex()+1).arg(m_doc.getSize());
    }
    setStatus(1, s);
}

void MainWindow::initStatusBar()
{
    ui->statusBar->adjustSize();
    int label0Size = 500;

    m_labels = new QLabel* [3];
    m_labels[0] = new QLabel("", ui->statusBar);
    ui->statusBar->addWidget(m_labels[0], label0Size);

    m_labels[2] = new QLabel("", ui->statusBar);
    ui->statusBar->addWidget(m_labels[2], 150);
    m_labels[2]->setAlignment(Qt::AlignRight);

    m_labels[1] = new QLabel("", ui->statusBar);
    m_labels[1]->setAlignment(Qt::AlignRight);
    ui->statusBar->addWidget(m_labels[1], 640 - label0Size);        
}

void MainWindow::colorButtonPressed()
{
    QColorDialog *d = new QColorDialog(this);
    if (d->exec() == QDialog::Accepted) {
        QColor color = d->currentColor();
        uint uColor = color.red() +
                (color.green() << 8) +
                (color.blue() << 16);
        emit penColorChanged(uColor);
    }
    delete d;
}

void MainWindow::on_actionTransparency_toggled(bool b )
{
    emit transparencyToggled(b);
}

void MainWindow::on_actionDelete_image_triggered()
{
    int index = m_doc.getCurrentIndex();
    if (m_doc.getCurrent()) {
        QMessageBox::StandardButton
                ret = QMessageBox::warning(this, tr(m_appName),
                       tr("This action cannot be undone. Are you sure that\n" \
                          "you want to delete this image?"),
                       QMessageBox::Ok | QMessageBox::Cancel);
        if (ret == QMessageBox::Ok) {
            CFrameSet & frameSet = m_doc.getImageSet();
            emit frameChanged(NULL);
            frameSet.removeAt(index);
            if (index > m_doc.getSize() - 2 ) {
                --index;
            }
            m_doc.setCurrentIndex(index);
            m_doc.setDirty( true );
            emit frameChanged(m_doc.getCurrent());          
            if (!m_doc.getSize()) {
                m_doc.setCurrentIndex(-1);
            }
            updateStatus();
            updateMenus();
        }
    }
}

void MainWindow::on_actionDuplicate_triggered()
{
    int index = m_doc.getCurrentIndex();
    if (m_doc.getCurrent()) {
        CFrame *frame = new CFrame(m_doc.getCurrent());
        CFrameSet & frameSet = m_doc.getImageSet();
        frameSet.insertAt(index, frame);
        m_doc.setCurrentIndex(++index);
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(m_doc.getCurrent());
    }
}

void MainWindow::on_actionFlip_Horizontal_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    if (frame) {
        frame->push();
        frame->flipH();
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
    }
}

void MainWindow::on_actionFlip_Vertical_triggered()
{
    if (m_doc.getCurrent()) {
        m_doc.getCurrent()->push();
        m_doc.getCurrent()->flipV();
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(m_doc.getCurrent());
    }
}

void MainWindow::on_actionEnlarge_triggered()
{
    if (m_doc.getCurrent()) {
        m_doc.getCurrent()->push();
        m_doc.getCurrent()->enlarge();
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(m_doc.getCurrent());
    }
}

void MainWindow::on_actionSpread_Vertical_triggered()
{
    if (m_doc.getCurrent()) {
        m_doc.getCurrent()->push();
        m_doc.getCurrent()->spreadV();
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(m_doc.getCurrent());
    }
}

void MainWindow::on_actionSpread_Horizontal_triggered()
{
    if (m_doc.getCurrent()) {
        m_doc.getCurrent()->push();
        m_doc.getCurrent()->spreadH();
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(m_doc.getCurrent());
    }
}

void MainWindow::on_actionShrink_triggered()
{
    if (m_doc.getCurrent()) {
        m_doc.getCurrent()->push();
        m_doc.getCurrent()->shrink();
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(m_doc.getCurrent());
    }
}

void MainWindow::on_actionInverse_triggered()
{
    if (m_doc.getCurrent()) {
        m_doc.getCurrent()->push();
        m_doc.getCurrent()->inverse();
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(m_doc.getCurrent());
    }
}

void MainWindow::on_actionExport_triggered()
{

}

void MainWindow::on_actionShift_UP_triggered()
{
    if (m_doc.getCurrent()) {
        m_doc.getCurrent()->push();
        m_doc.getCurrent()->shiftUP();
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(m_doc.getCurrent());
    }
}

void MainWindow::on_actionShift_Down_triggered()
{
    if (m_doc.getCurrent()) {
        m_doc.getCurrent()->push();
        m_doc.getCurrent()->shiftDOWN();
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(m_doc.getCurrent());
    }
}

void MainWindow::on_actionShift_Left_triggered()
{
    if (m_doc.getCurrent()) {
        m_doc.getCurrent()->push();
        m_doc.getCurrent()->shiftLEFT();
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(m_doc.getCurrent());
    }
}

void MainWindow::on_actionShift_Right_triggered()
{
    if (m_doc.getCurrent()) {
        m_doc.getCurrent()->push();
        m_doc.getCurrent()->shiftRIGHT();
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(m_doc.getCurrent());
    }
}

void MainWindow::on_actionRotate_triggered()
{
    if (m_doc.getCurrent()) {
        m_doc.getCurrent()->push();
        m_doc.getCurrent()->rotate();
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(m_doc.getCurrent());
    }
}

void MainWindow::on_actionClear_triggered()
{
    if (m_doc.getSize()){
        CFrame *frame = m_doc.getCurrent();
        frame->push();
        frame->clear();
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
        m_doc.setDirty(true);
    }
}

void MainWindow::on_actionSwap_Green_Blue_triggered()
{
    if (m_doc.getSize()){
        CFrame *frame = m_doc.getCurrent();
        frame->push();
        frame->argb2arbg();
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
        m_doc.setDirty(true);
    }
}

void MainWindow::on_actionSwap_Red_Blue_triggered()
{
    if (m_doc.getSize()){
        CFrame *frame = m_doc.getCurrent();
        frame->push();
        frame->abgr2argb();
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
        m_doc.setDirty(true);
    }
}

void MainWindow::on_actionResize_triggered()
{
    CDlgImageSize *dlg = new CDlgImageSize();
    CFrame *frame = m_doc.getCurrent();
    dlg->setSize(frame->m_nLen, frame->m_nHei);
    dlg->setWindowTitle(tr("Resize Image"));
    if (dlg->exec()== QDialog::Accepted) {
        frame->push();
        frame->resize(dlg->getWidth(), dlg->getHeight());
        m_lastWidth = dlg->getWidth();
        m_lastHeight = dlg->getHeight();
        emit frameChanged(frame);
        m_doc.setDirty(true);
        updateStatus();
        updateMenus();
    }
    delete dlg;
}

void MainWindow::on_actionPen_triggered()
{
    int tool = TOOL_PEN;
    emit toolChanged(tool);
}

void MainWindow::on_actionZoom_triggered()
{
    int tool = TOOL_ZOOM;
    emit toolChanged(tool);
}

void MainWindow::on_actionEraser_triggered()
{
     int tool = TOOL_ERASER;
     emit toolChanged(tool);
}

void MainWindow::on_actionFlood_fill_triggered()
{
    int tool = TOOL_FLOODFILL;
    emit toolChanged(tool);
}

void MainWindow::on_actionLine_triggered()
{
    int tool = TOOL_LINE;
    emit toolChanged(tool);
}

void MainWindow::on_actionCircle_triggered( )
{
    int tool = TOOL_CIRCLE;
    emit toolChanged(tool);
}

void MainWindow::on_actionRectangle_triggered( )
{
    int tool = TOOL_RECT;
    emit toolChanged(tool);
}

void MainWindow::on_actionColor_Select_triggered( )
{
    int tool = TOOL_COLORSELECT;
    emit toolChanged(tool);
}

void MainWindow::on_actionMap_triggered()
{
    int tool = TOOL_MAP;
    emit toolChanged(tool);
}

void MainWindow::on_actionSelect_triggered()
{
    int tool = TOOL_SELECT;
    emit toolChanged(tool);
}

void MainWindow::setPenColor(uint penColor)
{
    updateColorButton(penColor);
}

void MainWindow::on_action1_toggled(bool checked )
{
    if (checked) {
        setPenSize(1);
    }
}

void MainWindow::on_action2_toggled(bool checked )
{
    if (checked) {
        setPenSize(2);
    }
}

void MainWindow::on_action3_toggled(bool checked )
{
    if (checked) {
        setPenSize(3);
    }
}

void MainWindow::on_action4_toggled(bool checked )
{
    if (checked) {
        setPenSize(4);
    }
}

void MainWindow::on_action5_toggled(bool checked )
{
    if (checked) {
        setPenSize(5);
    }
}

void MainWindow::on_action6_toggled(bool checked )
{
    if (checked) {
        setPenSize(6);
    }
}

void MainWindow::on_action7_toggled(bool checked )
{
    if (checked) {
        setPenSize(7);
    }
}

void MainWindow::on_action8_toggled(bool checked )
{
    if (checked) {
        setPenSize(8);
    }
}

void MainWindow::on_action9_toggled(bool checked )
{
    if (checked) {
        setPenSize(9);
    }
}

void MainWindow::setPenSize(int size)
{
    emit penSizeChanged(size);
}

void MainWindow::on_action100_triggered()
{
    emit zoomChanged(1);
}

void MainWindow::on_action400_triggered()
{
    emit zoomChanged(4);
}

void MainWindow::on_action800_triggered()
{
    emit zoomChanged(8);
}

void MainWindow::on_action1600_triggered()
{
    emit zoomChanged(16);
}

void MainWindow::updateColorButton(const uint32_t color)
{
    int red = color & 0xff;
    int green = color >> 8 & 0xff;
    int blue = color >> 16 & 0xff;
    m_button->setStyleSheet(
        QString("* { background-color: rgb(%1,%2,%3) }")
        .arg(red).arg(green).arg(blue));
}

void MainWindow::changeZoom(int zoom)
{
    switch (zoom) {
    case 1:
        ui->action100->setChecked( true );
        break;

    case 4:
        ui->action400->setChecked( true );
        break;

    case 8:
        ui->action800->setChecked( true );
        break;

    case 16:
        ui->action1600->setChecked( true );
    }
}

void MainWindow::on_actionShowGrid_triggered(bool checked)
{
    emit gridEnabled( checked );
    emit frameChanged(m_doc.getCurrent());
}

void MainWindow::imageChanged()
{
    m_doc.setDirty(true);
    updateStatus();
    updateMenus();
}

void MainWindow::on_actionUndo_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    frame->undo();
    m_doc.setDirty( true );
    updateStatus();
    updateMenus();
    emit frameChanged(m_doc.getCurrent());
}

void MainWindow::on_actionRedo_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    frame->redo();
    m_doc.setDirty( true );
    updateStatus();
    updateMenus();
    emit frameChanged(m_doc.getCurrent());
}

void MainWindow::on_actionCut_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    if (frame){
        uint8_t *png;
        int pngSize;
        frame->toPng(png, pngSize);

        QImage img;
        if (!img.loadFromData( png, pngSize )) {
            qDebug("failed to load png\n");
        }
        delete [] png;

        QClipboard *clipboard = QApplication::clipboard();
        QMimeData *data = new QMimeData;
        data->setImageData(img);
        clipboard->setMimeData(data, QClipboard::Clipboard);

        frame->push();
        frame->clear();
        m_doc.setDirty(true);
    }
}

void MainWindow::on_actionCopy_triggered()
{    
    CFrame *frame = m_doc.getCurrent();
    if (frame){
        uint8_t *png;
        int pngSize;
        frame->toPng(png, pngSize);

        QImage img;
        if (!img.loadFromData( png, pngSize )) {
            qDebug("failed to load png\n");
        }
        delete [] png;

        QClipboard *clipboard = QApplication::clipboard();
        QMimeData *data = new QMimeData;
        data->setImageData(img);
        clipboard->setMimeData(data, QClipboard::Clipboard);
    }
}

void MainWindow::on_actionPaste_triggered()
{
    QClipboard *clipboard = QApplication::clipboard();
    QImage img = clipboard->image(QClipboard::Clipboard);
    if (!img.isNull()) {
        QSize size = img.size();
        int w = size.width();
        w += (8 - (w & 7)) & 7;
        int h = size.height();
        h += (8 - (h & 7)) & 7;
        img = img.convertToFormat(QImage::Format_RGB32,Qt::AutoColor);
        CFrame *frame = m_doc.getCurrent();
        if (frame) {
            frame->push();
            frame->resize(w,h);
            //frame->resize(std::max(w, frame->m_nLen), std::max(h, frame->m_nHei));
            frame->clear();
        } else {
            frame = new CFrame(w,h);
            m_doc.add(frame);
            m_doc.setCurrentIndex(0);
        }

        for (int y=0; y < size.height(); ++y) {
            const uchar *bits = img.constScanLine(y);
            memcpy(& frame->at(0,y), bits, size.width() * sizeof(uint32_t));
        }

        frame->abgr2argb();
        emit frameChanged(m_doc.getCurrent());
        m_doc.setDirty(true);
        updateStatus();
        updateMenus();
    } else {
        warningMessage(tr("Clipboard doesn't contain any images."));
    }
}

void MainWindow::on_actionInsert_image_triggered()
{
    CDlgImageSize *dlg = new CDlgImageSize();
    dlg->setWindowTitle(tr("Insert Image"));
    dlg->setSize(m_lastWidth, m_lastHeight);
    if (dlg->exec()== QDialog::Accepted) {
        CFrame *frame = new CFrame(dlg->getWidth(), dlg->getHeight());
        CFrameSet & set = m_doc.getImageSet();
        m_lastWidth = dlg->getWidth();
        m_lastHeight = dlg->getHeight();
        if (set.getSize()) {
            set.insertAt(m_doc.getCurrentIndex(), frame);
        } else {
            set.add(frame);
            m_doc.setCurrentIndex(0);
        }
        emit frameChanged(m_doc.getCurrent());
        m_doc.setDirty(true);
        updateStatus();
        updateMenus();
    }
    delete dlg;
}

void MainWindow::on_actionAppend_image_triggered()
{
    CDlgImageSize *dlg = new CDlgImageSize();
    dlg->setWindowTitle(tr("Append Image"));
    dlg->setSize(m_lastWidth, m_lastHeight);
    if (dlg->exec()== QDialog::Accepted) {
        CFrame *frame = new CFrame(dlg->getWidth(), dlg->getHeight());
        CFrameSet & set = m_doc.getImageSet();
        m_lastWidth = dlg->getWidth();
        m_lastHeight = dlg->getHeight();
        set.add(frame);
        emit frameChanged(frame);
        m_doc.setDirty(true);
        m_doc.setCurrentIndex(m_doc.getSize()-1);
        updateStatus();
        updateMenus();
    }
    delete dlg;
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_actionSave_Settings_triggered(bool checked)
{
    m_saveSettings = checked;
}

void MainWindow::on_actionShadow_triggered(bool checked)
{
    if (checked && m_doc.getCurrent()) {
        emit shadowSet(m_doc.getCurrent());
    } else {
        emit shadowSet(NULL);
    }
}

void MainWindow::on_actionSplit_triggered()
{
    CDlgGoto *dlg = new CDlgGoto(this);
    dlg->setWindowTitle(tr("Split into pieces..."));
    CFrame *frame = m_doc.getCurrent();
    dlg->initSizes(std::max(frame->m_nLen, frame->m_nHei));
    dlg->select( 0 );
    if (dlg->exec() == QDialog::Accepted) {
        int sizes[] = {
            16, 24, 32, 48, 64, 96, 128, 256
        };
        int pxSize = sizes[dlg->getIndex()];
        CFrameSet *frames = frame->split(pxSize);
        CFrameSet & set = m_doc.getImageSet();
        int j = m_doc.getCurrentIndex();
        set.removeAt(j);
        for (int i=0; i < frames->getSize();++i) {
            if (!(*frames)[i]->isEmpty()) {
                set.insertAt(j, (*frames)[i]);
                ++j;
            } else {
                delete (*frames)[i];
            }
        }
        frames->removeAll();
    }

    emit frameChanged(m_doc.getCurrent());
    m_doc.setDirty(true);
    updateStatus();
    updateMenus();
}

void MainWindow::on_actionPreview_toggled(bool arg1)
{
    if (arg1) {
        m_previewDock->show();
    } else {
        m_previewDock->hide();
    }
}

void MainWindow::uncheckPreview()
{
    ui->actionPreview->setChecked(false);
}

void MainWindow::uncheckAnimate()
{
    ui->actionAnimated_view->setChecked(false);
}

void MainWindow::uncheckToolbox()
{
    ui->actionToolbox->setChecked(false);
}

void MainWindow::on_actionToolbox_toggled(bool arg1)
{
    if (arg1) {
        m_pixelBox->show();
    } else {
        m_pixelBox->hide();
    }
}

void MainWindow::on_actionAdd_static_view_triggered()
{
    CWPreviewDock *dock = new CWPreviewDock( this );
    dock->setTitleTemplate(tr("Static %1%"));
    dock->setWindowTitle(tr("Static 100%"));
    this->addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setStatic();
    CPreviewWidget *view = dock->getView();
    view->assignStatic(m_doc.getCurrent());
}

void MainWindow::alphaPressed()
{
    CDlgAlpha *dlg = new CDlgAlpha(this);
    dlg->setWindowTitle(tr("Set Alpha Channel"));
    dlg->setAlpha(m_view->alpha());
    if (dlg->exec()){
        setAlpha(dlg->alpha());
    }
    delete dlg;
}

void MainWindow::on_actionFade_triggered()
{
    if (m_doc.getCurrent()) {
        CDlgAlpha *dlg = new CDlgAlpha(this);
        dlg->setWindowTitle(tr("Fade Image"));
        dlg->setAlpha(m_fadeFactor);
        if (dlg->exec()){
            m_fadeFactor = dlg->alpha();
            m_doc.getCurrent()->push();
            m_doc.getCurrent()->fade(m_fadeFactor);
            m_doc.setDirty( true );
            updateStatus();
            updateMenus();
            emit frameChanged(m_doc.getCurrent());
        }
        delete dlg;
    }
}

void MainWindow::setAlpha(int alpha)
{
    setAlphaU((uchar)(alpha & 0xff));
    emit alphaChanged((uint) (alpha & 0xff));
}

void MainWindow::setAlphaU(uchar alpha)
{
    QFileWrap file;
    if (file.open(":/images/pd/Rocket000_emblem-spades.png")) {
    //if (file.open(":/images/pd/pierig_four-leaf_clover_1.png")) {
        CFrameSet set;
        if (set.extract(file)) {
            uchar *png;
            int pngSize;
            set[0]->fade(alpha);
            set[0]->toPng(png, pngSize);
            QImage img;
            if (!img.loadFromData( png, pngSize )) {
                qDebug("failed to load png\n");
            }
            delete [] png;
            QPixmap pm = QPixmap::fromImage(img);
            m_actionAlpha->setIcon(QIcon(pm));
        }
        file.close();
    }
}

void MainWindow::on_actionFilp_Horizontal_All_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    if (frame) {
        CFrameSet & set = m_doc.getImageSet();
        for (int i=0; i < m_doc.getSize(); ++i) {
            CFrame *frame = set[i];
            frame->push();
            frame->flipH();
        }
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
    }
}

void MainWindow::on_actionFlip_Vertical_All_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    if (frame) {
        CFrameSet & set = m_doc.getImageSet();
        for (int i=0; i < m_doc.getSize(); ++i) {
            CFrame *frame = set[i];
            frame->push();
            frame->flipV();
        }
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
    }
}

void MainWindow::on_actionRotate_All_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    if (frame) {
        CFrameSet & set = m_doc.getImageSet();
        for (int i=0; i < m_doc.getSize(); ++i) {
            CFrame *frame = set[i];
            frame->push();
            frame->rotate();
        }
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
    }
}

void MainWindow::on_actionInverse_All_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    if (frame) {
        CFrameSet & set = m_doc.getImageSet();
        for (int i=0; i < m_doc.getSize(); ++i) {
            CFrame *frame = set[i];
            frame->push();
            frame->inverse();
        }
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
    }
}

void MainWindow::on_actionEnlarge_All_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    if (frame) {
        CFrameSet & set = m_doc.getImageSet();
        for (int i=0; i < m_doc.getSize(); ++i) {
            CFrame *frame = set[i];
            frame->push();
            frame->enlarge();
        }
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
    }
}

void MainWindow::on_actionShrink_All_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    if (frame) {
        CFrameSet & set = m_doc.getImageSet();
        for (int i=0; i < m_doc.getSize(); ++i) {
            CFrame *frame = set[i];
            frame->push();
            frame->shrink();
        }
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
    }
}

void MainWindow::on_actionResize_All_triggered()
{
    CDlgImageSize *dlg = new CDlgImageSize();
    CFrame *frame = m_doc.getCurrent();
    dlg->setSize(frame->m_nLen, frame->m_nHei);
    dlg->setWindowTitle(tr("Resize All Images"));
    if (dlg->exec()== QDialog::Accepted) {
        CFrameSet & set = m_doc.getImageSet();
        for (int i=0; i < m_doc.getSize(); ++i) {
            CFrame *frame = set[i];
            frame->push();
            frame->resize(dlg->getWidth(), dlg->getHeight());
        }
        m_lastWidth = dlg->getWidth();
        m_lastHeight = dlg->getHeight();
        emit frameChanged(frame);
        m_doc.setDirty(true);
        updateStatus();
        updateMenus();
    }
    delete dlg;
}

void MainWindow::on_actionDelete_All_triggered()
{
    if (m_doc.getCurrent()) {
        QMessageBox::StandardButton
                ret = QMessageBox::warning(this, tr(m_appName),
                       tr("This action cannot be undone. Are you sure that\n" \
                          "you want to delete all images?"),
                       QMessageBox::Ok | QMessageBox::Cancel);
        if (ret == QMessageBox::Ok) {
            CFrameSet & frameSet = m_doc.getImageSet();
            frameSet.removeAll();
            emit frameChanged(NULL);
            m_doc.setCurrentIndex(-1);
            m_doc.setDirty( true );
            updateStatus();
            updateMenus();
        }
    }
}

void MainWindow::on_actionShift_Up_All_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    if (frame) {
        CFrameSet & set = m_doc.getImageSet();
        for (int i=0; i < m_doc.getSize(); ++i) {
            CFrame *frame = set[i];
            frame->push();
            frame->shiftUP();
        }
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
    }
}

void MainWindow::on_actionShift_Down_All_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    if (frame) {
        CFrameSet & set = m_doc.getImageSet();
        for (int i=0; i < m_doc.getSize(); ++i) {
            CFrame *frame = set[i];
            frame->push();
            frame->shiftDOWN();
        }
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
    }
}

void MainWindow::on_actionShift_Left_All_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    if (frame) {
        CFrameSet & set = m_doc.getImageSet();
        for (int i=0; i < m_doc.getSize(); ++i) {
            CFrame *frame = set[i];
            frame->push();
            frame->shiftLEFT();
        }
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
    }
}

void MainWindow::on_actionShift_Right_All_triggered()
{
    CFrame *frame = m_doc.getCurrent();
    if (frame) {
        CFrameSet & set = m_doc.getImageSet();
        for (int i=0; i < m_doc.getSize(); ++i) {
            CFrame *frame = set[i];
            frame->push();
            frame->shiftRIGHT();
        }
        m_doc.setDirty( true );
        updateStatus();
        updateMenus();
        emit frameChanged(frame);
    }
}

void MainWindow::on_actionConfigure_Shortcuts_triggered()
{
    QAction **actions = actionShortcuts();
    QStringList listActions;
    QStringList listShortcuts;
    for (uint i=0; actions[i];++i) {
        QAction *a = actions[i];
        QKeySequence s = a->shortcut();
        listShortcuts.append(s.toString());
        listActions.append(a->text());
    }

    CDlgShortCuts *dlg = new CDlgShortCuts();
    dlg->load(listActions, listShortcuts, defaultShortcuts());
    if (dlg->exec()==QDialog::Accepted) {
        dlg->save(listShortcuts);
        for (uint i=0; actions[i];++i) {
            QAction *a = actions[i];
            a->setShortcut(QKeySequence(listShortcuts[i]));
        }
    }
    delete dlg;
}

QAction** MainWindow::actionShortcuts()
{
    static QAction *actions[]={
        ui->actionFlip_Horizontal,
        ui->actionFlip_Vertical,
        ui->actionCut,
        ui->actionCopy,
        ui->actionClear,
        ui->actionInverse,
        ui->actionMove,
        ui->actionGo_to_First,
        ui->actionGo_to_Last,
        ui->actionPrevious,
        ui->actionNext,
        ui->actionRotate,
        ui->actionShrink,
        ui->actionSpread_Horizontal,
        ui->actionSpread_Vertical,
        ui->actionDelete_image,
        ui->actionEnlarge,
        ui->actionShrink,
        ui->actionDuplicate,
        ui->actionResize,
        ui->actionShift_UP,
        ui->actionShift_Down,
        ui->actionShift_Left,
        ui->actionShift_Right,
        ui->actionShadow,
        ui->actionSplit,
        ui->actionAdd_static_view,
        ui->actionNew,
        ui->actionOpen,
        ui->actionSave,
        ui->actionSave_As,
        ui->actionUndo,
        ui->actionRedo,
        ui->actionExport,
        ui->actionPen,
        ui->actionEraser,
        ui->actionRectangle,
        ui->actionFlood_fill,
        ui->actionCircle,
        ui->actionSelect,
        ui->actionMap,
        ui->actionZoom,
        ui->actionTransparency,
        ui->actionShadow,
        ui->actionPreview,
        ui->actionAdd_static_view,
        ui->action100,
        ui->action400,
        ui->action800,
        ui->action1600,
        ui->action1,
        ui->action2,
        ui->action3,
        ui->action4,
        ui->action5,
        ui->action6,
        ui->action7,
        ui->action8,
        ui->action9,
        m_actionFavColor,
        m_actionAlpha,
        ui->actionFade,
        ui->actionPaintAlphaOnly,
        ui->actionPaintNormal,
        ui->actionPaintColorOnly,
        ui->actionTapestry,
        NULL
    };
    return actions;
}

QStringList & MainWindow::defaultShortcuts()
{
    static QStringList listShortcuts;
    if (!listShortcuts.count()) {
        QAction **actions = actionShortcuts();
        for (uint i=0; actions[i];++i) {
            QAction *a = actions[i];
            QKeySequence s = a->shortcut();
            listShortcuts.append(s.toString());
        }
    }
    return listShortcuts;
}

void MainWindow::initActionGroups()
{
    QActionGroup  *groupTools = new QActionGroup(this);
    groupTools->addAction(ui->actionPen);
    groupTools->addAction(ui->actionEraser);
    groupTools->addAction(ui->actionFlood_fill);
    groupTools->addAction(ui->actionLine);
    groupTools->addAction(ui->actionRectangle);
    groupTools->addAction(ui->actionCircle);
    groupTools->addAction(ui->actionColor_Select);
    groupTools->addAction(ui->actionSelect);
    groupTools->addAction(ui->actionMap);
    groupTools->addAction(ui->actionZoom);
    groupTools->setExclusive(true);

    QActionGroup *groupPenSize = new QActionGroup(this);
    groupPenSize->addAction(ui->action1);
    groupPenSize->addAction(ui->action2);
    groupPenSize->addAction(ui->action3);
    groupPenSize->addAction(ui->action4);
    groupPenSize->addAction(ui->action5);
    groupPenSize->addAction(ui->action6);
    groupPenSize->addAction(ui->action7);
    groupPenSize->addAction(ui->action8);
    groupPenSize->addAction(ui->action9);
    groupTools->setExclusive(true);

    QActionGroup *groupZoom = new QActionGroup(this);
    groupZoom->addAction(ui->action100);
    groupZoom->addAction(ui->action400);
    groupZoom->addAction(ui->action800);
    groupZoom->addAction(ui->action1600);
    groupZoom->setExclusive(true);

    QActionGroup *groupPaint = new QActionGroup(this);
    groupPaint->addAction(ui->actionPaintNormal);
    groupPaint->addAction(ui->actionPaintAlphaOnly);
    groupPaint->addAction(ui->actionPaintColorOnly);
    groupPaint->addAction(ui->actionTapestry);
    groupPaint->setExclusive(true);
}

void MainWindow::on_actionMain_toggled(bool arg1)
{
    if (arg1) {
        ui->mainToolBar->show();
    } else {
        ui->mainToolBar->hide();
    }
    m_showMain = arg1;
}

void MainWindow::on_actionColor_toggled(bool arg1)
{
    if (arg1) {
        m_toolbar1->show();
    } else {
        m_toolbar1->hide();
    }
    m_showTB1 = arg1;
}

void MainWindow::on_actionTools_toggled(bool arg1)
{
    if (arg1) {
        m_toolbar2->show();
    } else {
        m_toolbar2->hide();
    }
    m_showTB2 = arg1;
}

void MainWindow::on_actionEdit_Mode_toggled(bool arg1)
{
    if (arg1) {
        m_toolbar3->show();
    } else {
        m_toolbar3->hide();
    }
    m_showTB3 = arg1;
}

void MainWindow::queryNextFrame(int currFrame)
{
    ++currFrame;
    if (currFrame >= m_doc.getSize()) {
        currFrame = 0;
    }

    CFrame * frame = NULL;
    if (!m_doc.getSize()) {
        currFrame = -1;
    } else {
        frame = m_doc[currFrame];
    }
    emit nextFrame(currFrame, frame);
}

void MainWindow::on_actionAnimated_view_toggled(bool arg1)
{
    if (arg1) {
        m_animate->show();
    } else {
        m_animate->hide();
    }
}

void MainWindow::on_actionAnimation_Speed_triggered()
{
    CDlgGoto *dlg = new CDlgGoto(this);
    dlg->setWindowTitle(tr("Set Animation Speed"));
    dlg->initSpeed(1000 / m_animate->getView()->speed());
    if (dlg->exec()) {
        m_animate->getView()->setSpeed(1000 / (dlg->getIndex()+1));
    }
}

void MainWindow::on_actionPaintNormal_triggered()
{
    emit viewModeChanged(CFrameWidget::MODE_NORMAL);
    emit frameChanged(m_doc.getCurrent());
}

void MainWindow::on_actionPaintColorOnly_triggered()
{
    emit viewModeChanged(CFrameWidget::MODE_COLOR_ONLY);
    emit frameChanged(m_doc.getCurrent());
}

void MainWindow::on_actionPaintAlphaOnly_triggered()
{
    emit viewModeChanged(CFrameWidget::MODE_ALPHA_ONLY);
    emit frameChanged(m_doc.getCurrent());
}

void MainWindow::on_actionTapestry_triggered()
{
    emit viewModeChanged(CFrameWidget::MODE_TILED_VIEW);
    emit frameChanged(m_doc.getCurrent());
}
