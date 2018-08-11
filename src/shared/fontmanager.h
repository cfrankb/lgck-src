#ifndef FONTMANAGER_H
#define FONTMANAGER_H

class IFile;
class CFont;
#include <string>
#include "ISerial.h"

#define DEFAULT_FONT "Arial"

class CFontManager: public ISerial
{
public:
    CFontManager();
    ~CFontManager();

    int indexOf(const char *name);
    CFont *find(const char *name);
    void removeAt(int i);
    CFont & add(CFont & font, const char * name);
    int getSize();
    void forget();
    void reset();
    virtual bool read(IFile & file);
    virtual bool write(IFile & file);
    CFont *operator[](int i);
    CFont *operator[](const char *s);

    enum {
        NOT_FOUND = -1
    };
protected:
    void resize();

    enum {
        GROWBY = 20,
        MAX = 32,
        VERSION = 2
    };

    typedef struct {
        CFont * font;
        std::string name;
    } FONT;

    void deleteFont(FONT *font);

    int m_size;
    int m_max;
    FONT **m_fonts;
};

#endif // FONTMANAGER_H
