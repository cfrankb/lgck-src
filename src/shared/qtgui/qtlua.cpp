/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2014  Francois Blanchette

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

#include "LuaVM.h"
#include "qtlua.h"
#include <string>
#include <unordered_map>
#include "FileWrap.h"
#include <QLabel>
#include <QLineEdit>
#include <QWizardPage>
#include <QGridLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QListWidget>
#include <QVariant>
#include <QSlider>
#include "wbtncolor.h"
#include "WizScript.h"
#include "../shared/qtgui/qfilewrap.h"

CLuaVM g_lua;
std::unordered_map<unsigned int, void *> g_controls;
std::unordered_map<std::string, unsigned int>g_bindings;
unsigned int g_nextID = 1;

int qtLua_call(const char *obj, const char *fnName, QVariant & result)
{
    lua_State *L = g_lua.getState();
    lua_getglobal(L, obj);
    lua_getfield(L, -1, fnName);
    lua_pushvalue(L, -2);
    int status = lua_pcall(L, 1, 1, 0);
    if (status != 0) {
        g_lua.reportErrors(status);
        lua_pop(L, 1);
        return -1;
    } else {
        if (lua_isnumber(L, -1)) {
            result = QVariant(lua_tonumber(L, -1));
        }
        if (lua_isstring(L, -1)) {
            result = QVariant(lua_tostring(L, -1));
        }
        if (lua_isboolean(L, -1)) {
            result = QVariant(lua_toboolean(L, -1));
        }
        lua_pop(L, 1);
        return 1;
    }
}

int qtLua_call(const char *fnName)
{
    lua_State *L = g_lua.getState();
    lua_getglobal(L, fnName);
    int status = lua_pcall(L, 0, 1, 0);
    if ( status != 0) {
        g_lua.reportErrors(status);
        lua_pop(L, 1);
        return -1;
    } else {
        if (!lua_isnumber(L, -1)) {
            g_lua.debug("function must return a number");
        }
        int z = lua_tonumber(L, -1);
        lua_pop(L, 1);
        return z;
    }
}

void qtLua_exec(const std::string s)
{
    g_lua.exec(s.c_str());
}

void qtLua_init()
{
    // registers functions
    for (int i = 0; exports[i].fnName; ++i) {
        g_lua.registerFn(exports[i].fnName, exports[i].fnAddr);
    }

    QFileWrap file;
    if (file.open(":/scripts/qtlua.lua", "r")) {
        int size = file.getSize();
        char *buf = new char[size+1];
        buf[size]=0;
        file.read(buf, size);
        file.close();
        g_lua.exec(buf);
        delete []buf;
    }
}

void error(const char *name, int args)
{
    CLuaVM::debugv("error: fn `%s` needs `%d` args", name, args);
}

int addWidget(void * c)
{
    g_controls[g_nextID++] = c;
    lua_pushinteger(g_lua.getState(), g_nextID - 1);
    return 1;
}

int externWidget(void * c)
{
    g_controls[g_nextID++] = c;
    return g_nextID - 1;
}

int bindWidget(lua_State *L)
{
    int argc = lua_gettop(L);
    if ( argc != 2)  {
        error(__func__, 2);
    } else {
        int ID = lua_tointeger(L, 2);
        const char *s = lua_tostring(L, 1);
        g_bindings[s] = ID;
    }
    return 0;
}

void bindWidget(const char *s, int ID)
{
    g_bindings[s] = ID;
}

int findWidget(lua_State *L)
{
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        error(__func__, 1);
    } else {
        const char *s = lua_tostring(L, 1);
        lua_pushinteger(L, g_bindings[s]);
        return 1;
    }
    return 0;
}

void freeWidget(int id)
{
    g_controls[id] = nullptr;
}

int findWidget(char *s)
{
    return g_bindings[s];
}

int rgb2tuple(lua_State *L)
{
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        error(__func__, 1);
    } else {
        unsigned int rgb = lua_tointeger(L, 1);
        int red = rgb & 0xff;
        int green = (rgb >> 8) & 0xff;
        int blue = (rgb >> 16) & 0xff;
        lua_pushinteger(L, red);
        lua_pushinteger(L, green);
        lua_pushinteger(L, blue);
        return 3;
    }
    return 0;
}


///////////////////////////////////////////////////////////////////
// auto-generated
int cwbtncolor_color(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<CWBtnColor*>(g_controls[id])->color();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int cwbtncolor_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<CWBtnColor*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int cwbtncolor_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) {
       error(__func__, 0);
    } else {
        return addWidget(new CWBtnColor());
    }
    return 0;
}

int cwbtncolor_setColor(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 4) {
       error(__func__, 4);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int arg1 = lua_tointeger(L, 3);
        int arg2 = lua_tointeger(L, 4);
        static_cast<CWBtnColor*>(g_controls[id])->setColor(
            arg0,
            arg1,
            arg2);
    }
    return 0;
}

