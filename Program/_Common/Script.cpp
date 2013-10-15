/*************************************************************
 *													         *	
 *  정수만이 가능한 Recursive descent parser		         *	
 *  변수의 사용이 가능하고 함수를 호출할 수 있으며,			 *
 *  goto 명령 등 Little C에서 제공하지 않는 기능을 제공한다. *
 *                            								 *			        
 *  by Travis nam			                       			 *
 *												             *			
 *************************************************************/

#include "stdafx.h"


// key word table
struct Command 
{
	const char* command;
	Toke tok;
};


// raiders.fix.2006.11.21
Command _table[] = 
{ 
	" "       ,NONE_    ,
	" "       ,ARG     ,
	"int"     ,VAR     , 
	"if"      ,IF      , 
	"else"    ,ELSE    , 
	"for"     ,FOR     , 
	"do"      ,DO      , 
	"while"   ,WHILE   , 
	"break"   ,BREAK   ,
	"switch"  ,SWITCH  , 
	"answer"  ,ANSWER  ,
	"Select"  ,SELECT  ,
	"YesNo"   ,YESNO   ,
	"case"    ,CASE    , 
	"default" ,DEFAULT ,
	"goto"    ,GOTO    , 
	"return"  ,RETURN  , 
	" "       ,EOL     , 
	"#define" ,DEFINE  , 
	"#include",INCLUDE , 
	"enum"	  ,ENUM    , 
	" "       ,FINISHED,
	0         ,END      // 테이블의 끝을 표시
};



void CParser::SntxErr( LPCTSTR lpszHeader, int error)
{
	throw "fuck";
}

////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////
map<string, int> CScript::m_defines;
map<string, string>	CScript::m_mapString;

#if defined( __WORLDSERVER ) 
	#if defined(__REMOVE_SCIRPT_060712)
		InterFuncType interFunc[] = { 0, 0, 0 };
	#else
		extern InterFuncType interFunc[];
	#endif	
#else
	InterFuncType interFunc[] = { 0, 0, 0 };
#endif



CScript::CScript( BOOL bComma )
{
	process       = INTERPRITER;
	m_pOldProg    = 0;
	gVarIndex     = 0; // index into global variable table
	funcIndex     = 0; // index to into function table
	cmd           = 0;
	m_bComma      = bComma;

#if !defined(__REMOVE_SCIRPT_060712)
	functos       = 0; // index to top of function call stack
	lvartos       = 0; // index into local variable stack
	labeltos      = 0; // index into local label table
	retValue      = 0; // function return value
	blocktos      = 0; //
	answerCnt     = 0;
	labelIndex    =-1;
	retSign	      = 0;
	m_paTimer = NULL;
	ZeroMemory( m_anInternalVal, sizeof( m_anInternalVal ) );
#endif // !defined(__REMOVE_SCIRPT_060712)
}

CScript::~CScript()
{
	Free();
}

void CScript::LoadString( void )
{
	CScanner::GetToken();
	while( tok != FINISHED )
	{
		CString str	= Token;
		if( str.Find( "IDS", 0 ) != 0 )
		{
			Error( "LoadString: %s", m_strFileName );
			GetToken();
			continue;
		}
		GetLastFull();
		
		bool bResult = m_mapString.insert( map<string, string>::value_type( (LPCTSTR)str, (LPCTSTR)Token ) ).second;
		if( !bResult )
			Error( "string error: %s", str );

		CScanner::GetToken();
	}
}

