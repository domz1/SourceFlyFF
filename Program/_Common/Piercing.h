#pragma once

/* CmnHdr.h∑Œ ø≈∞Â¿Ω!!
#define MAX_PIERCING_SUIT		4
#define MAX_PIERCING_WEAPON		10
#define	MAX_PIERCING_ULTIMATE	5
#define	MAX_PIERCING			15
*/
class CPiercing
{
public:
	CPiercing();
	virtual	~CPiercing();
	CPiercing&	operator=( const CPiercing & piercing );
	virtual	void	Serialize( CAr & ar );
	void	SetPiercingSize( int nSize );
	int		GetPiercingSize();
	void	SetPiercingItem( int nth, DWORD dwItem );
	DWORD	GetPiercingItem( int nth );
	void	Clear( void );
	BOOL	IsPiercedItem();
	
private:
	vector<DWORD>	m_vPiercing;

#if __VER >= 12 // __EXT_PIERCING
public:
	void	SetUltimatePiercingSize( int nSize );
	int		GetUltimatePiercingSize();
	void	SetUltimatePiercingItem( int nth, DWORD dwItem );
	DWORD	GetUltimatePiercingItem( int nth );

private:
	vector<DWORD> m_vUltimatePiercing;
#endif // __EXT_PIERCING

#if __VER >= 15 // __PETVIS
	vector<time_t>	m_vtmPetVis;
public:
	void	SetVisKeepTimeSize( int nSize )	{ m_vtmPetVis.resize( min( nSize, MAX_VIS ), 0 ); }
	void	SetVisKeepTime( int nth, time_t tmKeep );
	time_t	GetVisKeepTime( int nth );
#endif // __PETVIS
};
