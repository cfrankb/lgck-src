/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2011  Francois Blanchette

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

// Frame.cpp : implementation file
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <zlib.h>
#include "Frame.h"
#include "FrameSet.h"
#include "DotArray.h"
#include "CRC.h"
#include "IFile.h"
#include "FileWrap.h"
#include "helper.h"

/////////////////////////////////////////////////////////////////////////////
// CFrame

CFrame::~CFrame()
{
    forget();
}

/////////////////////////////////////////////////////////////////////////////
// CFrame message handlers

CFrame::CFrame(int p_nLen, int p_nHei)
{
    //qDebug("CFrame::CFrame(int, int)");
    m_bCustomMap = false;
    m_nLen = p_nLen;
    m_nHei = p_nHei;

    // generate blank bitmap
    m_rgb = new UINT32 [m_nLen * m_nHei ]  ;
    memset(m_rgb, 0, m_nLen * m_nHei * 4);

    // generate blank map
    m_map.resize(m_nLen, m_nHei);
    m_bCustomMap = false;

    m_undoFrames = NULL;
    m_undoSize = 0;
    m_undoPtr = 0;
}

CFrame::CFrame(CFrame* src)
{
     if (src) {
        m_nHei = src->m_nHei;
        m_nLen = src->m_nLen;
        m_rgb = new UINT32 [ m_nLen * m_nHei ];
        m_map.resize(src->m_nLen , src->m_nHei );
        m_bCustomMap = src->m_bCustomMap;

        if (src->m_rgb) {
            memcpy( m_rgb, src->m_rgb, m_nLen* m_nHei * sizeof(UINT32));
        } else {
            // generate blank bitmap
            memset(m_rgb, 0, m_nLen * m_nHei * sizeof(UINT32));
        }

        m_map = src->getMap();
    } else {
         init();
    }
    // create undo buffer
    m_undoFrames = NULL;
    m_undoSize = 0;
    m_undoPtr = 0;
}

CFrame & CFrame::operator = (const CFrame & src)
{
    m_nHei = src.m_nHei;
    m_nLen = src.m_nLen;
    m_rgb = new UINT32 [ m_nLen * m_nHei ];
    m_map = src.getMap();
    if (src.getRGB()) {
        memcpy( m_rgb, src.getRGB(), m_nLen* m_nHei * sizeof(UINT32));
    } else {
        memset(m_rgb, 0, m_nLen * m_nHei * sizeof(UINT32));
    }

    m_bCustomMap = src.m_bCustomMap;
    m_map = src.getMap();

    return *this;
}

void CFrame::init()
{
    m_nHei = 0;
    m_nLen = 0;
    m_bCustomMap = false;
    m_rgb = NULL;
}

void CFrame::forget()
{
    if (m_rgb != NULL) {
        delete [] m_rgb;
        m_rgb = NULL;
    }

    if (m_undoFrames) {
        for (int i=0; i < m_undoSize; ++i) {
            delete m_undoFrames[i];
        }
        delete [] m_undoFrames;
        m_undoFrames = NULL;
        m_undoSize = 0;
        m_undoPtr = 0;
    }

    m_nLen = 0;
    m_nHei = 0;
}

void CFrame::write(IFile & file)
{
    file.write(&m_nLen,4);
    file.write(&m_nHei,4);
    // OBL5 0x500 expects this field to be 0
    // as it is reserved for future use.
    // int filler = 0;
    file.write(&m_bCustomMap,4);

    if ((m_nLen>0) && (m_nHei>0)) {
        ULONG nDestLen;
        UINT8 *pDest;
        int err = compressData((UINT8 *)m_rgb, 4 * m_nLen * m_nHei, &pDest, nDestLen);
        if (err != Z_OK) {
            qDebug("CFrame::write error: %d", err);
        }

        file.write(&nDestLen,4);
        file.write(pDest, nDestLen);
        delete [] pDest;

        if (m_bCustomMap) {
            m_map.write(file);
        }
    }
}

bool CFrame::read(IFile &file, int version)
{
    // clear existing bitmap and map
    if (m_rgb) {
        delete [] m_rgb;
        m_rgb = NULL;
    }

    m_nLen = 0;
    m_nHei = 0;

    if (version == 0x500) {
        // read image size and customMap settings
        file.read (&m_nLen, sizeof (m_nLen));
        file.read (&m_nHei, sizeof (m_nHei));
        //    int filler; // OBL5 0x500 expects this field to be 0
        file.read (&m_bCustomMap, sizeof (m_bCustomMap));

        //qDebug("size = (%d, %d); filler= %d\n", m_nLen, m_nHei, filler);
        if ((m_nLen>0) && (m_nHei>0)) {
            // this image is not zero-length

            UINT32 nSrcLen;
            file.read(&nSrcLen, 4);

            UINT8 *pSrc = new UINT8 [ nSrcLen ];
            file.read(pSrc, nSrcLen);

            ULONG nDestLen = m_nLen * m_nHei * 4;

            // create a new bitmap
            m_rgb = new UINT32 [ m_nLen * m_nHei ] ;

            int err = uncompress(
                    (UINT8 *)m_rgb,
                    (ULONG *)& nDestLen,
                    (UINT8 *)pSrc,
                    (ULONG)nSrcLen);

            delete [] pSrc;

            if (err) {
                //qDebug("CFrame::Read err=%d\n", err);
                return false;
            }

            // create a new map
            m_map.resize(m_nLen, m_nHei );
            if (m_bCustomMap) {
                m_map.read(file);
            } else {
                updateMap();
            }
        }
    } else  {
        //qDebug("CFrame::read version (=%.4x) not supported\n", version);
    }

    return true;
}

