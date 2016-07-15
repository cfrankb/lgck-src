#ifndef INVENTORYTABLE_H
#define INVENTORYTABLE_H

class CInventory;
class IFile;

class CInventoryTable
{
public:
    CInventoryTable();
    ~CInventoryTable();
    CInventory * operator[] (int i);
    CInventory * operator[] (const char *s);
    enum {
        IT_DISCARDABLE,
        IT_PERSISTENT,
        IT_FREE
    };
    void reset(bool hard=false);
    int addInventory(const char *name, int options=IT_DISCARDABLE);
    int indexOf(const char *name) const;
    void read(IFile & file);
    void write(IFile & file);
protected:
    enum {
        GROW_BY = 10
    };
    int m_size;
    int m_max;
    CInventory *m_inventories;
    int findFree();
};

#endif // INVENTORYTABLE_H
