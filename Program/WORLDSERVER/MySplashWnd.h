#ifndef __MYSPLASHWND_H__
#define __MYSPLASHWND_H__

#pragma once

class CMySplashWnd : public CWnd
{
	UINT	m_nBitmapID;
	UINT	m_nDuration;
	UINT	m_nTimerID;
	CBitmap	m_bitmap;
	CPalette	m_pal;
	CWnd	m_wndInvisible;

public:
// Construction
	CMySplashWnd( UINT nBitmapID, UINT nDuration = 2500 );

// Attributes
public:
	BOOL	Create( void );

// Operations
public:
	virtual	BOOL	PreTranslateMessage( MSG* pMsg );

// Implementation
public:
	afx_msg	void	OnPaint( void );
	afx_msg	void	OnTimer( UINT nIDEvent );

	DECLARE_MESSAGE_MAP()

protected:
	BOOL	GetBitmapAndPalette( UINT nIDResource, CBitmap &bitmap, CPalette &pal );

};

#endif	// __MYSPLASHWND_H__