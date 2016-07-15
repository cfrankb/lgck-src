#ifndef WHOTKEY_H
#define WHOTKEY_H

#include <QLineEdit>

class CWHotKey : public QLineEdit
{
    Q_OBJECT
public:
    explicit CWHotKey(QWidget *parent = 0);
    
protected:
    virtual void keyPressEvent ( QKeyEvent * e );

signals:
    
public slots:
    
};

#endif // WHOTKEY_H
