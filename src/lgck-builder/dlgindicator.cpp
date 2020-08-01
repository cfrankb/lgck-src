#include "dlgindicator.h"
#include "ui_dlgindicator.h"

CDlgIndicator::CDlgIndicator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgIndicator)
{
    // https://doc.qt.io/qt-5/qtextbrowser.html
    // https://stackoverflow.com/questions/54501745/performantly-appending-rich-text-into-qtextedit-or-qtextbrowser-in-qt
    // https://stackoverflow.com/questions/22771725/how-to-have-a-qtextbrowser-to-display-contents-of-a-qtextedit
    // simple help system: https://www.informit.com/articles/article.aspx?p=1405554&seqNum=3
    // https://flylib.com/books/en/2.18.1/using_qtextbrowser_as_a_simple_help_engine.html
    ui->setupUi(this);
    QTextDocument *doc = ui->eTextBrowser->document();
    m_html = doc->toHtml();
}

CDlgIndicator::~CDlgIndicator()
{
    delete ui;
}

void CDlgIndicator::setText(QString text)
{
    ui->eTextBrowser->setOpenExternalLinks(true);
    QTextDocument *doc = ui->eTextBrowser->document();
    QStringList list = m_html.split("***");
    doc->setHtml(list[0] + text + list[1]);
    QTextCursor cursor = ui->eTextBrowser->textCursor();
    cursor.setPosition(0);
    ui->eTextBrowser->setTextCursor(cursor);
}
