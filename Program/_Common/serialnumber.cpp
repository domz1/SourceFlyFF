#include "stdafx.h"
#include "serialnumber.h"

CSerialNumber*	CSerialNumber::GetInstance( void )
{
	static	CSerialNumber	sSerialNumber;
	return &sSerialNumber;
}

void CSerialNumber::Init( int nServer, int nMulti )
{
	m_lLowPart	= ( ( nServer << 28 ) & 0xf0000000 ) 
							| ( ( nMulti << 24 ) & 0x0f000000 );
}

SERIALNUMBER	CSerialNumber::Get( void )
{
	return xRand();
}