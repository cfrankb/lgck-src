#include "stdafx.h"
#include "WizScript.h"
#include "ui_WizScript.h"
#include "FileWrap.h"
#include "GameFile.h"
#include "qtgui/cheat.h"
#include "Frame.h"

void qtLua_init();
int qtLua_call(const char *fnName);
int qtLua_call(const char *obj, const char *fnName, QVariant & result);
int externWidget(void * c);
void qtLua_exec(const std::string s);
void bindWidget(const char *s,int ID);
int findWidget(char *s);

CWizScript::CWizScript(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::CWizScript)
{
    m_gameFile = NULL;
    ui->setupUi(this);
    qtLua_init();
    int wizID = externWidget(this);
    bindWidget("wizard", wizID);
    m_index = -1;
    for (int i=0; m_actions[i].name; ++i) {
        ui->cbAction->addItem(m_actions[i].name);
    }
    ui->sDesc->setText(m_actions[0].desc);
    ui->sSample->setText(m_actions[0].sample);
    ui->sSample->setReadOnly(true);
    ui->sSample->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

CWizScript::~CWizScript()
{
    delete ui;
}

int CWizScript::nextId() const
{
    switch(currentId()) {
    case page_intro:
        return page_select;
    case page_select:
        if (is_custom()) {
            return page_custom;
        } else {
            return page_finish;
        }
    case page_custom:
        return page_finish;
    case page_finish:
    default:
        return -1;
    }
}

void CWizScript::init(CGameFile *gf)
{
    m_gameFile = gf;
    ui->cbAction->setCurrentIndex(0);
    loadScript(0);
}

void CWizScript::on_cbAction_currentIndexChanged(int index)
{
    ui->sDesc->setText(m_actions[index].desc);
    ui->sSample->setText(m_actions[index].sample);
    unloadScript();
    loadScript(index);
    m_index = index;
}

void CWizScript::loadScript(int id)
{
    if (!m_gameFile) {
        qCritical("gameFile not defined");
        return;
    }
    if (m_actions[id].loader[0]) {
        // load script
        qDebug("DEBUG: attempting to load script: id = %d", id);
        qtLua_exec(m_actions[id].loader);
    }
}

void CWizScript::unloadScript()
{
    if (m_index != -1
            && m_actions[m_index].loader[0]) {
        // unload script
        qtLua_exec("script:unload()");
    }
}

int CWizScript::is_custom() const
{
    // check if custom page is used
    if (m_index != -1
            && m_actions[m_index].loader[0]) {
        // TODO: test custom()
        return 1;
    }
    return 0;
}

std::string CWizScript::getScript()
{
    std::string result = "";
    QVariant var;
    if (m_actions[m_index].loader[0]) {
        if (qtLua_call("script", "save", var)==1) {
            result = q2c(var.toString());
        }
    } else {
        result = m_actions[m_index].sample;
    }

    return result + "\n";
}

void CWizScript::initComboBox(QComboBox* combo, const char *type)
{
    if (!m_gameFile) {
        qCritical("m_gameFile is NULL");
        return;
    }

    CGameFile & gf = *m_gameFile;
    if (strcmp(type, "sprite") == 0) {
        qDebug(">>sprite");
        CONST_DATA *data = gf.getSpriteList();
        for (int i=0; data[i].name != ""; ++i) {
            CProto & proto = gf.toProto(i);
            CFrameSet & frameSet = gf.toFrameSet(proto.m_nFrameSet);
            uint8_t *png;
            int size;
            frameSet[0]->toPng(png, size);
            QImage img;
            if (!img.loadFromData( png, size )) {
                qWarning("failed to load png (%d)\n", i);
            }
            delete [] png;
            QPixmap pm = QPixmap::fromImage(img);
            QIcon icon;
            icon.addPixmap(pm, QIcon::Normal, QIcon::On);
            combo->addItem(icon, data[i].value.c_str());
        }
        delete [] data;
    }

    if (strcmp(type, "sound") == 0) {
        qDebug(">>sound");
        CONST_DATA *data = gf.getSoundList();
        for (int i=0; data[i].name != ""; ++i) {
            combo->addItem(data[i].value.c_str());
        }
        delete [] data;
    }

    if (strcmp(type, "image") == 0) {
        qDebug(">>image");
        CONST_DATA *data = gf.getImageList();
        for (int i=0; data[i].name != ""; ++i) {
            CFrameSet & frameSet = gf.toFrameSet(i);
            uint8_t *png;
            int size;
            frameSet[0]->toPng(png, size);
            QImage img;
            if (!img.loadFromData( png, size )) {
                qWarning("failed to load png (%d)\n", i);
            }
            delete [] png;
            QPixmap pm = QPixmap::fromImage(img);
            QIcon icon;
            icon.addPixmap(pm, QIcon::Normal, QIcon::On);
            combo->addItem(icon, data[i].value.c_str());
        }
        delete [] data;
    }
}
