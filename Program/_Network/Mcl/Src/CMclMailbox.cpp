//
// FILE: CMclMailbox.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////

#include "CMclMailbox.h"

CMclMailbox::CMclMailbox( DWORD dwMaxDepth, DWORD cbMsgSize, LPCTSTR lpszName, 
                            LPSECURITY_ATTRIBUTES lpMailboxAttributes) :
                                    m_cSharedMemory( dwMaxDepth * cbMsgSize + sizeof(MailboxHdr), lpszName, lpMailboxAttributes),
                                    m_pHdr(NULL),
                                    m_pBase(NULL),
                                    m_cGuardMutexAPtr(NULL),
                                    m_cFreeCountSemaphoreAPtr(NULL),
                                    m_cPendingCountSemaphoreAPtr(NULL),
                                    m_dwStatus(NO_ERROR),
                                    m_bIsCreator(FALSE) {
    // temporary buffer for our names...
    TCHAR szName[MAX_PATH];

    // check the status of the shared memory object in case
    // exceptions are disabled...
    CMCL_CHECK_CREATION_STATUS(m_cSharedMemory.Status(), m_dwStatus);

    // create the name for the guard mutex...
    CreateGuardMutexName( szName, lpszName);

    // create the guard mutex...
    // check for errors in case exceptions are disabled...
    m_cGuardMutexAPtr = new CMclMutex( TRUE, szName, lpMailboxAttributes);
    CMCL_CHECK_AUTOPTR_OBJECT(m_cGuardMutexAPtr, m_dwStatus);
    CMCL_CHECK_CREATION_STATUS(m_cGuardMutexAPtr->Status(), m_dwStatus);

    // check if we are the creator of the guard mutex...
    if (m_cGuardMutexAPtr->Status() == ERROR_ALREADY_EXISTS) {
        // in this case we don't own the mutex because we did not 
        // create it so we need to wait for it to prevent race conditions
        // with other objects created at about the same time,
        // this ensure that the creating object always finishes its
        // constructor first...
        m_cGuardMutexAPtr->Wait(INFINITE);
        m_bIsCreator = FALSE;
    }
    else {
        // we own the mutex now...
        m_bIsCreator = TRUE;
    }
    
    // create the name for the free count semaphore...
    CreateFreeCountSemaphoreName( szName, lpszName);

    // create the free count semaphore...
    // check for errors in case exceptions are disabled...
    m_cFreeCountSemaphoreAPtr = new CMclSemaphore( dwMaxDepth, dwMaxDepth, szName, lpMailboxAttributes);
    CMCL_CHECK_AUTOPTR_OBJECT(m_cFreeCountSemaphoreAPtr, m_dwStatus);
    CMCL_CHECK_CREATION_STATUS(m_cFreeCountSemaphoreAPtr->Status(), m_dwStatus);

    // create the name for the pending count semaphore...
    CreatePendingCountSemaphoreName( szName, lpszName);

    // create the pending count semaphore...
    // check for errors in case exceptions are disabled...
    m_cPendingCountSemaphoreAPtr = new CMclSemaphore( 0, dwMaxDepth, szName, lpMailboxAttributes);
    CMCL_CHECK_AUTOPTR_OBJECT(m_cPendingCountSemaphoreAPtr, m_dwStatus);
    CMCL_CHECK_CREATION_STATUS(m_cPendingCountSemaphoreAPtr->Status(), m_dwStatus);
    
    // store pointer to beginning of shared memory...
    m_pHdr = (MailboxHdr *) m_cSharedMemory.GetPtr();

    // store pointer to base of mailbox slots...
    m_pBase = (void *) ((BYTE *) m_pHdr + sizeof(MailboxHdr));

    // initialize the mailbox slots if we are the creator of the shared memory...
    // if we are the creator of the shared memory, we are the creator of the
    // guard mutex and we initially own it...
    if (m_bIsCreator) {
        m_pHdr->dwMaxDepth = dwMaxDepth;
        m_pHdr->cbMsgSize = cbMsgSize;
        m_pHdr->dwBaseOffset = sizeof(MailboxHdr);
        m_pHdr->dwHeadIndex = 0;
        m_pHdr->dwTailIndex = 0;
    }

    // we need to release the guard mutex whether or not
    // we are the creator...
    m_cGuardMutexAPtr->Release();                                                                

    if (!m_bIsCreator) {
        // it is an error for a second object to try to open the
        // mailbox with different message buffer parameters, so check for this condition...
        if ((m_pHdr->dwMaxDepth != dwMaxDepth) || (m_pHdr->cbMsgSize != cbMsgSize)) {
            m_dwStatus = ERROR_INVALID_PARAMETER;
            CMclThrowError(m_dwStatus);
            return;
        }
    }
}