int cwizscript_accept(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<CWizScript*>(g_controls[id])->accept();
    }
    return 0;
}

int cwizscript_addPage(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int tmp = static_cast<CWizScript*>(g_controls[id])->addPage(static_cast<QWizardPage*>(g_controls[arg0]));
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int cwizscript_back(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<CWizScript*>(g_controls[id])->back();
    }
    return 0;
}

int cwizscript_buttonText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        const QString & tmp = static_cast<CWizScript*>(g_controls[id])->buttonText(static_cast<QWizard::WizardButton>(arg0));
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int cwizscript_currentId(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<CWizScript*>(g_controls[id])->currentId();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int cwizscript_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<CWizScript*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int cwizscript_initComboBox(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        const char* arg1 = lua_tostring(L, 3);
        static_cast<CWizScript*>(g_controls[id])->initComboBox(
            static_cast<QComboBox*>(g_controls[arg0]),
            arg1);
    }
    return 0;
}

int cwizscript_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) {
       error(__func__, 0);
    } else {
        return addWidget(new CWizScript());
    }
    return 0;
}

int cwizscript_next(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<CWizScript*>(g_controls[id])->next();
    }
    return 0;
}

int cwizscript_reject(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<CWizScript*>(g_controls[id])->reject();
    }
    return 0;
}

int cwizscript_removePage(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<CWizScript*>(g_controls[id])->removePage(arg0);
    }
    return 0;
}

int cwizscript_restart(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<CWizScript*>(g_controls[id])->restart();
    }
    return 0;
}

int cwizscript_setButton(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int arg1 = lua_tointeger(L, 3);
        static_cast<CWizScript*>(g_controls[id])->setButton(
            static_cast<QWizard::WizardButton>(arg0),
            static_cast<QAbstractButton*>(g_controls[arg1]));
    }
    return 0;
}

int cwizscript_setButtonText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        const char* arg1 = lua_tostring(L, 3);
        static_cast<CWizScript*>(g_controls[id])->setButtonText(
            static_cast<QWizard::WizardButton>(arg0),
            arg1);
    }
    return 0;
}

int cwizscript_setDefaultProperty(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 4) {
       error(__func__, 4);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        const char* arg1 = lua_tostring(L, 3);
        const char* arg2 = lua_tostring(L, 4);
        static_cast<CWizScript*>(g_controls[id])->setDefaultProperty(
            arg0,
            arg1,
            arg2);
    }
    return 0;
}

int cwizscript_setField(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        const char* arg1 = lua_tostring(L, 3);
        static_cast<CWizScript*>(g_controls[id])->setField(
            arg0,
            arg1);
    }
    return 0;
}

int cwizscript_setOption(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 2 and argc > 3) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        bool arg1 = (argc >= 3) ? lua_toboolean(L, 3) : true;
        static_cast<CWizScript*>(g_controls[id])->setOption(
            static_cast<QWizard::WizardOption>(arg0),
            arg1);
    }
    return 0;
}

int cwizscript_setPage(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int arg1 = lua_tointeger(L, 3);
        static_cast<CWizScript*>(g_controls[id])->setPage(
            arg0,
            static_cast<QWizardPage*>(g_controls[arg1]));
    }
    return 0;
}

int cwizscript_setSideWidget(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<CWizScript*>(g_controls[id])->setSideWidget(static_cast<QWidget*>(g_controls[arg0]));
    }
    return 0;
}

int cwizscript_setStartId(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<CWizScript*>(g_controls[id])->setStartId(arg0);
    }
    return 0;
}

int cwizscript_setSubTitleFormat(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<CWizScript*>(g_controls[id])->setSubTitleFormat(static_cast<Qt::TextFormat>(arg0));
    }
    return 0;
}

int cwizscript_setTitleFormat(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<CWizScript*>(g_controls[id])->setTitleFormat(static_cast<Qt::TextFormat>(arg0));
    }
    return 0;
}

int cwizscript_setVisible(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<CWizScript*>(g_controls[id])->setVisible(arg0);
    }
    return 0;
}

int cwizscript_setWindowTitle(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<CWizScript*>(g_controls[id])->setWindowTitle(arg0);
    }
    return 0;
}

int cwizscript_setWizardStyle(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<CWizScript*>(g_controls[id])->setWizardStyle(static_cast<QWizard::WizardStyle>(arg0));
    }
    return 0;
}

