#ifndef __WNDQUEST__H
#define __WNDQUEST__H

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
#include "TreeInformation.h"

extern CTreeInformationManager g_QuestTreeInfoManager;

class CWndQuestTreeCtrl : public CWndTreeCtrl
{
public:
	CWndQuestTreeCtrl( void );
	virtual ~CWndQuestTreeCtrl( void );

public:
	virtual BOOL OnChildNotify( UINT nCode, UINT nID, LRESULT* pLResult );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
};
#endif // __IMPROVE_QUEST_INTERFACE

class CWndRemoveQuest : public CWndMessageBox
{
public:
	int m_nRemoveQuestId;
	CWndRemoveQuest() { m_nRemoveQuestId = -1; }
	virtual ~CWndRemoveQuest() { }
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};

class CWndQuest : public CWndNeuz 
{ 
public: 
	CWndQuest(); 
	~CWndQuest();

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	enum { MAX_CHECK_NUMBER = 5 };
#endif // __IMPROVE_QUEST_INTERFACE

	int m_idSelQuest;
	CWordArray m_aOpenTree;
	void Update( int nNewQuestId = -1 );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	void RemoveQuest( void );
#else // __IMPROVE_QUEST_INTERFACE
#if __VER >= 13 // __QUEST_HELPER
	void UpdateText(BOOL bClick = FALSE);
#else //__QUEST_HELPER
	void UpdateText();
#endif //__QUEST_HELPER
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	void ControlOpenTree( const LPTREEELEM lpTreeElem );
#endif // __IMPROVE_QUEST_INTERFACE

	void TreeOpen();
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	CString MakeQuestString( CString& string, BOOL bCond ); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
#endif // __IMPROVE_QUEST_INTERFACE
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
private:
	CWndQuestTreeCtrl* GetQuestTreeSelf( const DWORD dwQuestID );
	void AddOpenTree( CWordArray& raOpenTree, CPtrArray& rPtrArray );
	void OpenTreeArray( CPtrArray& rPtrArray, BOOL bOpen = TRUE );
	void InsertQuestItem( const DWORD dwQuestID, CDWordArray& raOldHeadQuestID, const BOOL bCompleteQuest, const int nNewQuestId = -1 );
	DWORD FindOldHeadQuest( const CDWordArray& raOldHeadQuestID, const DWORD dwNowHeadQuestID ) const;
	BOOL IsCheckedQuestID( DWORD dwQuestID );

private:
	CWndQuestTreeCtrl m_WndScenario;
	CWndQuestTreeCtrl m_WndNormal;
	CWndQuestTreeCtrl m_WndRequest;
	CWndQuestTreeCtrl m_WndEvent;
#endif // __IMPROVE_QUEST_INTERFACE
}; 

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
class CWndQConditionTreeCtrl : public CWndTreeCtrl
{
public:
	CWndQConditionTreeCtrl( void );
	virtual ~CWndQConditionTreeCtrl( void );

public:
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
};

class CWndQuestDetail : public CWndNeuz
{
public:
	CWndQuestDetail( DWORD dwQuestID = -1 );
	virtual ~CWndQuestDetail( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void PaintFrame( C2DRender* p2DRender );

public:
	void UpdateQuestText( BOOL bClick = FALSE );
	void UpdateQuestDetailText( DWORD dwQuestID, LPQUEST lpQuest, BOOL bComplete, BOOL bClick = FALSE );
	void SetQuestID( DWORD dwQuestID );
	DWORD GetQuestID( void ) const;

private:
	DWORD m_dwQuestID;
	CWndQConditionTreeCtrl m_WndQConditionTreeCtrl;
};

void MakeQuestConditionItems( DWORD dwQuestID, 
						      CWndTreeCtrl* pWndTreeCtrl, 
							  BOOL bClick = FALSE, 
							  LPTREEELEM lpTreeElem = NULL, 
							  DWORD dwStartColor = D3DCOLOR_ARGB( 255, 255, 255, 255 )/*adeilson*/, 
							  DWORD dwEndColor = D3DCOLOR_ARGB( 255, 255, 255, 255 )/*adeilson*/, 
							  DWORD dwSelectColor =  D3DCOLOR_ARGB( 255, 255, 255, 255 ) );/*adeilson*/
const CString MakeString( const CString& string, BOOL bCond );
DWORD MakeTextColor( DWORD dwStartColor = D3DCOLOR_ARGB( 255, 255, 255, 255 )/*adeilson*/, DWORD dwEndColor = D3DCOLOR_ARGB( 255, 255, 255, 255 )/*adeilson*/, int nCurrentNumber = 0, int nCompleteNumber = 0 );
DWORD GetRootHeadQuest( DWORD dwHeadQuest );
DWORD SetQuestDestinationInformation( DWORD dwQuestID, DWORD dwGoalIndex );
void ProcessQuestDestinationWorldMap( DWORD dwGoalTextID );
#endif // __IMPROVE_QUEST_INTERFACE

#endif
