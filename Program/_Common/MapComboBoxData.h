#ifndef __MAP_COMBO_BOX_DATA_H__
#define __MAP_COMBO_BOX_DATA_H__

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
class CMapComboBoxData
{
public:
	enum Category
	{
		MAP_CATEGORY, MAP_NAME, NPC_NAME
	};

public:
	CMapComboBoxData( DWORD dwID, Category eCategory, const CString& strTitle );
	~CMapComboBoxData( void );

public:
	DWORD GetID( void ) const;
	Category GetCategory( void ) const;
	const CString& GetTitle( void ) const;
	void SetMapNumber( int nMapNumber );
	int GetMapNumber( void ) const;
	void SetPictureFileName( const CString& strPictureFileName );
	const CString& GetPictureFileName( void ) const;
	void SetMapTexture( CTexture* const pMapTexture );
	CTexture* const GetMapTexture( void );
	void SetMapMonsterInformationFileName( const CString& strMapMonsterInformationFileName );
	const CString& GetMapMonsterInformationFileName( void ) const;
	void SetLocationID( BYTE byLocationID );
	BYTE GetLocationID( void ) const;
	void SetNPCPosition( const CPoint& pointNPCPosition );
	const CPoint& GetNPCPosition( void ) const;
	void SetParentID( DWORD dwParentID );
	DWORD GetParentID( void ) const;

private:
	DWORD m_dwID;
	Category m_eCategory;
	CString m_strTitle;
	CString m_strPictureFileName;
	CTexture* m_pMapTexture;
	CString m_strMapMonsterInformationFileName;
	BYTE m_byLocationID;
	CPoint m_pointNPCPosition;
	DWORD m_dwParentID;
};
typedef vector< CMapComboBoxData* > MapComboBoxDataVector;
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#endif // __MAP_COMBO_BOX_DATA_H__
