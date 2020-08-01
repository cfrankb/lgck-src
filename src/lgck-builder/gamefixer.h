#ifndef CGAMEFIXER_H
#define CGAMEFIXER_H

#include <QObject>

class CGame;

enum Severity:uint8_t{
    Okay,
    Warning,
    Error,
   // Fatal
};

typedef struct{
    Severity severity;
    QString message;
    QString tip;
    QString url;
} DesignError;

class CGameFixer: QObject
{
public:
    CGameFixer();
    void setGame(CGame *game);
    void addSampleSprites();
    void troubleshoot();
    Severity severity() {
        return m_severity;
    }
    const char *getIcon(Severity severity, bool flip=true);
    const char *getIcon();
    QString getTooltip();
    QString getStatus();
    const std::vector<DesignError> errors();
    QString getText();
    bool ready() {
        return m_ready;
    }

protected:
    CGame *m_game;
    bool m_flip;
    bool m_ready;
    Severity m_severity;
    std::vector<DesignError> m_errors;
};

#endif // CGAMEFIXER_H
