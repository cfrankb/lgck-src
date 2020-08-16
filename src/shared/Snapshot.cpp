/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

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
    m_bk = nullptr;
    m_fw = nullptr;
    m_layers = nullptr;
    m_displayConf = nullptr;
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
        m_bk = nullptr;
    }
    if (m_fw) {
        delete m_fw;
        m_fw = nullptr;
    }
    if (m_layers) {
        delete m_layers;
        m_layers = nullptr;
    }
    if (m_displayConf) {
        delete m_displayConf;
        m_displayConf = nullptr;
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
    return m_bk != nullptr && m_fw != nullptr
            && m_layers != nullptr;
}

void CSnapshot::take(CGame & game)
{
    forget();
    m_fw = new CScene;
    m_bk = new CScene;
    m_layers = new CLevel;
    *m_fw = *(game._fw());
    *m_bk = *(game._bk());
    *m_layers = *(game.layers());
    m_displayConf = new CDisplayConfig;
    m_vars["__mx"] = game._mx();
    m_vars["__my"] = game._my();
    m_vars["__levelTriggerCalled"] = game.var("__levelTriggerCalled");
    game.saveDisplays(m_displayConf);
}

bool CSnapshot::reload(CGame & game)
{
    if (has_snapshot()) {
        *(game._fw()) = *m_fw;
        *(game._bk()) = *m_bk;
        *(game.layers()) = *m_layers;
        game._mx() = m_vars["__mx"];
        game._my() = m_vars["__my"];
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
