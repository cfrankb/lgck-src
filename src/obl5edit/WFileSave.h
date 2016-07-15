#ifndef WFILESAVE_H
#define WFILESAVE_H

#include <QFileDialog>

class CWFileSave : public QFileDialog
{
    Q_OBJECT
public:
    //explicit CWFileSave(QObject *parent = 0);
    explicit CWFileSave(QWidget *parent = 0,
         const QString &caption = QString(),
         const QString &directory = QString(),
         const QString &filter = QString());
signals:
    
public slots:

private slots:
    void changeDefaultSuffix ( const QString & filter );
    
};

#endif // WFILESAVE_H
