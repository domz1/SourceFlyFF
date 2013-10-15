#include "stdafx.h"
#include "testcase.h"

ITestCase::ITestCase()
{
	CTestCaseMgr::Instance()->AddTest( this );
}

CTestCaseMgr::~CTestCaseMgr()
{
	Clear();
}

void CTestCaseMgr::Clear()
{
	m_vTestCases.clear();
}

void CTestCaseMgr::AddTest( ITestCase* pTestCase )
{
	m_vTestCases.push_back( pTestCase );
}

CTestCaseMgr* CTestCaseMgr::Instance()
{
	static CTestCaseMgr sTestCaseMgr;
	return &sTestCaseMgr;
}

void CTestCaseMgr::Test()
{
	BOOL bResult	= TRUE;
	for( VTC::iterator i = m_vTestCases.begin(); i != m_vTestCases.end(); ++i )
		(*i)->Test();
}
