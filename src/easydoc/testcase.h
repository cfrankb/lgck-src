/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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

enum ConditionOperator:int {
    equal,
    not_equal,
    greater,
    smaller,
    greater_equal,
    smaller_equal,
    nop
};

typedef struct {
  int argNum;
  int type;
  int op;
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
