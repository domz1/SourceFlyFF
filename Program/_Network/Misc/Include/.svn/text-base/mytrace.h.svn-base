#ifndef __MYTRACE_H__
#define __MYTRACE_H__

#pragma once

#include "CMcl.h"

#define	MAX_LOG	26

typedef struct _MYTRACE
{
	u_long	uKey;
	BOOL	fInverse;
	char	szText[160];
} MYTRACE, *PMYTRACE;

class CMyTrace
{
private:
	MYTRACE		m_mts[MAX_LOG];

	u_short		m_uSize;
	u_short		m_uHead;
	u_short		m_uTail;

	HFONT	m_hfont;
	HWND	m_hWnd;
	COLORREF	m_cr;
	COLORREF	m_crBk;

	char	m_szStatusBar[160];

	CMclCritSec		m_AddRemoveLock;

private:
	void	TextOut( HDC hDC, int x, int y, LPCTSTR lpString, BOOL fInverse );
	int		FindKey( u_long uKey );

public:
	// Constructions
	CMyTrace();
	virtual	~CMyTrace();

	// Operations;
	void	Initialize( HWND hWnd, LPCTSTR lpszFace, COLORREF cr, COLORREF crBk )
		{	m_hWnd	= hWnd;
			m_cr	= cr;
			m_crBk	= crBk;
			m_hfont		= CreateFont( 24, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, PROOF_QUALITY, VARIABLE_PITCH | FF_DONTCARE, lpszFace );
		}

	void	Add( u_long uKey, BOOL fInverse, LPCTSTR lpszFormat, ... );
	void	AddLine( char ch );
	void	Paint( HDC hDC );
	void	GetText( u_long uKey, LPSTR lpszText );

	static	u_long	Key( LPCSTR szKey );
};

#endif	// __MYTRACE_H__