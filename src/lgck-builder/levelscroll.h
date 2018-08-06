#ifndef CLEVELSCROLL_H
#define CLEVELSCROLL_H

#include <QAbstractScrollArea>
class CGame;

class CLevelScroll : public QAbstractScrollArea
{
    Q_OBJECT

public:
    CLevelScroll(QWidget *parent, CGame *game);
    void eventHandler();
    void editPath();
    void getGLInfo(QString & vendor,
                   QString & renderer,
                   QString & version,
                   QString & extensions);

protected:
    virtual void resizeEvent(QResizeEvent * event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

    void initMouse();
    void select(int x1, int y1, int x2, int y2);
    bool isGameMode() {
        return m_gameMode;
    }
    void keyReflector(const QKeyEvent *event, bool pressed);

    typedef struct {
        bool noScroll;
        bool drag;
        int x;
        int y;
        int oldX;
        int oldY;
        bool lButton;
        bool rButton;
        bool mButton;
        int orgX;
        int orgY;
        int destX;
        int destY;
    } Mouse;

    Mouse m_mouse;
    CGame *m_game;
    int m_zoom;
    bool m_editPath;
    int m_ticks;
    bool m_gameMode;

Q_SIGNALS:
    void pathEnded();
    void statusChanged(int, QString);
    void menuChanged();
    void setMX(int);
    void setMY(int);
    void viewSizeChanged(int len, int hei);
    void newLevelReq();
    void keyChanged(int, int);

protected slots:
    void setGameMode(bool gm);
    void getGameMode(bool & gm);
    void updateScrollbars(int w, int h);
    void enterEditPath();
    void leaveEditPath();
    void changeLevel(int i=0);
    void showScrollbars(bool);
    void mousePos(int &, int &, int &, int &);
    void mxChanged(int);
    void myChanged(int);
    void scrollStatus(int &, int &);
};

#endif // CLEVELSCROLL_H
