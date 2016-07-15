#ifndef DRAWLIST_H
#define DRAWLIST_H

#include <array>

typedef struct
{
    int x1;
    int y1;
    union {
        int x2;
        int frameSet;
    };
    union {
        int y2;
        int frameNo;
    };
    unsigned int rgb;
    bool fill;
} Drawable;

class CDrawList
{
public:
    CDrawList();
    ~CDrawList();

    void push(Drawable *item);
    Drawable *pop();
    void forget();

protected:
    std::array<Drawable *,0> m_array;

};

#endif // DRAWLIST_H