int cwizscript_startId(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<CWizScript*>(g_controls[id])->startId();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int cwizscript_subTitleFormat(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<CWizScript*>(g_controls[id])->subTitleFormat();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int cwizscript_testOption(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<CWizScript*>(g_controls[id])->testOption(static_cast<QWizard::WizardOption>(arg0));
    }
    return 0;
}

int cwizscript_titleFormat(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<CWizScript*>(g_controls[id])->titleFormat();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int cwizscript_validateCurrentPage(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<CWizScript*>(g_controls[id])->validateCurrentPage();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int cwizscript_wizardStyle(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<CWizScript*>(g_controls[id])->wizardStyle();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qbuttongroup_addButton(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int arg1 = lua_tointeger(L, 3);
        static_cast<QButtonGroup*>(g_controls[id])->addButton(
            static_cast<QAbstractButton*>(g_controls[arg0]),
            arg1);
    }
    return 0;
}

int qbuttongroup_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QButtonGroup*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qbuttongroup_exclusive(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QButtonGroup*>(g_controls[id])->exclusive();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qbuttongroup_id(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int tmp = static_cast<QButtonGroup*>(g_controls[id])->id(static_cast<QAbstractButton*>(g_controls[arg0]));
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qbuttongroup_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) {
       error(__func__, 0);
    } else {
        return addWidget(new QButtonGroup());
    }
    return 0;
}

int qbuttongroup_removeButton(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QButtonGroup*>(g_controls[id])->removeButton(static_cast<QAbstractButton*>(g_controls[arg0]));
    }
    return 0;
}

int qbuttongroup_setExclusive(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QButtonGroup*>(g_controls[id])->setExclusive(arg0);
    }
    return 0;
}

int qbuttongroup_setId(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int arg1 = lua_tointeger(L, 3);
        static_cast<QButtonGroup*>(g_controls[id])->setId(
            static_cast<QAbstractButton*>(g_controls[arg0]),
            arg1);
    }
    return 0;
}

int qcheckbox_autoExclusive(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QCheckBox*>(g_controls[id])->autoExclusive();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qcheckbox_checkState(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QCheckBox*>(g_controls[id])->checkState();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qcheckbox_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QCheckBox*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qcheckbox_isCheckable(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QCheckBox*>(g_controls[id])->isCheckable();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qcheckbox_isChecked(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QCheckBox*>(g_controls[id])->isChecked();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qcheckbox_isTristate(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QCheckBox*>(g_controls[id])->isTristate();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qcheckbox_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 0 and argc > 1) {
       error(__func__, 0);
    } else {
        const char* arg0 = (argc >= 1) ? lua_tostring(L, 1) : "";
        return addWidget(new QCheckBox(arg0));
    }
    return 0;
}

int qcheckbox_setAutoExclusive(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QCheckBox*>(g_controls[id])->setAutoExclusive(arg0);
    }
    return 0;
}

int qcheckbox_setCheckable(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QCheckBox*>(g_controls[id])->setCheckable(arg0);
    }
    return 0;
}

int qcheckbox_setChecked(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QCheckBox*>(g_controls[id])->setChecked(arg0);
    }
    return 0;
}

int qcheckbox_setText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QCheckBox*>(g_controls[id])->setText(arg0);
    }
    return 0;
}

int qcheckbox_setTristate(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 1 and argc > 2) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = (argc >= 2) ? lua_toboolean(L, 2) : true;
        static_cast<QCheckBox*>(g_controls[id])->setTristate(arg0);
    }
    return 0;
}

int qcheckbox_text(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        const QString & tmp = static_cast<QCheckBox*>(g_controls[id])->text();
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qcombobox_addItem(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QComboBox*>(g_controls[id])->addItem(arg0);
    }
    return 0;
}

int qcombobox_count(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QComboBox*>(g_controls[id])->count();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qcombobox_currentIndex(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QComboBox*>(g_controls[id])->currentIndex();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qcombobox_currentText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        const QString & tmp = static_cast<QComboBox*>(g_controls[id])->currentText();
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qcombobox_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QComboBox*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qcombobox_duplicatesEnabled(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QComboBox*>(g_controls[id])->duplicatesEnabled();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qcombobox_hasFrame(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QComboBox*>(g_controls[id])->hasFrame();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qcombobox_hidePopup(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QComboBox*>(g_controls[id])->hidePopup();
    }
    return 0;
}

int qcombobox_insertItem(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        const char* arg1 = lua_tostring(L, 3);
        static_cast<QComboBox*>(g_controls[id])->insertItem(
            arg0,
            arg1);
    }
    return 0;
}

int qcombobox_insertPolicy(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QComboBox*>(g_controls[id])->insertPolicy();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qcombobox_insertSeparator(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QComboBox*>(g_controls[id])->insertSeparator(arg0);
    }
    return 0;
}

int qcombobox_isEditable(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QComboBox*>(g_controls[id])->isEditable();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qcombobox_itemText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        const QString & tmp = static_cast<QComboBox*>(g_controls[id])->itemText(arg0);
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qcombobox_maxCount(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QComboBox*>(g_controls[id])->maxCount();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qcombobox_maxVisibleItems(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QComboBox*>(g_controls[id])->maxVisibleItems();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qcombobox_minimumContentsLength(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QComboBox*>(g_controls[id])->minimumContentsLength();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qcombobox_modelColumn(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QComboBox*>(g_controls[id])->modelColumn();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qcombobox_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) {
       error(__func__, 0);
    } else {
        return addWidget(new QComboBox());
    }
    return 0;
}

int qcombobox_removeItem(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QComboBox*>(g_controls[id])->removeItem(arg0);
    }
    return 0;
}

int qcombobox_setCurrentIndex(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QComboBox*>(g_controls[id])->setCurrentIndex(arg0);
    }
    return 0;
}

int qcombobox_setDuplicatesEnabled(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QComboBox*>(g_controls[id])->setDuplicatesEnabled(arg0);
    }
    return 0;
}

int qcombobox_setFrame(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QComboBox*>(g_controls[id])->setFrame(arg0);
    }
    return 0;
}

int qgridlayout_addWidget(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 4 and argc > 6) {
       error(__func__, 4);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int arg1 = lua_tointeger(L, 3);
        int arg2 = lua_tointeger(L, 4);
        int arg3 = (argc >= 5) ? lua_tointeger(L, 5) : 1;
        int arg4 = (argc >= 6) ? lua_tointeger(L, 6) : 1;
        static_cast<QGridLayout*>(g_controls[id])->addWidget(
            static_cast<QWidget*>(g_controls[arg0]),
            arg1,
            arg2,
            arg3,
            arg4);
    }
    return 0;
}

