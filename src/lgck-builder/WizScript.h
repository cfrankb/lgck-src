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
