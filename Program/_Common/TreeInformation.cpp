#include "StdAfx.h"
#include "TreeInformation.h"

#if __VER >= 15 /* __IMPROVE_QUEST_INTERFACE */ && defined( __CLIENT )
//-----------------------------------------------------------------------------
CTreeInformation::CTreeInformation( void ) : 
m_dwData( 0 ), 
m_bOpen( TRUE )
{
}
//-----------------------------------------------------------------------------
CTreeInformation::~CTreeInformation( void )
{
}
//-----------------------------------------------------------------------------
void CTreeInformation::SetData( DWORD dwData )
{
	m_dwData = dwData;
}
//-----------------------------------------------------------------------------
DWORD CTreeInformation::GetData( void ) const
{
	return m_dwData;
}
//-----------------------------------------------------------------------------
void CTreeInformation::SetOpen( BOOL bOpen )
{
	m_bOpen = bOpen;
}
//-----------------------------------------------------------------------------
BOOL CTreeInformation::GetOpen( void ) const
{
	return m_bOpen;
}
//-----------------------------------------------------------------------------
int CTreeInformationManager::m_nSelectedTabNumber = 0;
CTreeInformationManager::QuestListGroup CTreeInformationManager::m_eQuestListGroup = CTreeInformationManager::CURRENT_QUEST_LIST;
//-----------------------------------------------------------------------------
CTreeInformationManager::CTreeInformationManager( void )
{
}
//-----------------------------------------------------------------------------
CTreeInformationManager::~CTreeInformationManager( void )
{
	DeleteAllTreeInformation();
}
//-----------------------------------------------------------------------------
TreeInformationList* CTreeInformationManager::GetTreeInformationList( void )
{
	return &m_TreeInformationList;
}
//-----------------------------------------------------------------------------
CTreeInformation* CTreeInformationManager::GetTreeInformation( DWORD dwData ) const
{
	for( TreeInformationList::const_iterator Itor = m_TreeInformationList.begin(); Itor != m_TreeInformationList.end(); ++Itor )
	{
		CTreeInformation* pTreeInformation = ( CTreeInformation* )( *Itor );
		DWORD dwIteratorData = pTreeInformation->GetData();
		if( dwIteratorData == dwData )
			return pTreeInformation;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
void CTreeInformationManager::InsertTreeInformation( DWORD dwData, BOOL bOpen )
{
	CTreeInformation* pTreeInformation = new CTreeInformation;
	pTreeInformation->SetData( dwData );
	pTreeInformation->SetOpen( bOpen );
	m_TreeInformationList.push_back( pTreeInformation );
}
//-----------------------------------------------------------------------------
void CTreeInformationManager::DeleteTreeInformation( DWORD dwData )
{
	for( TreeInformationList::iterator Itor = m_TreeInformationList.begin(); Itor != m_TreeInformationList.end(); ++Itor )
	{
		CTreeInformation* pTreeInformation = ( CTreeInformation* )( *Itor );
		DWORD dwIteratorData = pTreeInformation->GetData();
		if( dwIteratorData == dwData )
		{
			SAFE_DELETE( pTreeInformation );
			m_TreeInformationList.erase( Itor );
			return;
		}
	}
}
//-----------------------------------------------------------------------------
void CTreeInformationManager::DeleteAllTreeInformation( void )
{
	for( TreeInformationList::iterator Itor = m_TreeInformationList.begin(); Itor != m_TreeInformationList.end(); ++Itor )
		SAFE_DELETE( *Itor );
	m_TreeInformationList.clear();
	m_nSelectedTabNumber = 0;
	m_eQuestListGroup = CTreeInformationManager::CURRENT_QUEST_LIST;
}
//-----------------------------------------------------------------------------
int CTreeInformationManager::GetTreeInformationListSize( void ) const
{
	return m_TreeInformationList.size();
}
//-----------------------------------------------------------------------------
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )