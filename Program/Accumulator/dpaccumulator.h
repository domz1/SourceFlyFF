#ifndef __DPACCUMULATOR_H__
#define	__DPACCUMULATOR_H__

#include "dpmng.h"
#include "msghdr.h"

/*
#undef	theClass
#define theClass	CDPAccumulator
#undef theParameters
#define theParameters	CAr & ar
*/

class CAccumulatorDlg;
class CDPAccumulator : public CDPMng
{
private:
	CAccumulatorDlg*	m_pWnd;
public:
//	Constructions
	CDPAccumulator( CAccumulatorDlg* pWnd );
	virtual	~CDPAccumulator();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
// Operations
//	Handlers
//	USES_PFNENTRIES;
	void	OnErrorText( CAr & ar, DWORD dwLang );
};

#endif	// __DPACCUMULATOR_H__