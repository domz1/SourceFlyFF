
// Desc : console dialog
// Date : 20090714
// Author : gmpbigsun

#pragma  once 

#ifdef __BS_CONSOLE

#include "ConsoleCommon.h"

void _auto_start( );
void _update_console( );
void _update_auto_login( const DWORD dwDelta );

// console data
struct CON_DATA
{
	enum VALUE_TYPE 
	{
		VT_FUNCTION,
	//	VT_INT,
	//	VT_FLOAT,
	//	VT_BOOLEAN,
	};

	CON_DATA( const string& key, VALUE_TYPE vt, void* func, const string& exp ) 
	{ 
		_strKey = key;
		_strExplain = exp;
		_func.func = NULL; 
		switch( vt )
		{
		case VT_FUNCTION : _func.func = func; break;
	//	case VT_INT : _func.ival = *((int*)func);
	//	case VT_FLOAT: _func.fval = *((float*)func);
	//	case VT_BOOLEAN : _func.bTrue = *((bool*)func);
		}
		
	}

	VALUE_TYPE _eVt;
	string _strKey;
	string _strExplain;
	
	union VAL
	{
		void* func;
		int ival;
		bool bTrue;
		float fval;
	};

	VAL _func;

//	int GetInt( ) { if( _eVt == VT_INT ) { return _func.ival; } return 0; }
//	float GetFloat( ) { if( _eVt == VT_FLOAT ) { return _func.fval; } return 0.0f; }
//	bool GetBool( ) { if( _eVt == VT_BOOLEAN ) { return _func.bTrue; } return true; }

	void CallFunction( std::vector<string>& arguments )
	{
		if( _func.func )
			( ( void (*)( std::vector<string>& ) ) _func.func ) ( arguments );
	}												

};

typedef std::map< string, CON_DATA >	ConDataConationer;
typedef ConDataConationer::iterator		ConDataIter;


void ConsoleRegister( );
void CONSOLE_REGISTER( const string& key, CON_DATA::VALUE_TYPE eType, void* func, const string& exp );

// Console Dialog
INT_PTR CALLBACK DlgConsoleProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

class DlgConsole : public con::Singleton< DlgConsole >
{
public:
	DlgConsole( );
	virtual ~DlgConsole( );
	
	void Init( HWND hWnd );
	void Show( );
	void Hide( );
	void ToggleShow( );
	void AddString( const char* str );
	int Log( char* fmt, ... );
	void Reset( );
	
	HWND GetHWnd( )			{ return _hWnd; }
	HWND GetParentHWnd( )	{ return _hWndParent; }

	void Parsing( const char* str );
	CON_DATA* FindData( const string& key );
	void AddData( const string& key, CON_DATA::VALUE_TYPE eType, void* func, const string& exp );

	void ShowAllCommand( );
	void Update( );
	
protected:
	bool _bShow;
	HWND _hWnd;
	HWND _hWndParent;

	ConDataConationer _cConDatas;

public:
	float _fSpeedMul;
	DWORD _dwHitter;

	//auto login
	CString _strAccount;
	CString _StrPassword;
	int _nServer;
	int _nCharacter;
	int _nPasswordReal;
	int _nPasswordFigure[ 4 ];

	//
	BOOL _bRandomMoving;		//무작위 이동
	BOOL _bTestToggle;
};

inline DlgConsole* gConsole( ) { return DlgConsole::GetSingletonPtr( ); }

#endif