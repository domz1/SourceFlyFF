#ifndef __SMC_H
#define __SMC_H

#include "scanner.h"

 
#define NUM_FUNC 	  	10 
#define NUM_GLOBAL_VARS 10 
#define NUM_LOCAL_VARS  10 
#define NUM_BLOCK		10 
#define ID_LEN          31
#define FUNC_CALLS      31
#define NUM_PARAMS      31

#define SMC_VAL    0
#define SMC_DST_ID 1
#define SMC_SRC_ID 2
#define SMC_CALLER 3
#define SMC_FUNC   4
#define SMC_QUEST  5



//  These are the constants used to call sntx_err()
//  when a syntax error occurs. Add more if you like.
//  NOTE: SYNTAX is a generic error message used when nothing else
//  seems appropriate.
enum ErrorMsg
{
	SYNTAX,UNBAL_PARENS,NO_EXP,EQUALS_EXPECTED,NOT_VAR,PARAM_ERR,
	SEMI_EXPECTED,UNBAL_BRACES,FUNC_UNDEF,TYPE_EXPECTED,NEST_FUNC,
	RET_NOCALL,PAREN_EXPECTED,WHILE_EXPECTED,QUOTE_EXPECTED,
	NOT_TEMP,TOO_MANY_LVARS
};

//  An array of these structures will hold
//  the info associated with global variable.
struct VarType 
{
	char varName[64];
	int  varType;
	int  value;
};
//  this is the function call stack.
struct FuncType 
{
	char name[64];
	char *loc; // location of function entry point in file 
};


////////////////////////////////////////////////////////////////////////////////
// 
// Parsor : 계산기
//
class CParser : public CScanner
{
public:
	CParser() { }
	virtual ~CParser() { }

	virtual	void SntxErr( LPCTSTR lpszHeader, int error);

#if !defined(__REMOVE_SCIRPT_060712)

protected:
	void EvalExp1(int *value);
	void EvalExp2(int *value);
	void EvalExp3(int *value);
	void EvalExp4(int *value);
	void EvalExp5(int *value);
	void EvalExp6(int *value);

	// override
	virtual void EvalExp0(int *value);
	virtual void atom(int *value);

public:
	void Compute(LPTSTR initProg,int* value);
	void Compute(int* value);
#endif // not __REMOVE_SCIRPT_060712
};

struct LblType 
{
	char name[64];
	char *loc;
	int  block;
};
struct FuncInfo 
{
	int var;
	int label;
	int answer;
	int block;
};



#define MAX_GLOBAL_ARRAY 16
#define MAX_SCRIPT_TIMER 6 

class CTimer;
////////////////////////////////////////////////////////////////////////////////
//
// CScript : 인터프리터
//
class CScript : public CParser
{
public:
	CScript( BOOL bComma = FALSE ); 
	virtual ~CScript();

	enum  { INTERPRITER, COMPILER };

protected:
	BOOL		process;					// 0 : 인터프리트, 1 : 컴파일 
	BOOL		m_bMarkInDefine;
	CHAR*		m_pOldProg; 

	int			gVarIndex ; // index into global variable table
	VarType     globalVars[NUM_GLOBAL_VARS];
	int			funcIndex ; // index to into function table
	FuncType    funcTable[NUM_FUNC];
	char		cmd;

#if !defined(__REMOVE_SCIRPT_060712)
	VarType     localVarStack[NUM_LOCAL_VARS ];
	LblType     labelTable   [45			 ];
	FuncInfo    callStack    [NUM_FUNC	     ];
	int			functos   ; // index to top of function call stack
	int			answerCnt ; // answer 카운터
	int			lvartos   ; // index into local variable stack
	int			labeltos  ; // index into local label table
	int			retValue  ; // function return value
	int			retSign   ; // return sing
	int			blocktos  ; 
	int			labelIndex;
#endif

public:
	static map<string, int>		m_defines;
	static map<string, string>	m_mapString;

#if !defined(__REMOVE_SCIRPT_060712)
	CTimer*						m_paTimer;
	int*						m_panGlobalVal;
	int							m_anInternalVal[MAX_GLOBAL_ARRAY];
#endif

public:
	void		DeclGlobal();
	void		DeclLocal();
	void		FindLabel();
	void		GetParams();
	int			GetTkn( BOOL bComma = FALSE );
	char		LookUp(const char *s);
	int			InternalFunc(char *s);
	void		ExecDefine();
	char*		FindFunc(char *name);

#if !defined(__REMOVE_SCIRPT_060712)
	void		Write(LPSTR* cplProg,char* str);
	int			IsFunc(char *str);
	void		Compile();
	void		Call();
	void		GetArgs();
	void		FuncRet();
	void		LocalPush(VarType i);
	FuncInfo	FuncPop();
	void		FuncPush(FuncInfo fi);
	void		AssignVar(char *varName, int value);
	int*		GetVarPtr(char *varName);
	int			FindVar(char *s);
	int			IsVar(char *s);
	void		ExecIf();
	void		ExecWhile();
	void		ExecDo();
	void		ExecFor();
	void		ExecSwitch();
	void		ExecAnswer();
	char*		ExecSelect();
	char*		ExecYesNo();
	virtual	void	EvalExp0(int *value);
	virtual	void	atom(int *value);
	BOOL			Run();
	int				InterpBlock(int aaa = 0);
	void			Compute(int *value);
	void			FindEob();


	void			SetIValue(int nVal,...);
	int				GetReturn()  { return m_anInternalVal[SMC_VAL   ]; }
	int				GetDstId()   { return m_anInternalVal[SMC_DST_ID]; }
	int				GetSrcId()   { return m_anInternalVal[SMC_SRC_ID]; }
	int				GetCaller()  { return m_anInternalVal[SMC_CALLER]; }
	void*			GetFunc()    { return (void*)m_anInternalVal[SMC_FUNC]; }
	int				GetQuest()   { return m_anInternalVal[SMC_QUEST]; }
	int				GetNpcId()   { return GetDstId(); }
	int				GetPcId()    { return GetSrcId(); }
#endif // not __REMOVE_SCIRPT_060712

	static BOOL		LookupDefine( LPCTSTR lpszString, int& rValue );
	static void		GetFindIdToArray ( LPCTSTR strDef, CStringArray* pStrArray );
	static int		GetDefineNum( LPCTSTR lpStr );

	virtual void	SntxErr( LPCTSTR lpszHeader, int error);
	virtual void	SetMark();
	virtual void	GoMark();
	virtual int		GetLineNum( LPTSTR lpProg = NULL );

	BOOL			Load( LPCTSTR lpszFileName, BOOL bMultiByte = TRUE, int nProcess = INTERPRITER );
	void			Free();
	void			PreScan();

	void			SetCompilerMode() { process = COMPILER; }
	void			SetInterpriterMode() { process = INTERPRITER; }
	BOOL			IsCompilerMode() { return process == COMPILER; }
	BOOL			IsInterpriterMode() { return process == INTERPRITER; }
	int				GetProcessMode() { return process; }
	int				GetToken( BOOL bComma = FALSE );
	int				GetToken_NoDef( BOOL bComma = FALSE );
	void			GetLastFull();
	void			LoadString();
}; 

struct InterFuncType 
{
	char* name;
	int (*p)(CScript *SMC);
};

#endif
 
