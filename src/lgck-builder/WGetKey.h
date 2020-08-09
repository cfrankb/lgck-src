#ifndef WGETKEY_H
#define WGETKEY_H

#include <QLineEdit>

class CWGetKey : public QLineEdit
{
    Q_OBJECT
public:
    explicit CWGetKey(QWidget *parent = 0);
    
protected:
    virtual void keyPressEvent ( QKeyEvent * e );

signals:
    
public slots:
    
};

#endif // WGETKEY_H