int qgridlayout_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QGridLayout*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qgridlayout_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) {
       error(__func__, 0);
    } else {
        return addWidget(new QGridLayout());
    }
    return 0;
}

int qlabel_alignment(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QLabel*>(g_controls[id])->alignment();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qlabel_clear(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QLabel*>(g_controls[id])->clear();
    }
    return 0;
}

int qlabel_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QLabel*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qlabel_hasScaledContents(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QLabel*>(g_controls[id])->hasScaledContents();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qlabel_hasSelectedText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QLabel*>(g_controls[id])->hasSelectedText();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qlabel_indent(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QLabel*>(g_controls[id])->indent();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qlabel_margin(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QLabel*>(g_controls[id])->margin();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qlabel_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 0 and argc > 1) {
       error(__func__, 0);
    } else {
        const char* arg0 = (argc >= 1) ? lua_tostring(L, 1) : "";
        return addWidget(new QLabel(arg0));
    }
    return 0;
}

int qlabel_openExternalLinks(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QLabel*>(g_controls[id])->openExternalLinks();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qlabel_selectedText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        const QString & tmp = static_cast<QLabel*>(g_controls[id])->selectedText();
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qlabel_selectionStart(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QLabel*>(g_controls[id])->selectionStart();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qlabel_setAlignment(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QLabel*>(g_controls[id])->setAlignment(static_cast<Qt::Alignment>(arg0));
    }
    return 0;
}

int qlabel_setBuddy(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QLabel*>(g_controls[id])->setBuddy(static_cast<QWidget*>(g_controls[arg0]));
    }
    return 0;
}

int qlabel_setFrameStyle(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QLabel*>(g_controls[id])->setFrameStyle(arg0);
    }
    return 0;
}

int qlabel_setIndent(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QLabel*>(g_controls[id])->setIndent(arg0);
    }
    return 0;
}

int qlabel_setMargin(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QLabel*>(g_controls[id])->setMargin(arg0);
    }
    return 0;
}

int qlabel_setNum(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QLabel*>(g_controls[id])->setNum(arg0);
    }
    return 0;
}

int qlabel_setOpenExternalLinks(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QLabel*>(g_controls[id])->setOpenExternalLinks(arg0);
    }
    return 0;
}

int qlabel_setScaledContents(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QLabel*>(g_controls[id])->setScaledContents(arg0);
    }
    return 0;
}

int qlabel_setText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QLabel*>(g_controls[id])->setText(arg0);
    }
    return 0;
}

int qlabel_setTextFormat(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QLabel*>(g_controls[id])->setTextFormat(static_cast<Qt::TextFormat>(arg0));
    }
    return 0;
}

int qlabel_setTextInteractionFlags(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QLabel*>(g_controls[id])->setTextInteractionFlags(static_cast<Qt::TextInteractionFlags>(arg0));
    }
    return 0;
}

int qlabel_setWordWrap(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QLabel*>(g_controls[id])->setWordWrap(arg0);
    }
    return 0;
}

