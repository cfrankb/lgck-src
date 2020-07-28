#include "WFileSave.h"

CWFileSave::CWFileSave(QWidget *parent ,
     const QString &caption,
     const QString &directory,
     const QString &filter):
    QFileDialog(parent, caption,directory,filter)
{

}

void CWFileSave::changeDefaultSuffix(const QString & filter)
{
    if (filter.contains("*.png")) {
        setDefaultSuffix("png");
    } else if (filter.contains("*.obl")) {
        setDefaultSuffix("obl");
    } else if (filter.contains("*.proto")) {
        setDefaultSuffix("proto");
    } else {
        setDefaultSuffix("");
    }
}
