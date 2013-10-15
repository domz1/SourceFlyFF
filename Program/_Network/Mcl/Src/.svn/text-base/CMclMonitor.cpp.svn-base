//
// FILE: CMclMonitor.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////

#include "CMclMonitor.h"

//////////////////////////////////////////////////
// CMclCondition abstract base class            //
//////////////////////////////////////////////////

// constructor initializes an auto-reset event...
CMclCondition::CMclCondition() : m_cEvent(FALSE) {
    return;
}

// virtual destructor does nothing, is simply a placeholder
CMclCondition::~CMclCondition() {
    return;
}

void CMclCondition::WaitForEvent(void) {
    m_cEvent.Wait(INFINITE);
}

void CMclCondition::SignalEvent(void) {
    // it is very important the the event be set here and not
    // pulsed. there is a small time window inside WaitForCondition()
    // between where the event might be signaled before the thread
    // calls WaitForEvent(). anyway, this is an auto-reset event so the
    // signal state will be cleared after the thread resumes...
    m_cEvent.Set();
}

/////////////////////////////////////////////////////
// CMclMonitor::CMclConditionNode utility class    //
/////////////////////////////////////////////////////
CMclMonitor::CMclConditionNode::CMclConditionNode( CMclCondition *pcCondition) {
    m_pcCondition = pcCondition;
    m_pcnNext = NULL;
    m_pcnPrev = NULL;
}

///////////////////////////
// CMclMonitor class     //
///////////////////////////
CMclMonitor::CMclMonitor() : m_cnMaster(NULL), m_cnFree(NULL), 
                            m_csMutex(1,1), m_dwStatus(NO_ERROR) {
    CMCL_CHECK_CREATION_STATUS(m_csMutex.Status(), m_dwStatus);
    m_cnMaster.m_pcnNext = m_cnMaster.m_pcnPrev = &m_cnMaster;
    m_cnFree.m_pcnNext = m_cnFree.m_pcnPrev = &m_cnFree;
    return;
}

CMclMonitor::~CMclMonitor() {
    Cleanup();
}

void CMclMonitor::Enter(void) {
    m_csMutex.Wait(INFINITE);
}

void CMclMonitor::Leave(void) {
    LeaveAndScanConditions();
}

BOOL CMclMonitor::WaitForCondition( CMclCondition *pcCondition) {
    if (pcCondition->Condition() == FALSE) {
        // the condition is FALSE, so we'll put the
        // condition on the monitor list...
        
        // create a new list node...
        CMclConditionNode *pNewNode = AllocateNodeForCondition(pcCondition);
        if (pNewNode == NULL) {
            // this is a memory allocation failure...
            CMclThrowError(ERROR_OUTOFMEMORY);
            return FALSE;
        }

        // put the node on the list...
        AppendConditionNode(pNewNode);

        // leave the monitor by resuming a waiting
        // thread or just releasing the semaphore...
        // this could cause other threads to execute
        // which might release our condition event
        // before we even get a chance to wait for it,
        // therefore the event must be set and not
        // pulsed in CMclCondition::SignalEvent()...
        LeaveAndScanConditions();

        // wait for the event...
        pcCondition->WaitForEvent();
    }
    
    return TRUE;
}

void CMclMonitor::LeaveAndScanConditions(void) {
    // traverse the list of CMclConditionNodes,
    // looking for a condition which is TRUE...
    CMclConditionNode *pNode = m_cnMaster.m_pcnNext;
    while (pNode != &m_cnMaster) {
        if (pNode->m_pcCondition->Condition()) {
            break;
        }
        pNode = pNode->m_pcnNext;
    }

    if (pNode != &m_cnMaster) {
        // we have found a node with a true condition,
        // remove the node and signal the condition...
        CMclCondition *pcCondition = pNode->m_pcCondition;
        RemoveConditionNode(pNode);
        AddToFreeList(pNode);
        pcCondition->SignalEvent();
    }
    else {
        // no nodes are ready to be signaled, so just
        // release the mutual exclusion semaphore...
        m_csMutex.Release(1);
    }
}

void CMclMonitor::AppendConditionNode( CMclConditionNode *pcnNewNode) {
    m_cnMaster.m_pcnPrev->m_pcnNext = pcnNewNode;
    pcnNewNode->m_pcnPrev = m_cnMaster.m_pcnPrev;
    m_cnMaster.m_pcnPrev = pcnNewNode;
    pcnNewNode->m_pcnNext = &m_cnMaster;
}

void CMclMonitor::RemoveConditionNode( CMclConditionNode *pcnOldNode) {
    pcnOldNode->m_pcnPrev->m_pcnNext = pcnOldNode->m_pcnNext;
    pcnOldNode->m_pcnNext->m_pcnPrev = pcnOldNode->m_pcnPrev;
}

void CMclMonitor::AddToFreeList(CMclConditionNode *pcnFreeNode) {
    // reinitialize the node's condition...
    pcnFreeNode->m_pcCondition = NULL;

    // add the node to the free list...
    m_cnFree.m_pcnPrev->m_pcnNext = pcnFreeNode;
    pcnFreeNode->m_pcnPrev = m_cnFree.m_pcnPrev;
    m_cnFree.m_pcnPrev = pcnFreeNode;
    pcnFreeNode->m_pcnNext = &m_cnFree;
}

CMclMonitor::CMclConditionNode *CMclMonitor::AllocateNodeForCondition(CMclCondition *pcCondition) {
    CMclConditionNode *pNode = m_cnFree.m_pcnNext;
    if (pNode != &m_cnFree) {
        // if there are any nodes on the list,
        // remove the first one from the list to use...
        pNode->m_pcnPrev->m_pcnNext = pNode->m_pcnNext;
        pNode->m_pcnNext->m_pcnPrev = pNode->m_pcnPrev;
        pNode->m_pcCondition = pcCondition;
        pNode->m_pcnPrev = pNode;
        pNode->m_pcnNext = pNode;
    }
    else {
        // otherwise allocate a new one...
        pNode = new CMclConditionNode(pcCondition);
    }

    // return the initialized node...
    return pNode;
}

void CMclMonitor::Cleanup(void) {
    // traverse the list of active condition nodes,
    // deleting each node...
    CMclConditionNode *pNode = m_cnMaster.m_pcnNext;
    while (pNode != &m_cnMaster) {
        CMclConditionNode *pOldNode = pNode;
        pNode = pNode->m_pcnNext;
        safe_delete( pOldNode );
    }

    // traverse the list of free nodes,
    // deleting each node...
    pNode = m_cnFree.m_pcnNext;
    while (pNode != &m_cnFree) {
        CMclConditionNode *pOldNode = pNode;
        pNode = pNode->m_pcnNext;
        safe_delete( pOldNode );
    }
}

DWORD CMclMonitor::Status(void) const {
    return m_dwStatus;
}