void CFrame::toBmp(UINT8 * & bmp, int & totalSize)
{
    // BM was read separatly
    typedef struct {
        //UINT8 m_sig;        // "BM"
        UINT32 m_nTotalSize;// 3a 00 00 00
        UINT32 m_nZero;     // 00 00 00 00 ???
        UINT32 m_nDiff;     // 36 00 00 00 TotalSize - ImageSize
        UINT32 m_n28;       // 28 00 00 00 ???

        UINT32 m_nLen;      // 80 00 00 00
        UINT32 m_nHei;      // 80 00 00 00
        INT16 m_nPlanes;    // 01 00
        INT16 m_nBitCount;  // 18 00
        UINT32 m_nCompress; // 00 00 00 00

        UINT32 m_nImageSize;// c0 00 00 00
        UINT32 m_nXPix;     // 00 00 00 00 X pix/m
        UINT32 m_nYPix;     // 00 00 00 00 Y pix/m
        UINT32 m_nClrUsed;  // 00 00 00 00 ClrUsed

        UINT32 m_nClrImpt;  // 00 00 00 00 ClrImportant
    } USER_BMPHEADER;

    int pitch = m_nLen * 3;
    if (pitch % 4) {
        pitch = pitch - (pitch % 4) + 4;
    }

    totalSize = bmpDataOffset + pitch * m_nHei;

    bmp = new UINT8 [totalSize];
    bmp[0] = 'B';
    bmp[1] = 'M';

    USER_BMPHEADER & bmpHeader = * ((USER_BMPHEADER *) (bmp + 2));
    bmpHeader.m_nTotalSize = totalSize;
    bmpHeader.m_nZero = 0;
    bmpHeader.m_nDiff = bmpDataOffset;
    bmpHeader.m_n28   = bmpHeaderSize;

    bmpHeader.m_nLen = m_nLen;
    bmpHeader.m_nHei = m_nHei;
    bmpHeader.m_nPlanes = 1; // always 1
    bmpHeader.m_nBitCount = 24;
    bmpHeader.m_nCompress = 0; // 00 00 00 00

    bmpHeader.m_nImageSize = pitch * m_nHei;
    bmpHeader.m_nXPix = 0;     // 00 00 00 00 X pix/m
    bmpHeader.m_nYPix = 0;     // 00 00 00 00 Y pix/m
    bmpHeader.m_nClrUsed = 0;  // 00 00 00 00 ClrUsed
    bmpHeader.m_nClrImpt = 0;  // 00 00 00 00 ClrImportant

    for (int y=0; y < m_nHei; ++y) {
        for (int x=0; x < m_nLen; ++x) {
            UINT8 *s = (UINT8*) & point(x, m_nHei - y - 1);
            UINT8 *d = bmp + bmpDataOffset + x * 3 + y * pitch;
            d[0] = s[2];
            d[1] = s[1];
            d[2] = s[0];
        }
    }
}

UINT32 CFrame::toNet (const UINT32 a)
{
    UINT32 b;
    UINT8 *s = (UINT8*) &a;
    UINT8 *d = (UINT8*) &b;

    d[0] = s[3];
    d[1] = s[2];
    d[2] = s[1];
    d[3] = s[0];

    return b;
}