void CScript::SntxErr( LPCTSTR lpszHeader, int error)
{
	if( m_bErrorCheck == FALSE )
		return;
	SetMark();
	PutBack();
	BYTE str[80];
	int nLen = strlen(m_pProg);
	if(nLen > 79) nLen = 79;
	for(int i = 0; i < nLen; i++)
		str[i] = m_pProg[i];
	str[i] = 0; 
	CString string; 
	if( lpszHeader )
		string.Format( "FileName %s(%d) : %s : %s", m_strFileName, GetLineNum(), lpszHeader, str );
	else
		string.Format( "FileName %s(%d) : %s", m_strFileName, GetLineNum(), str );

	Error( string );
	GoMark();
}
int CScript::GetLineNum( LPTSTR lpProg )
{
	if( m_pOldProg == NULL )
		return CScanner::GetLineNum();
	return CScanner::GetLineNum( m_pOldProg );
}
void CScript::GetLastFull()
{
	CScanner::GetLastFull();	
	cmd = 0;
	
	if( *token == 0 && m_pOldProg ) 
	{
		m_pProg = m_pOldProg; 
		m_pOldProg = 0;
		GetLastFull();	
	}
}
int CScript::GetTkn( BOOL bComma )
{
	CScanner::GetToken( bComma ); 
	cmd = 0;

	if( tokenType == DELIMITER ) 
	{
		if( *token == '{' || *token == '}' ) // 블럭 처리
			tokenType = BLOCK;
	}
	else
	if( tokenType == TEMP ) 
	{
		if( tok = LookUp( token ) ) 
			tokenType = KEYWORD;
		else 
			tokenType = IDENTIFIER;
	}
	return tokenType;
}
int CScript::GetToken( BOOL bComma )
{
	GetTkn( bComma );

	if( tokenType == IDENTIFIER ) 
	{
		// 인터프리터 모드에서만 실행 
		if( IsInterpriterMode() )
		{
			map<string, string>::iterator i = CScript::m_mapString.find( m_mszToken );
			if( i != CScript::m_mapString.end() )
			{
				tokenType = STRING;
				lstrcpy( m_mszToken, i->second.c_str() );
				Token	= m_mszToken;
			}
			else
			{
				int nValue = 0;
				if( LookupDefine( token, nValue ) )
				{
					tokenType = NUMBER;
					wsprintf( m_mszToken, "%d", nValue );
					Token = m_mszToken;
				}
				else
				{
					if( m_dwDef == 1 )
					{
						// 프로퍼티 때문에. =을 -1로 사용하는 것은 잠정적으로 문제가 있음.
						if( *token != '\0' && *token != '=' && *token != '-' && *token != '+' ) 
						{
							CString string;
							string.Format( "%s Not Found.", token );
							SntxErr( string, 0 );
						}
					}
					else if( m_dwDef == 2 )
					{
						// 키워드도 아니고, 내장함수도 아니고, 유저함수도 아니라면 도대체 뭐지?
						if( !( tok = LookUp( token ) ) && InternalFunc( token ) == -1 && FindFunc( token ) == FALSE && *token != '#' )
						{
							CString string;
							string.Format( "%s Not Found.", token );
							SntxErr( string, 0 );
						}
					}
				}
			}
		}
	}
	else if( tok == FINISHED && m_pOldProg ) 
	{
		m_pProg = m_pOldProg; 
		m_pOldProg = 0;
		return GetToken( bComma );
	}
	return tokenType;
}

int CScript::GetToken_NoDef( BOOL bComma )
{
	GetTkn( bComma );
	if( tok == FINISHED && m_pOldProg ) 
	{
		m_pProg = m_pOldProg; 
		m_pOldProg = 0;
		return GetTkn( bComma );
	}
	return tokenType;
}

void CScript::SetMark()
{
	CScanner::SetMark();
	// Is in Define?
	if( m_pOldProg )
		m_bMarkInDefine = TRUE;
	else
		m_bMarkInDefine = FALSE;
}
void CScript::GoMark()
{
	// define 
	if( m_bMarkInDefine == TRUE )
	{
		if( m_pOldProg == NULL )
		{
			m_pOldProg = m_pProg; 
		}
	}
	else
	{
		if( m_pOldProg )
		{
			m_pOldProg = NULL;
		}
	}
	CScanner::GoMark();
}

void CScript::DeclGlobal()
{
	GetToken(); // get type 

	globalVars[gVarIndex].varType = tok;
	globalVars[gVarIndex].value = 0; // init to 0 

	do { // process comma-separated list 
		GetToken(); // get name 
		strcpy(globalVars[gVarIndex].varName, token);
		GetToken();
		gVarIndex++;
	} while(*token==',');
	if(*token!=';') SntxErr( NULL, SEMI_EXPECTED);
}

void CScript::PreScan()
{
	int brace = 0; 
	funcIndex = 0;
	char* p = m_pProg;
	GetTkn();

	if( !strcmp(token,"#") )
	{
		process = COMPILER;
	}
	else 
	{
		process = INTERPRITER;
		m_pProg = p;
	}
	
	do 
	{
		if( brace ) 			
			return;	// bypass code inside functions 

		GetTkn();

		if(tok==DEFINE) 
			ExecDefine();
		else if(tok==ENUM) 
		{
			int eNum = 0; 
			GetTkn(); // {
			do 
			{
				GetTkn();
				GetTkn(); //  , or =
				if(*token=='=') 
				{
					GetTkn(); // num
					eNum = atoi(token);
					GetTkn(); // ,
				}
				
				if( m_defines.insert( make_pair( Token, eNum ) ).second == false )
					TRACE("Enum %s dup.\n", Token);
				eNum++;
			} 
			while(*token!='}');

			GetTkn(); // ;
		}
		else if(tok==INCLUDE) 
		{
			GetTkn(); 
			CString strFile( token );
			CHAR* pOldProg = m_pProg;
			CHAR* pOldBuf = m_pBuf;
			int nOldSize = m_nProgSize;
			m_pProg = m_pBuf = 0;
			CScanner::Load( strFile );
			PreScan(); 
			CScanner::Free();
			m_nProgSize = nOldSize;
			m_pProg = pOldProg;
			m_pBuf = pOldBuf; 
			tok = 0;
		}
		else if(tok==VAR) 
		{ 
			// is global var 
			PutBack(); 
			DeclGlobal();
		}
		else if(tokenType==IDENTIFIER) 
		{
			CString strTemp( token ); 

			GetTkn();
			if(*token=='(') 
			{ 
				// must be assume a function 
				funcTable[funcIndex].loc = m_pProg;
				strcpy(funcTable[funcIndex++].name,strTemp);
				while(*m_pProg!=')') 
					m_pProg++;	
				m_pProg++;
				// m_pProg points to opening cyrly brace of function 
			}
			else 
				PutBack();
		}
		else if(*token=='{') 
			brace++;
	} 
	while( tok != FINISHED );

	m_pProg = p;
}


