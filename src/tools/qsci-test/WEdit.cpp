/*
    LGCK Builder GUI
    Copyright (C) 1999, 2012  Francois Blanchette

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

#include "WEdit.h"
#include <Qsci/qsciscintilla.h>
#include "Lexer.h"
#include <Qsci/qsciapis.h>
#include <QCompleter>
#include <QStringListModel>
#include <QKeyEvent>
#include <QScrollBar>
#include <QDebug>
#include <QAbstractItemView>
#include "../shared/stdafx.h"
//#include "../shared/FileWrap.h"

#define SCE_LUA_WORD4 15
#define SCE_LUA_WORD5 16
#define SCE_LUA_WORD6 17
#define SCE_LUA_WORD7 18
#define SCE_LUA_WORD8 19

CWEdit::CWEdit(QWidget *parent) :
    QsciScintilla(parent)
{
    QFont *font = new QFont("Courier", 10);
    setFont(*font);

    // Line numbers
    // conventionnaly, margin 0 is for line numbers
    setMarginsFont(*font);
    QFontMetrics fm = QFontMetrics(*font);
    setMarginWidth(0, fm.width( "0000" ) + 5);
    setMarginLineNumbers(0, true);

    //setEdgeMode(QsciScintilla::EdgeLine);
    //setEdgeColumn(80);
    //setEdgeColor(QColor("#FF0000"));

    //m_lexer = new QsciLexerLua (this);
    m_lexer = new CLexer(this);

    setBraceMatching(QsciScintilla::SloppyBraceMatch);
    setAutoCompletionFillupsEnabled(false);
    setIndentationGuidesBackgroundColor(QColor("#e6e6de"));
    setCaretLineBackgroundColor(QColor("#e6fff0"));
    setCaretLineVisible(true);
    setIndentationGuidesBackgroundColor(QColor("#e6e6de"));
    ensureLineVisible(1);
    setUtf8(true);
    //setWhitespaceVisibility(QsciScintilla::WsVisible);
    setLexer(m_lexer);
    setFocus();

    connect(this, SIGNAL(cursorPositionChanged(int,int)),this, SLOT(CursorAtIndex(int,int)))   ;

    // start autocompleter
    QString fct;
    /*
    CFileWrap file;
    if (file.open(":/res/functions.txt")) {
        int size = file.getSize();
        char tmp[ size + 1];
        tmp[size]=0;
        file.read(tmp, size);
        file.close();
        fct = tmp;
    }*/

    m_words = fct.split("\n");
    //QClipboard *baseram = QApplication::clipboard();
    m_completer = new QCompleter();
    m_listmodel = new QStringListModel(m_completer);
    m_listmodel->setStringList(m_words);
    m_completer->setModel(m_listmodel);
    m_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setWidget(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseSensitive);
    connect(m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
    SendScintilla(SCI_STYLESETFORE, SCE_LUA_WORD5, 0xff00ff);
    SendScintilla(SCI_SETTABWIDTH, 8);
    SendScintilla(SCI_SETTABINDENTS, false);
}

CWEdit::~CWEdit() {

}

void CWEdit::CursorAtIndex(int line ,int pos )
{
    m_line = line;
    m_pos = pos;
    setAutoCompletionFillupsEnabled(false);
    int newPos = SendScintilla(SCI_GETCURRENTPOS);
    m_posX = SendScintilla(SCI_POINTXFROMPOSITION, 0, newPos);
    m_posY = SendScintilla(SCI_POINTYFROMPOSITION, 0, newPos);
    m_currentWord = getWord(newPos);
}

void CWEdit::keyPressEvent(QKeyEvent *e)
{
    if (m_completer && m_completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Escape:
           m_currentWord = "";
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior

        case Qt::Key_Right:
            m_completer->popup()->hide();
            insertCompletion(m_completer->currentCompletion());
            return;
        break;

       default:
           //qDebug("key: %d", e->key());
           break;
       }

       if (m_currentWord.isEmpty()) {
           m_completer->popup()->hide();
           return;
       }
    }

    if (m_completer
            && (e->key()==Qt::Key_Up || e->key()==Qt::Key_Down
                || e->key()==Qt::Key_Left
                || e->key()==Qt::Key_Right
                || e->key()==Qt::Key_Enter
                || e->key()==Qt::Key_Return
                || e->key()==Qt::Key_Home
                || e->key()==Qt::Key_End
                || e->key()==Qt::Key_PageUp
                || e->key()==Qt::Key_PageDown
                )) {
        QsciScintilla::keyPressEvent(e);
        m_completer->popup()->hide();
        return;
    }

     const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
 //    if (!m_completer || (ctrlOrShift && m_currentWord.isEmpty())) {
  //      return;
   //  }

     bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
     if (!isShortcut && (hasModifier || m_currentWord.isEmpty())){//  || completionPrefix.length() < 2  )) {
         m_completer->popup()->hide();
     }

     if (m_completer && m_currentWord.isEmpty()) {
         m_completer->popup()->hide();
     }

     QString word = m_currentWord;
     int key = e->key();
     if (key >= Qt::Key_A && key <= Qt::Key_Z) {
         int mod = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
         if (mod == Qt::ShiftModifier) {
             word += char(key - Qt::Key_A + 'A');
         } else if (!mod) {
             word += char(key - Qt::Key_A + 'a');
         }
     }

     if (key==Qt::Key_Backspace && !word.isEmpty()) {
         word = word.mid(0, word.length()-1);
     }

     //qDebug() << word << " : " << m_completer->completionPrefix();
     m_currentWord =word;
     if (m_completer && m_completer->popup()->isVisible()
             && word != m_completer->completionPrefix()) {
         if (//!word.isEmpty()
                 word.length()>=3
                 && canImproveWord()) {
             m_completer->setCompletionPrefix(word);
             m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
         } else {
             m_completer->popup()->hide();
         }
     }

     QsciScintillaBase::keyPressEvent(e);
}