void CFrame::toPng(UINT8 * & png, int & totalSize, UINT8 *obl5data, int obl5size)
{
    CCRC crc;

    // compress the data ....................................
    int scanLine = m_nLen * 4;
    ULONG dataSize = (scanLine + 1) * m_nHei;
    UINT8 * data = new UINT8 [ dataSize ];
    for (int y=0; y < m_nHei; ++y) {
        UINT8 * d = data + y * (scanLine + 1);
        *d = 0;
        memcpy(d + 1, m_rgb + y * m_nLen, scanLine);
    }

    UINT8 * cData;
    ULONG cDataSize;
    int err = compressData(data, dataSize, &cData, cDataSize);
    if (err != Z_OK) {
        qDebug("CFrame::toPng error: %d", err);
    }

    delete [] data;

    int cDataBlocks = cDataSize / pngChunkLimit;
    if (cDataSize % pngChunkLimit) {
        cDataBlocks ++;
    }

    totalSize = pngHeaderSize + png_IHDR_Size + 4 + cDataSize + 12 * cDataBlocks
                + sizeof(png_IEND)
                + obl5size;

    png = new UINT8 [ totalSize ];
    UINT8 *t = png;

    // png signature ---------------------------------------
    UINT8 sig [] = { 137, 80, 78, 71, 13, 10, 26, 10 };
    memcpy(t, sig, 8); t += 8;

    UINT32 crc32;

    // png_IHDR ---------------------------------------------
    png_IHDR & ihdr = * ((png_IHDR *) t);
    ihdr.Lenght = toNet(png_IHDR_Size - 8);
    memcpy(ihdr.ChunkType, "IHDR", 4);
    ihdr.Width = toNet(m_nLen);
    ihdr.Height = toNet(m_nHei);
    ihdr.BitDepth = 8;
    ihdr.ColorType = 6;
    ihdr.Compression = 0; //deflated
    ihdr.Filter = 0;
    ihdr.Interlace = 0;
    //ihdr.CRC = 0;
    t += png_IHDR_Size;
    crc32 = toNet( crc.crc( ((UINT8 *)&ihdr) + 4, png_IHDR_Size - 4) );
    memcpy (t, &crc32, 4); t+= 4;

    // png_IDAT ....................................................
    UINT32 cDataOffset = 0;
    UINT32 cDataLeft = cDataSize;
    do {
        int chunkSize;
        if (cDataLeft > pngChunkLimit) {
            chunkSize = pngChunkLimit;
        } else {
            chunkSize = cDataLeft;
        }

        UINT32 cDataSizeNet = toNet(chunkSize);
        memcpy (t, &cDataSizeNet, 4); t+= 4;
        UINT8 *chunkData = t;
        memcpy (t, "IDAT", 4); t+= 4;
        memcpy (t, cData + cDataOffset, chunkSize); t+= chunkSize;
        crc32 = toNet ( crc.crc(chunkData, chunkSize + 4) );
        memcpy (t, &crc32, 4); t+= 4;

        cDataOffset += chunkSize;
        cDataLeft -= chunkSize;
    } while (cDataLeft);

    // png_obLT
    if (obl5size) {
        memcpy(t, obl5data, obl5size);
        png_OBL5 & obl5 = * ((png_OBL5 *) t);
        obl5.Length = toNet(obl5size-12);        
        UINT32 iCrc = toNet ( crc.crc( t +4, obl5size-8) );
        memcpy(t + obl5size-4, &iCrc, 4);
        t += obl5size;
    }

    // png_IEND .................................................
    png_IEND & iend = * ((png_IEND *) t);
    iend.Lenght = 0;
    memcpy(iend.ChunkType, "IEND", 4);
    iend.CRC = toNet ( crc.crc( (UINT8*)"IEND", 4) );
    t += sizeof(png_IEND);

    delete [] cData;
}

void CFrame::resize( int len, int hei )
{
    CFrame *newFrame = new CFrame(len, hei);

    //Copy the original frame
    for (int y=0; y < std::min(hei, m_nHei); ++y) {
        for (int x=0; x < std::min(len, m_nLen); ++x) {
            newFrame->at(x,y) = at(x,y);
        }
    }

    delete [] m_rgb;
    m_rgb = newFrame->getRGB();
    newFrame->detach();
    delete newFrame;

    m_nLen = len;
    m_nHei = hei;

    // TODO: actually resize the map
    m_map.resize(m_nLen,  m_nHei );
}

void CFrame::setTransparency( UINT32 color )
{
    color &= COLOR_MASK;
    for (int i=0; i < m_nLen * m_nHei; ++i) {
        if ( (m_rgb[i] & COLOR_MASK) == color ) {
            m_rgb[i] = 0;
        }
    }
}

void CFrame::setTopPixelAsTranparency()
{
    setTransparency(m_rgb[0]);
}

void CFrame::updateMap()
{
    m_map.resize(m_nLen, m_nHei);
    int threhold = CSS3Map::GRID * CSS3Map::GRID / 4;
    for (int y = 0; y < m_map.height() ; ++y) {
        for (int x = 0; x < m_map.length(); ++x) {
            int data = 0;
            for (int i = 0; i < CSS3Map::GRID; ++i) {
                for (int j = 0; j < CSS3Map::GRID; ++j) {
                    data += ((point(x * CSS3Map::GRID + i, y * CSS3Map::GRID+ j)) & ALPHA_MASK) !=0;
                }
            }
            if (data >= threhold) {
                m_map.at(x, y) = 0xff;

            } else {
                m_map.at(x, y) = 0;
            }
        }
    }
}

void CFrame::outputDebug(const char *s)
{
    CFileWrap file;
    if (file.open("../debug/frame", "ab" )) {
        file.write(s, strlen(s));
        file.close();
    }
}