void CScript::ExecDefine()
{
	CScanner::GetToken( FALSE );
	string str = token;
	CScanner::GetToken( FALSE );
	
	int n;
	if( tokenType == NUMBER )
		n = atoi( token );
	else if( tokenType == HEX )
		sscanf( token, "%x", &n );
	else if( tokenType == 0 )
	{
		PutBack();
		return;
	}
	else
		return;

	if( m_defines.insert( make_pair( str, n ) ).second == false )
		TRACE("define %s dup\n", Token);
}


char CScript::LookUp(const char *s)
{
	if(!process) // 인터프리터 
	{
	#ifdef __REMOVE_SCIRPT_060712
		if( memcmp( s, "#define", 7 ) == 0 )
			return DEFINE;
	#else
		for(int i=0; i<22; ++i)
			if(!strcmp(_table[i].command,s)) 
				return cmd = _table[i].tok;
	#endif // __REMOVE_SCIRPT_060712
	}
	else if(s[0]=='@')  // 컴파일
		return _table[atoi(&s[1])].tok;
	return 0;
}

int CScript::InternalFunc(char *s)
{
	if(!process) // 인터프리터
	{
		for(int i = 0; interFunc[i].p; i++)
			if(!strcmp(interFunc[i].name,s))
				return i;
	}
	else // 컴파일 
	if(s[0]=='$') 
		return atoi(&s[1]);
	return -1;
}

char *CScript::FindFunc(char *name)
{
	int i;
	for(i = 0; i < funcIndex; i++)
		if(!strcmp(name, funcTable[i].name))
			return funcTable[i].loc;
	return NULL;
}



void CScript::Free()
{
	CScanner::Free(); 
	process = INTERPRITER;
	m_pOldProg       = 0;
#if !defined(__REMOVE_SCIRPT_060712)
	functos       = 0; // index to top of function call stack
	funcIndex     = 0; // index to into function table
	gVarIndex     = 0; // index into global variable table
	lvartos       = 0; // index into local variable stack
	labeltos      = 0; // index into local label table
	retValue      = 0; // function return value
	blocktos      = 0; //
	answerCnt     = 0;
	labelIndex    =-1;
	retSign	      = 0;
#endif // !defined(__REMOVE_SCIRPT_060712)
}
BOOL CScript::Load( LPCTSTR lpszFileName, BOOL bMultiByte, int nProcess)
{
	if( !CScanner::Load( lpszFileName, bMultiByte ) ) 
		return 0;

#if defined(__REMOVE_SCIRPT_060712)
	process = nProcess;
#else
	if(nProcess == INTERPRITER)
	{
		process = nProcess;
		return TRUE;
	}
	Compile();
#endif

	return TRUE;
}


BOOL CScript::LookupDefine( LPCTSTR lpszString, int& rValue )
{
	map< string, int >::iterator it = m_defines.find( lpszString );
	if( it != m_defines.end() )
	{
		rValue = it->second;
		return TRUE;
	}
	else
		return FALSE;
}

//
// remark 
//   lpId라는 define의 내용이 숫자라면 숫자를 돌려준다.
// 
// return 
//   그 Define에 Number를 돌려준다. 다른 경우 -1
//  
int CScript::GetDefineNum(LPCTSTR lpId)
{
	int nValue;
	if(LookupDefine(lpId,nValue) == FALSE)
		return -1;
	return nValue;
}

//
// XX_로 시작하는 것을 찾아준다.
//
void CScript::GetFindIdToArray(LPCTSTR lpStrDef,CStringArray* pStrArray)
{
	string strValue; 

	map<string, int>::iterator it;
	for( it = m_defines.begin(); it != m_defines.end(); ++it )
	{
		strValue = it->first;
		if( strValue.find( lpStrDef ) == 0 )	// 인덱스 0에서 찾았으면 결과배열에 넣는다.
			pStrArray->Add( strValue.c_str() );
	}
}


