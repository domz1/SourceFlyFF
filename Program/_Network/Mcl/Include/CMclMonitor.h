//
// FILE: CMclMonitor.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __CMCLMONITOR_H__
#define __CMCLMONITOR_H__

#include "CMclGlobal.h"
#include "CMclEvent.h"
#include "CMclSemaphore.h"

class CMclCondition {
    friend class CMclMonitor;

private:
    CMclEvent m_cEvent;

public:
    CMclCondition();
    virtual ~CMclCondition();

private:
    // wait on the internal event...
    void WaitForEvent(void);

    // signal the internal event...
    void SignalEvent(void);

    // this needs to be overloaded for each condition...
    virtual BOOL Condition(void) = 0;    
};

class CMclMonitor {
private:
    // private internal linked list class for tracking
    // the outstanding condition objects...
    class CMclConditionNode {
    public: 
        CMclCondition *m_pcCondition;
        CMclConditionNode *m_pcnNext;
        CMclConditionNode *m_pcnPrev;
    
    public:
        CMclConditionNode(CMclCondition *pcCondition);
    };
    
private:
    // master monitor gatekeeper semaphore,
    // we use a semaphore as a mutex because we need
    // the ability for a thread to release a mutex that
    // it did not wait on, and Win32 Mutexes are owned and
    // prohibit this behaviour...
    CMclSemaphore m_csMutex;

    // condition list master node...
    CMclConditionNode m_cnMaster;

    // free list master node...
    CMclConditionNode m_cnFree;

    // constructor status...
    DWORD m_dwStatus;

public:
    CMclMonitor();
    
    // virtual destructor does nothing, is simply a placeholder
    // for derived classes...
    virtual ~CMclMonitor();

    // procedures to control access to the monitor...
    void Enter(void);
    void Leave(void);
    BOOL WaitForCondition( CMclCondition *pcCondition);

    // query for status after construction if exceptions are disabled...
    DWORD Status(void) const;

private:
    void LeaveAndScanConditions(void);
    void AppendConditionNode( CMclConditionNode *pcnNewNode);
    void RemoveConditionNode( CMclConditionNode *pcnOldNode);
    void AddToFreeList(CMclConditionNode *pcnFreeNode);
    CMclConditionNode *AllocateNodeForCondition(CMclCondition *pcCondition);
    void Cleanup(void);
};

#endif