CMclMailbox::CMclMailbox( LPCTSTR lpszName) :   m_cSharedMemory(lpszName),
                                                m_pHdr(NULL),
                                                m_pBase(NULL),
                                                m_cGuardMutexAPtr(NULL),
                                                m_cFreeCountSemaphoreAPtr(NULL),
                                                m_cPendingCountSemaphoreAPtr(NULL),
                                                m_dwStatus(NO_ERROR),
                                                m_bIsCreator(FALSE) {
    // we are definitely NOT the creator of the mailbox here...

    // check the status of the shared memory object in case
    // exceptions are disabled...
    CMCL_CHECK_CREATION_STATUS(m_cSharedMemory.Status(), m_dwStatus);

    // temporary buffer for our names...
    TCHAR szName[MAX_PATH];
    
    // create the name for the guard mutex...
    CreateGuardMutexName( szName, lpszName);

    // create the guard mutex...
    // check for errors in case exceptions are disabled...
    m_cGuardMutexAPtr = new CMclMutex(szName);
    CMCL_CHECK_AUTOPTR_OBJECT(m_cGuardMutexAPtr, m_dwStatus);
    CMCL_CHECK_CREATION_STATUS(m_cGuardMutexAPtr->Status(), m_dwStatus);

    // to maintain synchronization with the other constructor,
    // we need to acquire the mutex before returning control
    // to the user to prevent race conditions...
    m_cGuardMutexAPtr->Wait(INFINITE);

    // create the name for the free count semaphore...
    CreateFreeCountSemaphoreName( szName, lpszName);

    // create the free count semaphore...
    // check for errors in case exceptions are disabled...
    m_cFreeCountSemaphoreAPtr = new CMclSemaphore( szName);
    CMCL_CHECK_AUTOPTR_OBJECT(m_cFreeCountSemaphoreAPtr, m_dwStatus);
    CMCL_CHECK_CREATION_STATUS(m_cFreeCountSemaphoreAPtr->Status(), m_dwStatus);

    // create the name for the pending count semaphore...
    CreatePendingCountSemaphoreName( szName, lpszName);

    // create the pending count semaphore...
    m_cPendingCountSemaphoreAPtr = new CMclSemaphore( szName);
    CMCL_CHECK_AUTOPTR_OBJECT(m_cPendingCountSemaphoreAPtr, m_dwStatus);
    CMCL_CHECK_CREATION_STATUS(m_cPendingCountSemaphoreAPtr->Status(), m_dwStatus);

    // store pointer to beginning of shared memory...
    m_pHdr = (MailboxHdr *) m_cSharedMemory.GetPtr();

    // store pointer to base of mailbox slots...
    m_pBase = (void *) ((BYTE *) m_pHdr + sizeof(MailboxHdr));

    // the creation constructor must have finished, so we
    // can release the guard mutex...
    m_cGuardMutexAPtr->Release();
}

CMclMailbox::~CMclMailbox() {
    // this is a place holder for derived classes,
    // the CMcl class objects should clean up automatically...
    return;
}

DWORD CMclMailbox::Status(void) const {
    return m_dwStatus;
}