#if !defined(__REMOVE_SCIRPT_060712)
void CScript::Write(LPSTR* cplProg,char* str)
{
	while(*str) 
		*((*cplProg)++) = *(str++);
}

void CScript::Compile()
{
	int r = 0; 
	char str[256]; 
	LPSTR cplProg  = new CHAR[m_nProgSize];
	LPSTR pBakProg = cplProg;
	Write(&cplProg,"# ");
	SetInterpriterMode();
	do 
	{
		GetToken();
		switch(tokenType) 
		{
			case STRING:
				Write(&cplProg,"\"");
				Write(&cplProg,token);
				Write(&cplProg,"\"");
				break;
			case IDENTIFIER:
				// 인터널 펑션만 컴파일한다.
				if((r=IsFunc(token))!=-1) 
				{
					str[0] = '$';	
					itoa(r,&str[1],10);
					Write(&cplProg,str);
				}
				else 
					Write(&cplProg,token);
				break;
			case KEYWORD:
				switch(cmd) 
				{
					case DEFINE: // define
					case INCLUDE: // include
						while(*m_pProg!='\r'&&*m_pProg!='\0') 
							m_pProg++;
						break;
					case ENUM: // enum
						GetTkn(); // {
						do 
						{
							GetTkn();
						} while(*token!='}');
						GetTkn(); // ;
						break;
					default:
						str[0] = '@';	itoa(cmd,&str[1],10);
						Write(&cplProg,str);
						break;
				}

				switch(cmd) 
				{
				case ELSE: case DO: case VAR: case RETURN: case GOTO: case CASE: 
					Write(&cplProg," "); 
					break;
				}
				break;
			default:
				Write(&cplProg,token);
				break;
		}
	} while(tok!=FINISHED);
	CScanner::Free();	
	*cplProg = 0; 
	cplProg++;

	m_pProg = new CHAR[(int)(cplProg-pBakProg)];
	memcpy(m_pProg,pBakProg,(int)(cplProg-pBakProg));
	safe_delete( pBakProg );
	m_bMemFlag = 0; 
	m_pBuf = m_pProg; 
	SetCompilerMode();
}
int CScript::IsFunc(char *str)
{
	for(int i = 0; interFunc[i].p; i++)
		if(!strcmp(interFunc[i].name,str))
			return i;
	return -1; // unknown command 
}

#endif

#if !defined(__REMOVE_SCIRPT_060712)

void CParser::Compute(LPTSTR initProg,int *value)
{
	SetProg(initProg);
	Compute(value);
}
void CParser::Compute(int *value)
{
	GetToken();
}

void CParser::EvalExp0(int *value)
{
	EvalExp1(value);
}
void CParser::EvalExp1(int *value)
{
	int partial_value; char op,op2;
	EvalExp2(value); op = *token; op2 = *(token+1);
	while(((op = *token) == '&' && (op2 = *(token+1)) == '&') || (op == '|' && op2 == '|')) 
	{
		GetToken();	EvalExp2(&partial_value);
		switch(op) 
		{
			case '&': *value = *value && partial_value; break;
			case '|': *value = *value || partial_value; break;
		}
	}
}
void CParser::EvalExp2(int *value)
{
	int partial_value;
	char op;

	EvalExp3(value);
	op = m_nDoubleOps;
	if(op) { 
		GetToken();	EvalExp3(&partial_value);
		switch(op) { // perform the relational operation 
			case LT: *value = *value <  partial_value; break;
			case LE: *value = *value <= partial_value; break;
			case GT: *value = *value >  partial_value; break;
			case GE: *value = *value >= partial_value; break;
			case EQ: *value = *value == partial_value; break;
			case NE: *value = *value != partial_value; break;
			case NT: *value = !(partial_value); break;
		}
	}
}
void CParser::EvalExp3(int *value)
{
	char op;
	int partial_value;

	EvalExp4(value);
	while((op = *token) == '+' || op == '-') {
		GetToken();
		EvalExp4(&partial_value);
		switch(op) { // add or subtract 
			case '-':
				*value = *value - partial_value;
				break;
			case '+':
				*value = *value + partial_value;
				break;
		}
	}
}
void CParser::EvalExp4(int *value)
{
	char op;
	int partial_value;

	EvalExp5(value);
	while((op = *token) == '*' || op == '/' || op == '%') {
		GetToken();
		EvalExp5(&partial_value);
		switch(op) { // mul, div, or modulus 
			case '*':
				*value = *value * partial_value;
				break;
			case '/':
				*value = (*value) / partial_value;
				break;
			case '%':
				*value = (*value) % partial_value;
				break;
		}
	}
}
void CParser::EvalExp5(int *value)
{
	char op;

	op = '\0';
	if(*token=='+'||*token=='-') {
		op = *token;
		GetToken();
	}
	EvalExp6(value);
	if(op)
		if(op=='-') *value = -(*value);
}
void CParser::EvalExp6(int *value)
{
	if(*token=='(') {
		GetToken();
		EvalExp0(value); // get subexpression 
		if(*token!=')') SntxErr( NULL, PAREN_EXPECTED);
		GetToken();
	}
	else atom(value);
}
void CParser::atom(int *value)
{
	switch(tokenType) {
		case NUMBER: // is numeric constant 
			*value = atoi(token);
			GetToken();
			return;
		default:
			if(*token!=')')    // Process empty expression 
				SntxErr( NULL, SYNTAX); // syntax error 
	}
}

