//
// FILE: CMclLinkedLists.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLLINKEDLISTS_H__
#define __CMCLLINKEDLISTS_H__

#include "CMclGlobal.h"
#include "CMclEvent.h"
#include "CMclCritSec.h"

// this linked list template class can be instantiated for
// any type which has an accessable copy constructor and
// assignment operator...
template <class T>
class CMclLinkedList {
protected:
    // abstract node class...
    class CMclLinkedListNode {
    public:
        CMclLinkedListNode *m_pNext;
        CMclLinkedListNode *m_pPrev;
        virtual void SetData( T & rData) = 0;
        virtual void GetData( T & rData) = 0;
    };

    // derived class for master and free list nodes...
    class CMclLinkedListSentinelNode : public CMclLinkedListNode {
    public:
        virtual void SetData( T & rData) {
            // this function should never be called...
            CMclThrowError(ERROR_ACCESS_DENIED);
        };
        virtual void GetData( T & rData) {
            // this function should never be called...
            CMclThrowError(ERROR_ACCESS_DENIED);
        };
    };

    // derived class for data nodes...
    class CMclLinkedListDataNode : public CMclLinkedListNode {
    public:
        T m_data;

        CMclLinkedListDataNode( T & rData) : m_data(rData) {
            return;
        };

        virtual void SetData( T & rData) {
            m_data = rData;
        };
        virtual void GetData( T & rData) {
            rData = m_data;
        };
    };

    CMclLinkedListSentinelNode m_MasterNode;
    CMclLinkedListSentinelNode m_FreeNode;
    CMclCritSec m_cCritSec;
    CMclEvent m_cNotEmpty;
    DWORD m_dwStatus;

public:
    CMclLinkedList() : m_dwStatus(NO_ERROR), m_cNotEmpty( TRUE, FALSE), m_cCritSec() {
        CMCL_CHECK_CREATION_STATUS(m_cNotEmpty.Status(), m_dwStatus);
        // both the master and free nodes point to themselves
        // to indicate that the lists are empty...
        m_MasterNode.m_pNext = m_MasterNode.m_pPrev = &m_MasterNode;
        m_FreeNode.m_pNext = m_FreeNode.m_pPrev = &m_FreeNode;
    };

    ~CMclLinkedList() {
        Cleanup();
    };

    BOOL PutOnHeadOfList( T & rData) {
        // acquire the list critical section lock...
        CMclAutoLock autoLock(m_cCritSec);

        // get a free list node and attach the data to it...
        CMclLinkedListNode *pNewNode = AllocateListNode(rData);
        if (pNewNode == NULL) {
            // this is a memory allocation failure...
            CMclThrowError(ERROR_OUTOFMEMORY);
            return FALSE;
        }

        // put the node at the head of the list...
        pNewNode->m_pNext = m_MasterNode.m_pNext;
        m_MasterNode.m_pNext->m_pPrev = pNewNode;
        pNewNode->m_pPrev = &m_MasterNode;
        m_MasterNode.m_pNext = pNewNode;

        // signal the event which indicates that the list
        // has at least one data element...
        m_cNotEmpty.Set();
        return TRUE;
    };

    BOOL PutOnTailOfList( T & rData) {
        // acquire the list critical section lock...
        CMclAutoLock autoLock(m_cCritSec);

        // get a free list node and attach the data to it...
        CMclLinkedListNode *pNewNode = AllocateListNode(rData);
        if (pNewNode == NULL) {
            // this is a memory allocation failure...
            CMclThrowError(ERROR_OUTOFMEMORY);
            return FALSE;
        }

        // put the node at the tail of the list...        
        m_MasterNode.m_pPrev->m_pNext = pNewNode;
        pNewNode->m_pPrev = m_MasterNode.m_pPrev;
        m_MasterNode.m_pPrev = pNewNode;
        pNewNode->m_pNext = &m_MasterNode;

        // signal the event which indicates that the list
        // has at least one data element...
        m_cNotEmpty.Set();
        return TRUE;
    };
    
    BOOL GetFromHeadOfList( T & rData, DWORD dwTimeout, CMclEvent *pInterrupt = NULL) {
        // wait until there is an element on the list or the
        // interrupt event is signaled...
        if (pInterrupt) {
            m_dwStatus = pInterrupt->WaitForTwo( m_cNotEmpty, FALSE, dwTimeout);
            if (!CMclWaitSucceeded(m_dwStatus, 2))
                return FALSE;
            if (CMclWaitSucceededIndex(m_dwStatus) == 0)
                return FALSE;
        }
        else {
            m_dwStatus = m_cNotEmpty.Wait(dwTimeout);
            if (!CMclWaitSucceeded(m_dwStatus, 1))
                return FALSE;
        }

        // acquire the list critical section lock...
        CMclAutoLock autoLock(m_cCritSec);

        // take the node off the head of the list...
        CMclLinkedListNode *pNode = m_MasterNode.m_pNext;        
        m_MasterNode.m_pNext = pNode->m_pNext;
        pNode->m_pNext->m_pPrev = &m_MasterNode;

        // reset the not empty event if the list is now empty...
        if (m_MasterNode.m_pNext == &m_MasterNode)
            m_cNotEmpty.Reset();

        // copy the data from the list node...
        pNode->GetData(rData);

        // add the list node to the free list...
        AddToFreeList(pNode);

        // all done...
        return TRUE;
    };