int qlabel_text(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        const QString & tmp = static_cast<QLabel*>(g_controls[id])->text();
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qlabel_textFormat(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QLabel*>(g_controls[id])->textFormat();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qlabel_textInteractionFlags(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QLabel*>(g_controls[id])->textInteractionFlags();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qlabel_wordWrap(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QLabel*>(g_controls[id])->wordWrap();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qlineedit_alignment(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QLineEdit*>(g_controls[id])->alignment();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qlineedit_backspace(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QLineEdit*>(g_controls[id])->backspace();
    }
    return 0;
}

int qlineedit_copy(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QLineEdit*>(g_controls[id])->copy();
    }
    return 0;
}

int qlineedit_cursorBackward(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 2 and argc > 3) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        int arg1 = (argc >= 3) ? lua_tointeger(L, 3) : 1;
        static_cast<QLineEdit*>(g_controls[id])->cursorBackward(
            arg0,
            arg1);
    }
    return 0;
}

int qlineedit_cursorForward(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 2 and argc > 3) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        int arg1 = (argc >= 3) ? lua_tointeger(L, 3) : 1;
        static_cast<QLineEdit*>(g_controls[id])->cursorForward(
            arg0,
            arg1);
    }
    return 0;
}

int qlineedit_cursorMoveStyle(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QLineEdit*>(g_controls[id])->cursorMoveStyle();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qlineedit_cursorPosition(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QLineEdit*>(g_controls[id])->cursorPosition();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qlineedit_cut(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QLineEdit*>(g_controls[id])->cut();
    }
    return 0;
}

int qlineedit_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QLineEdit*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qlineedit_deselect(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QLineEdit*>(g_controls[id])->deselect();
    }
    return 0;
}

int qlineedit_displayText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        const QString & tmp = static_cast<QLineEdit*>(g_controls[id])->displayText();
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qlineedit_end(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QLineEdit*>(g_controls[id])->end(arg0);
    }
    return 0;
}

int qlineedit_hasSelectedText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QLineEdit*>(g_controls[id])->hasSelectedText();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qlineedit_home(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QLineEdit*>(g_controls[id])->home(arg0);
    }
    return 0;
}

int qlineedit_inputMask(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        const QString & tmp = static_cast<QLineEdit*>(g_controls[id])->inputMask();
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qlineedit_insert(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QLineEdit*>(g_controls[id])->insert(arg0);
    }
    return 0;
}

int qlineedit_isModified(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QLineEdit*>(g_controls[id])->isModified();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qlineedit_isReadOnly(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QLineEdit*>(g_controls[id])->isReadOnly();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qlineedit_isRedoAvailable(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QLineEdit*>(g_controls[id])->isRedoAvailable();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qlineedit_isUndoAvailable(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QLineEdit*>(g_controls[id])->isUndoAvailable();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qlineedit_maxLength(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QLineEdit*>(g_controls[id])->maxLength();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qlineedit_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 0 and argc > 1) {
       error(__func__, 0);
    } else {
        const char* arg0 = (argc >= 1) ? lua_tostring(L, 1) : "";
        return addWidget(new QLineEdit(arg0));
    }
    return 0;
}

int qlineedit_paste(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QLineEdit*>(g_controls[id])->paste();
    }
    return 0;
}

int qlineedit_placeholderText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        const QString & tmp = static_cast<QLineEdit*>(g_controls[id])->placeholderText();
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qlineedit_redo(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QLineEdit*>(g_controls[id])->redo();
    }
    return 0;
}

int qlineedit_selectAll(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QLineEdit*>(g_controls[id])->selectAll();
    }
    return 0;
}

int qlineedit_setAlignment(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QLineEdit*>(g_controls[id])->setAlignment(static_cast<Qt::Alignment>(arg0));
    }
    return 0;
}

int qlineedit_setCursorMoveStyle(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QLineEdit*>(g_controls[id])->setCursorMoveStyle(static_cast<Qt::CursorMoveStyle>(arg0));
    }
    return 0;
}

int qlineedit_setCursorPosition(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QLineEdit*>(g_controls[id])->setCursorPosition(arg0);
    }
    return 0;
}

int qlineedit_setInputMask(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QLineEdit*>(g_controls[id])->setInputMask(arg0);
    }
    return 0;
}

int qlineedit_setMaxLength(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QLineEdit*>(g_controls[id])->setMaxLength(arg0);
    }
    return 0;
}

int qlineedit_setModified(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QLineEdit*>(g_controls[id])->setModified(arg0);
    }
    return 0;
}

int qlineedit_setPlaceholderText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QLineEdit*>(g_controls[id])->setPlaceholderText(arg0);
    }
    return 0;
}

int qlineedit_setReadOnly(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QLineEdit*>(g_controls[id])->setReadOnly(arg0);
    }
    return 0;
}

int qlineedit_setSelection(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int arg1 = lua_tointeger(L, 3);
        static_cast<QLineEdit*>(g_controls[id])->setSelection(
            arg0,
            arg1);
    }
    return 0;
}

int qlineedit_setText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QLineEdit*>(g_controls[id])->setText(arg0);
    }
    return 0;
}

