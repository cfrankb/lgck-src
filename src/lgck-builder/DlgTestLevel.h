#ifndef DLGTESTLEVEL_H
#define DLGTESTLEVEL_H

#include <QDialog>

namespace Ui {
    class CDlgTestLevel;
}

class CDlgTestLevel : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgTestLevel(QWidget *parent = 0);
    ~CDlgTestLevel();

    void setSkill(int skill);
    int getSkill();
    int getLives();
    int getScore();
    int getHP();
    bool getContinue();
    void setLives(int lives);
    void setScore(int score);
    void setHP(int hp);
    void setContinue(bool bContinue);
    bool isExternal();
    void setExternal(bool external);
    int getWidth();
    int getHeight();
    int getRez();
    void setRez(int v);

private:
    Ui::CDlgTestLevel *ui;
    typedef struct {
        int w;
        int h;
    } REZ;
    static REZ m_rez[];

};

#endif // DLGTESTLEVEL_H