BOOL CScript::Run()
{
	m_dwDef = 2;
	m_pProg        = m_pBuf;
	m_pOldProg     = 0;
	functos     = 0; 
	funcIndex   = 0; 
	gVarIndex   = 0; 
	lvartos     = 0; 
	labeltos    = 0; 
	retValue    = 0; 
	blocktos    = 0; 
	answerCnt   = 0;
	labelIndex  =-1;
	retSign	    = 0;
	PreScan();
	m_pProg = FindFunc("main");
	m_pProg--;
	strcpy(token,"main");
	Call();	

	
	m_anInternalVal[0] = retValue; 
	return TRUE;
}




void CScript::SetIValue(int nVal,...) 
{
	va_list args;
	va_start( args, nVal );
	int i = 0;
	while( nVal != -1 )
	{
		m_anInternalVal[ i++ ] = nVal;
		nVal = va_arg( args, int );
	}
	va_end( args );
}
void CScript::Compute(int *value)
{
	GetToken();
	if(!*token) 
		SntxErr( NULL, NO_EXP);
	else
	{
		EvalExp0(value);
		PutBack(); // return last token read to input stream 
	}
}

void CScript::EvalExp0(int *value)
{
	char temp[ID_LEN]; // holds name of val receiving the assignment 
	int temp_tok;

	if(tokenType == IDENTIFIER) 
	{
		if(IsVar(token)) 
		{ // if a var, see if assignment 
			strcpy(temp,token);
			temp_tok = tokenType;
			GetToken();
			char op;
			op =  *token;
			if( ( op=='+' ||op=='-' || op=='|' || op=='&' ) && *m_pProg == '=' ) 
			{
				GetToken();
				GetToken();
				EvalExp0(value); // get value to assign 
				int *pnValue = GetVarPtr( temp );
				if( pnValue )
				{
					switch( op )
					{
					case '+': *pnValue += *value; break; // += 처리 
					case '-': *pnValue -= *value; break; // -= 처리 
					case '*': *pnValue *= *value; break; // *= 처리 
					case '/': *pnValue /= *value; break; // /= 처리 
					case '|': *pnValue |= *value; break; // |= 처리 
					case '&': *pnValue &= *value; break; // &= 처리 
					}
				}
			}
			else
			if(*token=='=' && *(token+1)!='=') 
			{ // is an assignment 
				GetToken();
				EvalExp0(value); // get value to assign 
				AssignVar(temp,*value); // assign the value 
				return;
			}
			else 
			{ // not an assignment 
				PutBack(); // restore original token 
				strcpy(token,temp);
				tokenType = temp_tok;
			}
		}
	}
	EvalExp1(value);
}

