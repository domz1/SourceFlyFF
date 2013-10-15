/***************************************************************************************************
**
** profile.h
**
** Real-Time Hierarchical Profiling for Game Programming Gems 3
**
** by Greg Hjelstrom & Byon Garrabrant
**
***************************************************************************************************/

/*
** A node in the Profile Hierarchy Tree
*/
class	CProfileNode {

public:
	CProfileNode( const char * name, CProfileNode * parent );
	~CProfileNode( void );

	CProfileNode * Get_Sub_Node( const char * name );

	CProfileNode * Get_Parent( void )		{ return Parent; }
	CProfileNode * Get_Sibling( void )		{ return Sibling; }
	CProfileNode * Get_Child( void )			{ return Child; }

	void				Reset( void );
	void				Call( void );
	bool				Return( void );

	const char *	Get_Name( void )				{ return Name; }
	int				Get_Total_Calls( void )		{ return TotalCalls; }
	float				Get_Total_Time( void )		{ return TotalTime; }

protected:

	const char *	Name;
	int				TotalCalls;
	float				TotalTime;
	__int64			StartTime;
	int				RecursionCounter;

	CProfileNode *	Parent;
	CProfileNode *	Child;
	CProfileNode *	Sibling;
};

/*
** An iterator to navigate through the tree
*/
class CProfileIterator
{
public:
	// Access all the children of the current parent
	void				First(void);
	void				Next(void);
	bool				Is_Done(void);
	void				NextPreOrder(void);
	unsigned long		GetPreOrderDepth(void) { return m_nPrefixDepth; }
		
	void				Enter_Child( int index );		// Make the given child the new parent
	void				Enter_Largest_Child( void );	// Make the largest child the new parent
	void				Enter_Parent( void );			// Make the current parent's parent the new parent

	// Access the current child
	const char *	Get_Current_Name( void )			{ return CurrentChild->Get_Name(); }
	int				Get_Current_Total_Calls( void )	{ return CurrentChild->Get_Total_Calls(); }
	float				Get_Current_Total_Time( void )	{ return CurrentChild->Get_Total_Time(); }

	// Access the current parent
	const char *	Get_Current_Parent_Name( void )			{ return CurrentParent->Get_Name(); }
	int				Get_Current_Parent_Total_Calls( void )	{ return CurrentParent->Get_Total_Calls(); }
	float				Get_Current_Parent_Total_Time( void )	{ return CurrentParent->Get_Total_Time(); }

	CProfileNode* GetCurrentNode() { return CurrentChild; }

private:
	unsigned long	m_nPrefixDepth;

protected:

	CProfileNode *	CurrentParent;
	CProfileNode *	CurrentChild;

	CProfileIterator( CProfileNode * start );
	friend	class		CProfileManager;
};


/*
** The Manager for the Profile system
*/
class	CProfileManager {
public:
	static	void Start_Profile( const char * name );
	static	void Stop_Profile( void );

	static	void Reset( void );
	static	void Increment_Frame_Counter( void );
	static	int Get_Frame_Count_Since_Reset( void )		{ return FrameCounter; }
	static	float Get_Time_Since_Reset( void );

	static	CProfileIterator *	Get_Iterator( void )	{ return new CProfileIterator( &Root ); }
	static	void Release_Iterator( CProfileIterator * iterator ) { delete iterator; }

private:
	static	CProfileNode			Root;
	static	CProfileNode *			CurrentNode;
	static	int						FrameCounter;
	static	__int64					ResetTime;
};


/*
** ProfileSampleClass is a simple way to profile a function's scope
** Use the PROFILE macro at the start of scope to time
*/
class	CProfile {
public:
	CProfile( const char * name )
	{ 
		CProfileManager::Start_Profile( name ); 
	}
	
	~CProfile( void )					
	{ 
		CProfileManager::Stop_Profile(); 
	}
};

#ifdef __NEW_PROFILE
#define	_PROFILE( name )			CProfile __profile( name )
#else // __NEW_PROFILE
#define	_PROFILE( name )
#endif // __NEW_PROFILE


