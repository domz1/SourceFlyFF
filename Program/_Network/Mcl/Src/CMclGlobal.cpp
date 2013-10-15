//
// FILE: CMclGlobal.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen
//
/////////////////////////////////////////////////////////////////////////

#include "CMclGlobal.h"

void CMclInternalThrowError( DWORD dwStatus, LPCTSTR lpFilename, int line) {
#ifdef _DEBUG
    // print the error for debug builds...
    TCHAR string[2*MAX_PATH];
    wsprintf( string, "CMcl Library Win32 Error 0x%08x(%d) at %s line %d\n", 
                dwStatus, dwStatus, lpFilename, line);        
    OutputDebugString(string);
#endif

#if __CMCL_THROW_EXCEPTIONS__
    // throw exception for fatal errors...
    throw dwStatus;
#endif
}