void CWEdit::keyReleaseEvent(QKeyEvent *e)
{     
    // qDebug() << m_currentWord << " : " << m_completer->completionPrefix();
    if (m_currentWord != m_completer->completionPrefix()) {
        m_completer->setCompletionPrefix(m_currentWord);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }

    int newPos = SendScintilla(SCI_GETCURRENTPOS);
    char ch = SendScintilla(SCI_GETCHARAT, newPos);
    int key = e->key();
    if (//!m_currentWord.isEmpty()
            (m_currentWord.length() >= 3)
            && canImproveWord()
            && (ch ==' ' || ch=='\t' || !ch || ch=='\n')
            && ((key >= Qt::Key_A && key <= Qt::Key_Z) || key==Qt::Key_Backspace)) {
        QRect cr = QRect (m_posX, m_posY ,20,20);
        cr.setWidth(m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());
//         qDebug() << "!### word " << m_currentWord;   /* word piece on cursor */

       m_completer->complete(cr); // popup it up!
   }
}

// Try and get a word from the text before the current position.  Return the
// word if one was found and adjust the position accordingly.

QString CWEdit::getWord(int &pos) const
{
    QString word;
    bool numeric = true;
    bool sc = false;
    char ch;

    while ((ch = getCharacter(pos)) != '\0')
    {
        if (!isWordCharacter(ch)
                && !(ch==':' && !sc))
        {
            ++pos;
            if (ch==':') {
                sc = true;
            }

            break;
        }

        if (ch < '0' || ch > '9')
            numeric = false;

        word.prepend(ch);
    }

    // We don't auto-complete numbers.
    if (numeric)
        word.truncate(0);

    return word;
}

// Get the "next" character (ie. the one before the current position) in the
// current line.  The character will be '\0' if there are no more.

char CWEdit::getCharacter(int &pos) const
{
    if (pos <= 0)
        return '\0';

    char ch = SendScintilla(SCI_GETCHARAT, --pos);

    // Don't go past the end of the previous line.
    if (ch == '\n' || ch == '\r')
    {
        ++pos;
        return '\0';
    }

    return ch;
}

void CWEdit::insertCompletion( QString completion )
{
    if (m_completer->widget() != this) {
        return;
    }

    int newPos = SendScintilla(SCI_GETCURRENTPOS);
    m_currentWord = getWord(newPos);
    int line = SendScintilla(SCI_LINEFROMPOSITION, newPos);
    int lineStart = SendScintilla(SCI_POSITIONFROMLINE, line);
    int cols = newPos - lineStart;
    int cole = cols + m_currentWord.length() ;
    //qDebug() << "### newpos: " << newPos;
    //qDebug() << "### selezione col: " << cols << "-" << cole;
    //qDebug() << "### word " << m_currentWord;
    setSelection(line,cols,line,cole);
    //QString QCompleter::currentCompletion () cons
//    QModelIndex QCompleter::currentIndex () const
  //  int QCompleter::currentRow () const

    completion += "( )";
    int extra = 0;
    if ( hasSelectedText() ) {
        cut();
        insert(completion);
        qDebug("aaa");
    } else {
         extra = completion.length() - m_currentWord.length();
         insert(completion.right(extra));
         qDebug("bbb");
    }

    setCursorPosition(line, cols - lineStart + completion.length() - 1);
}

bool CWEdit::canImproveWord()
{
    int len = m_currentWord.length();
    for (int i=0; i<m_words.count(); ++i) {
        QString & word = m_words[i];
    //    qDebug() << word.mid(1, len);
        if (word.length() > len &&
                word.mid(0, len) == m_currentWord) {
            return true;
        }
    }

    return false;
}