int qlineedit_setTextMargins(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 5) {
       error(__func__, 5);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int arg1 = lua_tointeger(L, 3);
        int arg2 = lua_tointeger(L, 4);
        int arg3 = lua_tointeger(L, 5);
        static_cast<QLineEdit*>(g_controls[id])->setTextMargins(
            arg0,
            arg1,
            arg2,
            arg3);
    }
    return 0;
}

int qlineedit_text(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        const QString & tmp = static_cast<QLineEdit*>(g_controls[id])->text();
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qlineedit_undo(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QLineEdit*>(g_controls[id])->undo();
    }
    return 0;
}

int qlistwidget_addItem(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QListWidget*>(g_controls[id])->addItem(arg0);
    }
    return 0;
}

int qlistwidget_count(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QListWidget*>(g_controls[id])->count();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qlistwidget_currentRow(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QListWidget*>(g_controls[id])->currentRow();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qlistwidget_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QListWidget*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qlistwidget_insertItem(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        const char* arg1 = lua_tostring(L, 3);
        static_cast<QListWidget*>(g_controls[id])->insertItem(
            arg0,
            arg1);
    }
    return 0;
}

int qlistwidget_isSortingEnabled(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QListWidget*>(g_controls[id])->isSortingEnabled();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qlistwidget_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) {
       error(__func__, 0);
    } else {
        return addWidget(new QListWidget());
    }
    return 0;
}

int qlistwidget_setCurrentRow(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QListWidget*>(g_controls[id])->setCurrentRow(arg0);
    }
    return 0;
}

int qlistwidget_setSortingEnabled(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QListWidget*>(g_controls[id])->setSortingEnabled(arg0);
    }
    return 0;
}

int qlistwidget_sortItems(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QListWidget*>(g_controls[id])->sortItems(static_cast<Qt::SortOrder>(arg0));
    }
    return 0;
}

int qradiobutton_autoExclusive(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QRadioButton*>(g_controls[id])->autoExclusive();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qradiobutton_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QRadioButton*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qradiobutton_isCheckable(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QRadioButton*>(g_controls[id])->isCheckable();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qradiobutton_isChecked(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QRadioButton*>(g_controls[id])->isChecked();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qradiobutton_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 0 and argc > 1) {
       error(__func__, 0);
    } else {
        const char* arg0 = (argc >= 1) ? lua_tostring(L, 1) : "";
        return addWidget(new QRadioButton(arg0));
    }
    return 0;
}

int qradiobutton_setAutoExclusive(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QRadioButton*>(g_controls[id])->setAutoExclusive(arg0);
    }
    return 0;
}

int qradiobutton_setCheckable(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QRadioButton*>(g_controls[id])->setCheckable(arg0);
    }
    return 0;
}

int qradiobutton_setChecked(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QRadioButton*>(g_controls[id])->setChecked(arg0);
    }
    return 0;
}

int qradiobutton_setText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QRadioButton*>(g_controls[id])->setText(arg0);
    }
    return 0;
}

int qradiobutton_text(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        const QString & tmp = static_cast<QRadioButton*>(g_controls[id])->text();
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qslider_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QSlider*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qslider_hasTracking(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QSlider*>(g_controls[id])->hasTracking();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qslider_invertedAppearance(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QSlider*>(g_controls[id])->invertedAppearance();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qslider_invertedControls(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QSlider*>(g_controls[id])->invertedControls();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qslider_isSliderDown(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QSlider*>(g_controls[id])->isSliderDown();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qslider_maximum(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSlider*>(g_controls[id])->maximum();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qslider_minimum(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSlider*>(g_controls[id])->minimum();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qslider_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 0 and argc > 1) {
       error(__func__, 0);
    } else {
        int arg0 = (argc >= 1) ? lua_tointeger(L, 1) : static_cast<int>(Qt::Vertical);
        return addWidget(new QSlider(static_cast<Qt::Orientation>(arg0)));
    }
    return 0;
}

int qslider_orientation(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSlider*>(g_controls[id])->orientation();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qslider_pageStep(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSlider*>(g_controls[id])->pageStep();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qslider_setInvertedAppearance(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QSlider*>(g_controls[id])->setInvertedAppearance(arg0);
    }
    return 0;
}

int qslider_setInvertedControls(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QSlider*>(g_controls[id])->setInvertedControls(arg0);
    }
    return 0;
}

int qslider_setMaximum(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QSlider*>(g_controls[id])->setMaximum(arg0);
    }
    return 0;
}

int qslider_setMinimum(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QSlider*>(g_controls[id])->setMinimum(arg0);
    }
    return 0;
}

int qslider_setPageStep(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QSlider*>(g_controls[id])->setPageStep(arg0);
    }
    return 0;
}

int qslider_setRange(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int arg1 = lua_tointeger(L, 3);
        static_cast<QSlider*>(g_controls[id])->setRange(
            arg0,
            arg1);
    }
    return 0;
}

int qslider_setSingleStep(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QSlider*>(g_controls[id])->setSingleStep(arg0);
    }
    return 0;
}

