#ifndef _PROTO_INDEX_H
#define _PROTO_INDEX_H

#include <string>
class CProtoArray;

class CProtoIndex
{

public:

    enum {
        FILTER_NONE = 0,
        FILTER_BACKGROUND = 1,
        FILTER_OBJECTS = 2,
        FILTER_PLAYER = 3,
        FILTER_MONSTER = 4,
        FILTER_BULLET = 5,
    };

    CProtoIndex(CProtoArray *parent, int custom = FILTER_NONE);
    ~CProtoIndex();

    void init();
    int findPos (int protoId);
    int findProto (int protoId);

    //int getAtIndex( int i );
    void removeIndex (int pos );
    void removeFromIndex (int protoId);
    int insert(int protoId);
    void resizeIndex(int newSize);
    int getSize();
    void forget();
    void debug();
    int operator [] (int i);
    static const char* getFilterName(int i);
    static int getFilterCount();

protected:

    bool isAccepted(int protoId);

    int *m_index;
    int m_size;
    int m_custom;
    CProtoArray * m_protoArray;

    static std::string m_arrFilters[];
};
#endif