void CScript::atom(int *value)
{
	int i;
	switch(tokenType) 
	{
		case IDENTIFIER:
			i = InternalFunc(token);
			if(i!=-1) 
			{ // Call "standard libray" function 
				retValue = (*interFunc[i].p)(this);
				*value = retValue;
			}
			else
			if(FindFunc(token)) 
			{ // Call user-defined function 
				Call();
				*value = retValue;
			}
			else *value = FindVar(token); // fet var's value 
			GetToken();
			return;
		case NUMBER: // is numeric constant 
			*value = atoi(token);
			GetToken();
			return;
		case DELIMITER: // see if cahracter constant 
			if(*token=='\'') 
			{
				CHAR* pProg = (CHAR*)m_pProg;
				*value = *pProg;
				pProg++;
				if(*pProg!='\'') SntxErr( NULL, QUOTE_EXPECTED);
				pProg++;
				m_pProg = pProg;

				GetToken();
			}
			return;
		default:
			if(*token!=')')    // Process empty expression 
				SntxErr( NULL, SYNTAX); // syntax error 
	}
}
//
//  return value
//  0 == block end ( { } ), one line end( ; )
//  1 == goto 
//  2 == break
//  3 == return
//
int CScript::InterpBlock(int aaa)
{
	int value,c,brk = 0;
	char block = 0;
	char tpTkn[64];
	CHAR* p;

	if(aaa) 
	{
		block = 1; blocktos++;
	}
	do {
		tokenType = GetToken();

		// if interpreting single statement, return on first semicolon 
		// see what kind of token is up 
		if(tokenType==IDENTIFIER) 
		{
			p = m_pProg;	
			strcpy( tpTkn, token );
			GetToken();
			if(*token!=':') 
			{
				strcpy(token,tpTkn);
				m_pProg = p;
				// not a keyword, so process expression 
				PutBack(); //  restore token to input stream
						   //  for further processing by Compute() 
				Compute(&value); // process the expression 
				GetToken();
				if(*token!=';') SntxErr( NULL, SEMI_EXPECTED);
			}
		}
		else
		if(tokenType==BLOCK) 
		{ // If block delimiter 
			if(*token=='{') 
			{    // is a block 
				block = 1;         // interpreting block, not statement 
				blocktos++;
			}
			else 
			{
				blocktos--;
				return 0;          // is a }, so return 
			}
		}
		else 									 // is keyword 
		switch(tok) 
		{
			case VAR: 	 // declare local variables 
				PutBack();
				DeclLocal();
				break;
			case DEFINE:
				ExecDefine();
				break;
			case RETURN: // return from function call 
				FuncRet(); 
				retSign = 1;
				break;
			case IF: // process an if statement  
				ExecIf();
				break;
			case ELSE:   // process an else statement 
				FindEob(); // find end of else block and continue execution 
				break;
			case WHILE:  // process a while loop 
				ExecWhile();
				break;
			case DO:     // process a do-while loop 
				ExecDo();
				break;
			case FOR:
				ExecFor();
				break;
			case BREAK:
				if( block ) blocktos--;
				return 2;
			case DEFAULT:
			case CASE:
				do GetToken(); while(*token!=':');
				break;
			case SWITCH:
				ExecSwitch();
				break;				
			case GOTO:
				GetToken();
				for(c = callStack[functos-1].label; c < labeltos; c++)
					if(!strcmp(labelTable[c].name,token)) { labelIndex = c; break; }
				break;
			case END:
				exit(0);
		}
		if( retSign ) 
		{
			// retSign은 블럭으로 리커시브 된 상태를 계속 리턴하게 해준다.
			// 리커시브 상태의 서브 함수에서 return한다. 코드 3
			if( blocktos-- != 1 )	
				return 3;
			// 모든 블럭의 재귀 호출이 리턴되면 retSign을 0으로 리셋한다.
			retSign = 0; 
			return 0;
		}
		if( labelIndex!=-1 ) 
		{
			if( !block ) 
				return 1;
			if( labelTable[labelIndex].block < blocktos ) 
			{
				blocktos--; 
				return 1;
			}
			else 
			{
				m_pProg = labelTable[labelIndex].loc;
				labelIndex = -1;
			}
		}
	} while( tok != FINISHED && block );

	return 0;
}



