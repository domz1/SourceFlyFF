//////////////////////////////////////////////////////////////////////////////
/// \file MiniDumper.h
/// \author excel96
/// \date 2003.11.18
//////////////////////////////////////////////////////////////////////////////

#ifndef __MINIDUMPER_H__
#define __MINIDUMPER_H__

#ifdef __MINIDUMP

/// 덤프할 데이터의 수준
enum DumpLevel
{
    DUMP_LEVEL_0, ///< MiniDumpNormal을 사용
    DUMP_LEVEL_1, ///< MiniDumpWithDataSegs를 사용
    DUMP_LEVEL_2  ///< MiniDumpWithFullMemory를 사용
};

extern void InitDumper( DumpLevel level );
extern LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExPtr);
#endif // __MINIDUMP

#endif //__MINIDUMPER_H__