bool CFrame::hasTransparency() const
{
    for (int i=0; i < m_nLen * m_nHei; ++i) {
        if (! (m_rgb[i] & ALPHA_MASK) ) {
            return true;
        }
    }

    return false;
}

CFrameSet *CFrame::split(int pxSize, bool whole)
{
    CFrameSet *frameSet = new CFrameSet();
    for (int y = 0; y < m_nHei; y += pxSize) {
        for (int x = 0; x < m_nLen; x += pxSize) {
            CFrame *frame;
            if (whole) {
                frame = new CFrame(pxSize, pxSize);
            } else {
                frame = new CFrame(std::min(pxSize, m_nLen - x), std::min(pxSize, m_nHei - y));
            }
            UINT32 *ni_rgb = frame->getRGB();
            for (int z = 0; z < std::min(pxSize, m_nHei - y); ++z ) {
                memcpy(& ni_rgb[z * frame->m_nLen],//pxSize],
                       & m_rgb[x + (y + z) * m_nLen],
                       std::min(pxSize, m_nLen - x) * sizeof(UINT32)
                       );
            }
            frameSet->add(frame);
            // TODO: copy the actual map
            frame->updateMap();
        }
    }
    return frameSet;
}

void CFrame::debug()
{
    char s[1024];
    sprintf(s,"[frame]%d x %d\n\n", m_map.height(), m_map.length());
    outputDebug(s);

    for (int y = 0; y < m_map.height() ; ++y) {
        memset(s, 0, 1024);

        for (int x = 0; x < m_map.length(); ++x) {
            if (m_map.at(x,y)) {
                s[x] = 'X';
            } else {
                s[x] = '.';
            }
        }

        strcat(s, "\n");
        outputDebug(s);
    }
    outputDebug("\n");
}

const UINT32* CFrame::dosPal()
{
    // original color palette
    static const UINT32 colors[] =    {
        0xff000000,0xffab0303,0xff03ab03,0xffabab03,0xff0303ab,0xffab03ab,0xff0357ab,0xffababab,
        0xff575757,0xffff5757,0xff57ff57,0xffffff57,0xff5757ff,0xffff57ff,0xff57ffff,0xffffffff,
        0xff000000,0xff171717,0xff232323,0xff2f2f2f,0xff3b3b3b,0xff474747,0xff535353,0xff636363,
        0xff737373,0xff838383,0xff939393,0xffa3a3a3,0xffb7b7b7,0xffcbcbcb,0xffe3e3e3,0xffffffff,
        0xffff0303,0xffff0343,0xffff037f,0xffff03bf,0xffff03ff,0xffbf03ff,0xff7f03ff,0xff4303ff,
        0xff0303ff,0xff0343ff,0xff037fff,0xff03bfff,0xff03ffff,0xff03ffbf,0xff03ff7f,0xff03ff43,
        0xff03ff03,0xff43ff03,0xff7fff03,0xffbfff03,0xffffff03,0xffffbf03,0xffff7f03,0xffff4303,
        0xffff7f7f,0xffff7f9f,0xffff7fbf,0xffff7fdf,0xffff7fff,0xffdf7fff,0xffbf7fff,0xff9f7fff,
        0xff7f7fff,0xff7f9fff,0xff7fbfff,0xff7fdfff,0xff7fffff,0xff7fffdf,0xff7fffbf,0xff7fff9f,
        0xff7fff7f,0xff9fff7f,0xffbfff7f,0xffdfff7f,0xffffff7f,0xffffdf7f,0xffffbf7f,0xffff9f7f,
        0xffffb7b7,0xffffb7c7,0xffffb7db,0xffffb7eb,0xffffb7ff,0xffebb7ff,0xffdbb7ff,0xffc7b7ff,
        0xffb7b7ff,0xffb7c7ff,0xffb7dbff,0xffb7ebff,0xffb7ffff,0xffb7ffeb,0xffb7ffdb,0xffb7ffc7,
        0xffb7ffb7,0xffc7ffb7,0xffdbffb7,0xffebffb7,0xffffffb7,0xffffebb7,0xffffdbb7,0xffffc7b7,
        0xff730303,0xff73031f,0xff73033b,0xff730357,0xff730373,0xff570373,0xff3b0373,0xff1f0373,
        0xff030373,0xff031f73,0xff033b73,0xff035773,0xff037373,0xff037357,0xff03733b,0xff03731f,
        0xff037303,0xff1f7303,0xff3b7303,0xff577303,0xff737303,0xff735703,0xff733b03,0xff731f03,
        0xff733b3b,0xff733b47,0xff733b57,0xff733b63,0xff733b73,0xff633b73,0xff573b73,0xff473b73,
        0xff3b3b73,0xff3b4773,0xff3b5773,0xff3b6373,0xff3b7373,0xff3b7363,0xff3b7357,0xff3b7347,
        0xff3b733b,0xff47733b,0xff57733b,0xff63733b,0xff73733b,0xff73633b,0xff73573b,0xff73473b,
        0xff735353,0xff73535b,0xff735363,0xff73536b,0xff735373,0xff6b5373,0xff635373,0xff5b5373,
        0xff535373,0xff535b73,0xff536373,0xff536b73,0xff537373,0xff53736b,0xff537363,0xff53735b,
        0xff537353,0xff5b7353,0xff637353,0xff6b7353,0xff737353,0xff736b53,0xff736353,0xff735b53,
        0xff430303,0xff430313,0xff430323,0xff430333,0xff430343,0xff330343,0xff230343,0xff130343,
        0xff030343,0xff031343,0xff032343,0xff033343,0xff034343,0xff034333,0xff034323,0xff034313,
        0xff034303,0xff134303,0xff234303,0xff334303,0xff434303,0xff433303,0xff432303,0xff431303,
        0xff432323,0xff43232b,0xff432333,0xff43233b,0xff432343,0xff3b2343,0xff332343,0xff2b2343,
        0xff232343,0xff232b43,0xff233343,0xff233b43,0xff234343,0xff23433b,0xff234333,0xff23432b,
        0xff234323,0xff2b4323,0xff334323,0xff3b4323,0xff434323,0xff433b23,0xff433323,0xff432b23,
        0xff432f2f,0xff432f33,0xff432f37,0xff432f3f,0xff432f43,0xff3f2f43,0xff372f43,0xff332f43,
        0xff2f2f43,0xff2f3343,0xff2f3743,0xff2f3f43,0xff2f4343,0xff2f433f,0xff2f4337,0xff2f4333,
        0xff2f432f,0xff33432f,0xff37432f,0xff3f432f,0xff43432f,0xff433f2f,0xff43372f,0xff43332f,
        0xff000000,0xff000000,0xff000000,0xff000000,0xff000000,0xff000000,0xff000000,0xff000000
    };

    return colors;
}

