#ifndef __TIMELIMIT_H__
#define __TIMELIMIT_H__

// '5초 내에 3회만 허용'과 같은 시도제한을 처리하는 클래스  
class CTimeLimit
{
private:
	DWORD	m_dwTick;
	int		m_nRequest;

public:
	CTimeLimit();
	CTimeLimit( int nLimit, DWORD dwPeriod );

	BOOL Check();  // 제한을 넘는지 검사  ( TRUE - 제한을 넘었다 )
	static void	SetLimitation( int nLimit, DWORD dwPeriod );
};


#endif // __TIMELIMIT_H__