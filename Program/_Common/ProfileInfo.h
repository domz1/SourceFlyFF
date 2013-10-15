// ProfileInfo.h: interface for the CProfileInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROFILEINFO_H__D49A75BB_8979_483C_AB71_DB5E5D196285__INCLUDED_)
#define AFX_PROFILEINFO_H__D49A75BB_8979_483C_AB71_DB5E5D196285__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __NEW_PROFILE

class CProfileInfo  
{
public:
	CProfileInfo();
	virtual ~CProfileInfo();
	static CProfileInfo* GetInstance();

	void	SetProfileInfo();
	BOOL	IsToggleProfiling()	{ return m_bToggleProfiling; }
	void	SetToggleProfiling();
	void	FileOut( const char* szFile );

	vector<string>	m_vecstrProfileInfo;
private:
	void	Clear();
	void	Reset();
	BOOL	m_bToggleProfiling;

};

#endif // __NEW_PROFILE

#endif // !defined(AFX_PROFILEINFO_H__D49A75BB_8979_483C_AB71_DB5E5D196285__INCLUDED_)
