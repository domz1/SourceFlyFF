#ifndef __WND_QUEST_QUICK_INFO_H__
#define __WND_QUEST_QUICK_INFO_H__

#if __VER >= 15 /* __IMPROVE_QUEST_INTERFACE */ && defined( __CLIENT )
//-----------------------------------------------------------------------------
class CWndQITreeCtrl : public CWndTreeCtrl
{
public:
	CWndQITreeCtrl( void );
	virtual ~CWndQITreeCtrl( void );

public:
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void PaintTree( C2DRender* p2DRender, CPoint& pt, CPtrArray& ptrArray );
};
//-----------------------------------------------------------------------------
class CWndQuestQuickInfo : public CWndNeuz
{
public:
	CWndQuestQuickInfo( void );
	virtual ~CWndQuestQuickInfo( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL Process( void );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnSize( UINT nType, int cx, int cy );

public:
	void SetUpdateTextSwitch( BOOL bUpdateText );
	void ExtendAppletHeight( void );

public:
	BOOL GetActiveGroundClick( void ) const;

private:
	void DeleteTree( void );

private:
	int m_nTreeInformationListSize;
	BOOL m_bOnSurface;
	BOOL m_bActiveGroundClick;
	DWORD m_tmOld;
	BOOL m_bUpdateText;
	CWndQITreeCtrl m_CWndQITreeCtrl;
};
//-----------------------------------------------------------------------------
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )

#endif // __WND_QUEST_QUICK_INFO_H__