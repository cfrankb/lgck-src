#ifndef __CSELECTION__
#define __CSELECTION__
class CLevelEntry;

class CSelection
{
public:
    CSelection();
    ~CSelection();

    CLevelEntry & operator[] (int i);
    void operator = (CSelection & src);
    void operator += (CSelection & src);
    void addEntry(const CLevelEntry & entry, int index=-1);
    void clear();
    bool isSingle();
    bool isMulti();
    int getSize();
    int getIndex(int i);
    void setIndex(int i, int index);
    void removeFromIndex(int index);
    void removeAt(int i);
    void removeProto(int proto);
    void applyDelta(int dx, int dy);
    void forget();

protected:

    enum {
        GROWBY = 10
    };

    int m_size;
    int m_max;
    CLevelEntry *m_entries;
    int *m_index;
};
 
#endif