bool CFrame::draw(CDotArray *dots, int size, int mode)
{
    bool changed = false;
    for (int i=0; i < (*dots).getSize(); ++i) {
        Dot & dot = (*dots)[i];
        for (int y=0; y < size; ++y) {
            for (int x=0; x < size; ++x)  {
                if (dot.x + x < m_nLen
                        && dot.y + y < m_nHei) {
                    switch(mode) {
                    case MODE_NORMAL:
                        if (at(dot.x +x, dot.y + y) != dot.color) {
                            at(dot.x +x, dot.y + y) = dot.color;
                            changed = true;
                        }
                        break;

                    case MODE_COLOR_ONLY:
                        if ( (at(dot.x +x, dot.y + y) & 0xffffff) != (dot.color & 0xffffff)) {
                            UINT8 *p = (UINT8*) & at(dot.x +x, dot.y + y);
                            UINT8 a = p[3];
                            at(dot.x +x, dot.y + y) = dot.color;
                            p[3] = a;
                            changed = true;
                        }
                        break;

                    case MODE_ALPHA_ONLY:
                        if (alphaAt(dot.x +x, dot.y + y) != (dot.color >> 24)) {
                            UINT8 *p = (UINT8*) & at(dot.x +x, dot.y + y);
                            p[3] = dot.color >> 24;
                            changed = true;
                        }
                        break;
                    }

                }
            }
        }
    }
    return changed;
}

void CFrame::save(CDotArray * dots, CDotArray * dotsOrg, int size)
{
    for (int i=0; i < (*dots).getSize(); ++i) {
        Dot & dot = (*dots)[i];
        for (int y=0; y < size; ++y) {
            for (int x=0; x < size; ++x)  {
                if (dot.x + x < m_nLen
                        && dot.y + y < m_nHei) {
                    dotsOrg->add(at(dot.x +x, dot.y + y), dot.x + x, dot.y + y);
                }
            }
        }
    }
}

void CFrame::floodFill(int x, int y, UINT32 bOldColor, UINT32 bNewColor)
{
    if (!isValid(x,y)) {
        return;
    }

    int ex = x;
    for (; (x>=0) && at(x,y)== bOldColor; --x) {
        at(x,y) = bNewColor;
        if ((y>0) && (at (x, y-1) == bOldColor)) {
            floodFill (x, y-1, bOldColor, bNewColor);
        }

        if ((y<m_nHei-1) && (at (x,y+1) ==bOldColor)) {
            floodFill (x,y+1, bOldColor, bNewColor);
        }
    }

    x = ++ex;
    if (!isValid(x,y)) {
        return;
    }

    for (; (x<m_nLen) && at(x,y)== bOldColor; ++x) {
        at(x,y) = bNewColor;
        if ((y>0) && (at (x,y-1) == bOldColor)) {
            floodFill (x,y-1, bOldColor, bNewColor);
        }

        if ((y<m_nHei-1) && (at (x,y+1) ==bOldColor)) {
            floodFill (x, y+1, bOldColor, bNewColor);
        }
    }
}

