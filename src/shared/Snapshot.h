#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <unordered_map>

class CScene;
class IFile;
class CGame;
class CLevel;
class CDisplayConfig;

class CSnapshot
{
public:

    CSnapshot();
    ~CSnapshot();

    void forget();
    void read(IFile &file);
    void write(IFile & file);
    bool has_snapshot();
    void take(CGame & game);
    bool reload(CGame & game);
    void clear();

protected:
    CScene *m_fw;
    CScene *m_bk;
    CLevel *m_layers;
    CDisplayConfig *m_displayConf;
    std::unordered_map<std::string, unsigned long long> m_vars;

    enum {
        VERSION = 0x0001
    };

    friend class CGame;
};

#endif // SNAPSHOT_H
