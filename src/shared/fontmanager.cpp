#include "fontmanager.h"
#include "string.h"
#include "../shared/Font.h"
#include "../shared/FileWrap.h"
#include "stdafx.h"

CFontManager::CFontManager()
{
    m_size = 0;
    m_max = MAX;
    m_fonts = new FONT*[MAX];
    reset();
}

CFontManager::~CFontManager()
{
    forget();
    delete [] m_fonts;
}

int CFontManager::getSize()
{
    return m_size;
}

void CFontManager::deleteFont(FONT *font)
{
    delete font->font;
    delete font;
}

void CFontManager::forget()
{
    for (int i=0; i < m_size; ++i) {
        deleteFont(m_fonts[i]);
        m_fonts[i] = NULL;
    }
    m_size = 0;
}

int CFontManager::indexOf(const char *name)
{
    for (int i=0; i < m_size; ++i) {
        if (!strcmp(m_fonts[i]->name.c_str(), name)) {
            return i;
        }
    }
    return NOT_FOUND;
}

CFont * CFontManager::find(const char *name)
{
    int i = indexOf(name);
    if (i != NOT_FOUND) {
        return m_fonts[i]->font;
    } else {
        return NULL;
    }
}

void CFontManager::removeAt(int i)
{
    if (i >= 0 && i < m_size) {
        deleteFont(m_fonts[i]);
        for (int j = i + 1; j < m_size; ++j) {
            m_fonts[j-1] = m_fonts[j];
        }
        -- m_size;
    }
}

void CFontManager::resize()
{
    if (m_size == m_max) {
        m_max += GROWBY;
        FONT **tmp = new FONT*[m_max];
        memcpy(tmp, m_fonts, sizeof(FONT*) * m_size);
        delete [] m_fonts;
        m_fonts = tmp;
    }
}

CFont & CFontManager::add(CFont & font, const char * name)
{
    FONT *newFont;
    int i = indexOf(name);
    if (i == NOT_FOUND) {
        // create new display
        newFont = new FONT;
        newFont->name = name;
        newFont->font = new CFont;
        *(newFont)->font = font;
        i = m_size;
        m_fonts[i] = newFont;
        ++m_size;
        resize();
    } else {
        // reuse existing display
        *(m_fonts[i])->font = font;
    }
    return *(m_fonts[i]->font);
}

void CFontManager::reset()
{
    // clear everything add the engine default
    forget();

    const char *fontName = ":/res/Tuffy_bold.fnt";
    CFileWrap file;
    if (file.open(fontName)){
        CFont font;
        font.read(file);
        file.close();
        add(font, DEFAULT_FONT);
    } else {
        qDebug("can't read %s", fontName);
    }
}

bool CFontManager::read(IFile & file)
{
    reset();
    unsigned int file_version = 0;
    file.read(&file_version, sizeof(file_version));
    if(file_version != VERSION) {
        qDebug("incorrect version CFontManager");
        return false;
    }
    int size;
    file.read(&size, sizeof(size));
    for (int i=0; i < size; ++i) {
        std::string name;
        CFont font;
        file >> name;
        font.read(file);
        add(font, name.c_str());
    }
    return true;
}

bool CFontManager::write(IFile & file)
{
    unsigned int version = VERSION;
    int size = m_size - 1;
    file.write(&version, sizeof(version));
    file.write(&size, sizeof(m_size));
    // skip over the default font
    for (int i=1; i < m_size; ++i) {
        FONT *font = m_fonts[i];
        file << font->name;
        font->font->write(file);
    }
    return true;
}


CFont *CFontManager::operator[](int i)
{
    return at(i);
}

CFont *CFontManager::at(int i)
{
    if (i >= 0 && i < m_size) {
        return m_fonts[i]->font;
    } else {
        return NULL;
    }
}

CFont *CFontManager::operator[](const char *s)
{
    return find(s);
}

const char* CFontManager::nameAt(int i)
{
    return m_fonts[i]->name.c_str();
}

void CFontManager::setName(int i, const char *name)
{
    m_fonts[i]->name = name;
}