void CFrame::floodFillAlpha(int x, int y, UINT8 oldAlpha, UINT8 newAlpha)
{
    if (!isValid(x,y)) {
        return;
    }

    int ex = x;
    for (; (x>=0) && alphaAt(x,y)== oldAlpha; --x) {
        UINT8 * p = (UINT8*)& point(x,y);
        p[3] = newAlpha;
        if ((y>0) && (alphaAt (x, y-1) == oldAlpha)) {
            floodFillAlpha (x, y-1, oldAlpha, newAlpha);
        }

        if ((y<m_nHei-1) && (alphaAt (x,y+1) == oldAlpha)) {
            floodFillAlpha (x,y+1, oldAlpha, newAlpha);
        }
    }

    x = ++ex;
    if (!isValid(x,y)) {
        return;
    }

    for (; (x<m_nLen) && alphaAt(x,y)== oldAlpha; ++x) {
        UINT8 * p = (UINT8*)& point(x,y);
        p[3] = newAlpha;
        if ((y>0) && (alphaAt (x, y-1) == oldAlpha)) {
            floodFillAlpha(x,y-1, oldAlpha, newAlpha);
        }

        if ((y<m_nHei-1) && (alphaAt (x,y+1) == oldAlpha)) {
            floodFillAlpha (x, y+1, oldAlpha, newAlpha);
        }
    }
}


CFrame* CFrame::clip(int mx, int my, int cx, int cy)
{
    int maxLen = m_nLen - mx;
    int maxHei = m_nHei - my;

    if (cx == -1) {
        cx = maxLen;
    }

    if (cy == -1) {
        cy = maxHei;
    }

    // check if out of bound
    if (cx < 1 || cy < 1 ) {
        cx = cy = 0;
    }

    // create clipped frame
    CFrame *t = new CFrame(cx, cy);

    // copy clipped region
    for (int y=0; y < cy; ++y) {
        for (int x=0; x < cx; ++x)  {
            t->at(x,y)= at(mx+x, my+y);
        }
    }

    // return new frame
    return t;
}

void CFrame::clear()
{
    for (int y=0; y < m_nHei; ++y) {
        for (int x=0; x < m_nLen; ++x)  {
            at(x,y) = 0;
        }
    }

    // TODO: updatemap
}

void CFrame::flipV()
{
    for (int y=0; y < m_nHei / 2; ++y) {
        for (int x=0; x < m_nLen; ++x) {
            UINT32 c = at(x,y);
            at(x,y) = at(x, m_nHei - y - 1) ;
            at(x, m_nHei - y - 1) = c;
        }
    }
    // TODO: updatemap
}

void CFrame::flipH()
{
    for (int y=0; y < m_nHei; ++y) {
        for (int x=0; x < m_nLen / 2; ++x) {
            UINT32 c = at(x,y);
            at(x,y) = at(m_nLen - x -1, y) ;
            at(m_nLen - x -1, y) = c;
        }
    }
    // TODO: updatemap
}

void CFrame::rotate()
{
    CFrame *newFrame = new CFrame(m_nHei, m_nLen);
    for (int y=0; y<m_nHei; ++y) {
        for (int x=0; x<m_nLen; ++x) {
            newFrame->at(newFrame->m_nLen - y -1 ,x) = at(x,y);
        }
    }

    m_nLen = newFrame->m_nLen;
    m_nHei = newFrame->m_nHei;
    delete [] m_rgb;
    m_rgb = newFrame->getRGB();

    newFrame->detach();
    delete newFrame;

    // TODO: updatemap
}

void CFrame::spreadH()
{
    UINT32 *rgb = new UINT32[m_nLen * m_nHei * 2];

    for (int y=0; y < m_nHei; ++y) {
        memcpy(rgb + y * m_nLen * 2 ,
               & at(0,y),
               m_nLen * sizeof(UINT32) );

        memcpy(rgb + y * m_nLen * 2 + m_nLen,
               & at(0,y),
               m_nLen * sizeof(UINT32) );
    }


    m_nLen *=2;
    delete [] m_rgb;
    m_rgb = rgb;

    // TODO: updatemap
}

void CFrame::spreadV()
{
    UINT32 *rgb = new UINT32[m_nLen * m_nHei * 2];

    for (int y=0; y < m_nHei; ++y) {
        memcpy(rgb + y * m_nLen  ,
               & at(0,y),
               m_nLen * sizeof(UINT32) );

        memcpy(rgb + y * m_nLen + m_nHei * m_nLen,
               & at(0,y),
               m_nLen * sizeof(UINT32) );
    }

    m_nHei *=2;
    delete [] m_rgb;
    m_rgb = rgb;

    // TODO: updatemap
}