BOOL CMclMailbox::IsCreator(void) {
    return m_bIsCreator;
}

BOOL CMclMailbox::GetProperties( LPDWORD lpdwDepth, LPDWORD lpcbMsgSize) {
    if (m_pHdr) {
        *lpdwDepth = m_pHdr->dwMaxDepth;
        *lpcbMsgSize = m_pHdr->cbMsgSize;
        return TRUE;
    }
    else {
        return FALSE;
    }
}

BOOL CMclMailbox::Post( const void *lpMsg, DWORD dwTimeout) {
    DWORD dwStatus = m_cFreeCountSemaphoreAPtr->Wait(dwTimeout);
    if (CMclWaitSucceeded(dwStatus, 1)) {
        dwStatus = m_cGuardMutexAPtr->Wait(dwTimeout);
        if (CMclWaitSucceeded(dwStatus, 1) || CMclWaitAbandoned(dwStatus, 1)) {
            CopyMemory( GetTailPtr(), lpMsg, m_pHdr->cbMsgSize);
            IncrementTail();
            m_cGuardMutexAPtr->Release();
        }
        m_cPendingCountSemaphoreAPtr->Release(1);
    }

    return (CMclWaitSucceeded(dwStatus, 1));
}

BOOL CMclMailbox::Get( void *lpMsg, DWORD dwTimeout) {
    DWORD dwStatus = m_cPendingCountSemaphoreAPtr->Wait(dwTimeout);
    if (CMclWaitSucceeded(dwStatus, 1)) {
        dwStatus = m_cGuardMutexAPtr->Wait(dwTimeout);
        if (CMclWaitSucceeded(dwStatus, 1) || CMclWaitAbandoned(dwStatus, 1)) {
            CopyMemory( lpMsg, GetHeadPtr(), m_pHdr->cbMsgSize);
            IncrementHead();
            m_cGuardMutexAPtr->Release();
        }
        m_cFreeCountSemaphoreAPtr->Release(1);
    }

    return (CMclWaitSucceeded(dwStatus, 1));
}

BOOL CMclMailbox::PostAlertable( const void *lpMsg, CMclEvent *pInterrupt, DWORD dwTimeout) {
    BOOL bStatus = FALSE;
    DWORD dwStatus = pInterrupt->WaitForTwo( *m_cFreeCountSemaphoreAPtr, FALSE, dwTimeout);
    if (CMclWaitSucceeded( dwStatus, 2)
            && (CMclWaitSucceededIndex(dwStatus) == (WAIT_OBJECT_0 + 1))) {
        dwStatus = m_cGuardMutexAPtr->Wait(dwTimeout);
        if (CMclWaitSucceeded(dwStatus, 1) || CMclWaitAbandoned(dwStatus, 1)) {
            CopyMemory( GetTailPtr(), lpMsg, m_pHdr->cbMsgSize);
            IncrementTail();
            m_cGuardMutexAPtr->Release();
            bStatus = TRUE;
        }
        m_cPendingCountSemaphoreAPtr->Release(1);
    }
    return bStatus;
}

BOOL CMclMailbox::GetAlertable( void *lpMsg, CMclEvent *pInterrupt, DWORD dwTimeout) {
    BOOL bStatus = FALSE;
    DWORD dwStatus = pInterrupt->WaitForTwo( *m_cPendingCountSemaphoreAPtr, FALSE, dwTimeout);    
    if (CMclWaitSucceeded( dwStatus, 2)
            && (CMclWaitSucceededIndex(dwStatus) == (WAIT_OBJECT_0 + 1))) {
        dwStatus = m_cGuardMutexAPtr->Wait(dwTimeout);
        if (CMclWaitSucceeded(dwStatus, 1) || CMclWaitAbandoned(dwStatus, 1)) {
            CopyMemory( lpMsg, GetHeadPtr(), m_pHdr->cbMsgSize);
            IncrementHead();
            m_cGuardMutexAPtr->Release();
            bStatus = TRUE;
        }
        m_cFreeCountSemaphoreAPtr->Release(1);
    }
    return bStatus;
}

