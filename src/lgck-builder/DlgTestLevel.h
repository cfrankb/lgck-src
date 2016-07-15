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

private:
    Ui::CDlgTestLevel *ui;
};

#endif // DLGTESTLEVEL_H
