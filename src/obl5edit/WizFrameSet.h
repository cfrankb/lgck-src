#ifndef WIZFRAMESET_H
#define WIZFRAMESET_H

#include <QWizard>
class CFrameSet;

namespace Ui {
class CWizFrameSet;
}

class CWizFrameSet : public QWizard
{
    Q_OBJECT
    
public:
    explicit CWizFrameSet(QWidget *parent = 0);
    ~CWizFrameSet();
    
    void init(int i);
    const char * getName();
    CFrameSet * getFrameSet();

signals:

    void imagesAdded();
    void imageDeleted();
    void refill();

private slots:
    void on_btnAddImages_clicked();
    void on_btnDeleteImage_clicked();

    void on_treeWidget_itemSelectionChanged();
    void updateButtons();

private:
    Ui::CWizFrameSet *ui;
    CFrameSet *m_frameSet;
};

#endif // WIZFRAMESET_H
