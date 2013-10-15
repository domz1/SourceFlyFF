#ifndef __TOOLS_H__
#define	__TOOLS_H__

extern	void md5( char* out, const BYTE* pbContent, DWORD cbContent );
extern void md5( char* out, const char* in );
extern	void MakeTutorialStateString( char * szOut, int nState, const char* szKey );

#endif	// __TOOLS_H__