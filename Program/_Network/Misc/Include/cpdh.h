#ifndef __CPDH_H__
#define	__CPDH_H__

#include <pdh.h>
#include <pdhmsg.h>

#define	MAX_RAW_VALUES	20

/*%Processor Time is the percentage of time that the processor is executing a non-Idle thread.
This counter was designed as a primary indicator of processor activity.
It is calculated by measuring the time that the processor spends executing the thread of the Idle process in each sample interval,
and subtracting that value from 100%. (Each processor has an Idle thread which consumes cycles when no other threads are ready to run.)
It can be viewed as the fraction of the time spent doing useful work. */
#define	CP_PROCESSOR_PROCESSOR_TIME	"\\Processor(_Total)\\% Processor Time"
/*Committed Bytes is the amount of committed virtual memory, in bytes.
(Committed memory is physical memory for which space has been reserved
on the disk paging file in case it needs to be written back to disk.) */
#define	CP_MEMORY_COMMITTED_BYTES	"\\Memory\\Committed Bytes"
/*Available Bytes is the amount of physical memory available to processes running on the computer, in bytes.
It is calculated by summing space on the Zeroed, Free, and Standby memory lists.
Free memory is ready for use; Zeroed memory is pages of memory filled with zeros to prevent later processes
from seeing data used by a previous process.
Standby memory is memory removed from a process's working set (its physical memory) on route to disk,
but is still available to be recalled. */
#define	CP_MEMORY_AVAILABLE_BYTES	"\\Memory\\Available Bytes"
#define	CP_MEMORY_AVAILABLE_KBYTES		"\\Memory\\Available KBytes"
#define	CP_MEMORY_AVAILABLE_MBYTES		"\\Memory\\Available MBytes"
/*% Committed Bytes In Use is the ratio of Memory:
Committed Bytes to the Memory: Commit Limit.
(Committed memory is physical memory in use for which space has been reserved in the paging file should it need to be written to disk.
The commit limit is determined by the size of the paging file.
If the paging file is enlarged, the commit limit increases, and the ratio is reduced.) */
#define	CP_MEMORY_COMMITTED_BYTES_IN_USE	"\\Memory\\% Committed Bytes In Use"
/*Commit Limit is the amount of virtual memory that can be committed without having to extend the paging file(s).
It is measured in bytes. (Committed memory is physical memory for which space has been reserved on the disk paging files.
There can be one paging file on each physical drive.)If the paging file(s) are be expanded, this limit increases accordingly. */
#define	CP_MEMORY_COMMIT_LIMIT	"\\Memory\\Commit Limit"

// NOTE: Find other counters using the function PdhBrowseCounters() (lookup in MSDN).
// This function was not implemented in this class.

#define	MAX_SIZE_PPDH_COUNTER	32

typedef	struct	tagPDH_COUNTER
{
	int		nIndex;		// The index of this counter, returned by AddCounter()
	LONG	lValue;		// The current value of this counter
	HCOUNTER	hCounter;	// Handle to the counter	- given to use by PDH Library
	int		nNextIndex;		// element to get the next raw value
	int		nOldestIndex;	// element containing the oldes raw value
	int		nRawCount;	// number of elements containing raw values
	PDH_RAW_COUNTER		apdhRawCounter[MAX_RAW_VALUES];	// Ring buffer to contain raw values
}	PDH_COUNTER,	*PPDH_COUNTER;

class CPdh
{
private:
	HQUERY	m_hQuery;	// the query to the pdh
	PPDH_COUNTER	m_appdhCounter[MAX_SIZE_PPDH_COUNTER];		// the current counters
	long	m_nSizeOfPdhCounter;

public:
//	Constructions
	CPdh();
	~CPdh();
//	Operations
	BOOL	OpenQuery( void );
	void	CloseQuery( void );
	int		AddCounter( const char* szFullCounterPath );
	BOOL	RemoveCounter( int npdhCounter );
	BOOL	CollectQueryData( void );
	long	GetCounterValue( int npdhCounter );
private:
	PPDH_COUNTER	GetPdhCounter( int npdhCounter );
	BOOL	UpdateValue( PPDH_COUNTER ppdhCounter );
	BOOL	UpdateRawValue( PPDH_COUNTER ppdhCounter );
};

class CPdh1: public CPdh
{
	int m_nProcessor;
	int m_nMemory;

public:
//	Constructions
	CPdh1();
	~CPdh1();
//	Operations
	long	GetProcessorTime( void );
	long	GetCommittedBytesInUse( void );
};

#endif	// __CPDH_H__
