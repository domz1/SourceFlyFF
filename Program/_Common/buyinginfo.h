#ifndef __CHARGEDITEM_H__
#define	__CHARGEDITEM_H__

typedef struct _BUYING_INFO
{
	DWORD	dwServerIndex;
	DWORD	dwPlayerId;
	DWORD	dwSenderId;
	DWORD	dwItemId;
	DWORD	dwItemNum;
	char  szBxaid[21];	// bxaid
	DWORD	dwRetVal;

	_BUYING_INFO()
	{
		dwServerIndex	= 0;
		dwPlayerId	= 0;
		dwItemId	= 0;
		dwItemNum	= 0;
		dwRetVal	= 0;
		ZeroMemory( szBxaid, sizeof(char)*21 );
		dwSenderId	= 0;
	}
}
BUYING_INFO, *PBUYING_INFO;

typedef struct _BUYING_INFO2: public _BUYING_INFO
{
	DWORD	dpid;
	DWORD	dwKey;
	_BUYING_INFO2() : _BUYING_INFO()
		{
			dpid	= 0xFFFFFFFF;
			dwKey	= 0;
		}
}
BUYING_INFO2, *PBUYING_INFO2;

typedef struct _BUYING_INFO3: public _BUYING_INFO2
{
	DWORD	dwTickCount;
	_BUYING_INFO3() : _BUYING_INFO2()
		{
//			dwTickCount		= GetTickCount();
		}
}
BUYING_INFO3, *PBUYING_INFO3;

#endif	// __CHARGEDITEM_H__