void CScript::DeclLocal()
{
	VarType i;

	GetToken(); // get type 

	i.varType = tok;
	i.value = 0; // init to 0 

	do { // process comma-separated list 
		GetToken(); // get var name 
		strcpy(i.varName,token);
		LocalPush(i);
		GetToken();
	} while(*token==',');
	if(*token!=';') SntxErr( NULL, SEMI_EXPECTED);
}
void CScript::FindLabel()
{
	char block = 0;
	char* p = m_pProg;
	char temp[64];
	m_dwDef = 0;
	do 
	{
		GetToken();
		if( tokenType == IDENTIFIER ) 
		{
			strcpy( temp, token );
			GetToken();
			if( *token==':' ) 
			{
				strcpy( labelTable[ labeltos ].name, temp );
				labelTable[ labeltos ].loc = m_pProg;
				labelTable[ labeltos ].block = block;
				labeltos++;
				if( labeltos >= 45 ) 
				{
					printf("label overflow");
				}
			}
		}
		else
		if( tokenType == BLOCK ) 
		{ // If block delimiter 
			if( *token == '{') block++;
			else block--;
			if( !block ) { m_pProg = p;	return; }
		}
	} while(tok != FINISHED);
	m_pProg = p;
	m_dwDef = 2;
}
void CScript::Call()
{
	char *loc;
	CHAR* temp;
	int lvartemp;
	FuncInfo fi;

	loc = FindFunc( token );     // find entry point of function
	if( loc == NULL )
		SntxErr( NULL, FUNC_UNDEF );   // function not defined
	else 
	{
		lvartemp = lvartos;    // save local var stack index
		GetArgs(); 			   // get function arguments
		temp = m_pProg; 		   // save return location
		fi.var    = lvartemp;
		fi.label  = labeltos;
		fi.answer = answerCnt;
		fi.block  = blocktos;
		retValue  = TRUE; // 펑션의 디폴트 리턴값은 1이다.
		FuncPush(fi);          // save local var stack index
		m_pProg      = loc; 	   // reset m_pProg to start of function
		GetParams(); 		   // load the functuon's parameters with the values of the arguments
		FindLabel();		   // find label and it's push label table
		blocktos  =
		retSign   = 0;
		InterpBlock(); 		   // interpret the function
		m_pProg      = temp; 	   // reset the program pointer
		fi        = FuncPop(); // reset the local var stack
		lvartemp  = fi.var;
		labeltos  = fi.label;
		answerCnt = fi.answer;
		lvartos   = fi.var;
		blocktos  = fi.block;
	}
}
void CScript::GetArgs()
{
	int value, count, temp[NUM_PARAMS];
	VarType i;
	count = 0;
	GetToken();
	if( *token != '(' ) 
		SntxErr( NULL, PAREN_EXPECTED);

	GetToken();
	if( *token == ')' ) 
		return;
	PutBack();

	// process a comma-separated list of values 
	do 
	{
		Compute(&value);
		temp[count] = value; // save temporarily 
		GetToken();
		count++;
	} while(*token==',');

	// now, push on localVarStack in reverse order 
	for(count--; count >= 0; count--) 
	{
		i.value = temp[count];
		i.varType = ARG;
		LocalPush(i);
	}
}
void CScript::GetParams()
{
	if(!lvartos)
		GetToken();
	else 
	{
		VarType *p; int i = lvartos-1;
		do { // process comma-separated list of parameters 
			GetToken();
			p = &localVarStack[i];
			if(*token!=')') {
				if(tok!=VAR) SntxErr( NULL, TYPE_EXPECTED);
				p->varType = tokenType;
				GetToken();

				// link parameter name with argument already on local var stack 
				strcpy(p->varName,token);
				GetToken();
				i--;
			}
			else break;
		} while(*token==',');
	}
	if(*token!=')') SntxErr( NULL, PAREN_EXPECTED);
}
void CScript::FuncRet()
{
	int value = 0;
	// get return valuem if any 
	Compute(&value);
	retValue = value;
}
void CScript::LocalPush(VarType i)
{
	if(lvartos > NUM_LOCAL_VARS)
		SntxErr( NULL, TOO_MANY_LVARS);
	localVarStack[lvartos] = i;
	lvartos++;
}
FuncInfo CScript::FuncPop()
{
	functos--;
	if(functos < 0) SntxErr( NULL, RET_NOCALL);
	return callStack[functos];
}
void CScript::FuncPush(FuncInfo fi)
{
	if(functos > NUM_FUNC)
		SntxErr( NULL, NEST_FUNC);
	callStack[functos] = fi;
	functos++;
}
void CScript::AssignVar(char *varName, int value)
{
	// first, see if it's a loca variable 
	for(int i = callStack[functos-1].var; i < lvartos; i++) {
		if(!strcmp(localVarStack[i].varName,varName)) {
			localVarStack[i].value = value;	return;
		}
	}
	// if not local, try global var table 
	for(i = 0; i < gVarIndex; i++)
		if(!strcmp(globalVars[i].varName,varName)) {
			globalVars[i].value = value; return;
		}
	SntxErr( NULL, NOT_VAR); // variable not found 
}
int* CScript::GetVarPtr(char *varName)
{
	// first, see if it's a loca variable 
	for(int i = callStack[functos-1].var; i < lvartos; i++) 
	{
		if(!strcmp(localVarStack[i].varName,varName)) 
		{
			return &localVarStack[i].value;
		}
	}
	// if not local, try global var table 
	for(i = 0; i < gVarIndex; i++)
	{
		if(!strcmp(globalVars[i].varName,varName)) 
		{
			return &globalVars[i].value;
		}
	}
	SntxErr( NULL, NOT_VAR); // variable not found 
	return NULL;
}

int CScript::FindVar(char *s)
{
	// first, see if it's a local variable 
	for( int i = callStack[functos-1].var; i < lvartos; i++)
		if(!strcmp(localVarStack[i].varName,s))
			return localVarStack[i].value;
	// otherwise, try global vars 
	for(i = 0; i < gVarIndex; i++)
		if(!strcmp(globalVars[i].varName,s))
			return globalVars[i].value;
	SntxErr( NULL, NOT_VAR); // vaiable not found 
	return 0;
}
int CScript::IsVar(char *s)
{
	// first, see if it's a local variable 
	for( int i = callStack[functos-1].var; i < lvartos; i++)
		if(!strcmp(localVarStack[i].varName,s))
			return 1;
	// otherwise, try global bars 
	for(i = 0; i < gVarIndex; i++)
		if(!strcmp(globalVars[i].varName,s))
			return 1;
	return 0;
}


// Find end of block
//
//  {     }!
//  {  ;  }!
//        ;!
//  ( ; ) ;!
//  ( ) { }!
//  { (;) }!
//  ! is end

void CScript::ExecIf()
{
	int cond; 
	CHAR* begin;

	Compute(&cond); // get left expression

	if(cond) 
	{ // is true so process target of IF
		begin = m_pProg;
		switch(InterpBlock()) 
		{
			case 2:	m_pProg = begin; FindEob();
			case 1: case 3: return;
		}
	}
	else 
	{    // otherwise skip around IF block and process the ELSE, if present
		GetToken();
		if(*token!='{') 
		{
			int flag = 0, iff = 0;
			for(;;) 
			{
				if(flag && (!iff || (iff && tok!=ELSE)))
					break;
				if(tok==IF) {
					iff++; flag = 0;
				}
				else
				if(tok==ELSE) {
					iff--; flag = 0;
				}
				else {
					PutBack();
					FindEob();
					flag = 1;
				}
				GetToken();
			}
		}
		else 
		{
			PutBack();
			FindEob(); // find start of next line
			GetToken();
		}
		if( tok != ELSE ) 
		{
			PutBack(); // restore token if no ELSE if present
			return;
		}
		begin = m_pProg;
		switch( InterpBlock() ) 
		{
			case 2:	m_pProg = begin; FindEob();
			case 1: case 3: return;
		}
	}
}
void CScript::ExecWhile()
{
	int cond;
	PutBack();
	CHAR* temp = m_pProg; // save location of top of while loop
	GetToken();
	Compute(&cond); // check the conditional expression
	if(cond) 
	{
		CHAR* begin = m_pProg;
		switch( InterpBlock() ) 
		{
			case 2: m_pProg = begin; FindEob();
			case 1:	case 3: return;
		}
	}
	else 
	{ // otherwise, skip around loop
		FindEob();
		return;
	}
	m_pProg = temp; // loop back to top
}
void CScript::ExecDo()
{
	int cond;	
	PutBack();
	CHAR* temp = m_pProg; // save location of top of do loop
	GetToken();  // get start of loop
	CHAR* begin = m_pProg;
	switch(InterpBlock()) 
	{
		case 2:	m_pProg = begin; FindEob(); FindEob(); // bypassed block and while
		case 1:	case 3: return;
	}
	GetToken();
	if(tok!=WHILE) 
		SntxErr( NULL, WHILE_EXPECTED);
	Compute(&cond);       // check the loop condition
	if( cond ) 
		m_pProg = temp; // if true loop; otherwise, cotinue on
}
void CScript::ExecFor()
{
	int cond;
	char *temp, *temp2;
	int brace;

	GetToken();
	Compute(&cond); // initialization expression 
	if(*token!=';') SntxErr( NULL, SEMI_EXPECTED);
	m_pProg++; // get past the ; 
	temp = m_pProg;
	for(;;) {
		cond = 1; // 다음 Compute에서 비교대상이 없으면 (;;) cond = 1을 유지하여 무한루프를 돌게 한다. 
		Compute(&cond); // check the condition 
		if(*token!=';') SntxErr( NULL, SEMI_EXPECTED);
		m_pProg++; // get past the ; 
		temp2 = m_pProg;

		// find the start of the for block 
		brace = 1;
		while(brace) {
			GetToken();
			if(*token=='(') brace++;
			if(*token==')') brace--;
		}
		if(cond) {
			char *begin = m_pProg;
			switch(InterpBlock()) {
				case 2: m_pProg = begin; FindEob();
				case 1:	case 3: return;
			}
		}
		else  { // otherwise, skip around loop 
			FindEob();
			return;
		}
		m_pProg = temp2;
		Compute(&cond); // do the increment 
		m_pProg = temp; // loop back to top 
	}
}
void CScript::ExecSwitch()
{
	int cond,cond2,block = 0;
	char *def   = 0;
	char *begin = m_pProg;

	Compute(&cond);

	for(;;) {
		GetToken();
		if(*token=='{') {
			if(block) {
				PutBack();
				FindEob();
			}
			else block = 1;
		}
		else
		if(*token=='}') {
			if(def) {
				m_pProg = def;
				if(InterpBlock(1) == 2) {
					m_pProg = begin; FindEob();
				}
			}
			break;
		}
		else
		if(!def) {
			if(tok == CASE) {
				Compute(&cond2);
				if(cond2==cond) {
					GetToken(); // bypass ':'
					def = m_pProg;
				}
			}
			else
			if(tok == DEFAULT) {
				GetToken(); // bypass ':'
				def = m_pProg;
			}
		}
	}
}

void CScript::FindEob(void)
{
	int brace = 0;
	for(;;) {
		GetToken();
		if(*token==';'&&!brace) break;
		else if(*token=='{') brace++;
		else if(*token=='}') { if(!(--brace)) break; }
		else if(*token=='(') {
			int bra = 1;
			while(bra) {
				GetToken();
				if(*token=='(') bra++;
				if(*token==')') bra--;
			}
		}
	}
}

#endif // __REMOVE_SCIRPT_060712