int qslider_setSliderDown(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QSlider*>(g_controls[id])->setSliderDown(arg0);
    }
    return 0;
}

int qslider_setSliderPosition(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QSlider*>(g_controls[id])->setSliderPosition(arg0);
    }
    return 0;
}

int qslider_setTickInterval(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QSlider*>(g_controls[id])->setTickInterval(arg0);
    }
    return 0;
}

int qslider_setTickPosition(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QSlider*>(g_controls[id])->setTickPosition(static_cast<QSlider::TickPosition>(arg0));
    }
    return 0;
}

int qslider_setTracking(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QSlider*>(g_controls[id])->setTracking(arg0);
    }
    return 0;
}

int qslider_singleStep(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSlider*>(g_controls[id])->singleStep();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qslider_sliderPosition(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSlider*>(g_controls[id])->sliderPosition();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qslider_tickInterval(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSlider*>(g_controls[id])->tickInterval();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qslider_tickPosition(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSlider*>(g_controls[id])->tickPosition();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qslider_triggerAction(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QSlider*>(g_controls[id])->triggerAction(static_cast<QSlider::SliderAction>(arg0));
    }
    return 0;
}

int qslider_value(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSlider*>(g_controls[id])->value();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qspinbox_cleanText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        const QString & tmp = static_cast<QSpinBox*>(g_controls[id])->cleanText();
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qspinbox_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QSpinBox*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qspinbox_maximum(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSpinBox*>(g_controls[id])->maximum();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qspinbox_minimum(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSpinBox*>(g_controls[id])->minimum();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qspinbox_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) {
       error(__func__, 0);
    } else {
        return addWidget(new QSpinBox());
    }
    return 0;
}

int qspinbox_prefix(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        const QString & tmp = static_cast<QSpinBox*>(g_controls[id])->prefix();
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qspinbox_setMaximum(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QSpinBox*>(g_controls[id])->setMaximum(arg0);
    }
    return 0;
}

int qspinbox_setMinimum(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QSpinBox*>(g_controls[id])->setMinimum(arg0);
    }
    return 0;
}

int qspinbox_setPrefix(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QSpinBox*>(g_controls[id])->setPrefix(arg0);
    }
    return 0;
}

int qspinbox_setRange(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int arg1 = lua_tointeger(L, 3);
        static_cast<QSpinBox*>(g_controls[id])->setRange(
            arg0,
            arg1);
    }
    return 0;
}

int qspinbox_setSingleStep(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QSpinBox*>(g_controls[id])->setSingleStep(arg0);
    }
    return 0;
}

int qspinbox_setSuffix(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QSpinBox*>(g_controls[id])->setSuffix(arg0);
    }
    return 0;
}

int qspinbox_setValue(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QSpinBox*>(g_controls[id])->setValue(arg0);
    }
    return 0;
}

int qspinbox_singleStep(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSpinBox*>(g_controls[id])->singleStep();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qspinbox_suffix(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        const QString & tmp = static_cast<QSpinBox*>(g_controls[id])->suffix();
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qspinbox_value(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QSpinBox*>(g_controls[id])->value();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qvboxlayout_addWidget(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QVBoxLayout*>(g_controls[id])->addWidget(static_cast<QWidget*>(g_controls[arg0]));
    }
    return 0;
}

int qvboxlayout_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QVBoxLayout*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qvboxlayout_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) {
       error(__func__, 0);
    } else {
        return addWidget(new QVBoxLayout());
    }
    return 0;
}

int qwizard_accept(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QWizard*>(g_controls[id])->accept();
    }
    return 0;
}

int qwizard_addPage(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int tmp = static_cast<QWizard*>(g_controls[id])->addPage(static_cast<QWizardPage*>(g_controls[arg0]));
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qwizard_back(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QWizard*>(g_controls[id])->back();
    }
    return 0;
}

int qwizard_buttonText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        const QString & tmp = static_cast<QWizard*>(g_controls[id])->buttonText(static_cast<QWizard::WizardButton>(arg0));
        lua_pushstring(L, tmp.toLatin1().data());
        return 1;
    }
    return 0;
}

int qwizard_currentId(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QWizard*>(g_controls[id])->currentId();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qwizard_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QWizard*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qwizard_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) {
       error(__func__, 0);
    } else {
        return addWidget(new QWizard());
    }
    return 0;
}

int qwizard_next(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QWizard*>(g_controls[id])->next();
    }
    return 0;
}

int qwizard_reject(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QWizard*>(g_controls[id])->reject();
    }
    return 0;
}

int qwizard_removePage(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QWizard*>(g_controls[id])->removePage(arg0);
    }
    return 0;
}

