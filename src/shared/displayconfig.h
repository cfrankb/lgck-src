#ifndef DISPLAYCONFIG_H
#define DISPLAYCONFIG_H


class CDisplay;
class IFile;

class CDisplayConfig
{
public:
    CDisplayConfig();
    ~CDisplayConfig();

    int indexOf(const char *name);
    CDisplay *find(const char *name);
    void removeAt(int i);
    CDisplay & add(CDisplay &display);
    int getSize();
    void forget();
    CDisplay & add(const char *name, int x, int y, int type);
    void reset();
    bool read(IFile & file);
    bool write(IFile & file);
    CDisplay *operator[](int i);
    CDisplay *operator[](const char *s);

protected:
    void resize();

    enum {
        GROWBY = 20,
        MAX = 32,
        NOT_FOUND = -1,
        VERSION = 2
    };

    int m_size;
    int m_max;
    CDisplay **m_displays;
};

#endif // DISPLAYCONFIG_H
