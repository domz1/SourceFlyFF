//
// FILE: CMclMailbox.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLMAILBOX_H__
#define __CMCLMAILBOX_H__

#include "CMclGlobal.h"
#include "CMclWaitableCollection.h"
#include "CMclSharedMemory.h"
#include "CMclMutex.h"
#include "CMclSemaphore.h"
#include "CMclAutoPtr.h"

class CMclMailbox {
private:
    struct MailboxHdr {
        DWORD dwMaxDepth;
        DWORD cbMsgSize;
        DWORD dwBaseOffset;
        DWORD dwHeadIndex;
        DWORD dwTailIndex;
    };

private:
    CMclSharedMemory m_cSharedMemory;
    MailboxHdr *m_pHdr;
    void *m_pBase;
    CMclMutexAutoPtr m_cGuardMutexAPtr;
    CMclSemaphoreAutoPtr m_cFreeCountSemaphoreAPtr;
    CMclSemaphoreAutoPtr m_cPendingCountSemaphoreAPtr;
    DWORD m_dwStatus;
    BOOL m_bIsCreator;

public:
    // create the mailbox if it doesn't exist and open it...
    CMclMailbox( DWORD dwMaxDepth, DWORD cbMsgSize, LPCTSTR lpszName = NULL, LPSECURITY_ATTRIBUTES lpMailboxAttributes = NULL);
    
    // just open the mailbox...
    CMclMailbox( LPCTSTR lpszName);
    
    // ensure that the destructor is virtual for derived classes...
    virtual ~CMclMailbox();

    // query for status after construction if exceptions are disabled...
    DWORD Status(void) const;

    // determines if this object created the mailbox...
    BOOL IsCreator(void);
    
    // read the message size and depth properties of the mailbox...
    BOOL GetProperties( LPDWORD lpdwDepth, LPDWORD lpcbMsgSize);
    
    // post a message to the mailbox, wait until there is a free slot
    // or the timeout expires...
    BOOL Post( const void *lpMsg, DWORD dwTimeout = INFINITE);

    // post a message to the mailbox, wait until a message can be written,
    // return FALSE immediately without posting if the pInterrupt event is signaled...
    BOOL PostAlertable( const void *lpMsg, CMclEvent *pInterrupt, DWORD dwTimeout = INFINITE);

    // post a message to the mailbox, wait until the message can be posted,
    // or any of the objects in the collection is signaled, or the
    // timeout expires...
    // WAIT_OBJECT_0 is the mailbox, with the waitable objects in the collection
    // being (WAIT_OBJECT_0 + 1) to (WAIT_OBJECT_0 + rCollection.GetCount())...
    DWORD PostAlertable( const void *lpMsg, const CMclWaitableCollection & rCollection, DWORD dwTimeout = INFINITE);

    // get a message from the mailbox, wait until there is a message
    // or the timeout expires...
    BOOL Get( void *lpMsg, DWORD dwTimeout = INFINITE);
    
    // get a message from the mailbox, wait until a message can be read,
    // return FALSE immediately without getting if the pInterrupt event is signaled...
    BOOL GetAlertable( void *lpMsg, CMclEvent *pInterrupt, DWORD dwTimeout = INFINITE);

    // get a message from the mailbox, wait until a message can be read,
    // or any of the objects in the collection is signaled, or the
    // timeout expires...
    // WAIT_OBJECT_0 is the mailbox, with the waitable objects in the collection
    // being (WAIT_OBJECT_0 + 1) to (WAIT_OBJECT_0 + rCollection.GetCount())...
    DWORD GetAlertable( void *lpMsg, const CMclWaitableCollection & rCollection, DWORD dwTimeout = INFINITE);

private:
    inline void IncrementHead(void);
    inline void IncrementTail(void);
    inline void *GetHeadPtr(void);
    inline void *GetTailPtr(void);
    void CreateGuardMutexName( LPTSTR lpszName, LPCTSTR lpszBasename);
    void CreateFreeCountSemaphoreName( LPTSTR lpszName, LPCTSTR lpszBasename);
    void CreatePendingCountSemaphoreName( LPTSTR lpszName, LPCTSTR lpszBasename);
};

#endif


