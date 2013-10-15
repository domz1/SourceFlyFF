#ifndef __SERIAL_NUMBER_H__
#define	__SERIAL_NUMBER_H__

class	CSerialNumber
{
private:
	LONG	m_lLowPart;
public:
// constructions
	CSerialNumber()	{	m_lLowPart	= 0;	}
	virtual	~CSerialNumber()	{}
// operations
	void	Init( int nServer, int nMulti );
	SERIALNUMBER	Get( void );
	static	CSerialNumber*	GetInstance( void );
};

#endif	// __SERIAL_NUMBER_H__