void CFrame::shrink()
{
    CFrame *newFrame = new CFrame(m_nLen / 2, m_nHei / 2);
    for(int y=0; y < m_nHei / 2; ++y ) {
        for(int x=0; x < m_nLen / 2; ++x ) {
            newFrame->at(x, y) = at(x * 2,y * 2);
        }
    }

    // why wasn't the old rgb deleted?
    if (m_rgb) {
        delete m_rgb;
    }
    m_rgb = newFrame->getRGB();
    newFrame->detach();
    delete newFrame;

    m_nLen /= 2;
    m_nHei /= 2;

    // TODO: actually resize the map
    m_map.resize(m_nLen, m_nHei );

    // TODO: fix this temp updateMap
    updateMap();
}

const CSS3Map & CFrame::getMap() const
{
    return m_map;
}

void CFrame::enlarge()
{
    CFrame *newFrame = new CFrame(m_nLen * 2, m_nHei * 2);
    for(int y=0; y < m_nHei; ++y ) {
        for(int x=0; x < m_nLen; ++x ) {
            UINT32 c = at(x,y);
            newFrame->at(x * 2, y * 2) = c;
            newFrame->at(x * 2 +1 , y * 2) = c;
            newFrame->at(x * 2, y * 2 + 1) = c;
            newFrame->at(x * 2 + 1, y * 2 + 1) = c;
        }
    }

    m_rgb = newFrame->getRGB();
    newFrame->detach();
    delete newFrame;

    m_nLen *= 2;
    m_nHei *= 2;

    // TODO: actually resize the map
    m_map.resize(m_nLen, m_nHei );

    // TODO: fix this temp updateMap
    updateMap();
}

void CFrame::shiftUP()
{
    UINT32 t[m_nLen];

    // copy first line to buffer
    memcpy(t, m_rgb, sizeof(UINT32) * m_nLen);

    // shift
    for (int y=0; y < m_nHei -1; ++y) {
        memcpy( & at(0, y), & at(0, y + 1), m_nLen * sizeof(UINT32));
    }

    // copy first line to last
    memcpy(& at(0, m_nHei - 1), t, sizeof(UINT32) * m_nLen);
}

void CFrame::shiftDOWN()
{
    UINT32 t[m_nLen];

    // copy first line to buffer
    memcpy(t, & at(0, m_nHei -1), sizeof(UINT32) * m_nLen);

    // shift
    for (int y=0; y < m_nHei -1; ++y) {
        memcpy( & at(0, m_nHei - y - 1), & at(0, m_nHei - y - 2), m_nLen * sizeof(UINT32));
    }

    // copy first line to last
    memcpy(m_rgb, t, sizeof(UINT32) * m_nLen);
}

void CFrame::shiftLEFT()
{
    for (int y=0; y < m_nHei; ++y) {
        UINT32 c = at(0, y);
        for (int x=0; x < m_nLen-1; ++x) {
            at(x,y) = at(x+1,y);
        }
        at(m_nLen-1, y) = c;
    }
}

void CFrame::shiftRIGHT()
{
    for (int y=0; y < m_nHei; ++y) {
        UINT32 c = at(m_nLen - 1, y);
        for (int x=0; x < m_nLen-1; ++x) {
            at(m_nLen - 1 - x,y) = at(m_nLen - 2 - x,y);
        }
        at(0, y) = c;
    }
}

bool CFrame::isEmpty() const
{
    for (int y=0; y < m_nHei; ++y) {
        for (int x=0; x < m_nLen; ++x) {
            if ((m_rgb[x + y * m_nLen] & 0xff000000)) {
                return false;
            }
        }
    }
    return true;
}

void CFrame::inverse()
{
    for (int y=0; y < m_nHei; ++y) {
        for (int x=0; x < m_nLen; ++x) {
            unsigned int & rgb = at(x,y);
            rgb = (~rgb & 0xffffff) + (rgb & 0xff000000);
        }
    }
}

void CFrame::copy(CFrame *frame)
{
    *this = *frame;
}

void CFrame::undo()
{
    if (m_undoPtr < m_undoSize) {
        CFrame *tmp = m_undoFrames[m_undoPtr];
        m_undoFrames[m_undoPtr] = new CFrame(this);
        copy(tmp);
        delete tmp;
        m_undoPtr ++;
    }
}

void CFrame::redo()
{
    if (m_undoPtr && m_undoSize) {
        CFrame *tmp = m_undoFrames[m_undoPtr-1];
        m_undoFrames[m_undoPtr-1] = new CFrame(this);
        copy(tmp);
        delete tmp;
        m_undoPtr --;
    }
}

void CFrame::push()
{
    if (!m_undoFrames) {
        m_undoFrames = new CFrame * [ MAX_UNDO ];
        m_undoPtr = 0;
        m_undoSize= 0;
    }

    if (m_undoPtr != 0)  {
        for (int i=0; i < m_undoPtr; ++i) {
            delete m_undoFrames[i];
        }

        for (int i = 0; i + m_undoPtr < m_undoSize; ++i) {
            m_undoFrames[i] = m_undoFrames[m_undoPtr + i];
        }
        m_undoSize -= m_undoPtr;
        m_undoPtr = 0;
    }

    if (m_undoSize == MAX_UNDO) {
        delete m_undoFrames[m_undoSize - 1];
    }

    // push everything back
    for (int i=std::min(m_undoSize, (int) MAX_UNDO-1); i > 0; --i) {
        m_undoFrames[i] = m_undoFrames[i-1];
    }

    if (m_undoSize < MAX_UNDO) {
        ++m_undoSize;
    }

    m_undoFrames[0] = new CFrame(this);
}

