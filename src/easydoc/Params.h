#ifndef PARAMS_H
#define PARAMS_H

#include <QString>
#include "stdafx.h"
#include "FileWrap.h"

class Param
{
public:
    Param(){}
    ~Param(){}

    Param & operator = (Param & s)
    {
        name = s.name;
        type = s.type;
        desc = s.desc;
        flags = s.flags;
        return *this;
    }

    QString name;
    QString type;
    QString desc;
    int flags;
} ;

class CParams
{
public:
    CParams();
    ~CParams();

    Param & operator [] (int i);
    const CParams & operator = (CParams & s);
    int add(Param & p);
    void removeAt(int i);
    int getSize();
    void forget();
    void read(CFileWrap & file, int version);
    void write(CFileWrap & file);
    void debug();

protected:
    enum {
       MAX_PARAMS = 32
    };
    int m_size;
    int m_max;
    Param *m_params;
};

#endif // PARAMS_H
