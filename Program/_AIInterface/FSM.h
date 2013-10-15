#if !defined(__FSM_H__)
#define __FSM_H__

// forward reference
class CFSMstate;

class CFSM  
{
private:
	map< int, CFSMstate* >	m_map;					

public:
	CFSM();										
	~CFSM();									

	CFSMstate *GetState( int nStateID );			
	BOOL AddState( CFSMstate* pState );				
	BOOL GetTransition( int nStateID, int nInput, int& nOutput );	
};

enum FSM_TYPE
{
	FSM_TYPE_0,
	FSM_TYPE_1,
	FSM_TYPE_2,
	FSM_TYPE_MAX
};

extern CFSM* GetFSM( DWORD type );

#endif // !defined(__FSM_H__)
