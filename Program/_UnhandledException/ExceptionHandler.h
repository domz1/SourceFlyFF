#ifndef __EXCEPTION_HANDLER_H__
#define __EXCEPTION_HANDLER_H__

#ifndef _DEBUG
	void	InitEH( void );
	void	_AddErrorMsg( LPCTSTR str, LPCTSTR szHead = "lux-" );
#endif // _DEBUG

#if defined(__CLIENT ) 
	#ifdef _DEBUG
		#define ADDERRORMSG(A)			((void)0)				// 디버그 모드일땐 무시
		#define ADDERRORMSG_NOHEAD(A)	((void)0)				
	#else	// _DEBUG
		#define ADDERRORMSG(A)			_AddErrorMsg( A )		// 릴리즈일때 실행
		#define ADDERRORMSG_NOHEAD(A)	_AddErrorMsg( A, "" )	
	#endif	// _DEBUG
#else
	 #define ADDERRORMSG(A)				((void)0)	
	 #define ADDERRORMSG_NOHEAD(A)		((void)0)
#endif

#endif // __EXCEPTION_HANDLER_H__