/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
