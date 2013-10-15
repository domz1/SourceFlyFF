#ifndef __USERMACRO_H__
#define __USERMACRO_H__

#define ALLOCBLOCK( ar )	\
							CAr ar;

#define	GETBLOCK( ar, lpBuf, nBufSize )	\
										int nBufSize;	\
										LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );

#define	USERPTR							( _it->second )

#define FOR_VISIBILITYRANGE( pCtrl )	\
										map< DWORD, CUser* >::iterator _it  = pCtrl->m_2pc.begin(); \
										map< DWORD, CUser* >::iterator _end = pCtrl->m_2pc.end(); \
										while( _it != _end )	\
										{

#define	NEXT_VISIBILITYRANGE( pCtrl )	\
											++_it;	\
										}

#endif // USERMACRO_H