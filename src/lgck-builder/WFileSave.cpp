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
    //const char oblFilter[] = "Object Blocks (*.obl *.obl5)";
    const char pngFilter[] = "PNG Images (*.png)";
    if (filter == tr(pngFilter)) {
        setDefaultSuffix("png");
    } else {
        setDefaultSuffix("obl");
    }
}