DWORD CMclMailbox::PostAlertable( const void *lpMsg, const CMclWaitableCollection & rCollection, DWORD dwTimeout) {
    CMclWaitableCollection cCollection;
    
    cCollection.AddObject(*m_cFreeCountSemaphoreAPtr);
    cCollection.AddCollection(rCollection);
    int nObjects = cCollection.GetCount();

    DWORD dwStatus = cCollection.Wait( FALSE, dwTimeout);
 
    if (CMclWaitSucceeded( dwStatus, nObjects)
            && (CMclWaitSucceededIndex(dwStatus) == WAIT_OBJECT_0)) {
        dwStatus = m_cGuardMutexAPtr->Wait(dwTimeout);
        if (CMclWaitSucceeded(dwStatus, 1) || CMclWaitAbandoned(dwStatus, 1)) {
            CopyMemory( GetTailPtr(), lpMsg, m_pHdr->cbMsgSize);
            IncrementTail();
            m_cGuardMutexAPtr->Release();
            dwStatus = WAIT_OBJECT_0;
        }
        m_cPendingCountSemaphoreAPtr->Release(1);
    }

    return dwStatus;
}

DWORD CMclMailbox::GetAlertable( void *lpMsg, const CMclWaitableCollection & rCollection, DWORD dwTimeout) {
    CMclWaitableCollection cCollection;
    
    cCollection.AddObject(*m_cPendingCountSemaphoreAPtr);
    cCollection.AddCollection(rCollection);
    int nObjects = cCollection.GetCount();

    DWORD dwStatus = cCollection.Wait( FALSE, dwTimeout);
 
    if (CMclWaitSucceeded( dwStatus, nObjects)
            && (CMclWaitSucceededIndex(dwStatus) == WAIT_OBJECT_0)) {
        dwStatus = m_cGuardMutexAPtr->Wait(dwTimeout);
        if (CMclWaitSucceeded(dwStatus, 1) || CMclWaitAbandoned(dwStatus, 1)) {
            CopyMemory( lpMsg, GetHeadPtr(), m_pHdr->cbMsgSize);
            IncrementHead();
            m_cGuardMutexAPtr->Release();
            dwStatus = WAIT_OBJECT_0;
        }
        m_cFreeCountSemaphoreAPtr->Release(1);
    }

    return dwStatus;
}

void CMclMailbox::IncrementHead(void) {
    m_pHdr->dwHeadIndex = (m_pHdr->dwHeadIndex + 1) % m_pHdr->dwMaxDepth;
}

void CMclMailbox::IncrementTail(void) {
    m_pHdr->dwTailIndex = (m_pHdr->dwTailIndex + 1) % m_pHdr->dwMaxDepth;
}

void * CMclMailbox::GetHeadPtr(void) {
    return (void *) ((BYTE *) m_pBase + (m_pHdr->dwHeadIndex * m_pHdr->cbMsgSize));
}

void * CMclMailbox::GetTailPtr(void) {
    return (void *) ((BYTE *) m_pBase + (m_pHdr->dwTailIndex * m_pHdr->cbMsgSize));
}

void CMclMailbox::CreateGuardMutexName( LPTSTR lpszName, LPCTSTR lpszBasename) {
    wsprintf( lpszName, TEXT("%s::%s"), TEXT("MailboxGuardMutex"), lpszBasename);
}

void CMclMailbox::CreateFreeCountSemaphoreName( LPTSTR lpszName, LPCTSTR lpszBasename) {
    wsprintf( lpszName, TEXT("%s::%s"), TEXT("MailboxFreeCountSemaphore"), lpszBasename);
}

void CMclMailbox::CreatePendingCountSemaphoreName( LPTSTR lpszName, LPCTSTR lpszBasename) {
    wsprintf( lpszName, TEXT("%s::%s"), TEXT("MailboxPendingCountSemaphore"), lpszBasename);
}

