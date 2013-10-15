#include "stdafx.h"
#include "tools.h"
#include <wincrypt.h>

void md5( char* out, const BYTE* pbContent, DWORD cbContent )
{
	HCRYPTPROV hCryptProv;
	HCRYPTHASH hHash;
	BYTE bHash[0x7f];
	DWORD dwHashLen	= 16;	// The MD5 algorithm always returns 16 bytes.
	*out	= '\0';
	
	if( CryptAcquireContext( &hCryptProv, 
		NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET ) )
	{
		if( CryptCreateHash( hCryptProv,
			CALG_MD5,	// algorithm identifier definitions see: wincrypt.h
			0, 0, &hHash ) )
		{
			if( CryptHashData( hHash, pbContent, cbContent, 0 ) )
			{
				if( CryptGetHashParam( hHash, HP_HASHVAL, bHash, &dwHashLen, 0 ) )
				{
					// Make a string version of the numeric digest value
					char tmp[4];
					for( int i = 0; i < 16; i++ )
					{
						sprintf( tmp, "%02x", bHash[i] );
						strcat( out, tmp );
					}
				}
			}
		}
    }
	CryptDestroyHash( hHash );
	CryptReleaseContext( hCryptProv, 0 );
}

void md5( char* out, const char* in )
{
	DWORD cbContent		= lstrlen( in );
	BYTE* pbContent	= (BYTE*)in;
	md5( out, pbContent, cbContent );
}

void MakeTutorialStateString( char * szOut, int nState, const char* szKey )
{
	char szIn[128]	= { 0,};
	sprintf( szIn, "%s%d", szKey, nState );
	md5( szOut, szIn );
}