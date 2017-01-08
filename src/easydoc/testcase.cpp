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
    file >> m_name;
    file >> m_code;
    file.read(&m_conditionCount, sizeof(m_conditionCount));
    for (int i=0; i < m_conditionCount; ++i) {
        file.read(&m_conditions[i].argNum,4);
        file.read(&m_conditions[i].type,4);
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
        file.write(&m_conditions[i].type,4);
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
