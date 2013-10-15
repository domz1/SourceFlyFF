#ifndef __TESTCASE_H
#define	__TESTCASE_H

class ITestCase
{
public:
	ITestCase();
	virtual	~ITestCase()	{};
	virtual	BOOL	Test()	= 0;
};

typedef	vector<ITestCase*>	VTC;
class CTestCaseMgr
{
private:
	CTestCaseMgr()	{}
public:
	virtual	~CTestCaseMgr();
	void	Clear();
	void	AddTest( ITestCase* pTestCase );
	static	CTestCaseMgr*	Instance();
	void	Test();
private:
	VTC	m_vTestCases;
};
#endif	// __TESTCASE_H