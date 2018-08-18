#include "Snapshot.h"
#include "stdafx.h"
#include "Scene.h"
#include "Game.h"
#include "Level.h"
#include "IFile.h"
#include "Map.h"
#include "displayconfig.h"

CSnapshot::CSnapshot()
{
    m_bk = NULL;
    m_fw = NULL;
    m_layers = NULL;
    m_displayConf = NULL;
}

CSnapshot::~CSnapshot()
{
    forget();
}

void CSnapshot::forget()
{
    m_vars.clear();
    if (m_bk) {
        delete m_bk;
        m_bk = NULL;
    }
    if (m_fw) {
        delete m_fw;
        m_fw = NULL;
    }
    if (m_layers) {
        delete m_layers;
        m_layers = NULL;
    }
    if (m_displayConf) {
        delete m_displayConf;
        m_displayConf = NULL;
    }

    m_vars.clear();
}

void CSnapshot::read(IFile &file)
{
    forget();
    unsigned int version =0;
    file.read(&version, sizeof(version));
    ASSERT(version == VERSION);

    char has_data = 0;
    file.read(&has_data, 1);
    if (has_data) {
        m_fw->read(file);
        m_bk->read(file);
        m_layers->read(file);
        m_displayConf->read(file);
    }

    // TODO: load vars
    //m_vars["__mx"] = game.m_mx;
    //m_vars["__my"] = game.m_my;
}

void CSnapshot::write(IFile &file)
{
    unsigned int version = VERSION;
    file.write(&version, sizeof(version));
    char has_data = 0;
    if (m_fw) {
        has_data = 1;
        file.write(&has_data, 1);
        m_fw->write(file);
        m_bk->write(file);
        m_layers->write(file);
        m_displayConf->write(file);
    } else {
        file.write(&has_data, 1);
    }
    // TODO: save vars
}

bool CSnapshot::has_snapshot()
{
    return m_bk != NULL && m_fw != NULL
            && m_layers != NULL;
}

void CSnapshot::take(CGame & game)
{
    forget();
    m_fw = new CScene;
    m_bk = new CScene;
    m_layers = new CLevel;
    *m_fw = *(game.m_sFW);
    *m_bk = *(game.m_sBK);
    *m_layers = *(game.m_layers);
    m_displayConf = new CDisplayConfig;
    m_vars["__mx"] = game.m_mx;
    m_vars["__my"] = game.m_my;
    m_vars["__levelTriggerCalled"] = game.var("__levelTriggerCalled");
    game.saveDisplays(m_displayConf);
}

bool CSnapshot::reload(CGame & game)
{
    if (has_snapshot()) {
        *(game.m_sFW) = *m_fw;
        *(game.m_sBK) = *m_bk;
        *(game.m_layers) = *m_layers;
        game.m_mx = m_vars["__mx"];
        game.m_my = m_vars["__my"];
        game.var("__levelTriggerCalled") = m_vars["__levelTriggerCalled"];
        game.restoreDisplays(m_displayConf);
        // reMap collision map
        game.remap();
        return true;
    } else {
        return false;
    }
}

void CSnapshot::clear()
{
    forget();
}
