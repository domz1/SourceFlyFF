#ifndef __TICKET_H__
#define	__TICKET_H__

typedef	struct	_TicketProp
{
	DWORD	dwWorldId;
	D3DXVECTOR3	vPos;
#ifdef __PAID_SET
	char	szWorldText[64];
#endif
}	TicketProp, *PTicketProp;

#ifdef __AZRIA_1023
typedef struct	_LayerStruct
{
	DWORD dwWorldId;
	int		nExpand;
}	LayerStruct;
typedef vector<LayerStruct>	VLS;
class CLayerProperty
{
public:
	CLayerProperty();
	virtual ~CLayerProperty();
	BOOL	LoadScript();
	int	GetExpanedLayer( DWORD dwWorldId );
private:
	VLS	m_vLayers;
};
#endif	// __AZRIA_1023

class CTicketProperty
{
public:
	CTicketProperty();
	virtual	~CTicketProperty();
	static	CTicketProperty*	GetInstance();
	TicketProp*	GetTicketProp( DWORD dwItemId );
	BOOL	IsTarget( DWORD dwWorldId );
	BOOL	LoadScript();
#ifdef __AZRIA_1023
	int		GetExpanedLayer( DWORD dwWorldId )	{	return m_lp.GetExpanedLayer( dwWorldId );	}
#endif	// __AZRIA_1023
private:
	map<DWORD, TicketProp>	m_mapTicketProp;
#ifdef __PAID_SET
	map<DWORD,DWORD>	m_mapTicketLink;
#endif
#ifdef __AZRIA_1023
	CLayerProperty	m_lp;
#endif // __AZRIA_1023
};
#endif	// __TICKET_H__
