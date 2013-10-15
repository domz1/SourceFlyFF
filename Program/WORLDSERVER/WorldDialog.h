#ifndef __WORLDDIALOG_H__
#define __WORLDDIALOG_H__

#include "FunctionsInfo.h"

class CNpcDialogInfo : public NPCDIALOG_INFO
{
public:
	int			m_nValue;
	int			m_nSrcId;
	int			m_nDstId;
	int			m_nQuestId;
	int*		m_pnGlobal;
	LPCTSTR		m_pszName;
	Functions*	m_pFuctions;

	virtual int			GetReturn()  { return m_nValue; }
	virtual int			GetDstId()   { return m_nDstId; }
	virtual int			GetSrcId()   { return m_nSrcId; }
	virtual int			GetCaller()  { return WES_DIALOG; }	// WES_DIALOG - 5
	virtual int			GetQuest()   { return m_nQuestId; }
	virtual int			GetNpcId()   { return GetDstId(); }
	virtual int			GetPcId()    { return GetSrcId(); }
	virtual int*		GetGlobal()  { return m_pnGlobal; }
	virtual LPCTSTR		GetName()    { return m_pszName; }
	virtual Functions*  GetFunctions() { return m_pFuctions; }
};

typedef INT (WINAPI *PFRUNDIALOG)(LPCTSTR szNpc, LPCTSTR szKey, NPCDIALOG_INFO* pInfo);
typedef INT (WINAPI *PFFINDSCIRPTKEY)(LPCTSTR szNpc, LPCTSTR szKey);
typedef void (WINAPI *PFSETLANGUAGE)(INT nLanguage);
typedef BOOL (WINAPI *PFINITWORLDDIALOG)();

class CWorldDialog
{
private:
	CWorldDialog();
public:
	~CWorldDialog();					

private:
	HMODULE			m_hDLL;
	PFRUNDIALOG		m_pfnRunDialog;
	PFFINDSCIRPTKEY	m_pfnFindScriptKey;
	PFINITWORLDDIALOG	m_pfnInitWorldDialog;

private:
	BOOL			Load();
	void			Free();


public:
	BOOL			Init();						
	int				Run( LPCTSTR szName, LPCTSTR szKey, NPCDIALOG_INFO* pInfo );
	BOOL			Find( LPCTSTR szName, LPCTSTR szKey );
	BOOL			Reload();

	static CWorldDialog& GetInstance();
};

#endif // __WORLDDIALOG_H__