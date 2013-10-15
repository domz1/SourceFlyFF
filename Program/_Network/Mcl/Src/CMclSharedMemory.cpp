//
// FILE: CMclSharedMemory.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////

#include "CMclSharedMemory.h"

CMclSharedMemory::CMclSharedMemory( LONG lSize, LPCTSTR lpName, 
                                   LPSECURITY_ATTRIBUTES lpSharedMemoryAttributes) : 
                                                                    m_lpSharedMemory(NULL),
                                                                    m_bIsCreator(FALSE),
                                                                    m_dwStatus(NO_ERROR) {
    m_hMapping = ::CreateFileMapping( (HANDLE) 0xFFFFFFFF, lpSharedMemoryAttributes, PAGE_READWRITE, 0, lSize, lpName);
    if (CMclIsValidHandle(m_hMapping)) {
        m_dwStatus = GetLastError();
        if (m_dwStatus == ERROR_ALREADY_EXISTS) {
            m_bIsCreator = FALSE;
        }
        else {
            m_bIsCreator = TRUE;
        }
    }
    else {
        m_dwStatus = ::GetLastError();
        CMclThrowError(m_dwStatus);
    }

    m_lpSharedMemory = (void *) ::MapViewOfFile( m_hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    
    if (m_lpSharedMemory == NULL) {
        // unable to map memory into address space, perhaps our address space is full?
        // this is a fatal error, close handle and throw exeption
        m_dwStatus = ::GetLastError();
        CMclThrowError(m_dwStatus);
    }
}

CMclSharedMemory::CMclSharedMemory( LPCTSTR lpName) : m_lpSharedMemory(NULL),
                                                                  m_bIsCreator(FALSE),
                                                                  m_dwStatus(NO_ERROR) {
    m_hMapping = ::OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, lpName);
    if (CMclIsValidHandle(m_hMapping)) {
        m_dwStatus = NO_ERROR;
    }
    else {
        m_dwStatus = ::GetLastError();
        return;
    }

    m_lpSharedMemory = (void *) ::MapViewOfFile( m_hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    
    if (m_lpSharedMemory == NULL) {
        // unable to map memory into address space, perhaps our address space is full?
        m_dwStatus = ::GetLastError();
    }
}
    
CMclSharedMemory::~CMclSharedMemory() {
    if (m_lpSharedMemory) {
        ::UnmapViewOfFile(m_lpSharedMemory);
        m_lpSharedMemory = NULL;
    }

    // close the file mapping handle...
    if (m_hMapping) {
        ::CloseHandle(m_hMapping);
    }
}

DWORD CMclSharedMemory::Status(void) const {
    return m_dwStatus;
}

void * CMclSharedMemory::GetPtr(void) {
    return m_lpSharedMemory;
}

BOOL CMclSharedMemory::IsCreator(void) {
    return m_bIsCreator;
}

