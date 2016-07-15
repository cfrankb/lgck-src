#ifndef EDOCPARSER_H
#define EDOCPARSER_H

class CEdocParser
{
public:
    CEdocParser();
    enum {
        ED_NONE,
        ED_HEAD,
        ED_CLASS,
        ED_FUNCTION
    };
};

#endif // EDOCPARSER_H
