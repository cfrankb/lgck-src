#ifndef WFILESAVE_H
#define WFILESAVE_H

#include <QFileDialog>

class CWFileDialog : public QFileDialog
{
    Q_OBJECT
public:
    explicit CWFileDialog(QWidget *parent = 0,
         const QString &caption = QString(),
         const QString &directory = QString(),
         const QString &filter = QString());
signals:
    
public slots:

private slots:
    void changeDefaultSuffix(const QString & filter);

};

#endif // WFILESAVE_H
