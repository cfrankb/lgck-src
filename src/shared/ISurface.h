#ifndef __ISURFACE_H
#define __ISURFACE_H

class CLevelEntry;

class ISurface
{
public:
    virtual ~ISurface(){}
    virtual CLevelEntry & atIndex (int n) const = 0;
    virtual int getSize() const = 0;
};

#endif
