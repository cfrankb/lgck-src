#ifndef PNGMAGIC_H
#define PNGMAGIC_H

class CFrame;
class CFrameSet;
class IFile;
typedef struct CFrame png_IHDR;

class CPngMagic
{
public:
    CPngMagic();
    bool parsePNG(CFrameSet & set, IFile &file);

protected:

    typedef struct {
        UINT32 Lenght;      // 4 UINT8s
        UINT8 ChunkType[4];
        UINT32 Width;       //: 4 UINT8s
        UINT32 Height;      //: 4 UINT8s
        UINT8 BitDepth;     //: 1 UINT8
        UINT8 ColorType;    //: 1 UINT8
        UINT8 Compression;  //: 1 UINT8
        UINT8 Filter;       //: 1 UINT8
        UINT8 Interlace;    //: 1 UINT8
    } png_IHDR;

    static UINT8 PaethPredictor(UINT8 a, UINT8 b, UINT8 c);
    bool _8bpp(
            CFrame *& frame,
            UINT8* cData,
            int cDataSize,
            const png_IHDR & ihdr,
            const UINT8 plte[][3],
            const bool trns_found,
            const UINT8 trns[],
            int offsetY);
    bool _4bpp(
            CFrame *& frame,
            UINT8* cData,
            int cDataSize,
            const png_IHDR & ihdr,
            const UINT8 plte[][3],
            const bool trns_found,
            const UINT8 trns[],
            int offsetY);
};

#endif // PNGMAGIC_H