    BOOL GetFromTailOfList( T & rData, DWORD dwTimeout, CMclEvent *pInterrupt = NULL) {
        // wait until there is an element on the list or the
        // interrupt event is signaled...
        if (pInterrupt) {
            m_dwStatus = pInterrupt->WaitForTwo( m_cNotEmpty, FALSE, dwTimeout);
            if (!CMclWaitSucceeded(m_dwStatus, 2))
                return FALSE;
            if (CMclWaitSucceededIndex(m_dwStatus) == 0)
                return FALSE;
        }
        else {
            m_dwStatus = m_cNotEmpty.Wait(dwTimeout);
            if (!CMclWaitSucceeded(m_dwStatus, 1))
                return FALSE;
        }

        // acquire the list critical section lock...
        CMclAutoLock autoLock(m_cCritSec);

        // take the node off the tail of the list...
        CMclLinkedListNode *pNode = m_MasterNode.m_pPrev;        
        m_MasterNode.m_pPrev = pNode->m_pPrev;
        pNode->m_pPrev->m_pNext = &m_MasterNode;

        // reset the not empty event if the list is now empty...
        if (m_MasterNode.m_pNext == &m_MasterNode)
            m_cNotEmpty.Reset();

        // copy the data from the list node...
        pNode->GetData(rData);

        // add the list node to the free list...
        AddToFreeList(pNode);

        // all done...
        return TRUE;
    };

    DWORD Status(void) {
        return m_dwStatus;
    };

protected:
    void AddToFreeList(CMclLinkedListNode *pFreeNode) {
        // attach node to the end of the free list...
        m_FreeNode.m_pPrev->m_pNext = pFreeNode;
        pFreeNode->m_pPrev = m_FreeNode.m_pPrev;
        m_FreeNode.m_pPrev = pFreeNode;
        pFreeNode->m_pNext = &m_FreeNode;
    };

    CMclLinkedListNode *AllocateListNode( T & rData) {
        // grab a node off the free list, or create
        // a new one if none are available...
        CMclLinkedListNode *pNode = m_FreeNode.m_pNext;
        if (pNode != &m_FreeNode) {
            pNode->m_pPrev->m_pNext = pNode->m_pNext;
            pNode->m_pNext->m_pPrev = pNode->m_pPrev;
            pNode->m_pPrev = pNode;
            pNode->m_pNext = pNode;
            pNode->SetData(rData);
        }
        else {
            pNode = new CMclLinkedListDataNode(rData);
        }
        return pNode;
    };

    void Cleanup(void) {
        // delete all of the list nodes on the master list...
        CMclLinkedListNode *pNode = m_MasterNode.m_pNext;
        while (pNode != &m_MasterNode) {
            CMclLinkedListNode *pOldNode = pNode;
            pNode = pNode->m_pNext;
            safe_delete( pOldNode );
        }

        // delete all of the list nodes on the free list...
        pNode = m_FreeNode.m_pNext;
        while (pNode != &m_FreeNode) {
            CMclLinkedListNode *pOldNode = pNode;
            pNode = pNode->m_pNext;
            safe_delete( pOldNode );
        }
    };
};

template <class T>
class CMclQueue : private CMclLinkedList<T> {
public:
    BOOL Put( T & rData) {
        return PutOnTailOfList(rData);
    };

    BOOL Get( T & rData, DWORD dwTimeout = INFINITE, CMclEvent *pInterrupt = NULL) {
        return GetFromHeadOfList( rData, dwTimeout, pInterrupt);
    };

    DWORD Status(void) {
        return CMclLinkedList<T>::Status();
    }
};

template <class T>
class CMclStack : private CMclLinkedList<T> {
public:
    BOOL Push( T & rData) {
        return PutOnHeadOfList(rData);
    };

    BOOL Pop( T & rData, DWORD dwTimeout = INFINITE, CMclEvent *pInterrupt = NULL) {
        return GetFromHeadOfList( rData, dwTimeout, pInterrupt);
    };

    DWORD Status(void) {
        return CMclLinkedList<T>::Status();
    }
};

#endif
