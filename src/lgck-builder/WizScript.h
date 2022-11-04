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
#ifndef WizScript_H
#define WizScript_H

typedef struct{
    const char *name;
    const char *path;
    const char *desc;
    const char *group;
    const char *sample;
    const char *code;
    const char *loader;
} WIZACTION;

#include <QWizard>
class CGameFile;
class QComboBox;

namespace Ui {
class CWizScript;
}

class CWizScript : public QWizard
{
    Q_OBJECT

public:
    explicit CWizScript(QWidget *parent = 0);
    ~CWizScript();
    virtual int nextId() const;
    enum {
        page_intro,
        page_finish,
        page_select,
        page_custom
    };

    void init(CGameFile *gf);
    std::string getScript();
    void initComboBox(QComboBox* combo, const char *type);

private slots:
    void on_cbAction_currentIndexChanged(int index);

protected:
    void loadScript(int id);
    void unloadScript();
    int is_custom() const;

private:
    Ui::CWizScript *ui;
    CGameFile *m_gameFile;
    static WIZACTION m_actions[];
    int m_index;
};

#endif // CWizScript_H
