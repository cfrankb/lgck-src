#ifndef WIZFONT_H
#define WIZFONT_H

#include <QWizard>

class QString;
class CFont;

namespace Ui {
class CWizFont;
}

class CWizFont : public QWizard
{
    Q_OBJECT

public:
    explicit CWizFont(QWidget *parent = 0);
    ~CWizFont();
    void importFont(CFont & font,QString &name);

private slots:
    void on_cbFonts_currentIndexChanged(const QString &arg1);

private:
    Ui::CWizFont *ui;
    enum {
        XBASE = 0,
        YBASE = 16,
        XSCALE = 20,
        YSCALE = 22,
        WIDTH = 256,
        FONTSIZE = 14
    };
    void updateFont(const QString &);
    void createBitmap(QImage & image, QString fontName, bool bold, bool grid, bool white);
    static char m_text[];
};

#endif // WIZFONT_H
