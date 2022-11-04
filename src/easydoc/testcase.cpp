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
#include "testcase.h"
#include "FileWrap.h"

CStep::CStep()
{
    m_conditionCount = 0;
}

CStep::~CStep()
{

}

void CStep::read(CFileWrap & file, int version)
{
    Q_UNUSED(version);
    file >> m_name;
    file >> m_code;
    file.read(&m_conditionCount, sizeof(m_conditionCount));
    for (int i=0; i < m_conditionCount; ++i) {
        m_conditions[i].op = 0;
        m_conditions[i].type = 0;
        file.read(&m_conditions[i].argNum,4);
        file.read(&m_conditions[i].op,2);
        file.read(&m_conditions[i].type,2);
        file >> m_conditions[i].value;
    }
}

void CStep::write(CFileWrap & file)
{
    file << m_name;
    file << m_code;
    file.write(&m_conditionCount, sizeof(m_conditionCount));
    for (int i=0; i < m_conditionCount; ++i) {
        file.write(&m_conditions[i].argNum,4);
        file.write(&m_conditions[i].op,2);
        file.write(&m_conditions[i].type,2);
        file << m_conditions[i].value;
    }
}

CTestCase::CTestCase()
{
    m_count = 0;
}

CTestCase::~CTestCase()
{

}

void CTestCase::removeAt(int i)
{
    for (int j = i; j < m_count - 1; ++j) {
        m_steps[j] = m_steps[j + 1];
    }
    --m_count;
}

int CTestCase::getSize()
{
    return m_count;
}

void CTestCase::read(CFileWrap & file, int version)
{
    // step count
    file.read(&m_count, sizeof(m_count));
    // for each step
    for (int i=0; i< m_count; ++i) {
        m_steps[i].read(file, version);
    }
}

void CTestCase::write(CFileWrap & file)
{
    // step count
    file.write(&m_count, sizeof(m_count));
    // for each step
    for (int i=0; i< m_count; ++i) {
        m_steps[i].write(file);
    }
}