int qwizard_restart(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        static_cast<QWizard*>(g_controls[id])->restart();
    }
    return 0;
}

int qwizard_setButton(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int arg1 = lua_tointeger(L, 3);
        static_cast<QWizard*>(g_controls[id])->setButton(
            static_cast<QWizard::WizardButton>(arg0),
            static_cast<QAbstractButton*>(g_controls[arg1]));
    }
    return 0;
}

int qwizard_setButtonText(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        const char* arg1 = lua_tostring(L, 3);
        static_cast<QWizard*>(g_controls[id])->setButtonText(
            static_cast<QWizard::WizardButton>(arg0),
            arg1);
    }
    return 0;
}

int qwizard_setDefaultProperty(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 4) {
       error(__func__, 4);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        const char* arg1 = lua_tostring(L, 3);
        const char* arg2 = lua_tostring(L, 4);
        static_cast<QWizard*>(g_controls[id])->setDefaultProperty(
            arg0,
            arg1,
            arg2);
    }
    return 0;
}

int qwizard_setField(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        const char* arg1 = lua_tostring(L, 3);
        static_cast<QWizard*>(g_controls[id])->setField(
            arg0,
            arg1);
    }
    return 0;
}

int qwizard_setOption(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 2 and argc > 3) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        bool arg1 = (argc >= 3) ? lua_toboolean(L, 3) : true;
        static_cast<QWizard*>(g_controls[id])->setOption(
            static_cast<QWizard::WizardOption>(arg0),
            arg1);
    }
    return 0;
}

int qwizard_setPage(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) {
       error(__func__, 3);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        int arg1 = lua_tointeger(L, 3);
        static_cast<QWizard*>(g_controls[id])->setPage(
            arg0,
            static_cast<QWizardPage*>(g_controls[arg1]));
    }
    return 0;
}

int qwizard_setSideWidget(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QWizard*>(g_controls[id])->setSideWidget(static_cast<QWidget*>(g_controls[arg0]));
    }
    return 0;
}

int qwizard_setStartId(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QWizard*>(g_controls[id])->setStartId(arg0);
    }
    return 0;
}

int qwizard_setSubTitleFormat(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QWizard*>(g_controls[id])->setSubTitleFormat(static_cast<Qt::TextFormat>(arg0));
    }
    return 0;
}

int qwizard_setTitleFormat(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QWizard*>(g_controls[id])->setTitleFormat(static_cast<Qt::TextFormat>(arg0));
    }
    return 0;
}

int qwizard_setVisible(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        bool arg0 = lua_toboolean(L, 2);
        static_cast<QWizard*>(g_controls[id])->setVisible(arg0);
    }
    return 0;
}

int qwizard_setWindowTitle(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QWizard*>(g_controls[id])->setWindowTitle(arg0);
    }
    return 0;
}

int qwizard_setWizardStyle(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QWizard*>(g_controls[id])->setWizardStyle(static_cast<QWizard::WizardStyle>(arg0));
    }
    return 0;
}

int qwizard_startId(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QWizard*>(g_controls[id])->startId();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qwizard_subTitleFormat(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QWizard*>(g_controls[id])->subTitleFormat();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qwizard_testOption(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QWizard*>(g_controls[id])->testOption(static_cast<QWizard::WizardOption>(arg0));
    }
    return 0;
}

int qwizard_titleFormat(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QWizard*>(g_controls[id])->titleFormat();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qwizard_validateCurrentPage(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        bool tmp = static_cast<QWizard*>(g_controls[id])->validateCurrentPage();
        lua_pushboolean(L, tmp);
        return 1;
    }
    return 0;
}

int qwizard_wizardStyle(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        int tmp = static_cast<QWizard*>(g_controls[id])->wizardStyle();
        lua_pushinteger(L, tmp);
        return 1;
    }
    return 0;
}

int qwizardpage_delete(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) {
       error(__func__, 1);
    } else {
        int id = lua_tointeger(L, 1);
        delete static_cast<QWizardPage*>(g_controls[id]);
        freeWidget(id);
    }
    return 0;
}

int qwizardpage_new(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) {
       error(__func__, 0);
    } else {
        return addWidget(new QWizardPage());
    }
    return 0;
}

int qwizardpage_setLayout(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        int arg0 = lua_tointeger(L, 2);
        static_cast<QWizardPage*>(g_controls[id])->setLayout(static_cast<QLayout*>(g_controls[arg0]));
    }
    return 0;
}

int qwizardpage_setSubTitle(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QWizardPage*>(g_controls[id])->setSubTitle(arg0);
    }
    return 0;
}

int qwizardpage_setTitle(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) {
       error(__func__, 2);
    } else {
        int id = lua_tointeger(L, 1);
        const char* arg0 = lua_tostring(L, 2);
        static_cast<QWizardPage*>(g_controls[id])->setTitle(arg0);
    }
    return 0;
}

