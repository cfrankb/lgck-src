#include "WFileSave.h"

CWFileSave::CWFileSave(QWidget *parent ,
     const QString &caption,
     const QString &directory,
     const QString &filter):
    QFileDialog(parent, caption,directory,filter)
{

}

void CWFileSave::changeDefaultSuffix ( const QString & filter )
{
    const QString pngFilter = tr("PNG Images (*.png)");
    if (filter == pngFilter) {
        setDefaultSuffix("png");
    } else {
        setDefaultSuffix("obl");
    }
}