bool CFrame::canUndo()
{
    return m_undoFrames != NULL
            && m_undoPtr < m_undoSize;
}

bool CFrame::canRedo()
{
    return m_undoFrames != NULL
            && m_undoPtr>0 && m_undoSize;
}

void CFrame::shadow(int factor)
{
    for (int y=0; y < m_nHei; ++y) {
        for (int x=0; x < m_nLen; ++x) {
            unsigned int & rgb = at(x,y);
            rgb = (rgb & 0xffffff) + (rgb & 0xff000000) / factor;
        }
    }
}

void CFrame::fade(int factor)
{
    for (int y=0; y < m_nHei; ++y) {
        for (int x=0; x < m_nLen; ++x) {
            unsigned int & rgb = at(x,y);
            rgb = (rgb & 0xffffff) + ((((rgb & 0xff000000) >> 24) * factor / 255) << 24);
        }
    }
}

CFrameSet* CFrame::explode(int count, short *xx, short *yy, CFrameSet *set)
{
    if (!set) {
        set = new CFrameSet();
    }

    int mx=0;
    for (int i=0; i<count; ++i){
        CFrame *frame = clip(mx, 0, xx[i], yy[i]);
        frame->updateMap();
        set->add(frame);
        mx += xx[i];
    }
    return set;
}

void CFrame::abgr2argb()
{
    // swap blue/red
    for (int i=0; i < m_nLen * m_nHei;++i) {
        UINT32 t= (m_rgb[i] & 0xff00ff00);
        if (t & 0xff000000) {
            t+= ((m_rgb[i] & 0xff) << 16)
                    + ((m_rgb[i] & 0xff0000) >> 16);
        }
        m_rgb[i] = t;
    }
}

void CFrame::argb2arbg()
{
    // swap green/blue
    for (int i=0; i < m_nLen * m_nHei;++i) {
        UINT32 t= (m_rgb[i] & 0xff0000ff);
        if (t & 0xff000000) {
            t+= ((m_rgb[i] & 0xff00) << 8)
                    + ((m_rgb[i] & 0xff0000) >> 8);
        }
        m_rgb[i] = t;
    }
}

CFrame * CFrame::toAlphaGray(int mx, int my, int cx, int cy)
{
    CFrame *frame;

    if (mx != 0 || my!=0 || cx !=-1||cy !=-1) {
        frame = this->clip(mx,my,cx,cy);
    } else {
        frame = new CFrame(this);
    }

    for (int i=0; i < frame->m_nLen * frame->m_nHei;++i) {
        unsigned char *p = (unsigned char*)(frame->m_rgb + i);
        p[0] = p[3];
        p[1] = p[3];
        p[2] = p[3];
        p[3] = 0xff;
    }
    return frame;
}

const char* CFrame::getChunkType()
{
    return "obLT";
}

void CFrame::fill(unsigned int rgba)
{
    for (int i=0; i< m_nLen * m_nHei;++i){
        m_rgb[i] = rgba;
    }
}

/////////////////////////////////////////////////////////////////////
// CSS3Map

CSS3Map::CSS3Map()
{
    m_map = NULL;
    m_len = 0;
    m_hei = 0;
}

CSS3Map::CSS3Map(const int px, const int py)
{
    m_map = NULL;
    resize(px, py);
}

CSS3Map::~CSS3Map()
{
    if (m_map) {
        delete [] m_map;
    }
}

void CSS3Map::resize(const int px, const int py)
{
    if (m_map) {
        delete [] m_map;
    }

    m_len = px / GRID;
    m_hei = py / GRID;
    m_map = new char[m_len * m_hei];
    memset(m_map, 0, m_len * m_hei);
}

int CSS3Map::length() const
{
    return m_len;
}

int CSS3Map::height() const
{
    return m_hei;
}

void CSS3Map::read(IFile & file)
{
    // TODO: read map from disk
    file.read(m_map, m_len * m_hei);
}

void CSS3Map::write(IFile &file) const
{
    // TODO: write map to disk
    file.write(m_map, m_len * m_hei);
}

bool CSS3Map::isNULL() const
{
    return !m_map;
}

CSS3Map & CSS3Map::operator = (const CSS3Map & src)
{
    resize(src.length(), src.height());
    if (!src.isNULL()) {
        memcpy(m_map, src.getMap(), m_len * m_hei);
    }

    return *this;
}

char * CSS3Map::getMap() const
{
    return m_map;
}
