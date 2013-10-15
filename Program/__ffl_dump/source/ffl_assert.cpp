
#include "../include/ffl_assert.h"
#include "../include/ffl_c_string.h"

#include <cstdio>
#include <cstdlib>


enum
{
	ffl_assertion_max_message	= (1024*2),
};

#define	FFL_ASSERTION_PARAM_FAILED_A		"ffl_assert failed. param is invalid."
#define	FFL_ASSERTION_FAILED_A				"Assertion failed."
#define	FFL_ASSERTION_FORMAT_A				"%s\r\n\r\nExpression: %s\r\nDescription: %s\r\n\r\nFunction: %s\r\nFile: %s\r\nLine: %d\r\n"

#define	FFL_ASSERTION_PARAM_FAILED_W		L"ffl_assert failed. param is invalid."
#define	FFL_ASSERTION_FAILED_W				L"Assertion failed."
#define	FFL_ASSERTION_FORMAT_W				L"%s\r\n\r\nExpression: %s\r\nDescription: %s\r\n\r\nFunction: %s\r\nFile: %s\r\nLine: %d\r\n"


static bool	ffl_check_assert_choice( int choice, bool * ignore_always )
{
	if( ignore_always == NULL )
	{
		return false;
	}

	if( choice == IDIGNORE )
	{
		*ignore_always = true;
		return true;
	}
	else if( choice == IDABORT )
	{
		::exit( EXIT_FAILURE );
	}

	return false;
}

bool	ffl_assert_function_a(	bool		exp,
							  const char *	exp_string,
							  const char *	description,
							  const char *	function_name,
							  const char *	file,
							  int				line,
							  bool *			ignore_always )
{
	if( exp == true )
	{
		return true;
	}

	HWND hwnd = ::GetActiveWindow();
	if( exp_string == NULL || description == NULL || function_name == NULL || file == NULL || ignore_always == NULL )
	{
		::MessageBoxA( hwnd, FFL_ASSERTION_PARAM_FAILED_A, FFL_ASSERTION_FAILED_A, MB_OK | MB_ICONERROR );
		return false;
	}

	char message[ffl_assertion_max_message] = { 0, };
	ffl_snprintf_a( message, FFL_COUNTOF( message ), FFL_ASSERTION_FORMAT_A, FFL_ASSERTION_FAILED_A, exp_string, description, function_name, file, line );
	int choice = ::MessageBoxA( hwnd, message, FFL_ASSERTION_FAILED_A, MB_ABORTRETRYIGNORE | MB_ICONERROR );

	return ffl_check_assert_choice( choice, ignore_always );
}

bool	ffl_assert_function_w(	bool			exp,
							  const wchar_t *	exp_string,
							  const wchar_t *	description,
							  const wchar_t *	function_name,
							  const wchar_t *	file,
							  int				line,
							  bool *			ignore_always )
{
	if( exp == true )
	{
		return true;
	}

	HWND hwnd = ::GetActiveWindow();
	if( exp_string == NULL || description == NULL || function_name == NULL || file == NULL || ignore_always == NULL )
	{
		::MessageBoxW( hwnd, FFL_ASSERTION_PARAM_FAILED_W, FFL_ASSERTION_FAILED_W, MB_OK | MB_ICONERROR );
		return false;
	}

	wchar_t message[ffl_assertion_max_message] = { 0, };
	ffl_snprintf_w( message, FFL_COUNTOF( message ), FFL_ASSERTION_FORMAT_W, FFL_ASSERTION_FAILED_W, exp_string, description, function_name, file, line );
	int choice = ::MessageBoxW( hwnd, message, FFL_ASSERTION_FAILED_W, MB_ABORTRETRYIGNORE | MB_ICONERROR );

	return ffl_check_assert_choice( choice, ignore_always );
}
