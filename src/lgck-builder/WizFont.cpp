#include "WizFont.h"
#include "ui_WizFont.h"

#include <QPainter>
#include <QFontDatabase>
#include "Font.h"

char CWizFont::m_text[] = "abcdefghijklmnopqrstuvwxyz" \
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
        "0123456789!@#$%^&*()-_=+[]" \
        "{}:;'\"|\\,.<>/?~` ";

CWizFont::CWizFont(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::CWizFont)
{
    ui->setupUi(this);
    QFontDatabase database;
    foreach (const QString &family, database.families()) {
        ui->cbFonts->addItem(family);
    }
}

CWizFont::~CWizFont()
{
    delete ui;
}

void CWizFont::updateFont(const QString & fontName)
{
    QImage image;
    bool bold = false;//ui->cBold->isChecked();
    bool grid = false;//ui->cGrid->isChecked();
    bool white = true;//ui->cWhite->isChecked();
    createBitmap(image, fontName, bold, grid, white);
    ui->sImage->setPixmap(QPixmap::fromImage(image));
}

void CWizFont::createBitmap(QImage & image, QString fontName, bool bold, bool grid, bool white)
{
    // setup the bitmap
    int count = strlen(m_text);
    int GRID_YBASE = 0;
    //int FONTSIZE = 14;
    int MAXX = WIDTH / XSCALE;
    int MAXY = count/MAXX + 1;
    int HEIGHT = MAXY * YSCALE;

    // Setup colors
    QPen fontColor = QPen(QColor(Qt::white));
    QPen gridColor = QPen(QColor(Qt::red));
    unsigned int bgColor = 0xff << 24;
    if (white) {
        fontColor = QPen(QColor(Qt::blue));
        bgColor = 0xffffffff;
    }

    // get image and fill it with bgcolor
    image = QImage(QSize(WIDTH, HEIGHT), QImage::Format_RGB32);
    image.fill(bgColor);

    QPainter painter;
    painter.begin(&image);

    // Set font to the painter
    QFont font(fontName.toLatin1().data(), FONTSIZE);
    font.setBold(bold);
    painter.setFont(font);

    // Draw bitmap
    int i=0;
    for (int y=0; y < MAXY; ++y) {
        if (grid) {
            painter.setPen(gridColor);
            painter.drawLine(QPoint(XBASE, GRID_YBASE + y * YSCALE),
                             QPoint(XBASE + MAXX * XSCALE, GRID_YBASE + y * YSCALE));
        }
        if (!m_text[i]) {
            break;
        }
        for (int x=0; x < MAXX;++x) {
            if (!y && grid) {
                painter.setPen(gridColor);
                painter.drawLine(QPoint(XBASE + x*XSCALE, GRID_YBASE + 0 * YSCALE),
                                 QPoint(XBASE + x*XSCALE, GRID_YBASE + MAXY * YSCALE));
            }
            painter.setPen(fontColor);
            if (!m_text[i]) {
                break;
            }
            char s[2] = { m_text[i], 0 };
            painter.drawText(XBASE + x * XSCALE, YBASE + y*YSCALE, s);
            ++i;
        }
    }

    // close painter
    painter.end();
}

void CWizFont::on_cbFonts_currentIndexChanged(const QString &arg1)
{
    updateFont(arg1);
}

void CWizFont::importFont(CFont & font)
{
//  QString tmp = ui->cbFonts->currentText()+".fnt";
//  QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), tmp, tr(m_fileFilter));
 // if (fileName.isEmpty()) {
  //    return;
 // }
  QString fontName = ui->cbFonts->currentText();
  QImage image;
  bool bold = false;//ui->cBold->isChecked();
  createBitmap(image, fontName, bold, false, false);
  QSize size = image.size();
  unsigned int * pixels =  new unsigned int[size.width() * size.height()];
  memcpy(pixels, image.constBits(), size.width() * size.height() * sizeof(unsigned int));
  long w = size.width();
  long h = size.height();
  int MAXX = WIDTH / XSCALE;
  int MAXY = strlen(m_text)/MAXX + 1;
  int i =0;
  for (int yy=0; yy < MAXY; ++yy) {
      if (!m_text[i]) {
          break;
      }
      for (int xx=0; xx < MAXX;++xx) {
          int base = yy * YSCALE* WIDTH + xx * XSCALE;
          int maxx = 0;
          for (int y=0; y < YSCALE; ++y) {
              for (int x=0; x < XSCALE; ++x) {
                  unsigned int & ptr = pixels[base + x + y * WIDTH];
                  if (ptr & 0xffffff && m_text[i]) {
                      if (m_text[i]=='i'
                              && x > 15) {
                          continue;
                      }
                      if (m_text[i]==' ') {
                          maxx = XSCALE;
                          continue;
                      }
                      maxx = std::max(maxx, x + 1);
                  } else {
                      ptr = 0;
                  }
              }
          }
          CFont::Glyph glyph = {
              xx * XSCALE,
              yy * YSCALE,
              maxx,//std::min(maxx+2, (int)XSCALE),
              YSCALE
          };
          font.addGlyph(m_text[i], glyph);
          if (m_text[i]) {
              ++i;
          }
      }
  }
  font.setScale(FONTSIZE, XSCALE, YSCALE);
  font.setPixmap(m_text, pixels, w, h);
  /*QImage img2(
       reinterpret_cast<unsigned char *>(pixels),
       size.width(),
       size.height(),
       size.width() * sizeof(unsigned int),
       QImage::Format_RGB32);

  CFileWrap file;
  if (file.open(fileName.toLatin1().data(), "wb")) {
      font.write(file);
      file.close();
  }*/
 // img2.save("/home/cfrankb/2.png", "PNG");
  delete [] pixels;
}
