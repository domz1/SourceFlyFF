#ifndef __MEMSPY_H__
#define __MEMSPY_H__

#pragma once

#include "CMcl.h"

typedef struct _NEWSTRUCT
{
	char	lpszFileName[MAX_PATH];
	int		nLine;
	size_t	nSize;
} NEWSTRUCT, *LPNEWSTRUCT;

class CMemSpy
{
private:
	map<LPVOID, LPNEWSTRUCT>	m_mapNewStruct;
	CMclCritSec		m_AddRemoveLock;

public:
	CMemSpy()
		{

		}

	virtual ~CMemSpy()
		{
			CMclAutoLock Lock( m_AddRemoveLock );
			if( !m_mapNewStruct.empty() )
			{
				TRACE( "Detected memory leaks!\n" );
				for( map<LPVOID, LPNEWSTRUCT>::iterator i = m_mapNewStruct.begin(); i != m_mapNewStruct.end(); ++i )
				{
					LPNEWSTRUCT lpNewStruct		= i->second;
					ASSERT( lpNewStruct->lpszFileName );
					TRACE( "%s(%d):\n", lpNewStruct->lpszFileName, lpNewStruct->nLine );
					SAFE_DELETE( lpNewStruct );
				}
				m_mapNewStruct.clear();
			}
		}
	
	void	Add( LPVOID lpMem, size_t nSize, LPCSTR lpszFileName, int nLine )
		{
			ASSERT( lpszFileName );
			ASSERT( nSize > 0 );
			ASSERT( nLine > 0 );
			LPNEWSTRUCT lpNewStruct		= new NEWSTRUCT;
			lpNewStruct->nSize	= nSize;
			strcpy( lpNewStruct->lpszFileName, lpszFileName );
			lpNewStruct->nLine	= nLine;
			m_AddRemoveLock.Enter();
			m_mapNewStruct[lpMem]	= lpNewStruct;
			m_AddRemoveLock.Leave();
		}

	void	Remove( LPVOID lpMem )
		{
			CMclAutoLock Lock( m_AddRemoveLock );
			map<LPVOID, LPNEWSTRUCT>::iterator i	= m_mapNewStruct.find( lpMem );
			if( i == m_mapNewStruct.end() )
			{
				TRACE( "Invalid address specified.\n" );
				ASSERT( 0 );
			}
			safe_delete( i->second );
			m_mapNewStruct.erase( i );
		}
};

#endif