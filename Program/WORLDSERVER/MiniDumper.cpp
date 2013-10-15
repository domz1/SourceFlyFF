//////////////////////////////////////////////////////////////////////////////
/// \file MiniDumper.cpp
/// \author excel96
/// \date 2003.11.18
//////////////////////////////////////////////////////////////////////////////
//#include "StdAfx.h"

#include "StdAfx.h"
#include "MiniDumper.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h> 
#include <stdarg.h> 
#include <dbghelp.h>

#ifdef __MINIDUMP

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
    HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
    );

/// 덤프 레벨.
static DumpLevel s_DumpLevel			   = DUMP_LEVEL_0;
/// 덤프 파일 이름에다가 덤프 파일이 생성된 날짜를 집어넣는가의 여부.
static bool		s_bAddTimeStamp            = true;
/// 덤프 파일 이름 문자열.
static TCHAR	s_szAppName[_MAX_PATH]     = {0,};
/// 덤프 파일 이름 문자열.
TCHAR			s_szFaultReason[1024*16]   = {0,};

static LPCTSTR GetFaultReason(struct _EXCEPTION_POINTERS* pExPtrs);
LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExPtr);

void filelog( LPVOID, LPCTSTR lpszFormat, ... )
{
	TCHAR szBuffer[1024];

	va_list args;
	va_start(args, lpszFormat);
	int n = _vsntprintf(szBuffer, 1023, lpszFormat, args);
	va_end(args);
	
	if( n > 0 )
	{
		FILE* fp = fopen( MakeFileNameDate ( "error.txt" ),"a");
		if( fp )
		{
			fprintf(fp, szBuffer);
			fclose(fp);	
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
///예외에 대한 정보를 받아서, 미니 덤프 파일을 생성한다. 
//////////////////////////////////////////////////////////////////////////////
LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExPtr)
{
    LONG    retval                   = EXCEPTION_CONTINUE_SEARCH;
    HMODULE hDLL                     = NULL;
    TCHAR   szDbgHelpPath[_MAX_PATH] = {0, };
    TCHAR   szDumpPath[MAX_PATH * 2] = {0,};

    // 먼저 실행 파일이 있는 디렉토리에서 DBGHELP.DLL을 로드해 본다.
    // Windows 2000 의 System32 디렉토리에 있는 DBGHELP.DLL 파일은 버전이 
    // 오래된 것일 수 있기 때문이다. (최소 5.1.2600.0 이상이어야 한다.)
    if (::GetModuleFileName(NULL, szDbgHelpPath, _MAX_PATH))
    {
        LPTSTR pSlash = ::strrchr(szDbgHelpPath, '\\');
        if (pSlash)
        {
            ::lstrcpy(pSlash + 1, "DBGHELP.DLL");
            hDLL = ::LoadLibrary(szDbgHelpPath);
        }
    }

    // 현재 디렉토리에 없다면, 아무 버전이나 로드한다.
    if (hDLL == NULL) hDLL = ::LoadLibrary("DBGHELP.DLL");

    // DBGHELP.DLL을 찾을 수 없다면 더 이상 진행할 수 없다.
    if (hDLL == NULL)
    {
        filelog(NULL, "DBGHELP.DLL not found");
        return retval;
    }

    // DLL 내부에서 MiniDumpWriteDump API를 찾는다.
    MINIDUMPWRITEDUMP pfnMiniDumpWriteDump = 
        (MINIDUMPWRITEDUMP)::GetProcAddress(hDLL, "MiniDumpWriteDump");

    // 미니덤프 함수를 찾을 수 없다면 더 이상 진행할 수 없다.
    if (pfnMiniDumpWriteDump == NULL)
    {
        filelog(NULL, "DBGHELP.DLL too old");
        return retval;
    }

    if (s_bAddTimeStamp)
    {
        // 현재 시간을 얻어온다.
        SYSTEMTIME t;
        ::GetLocalTime(&t);

        // 시간 문자열을 준비한다.
        TCHAR szTail[_MAX_PATH];
        _snprintf(szTail, _MAX_PATH-1, 
            " %04d-%02d-%02d %02d-%02d-%02d",
            t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

        // 덤프 파일 이름 += 시간 문자열
        ::lstrcat(szDumpPath, s_szAppName);
        ::lstrcat(szDumpPath, szTail);
    }

    // 덤프 파일 이름 += 확장자
    ::lstrcat(szDumpPath, ".dmp");

    // 파일을 생성한다.
    HANDLE hFile = ::CreateFile(
        szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // 파일을 생성할 수 없다면 더 이상 진행할 수 없다.
    if (hFile == INVALID_HANDLE_VALUE)
    {
        filelog(NULL, "Failed to create dump file '%s'", szDumpPath);
        return retval;
    }

    MINIDUMP_EXCEPTION_INFORMATION ExceptionParam;

    ExceptionParam.ThreadId = ::GetCurrentThreadId();
    ExceptionParam.ExceptionPointers = pExPtr;
    ExceptionParam.ClientPointers = FALSE;

    // 옵션에 따라 덤프 파일을 생성한다. 
    BOOL bResult = FALSE;
    switch (s_DumpLevel)
    {
        case DUMP_LEVEL_0: // MiniDumpNormal
            bResult = pfnMiniDumpWriteDump(
                ::GetCurrentProcess(), ::GetCurrentProcessId(), 
                hFile, MiniDumpNormal, &ExceptionParam, NULL, NULL);
            break;
        case DUMP_LEVEL_1: // MiniDumpWithDataSegs 
            bResult = pfnMiniDumpWriteDump(
                ::GetCurrentProcess(), ::GetCurrentProcessId(), 
                hFile, MiniDumpWithDataSegs, &ExceptionParam, NULL, NULL);
            break;
        case DUMP_LEVEL_2: // MiniDumpWithFullMemory 
            bResult = pfnMiniDumpWriteDump(
                ::GetCurrentProcess(), ::GetCurrentProcessId(), 
                hFile, MiniDumpWithFullMemory, &ExceptionParam, NULL, NULL);
            break;
        default:
            bResult = pfnMiniDumpWriteDump(
                ::GetCurrentProcess(), ::GetCurrentProcessId(), 
                hFile, MiniDumpNormal, &ExceptionParam, NULL, NULL);
            break;
    }

    // 덤프 파일 생성 결과를 로그 파일에다 기록한다.
    if (bResult)
    {
        TCHAR szMessage[8192] = {0,};
        lstrcat(szMessage, "Saved dump file to '");
        lstrcat(szMessage, szDumpPath);
        lstrcat(szMessage, "'.\nFault Reason : ");
        lstrcat(szMessage, GetFaultReason(pExPtr));

        filelog(NULL, szMessage);
        retval = EXCEPTION_EXECUTE_HANDLER;
    }
    else
    {
        filelog(NULL, "Failed to save dump file to '%s' (error %d)", 
            szDumpPath, ::GetLastError() );
    }

    ::CloseHandle(hFile);

    return retval;
}

LPCTSTR GetFaultReason(struct _EXCEPTION_POINTERS* pExPtrs)
{
    if (::IsBadReadPtr(pExPtrs, sizeof(EXCEPTION_POINTERS))) 
        return "BAD EXCEPTION POINTERS";

    // 간단한 에러 코드라면 그냥 변환할 수 있다.
    switch (pExPtrs->ExceptionRecord->ExceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION";
        case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT";
        case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT";
        case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
        case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
        case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT";
        case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION";
        case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW";
        case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK";
        case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO";
        case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW";
        case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION";
        case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR";
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
        case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW";
        case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION";
        case EXCEPTION_GUARD_PAGE:               return "EXCEPTION_GUARD_PAGE";
        case EXCEPTION_INVALID_HANDLE:           return "EXCEPTION_INVALID_HANDLE";
        case 0xE06D7363:                         return "Microsoft C++ Exception";
        default:
            break;
    }

    // 뭔가 좀 더 복잡한 에러라면...
    lstrcpy(s_szFaultReason, "Unknown"); 
    ::FormatMessage(
        FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
        ::GetModuleHandle("NTDLL.DLL"),
        pExPtrs->ExceptionRecord->ExceptionCode, 
        0,
        s_szFaultReason,
        0,
        NULL);

    return s_szFaultReason;
}

void InitDumper( DumpLevel level )
{
    s_DumpLevel     = level;

    // 모듈 경로를 알아낸다.
    TCHAR szFilename[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilename, _MAX_PATH);

    // 확장자를 제거한 모듈 경로를 준비해두고...
    TCHAR* dot = strrchr(szFilename, '.');
    ::lstrcpyn(s_szAppName, szFilename, (int)(dot - szFilename + 1));

    // 예외 처리 핸들러를 설정한다.
    ::SetUnhandledExceptionFilter(TopLevelFilter);
}

#endif //  __MINIDUMP