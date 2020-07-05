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
        uint32_t Lenght;      // 4 UINT8s
        uint8_t ChunkType[4];
        uint32_t Width;       //: 4 uint8_ts
        uint32_t Height;      //: 4 uint8_ts
        uint8_t BitDepth;     //: 1 uint8_t
        uint8_t ColorType;    //: 1 uint8_t
        uint8_t Compression;  //: 1 uint8_t
        uint8_t Filter;       //: 1 uint8_t
        uint8_t Interlace;    //: 1 uint8_t
    } png_IHDR;

    static uint8_t PaethPredictor(uint8_t a, uint8_t b, uint8_t c);
    bool _8bpp(
            CFrame *& frame,
            uint8_t* cData,
            int cDataSize,
            const png_IHDR & ihdr,
            const uint8_t plte[][3],
            const bool trns_found,
            const uint8_t trns[],
            int offsetY);
    bool _4bpp(
            CFrame *& frame,
            uint8_t* cData,
            int cDataSize,
            const png_IHDR & ihdr,
            const uint8_t plte[][3],
            const bool trns_found,
            const uint8_t trns[],
            int offsetY);
};

#endif // PNGMAGIC_H
