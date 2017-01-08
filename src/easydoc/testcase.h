#ifndef CTESTCASE_H
#define CTESTCASE_H

#include <QString>
class CFileWrap;

enum ConditionType:int {
   argCount,
   boolean,
   number,
   string,
   table
};

typedef struct {
  int argNum;
  int type;
  QString value;
} Condition;

class CStep
{
public:
    CStep();
    ~CStep();
    QString m_name;
    QString m_code;
    Condition m_conditions[8];
    int m_conditionCount;
    enum:int {
        MAX_CONDITIONS = 8
    };
    void read(CFileWrap & file, int version);
    void write(CFileWrap & file);
};

class CTestCase
{
public:
    CTestCase();
    ~CTestCase();

    enum:int {
        MAX_STEPS = 8
    };
    QString m_name;
    CStep m_steps[MAX_STEPS];
    int m_count;
    void removeAt(int i);
    int getSize();
    void read(CFileWrap & file, int version);
    void write(CFileWrap & file);
};

#endif // CTESTCASE_H
