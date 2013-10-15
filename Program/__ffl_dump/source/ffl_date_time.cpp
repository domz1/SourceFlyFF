#include "../include/ffl_date_time.h"

ffl_date_time::ffl_date_time()
{
	year_			= 0;
	month_			= 0;
	day_			= 0;

	hour_			= 0;
	minute_			= 0;
	second_			= 0;
	millisecond_	= 0;

	day_of_week_	= 0;

	update();
}

ffl_date_time::~ffl_date_time()
{
	year_			= 0;
	month_			= 0;
	day_			= 0;

	hour_			= 0;
	minute_			= 0;
	second_			= 0;
	millisecond_	= 0;

	day_of_week_	= 0;
}

void	ffl_date_time::update()
{
	SYSTEMTIME	system_time;
	::memset( &system_time, 0, sizeof(system_time) );
	::GetLocalTime( &system_time );

	year_			= system_time.wYear;
	month_			= system_time.wMonth;
	day_			= system_time.wDay;

	hour_			= system_time.wHour;
	minute_			= system_time.wMinute;
	second_			= system_time.wSecond;
	millisecond_	= system_time.wMilliseconds;

	day_of_week_	= system_time.wDayOfWeek;
}

int		ffl_date_time::year()		const
{
	return	year_;
}

int		ffl_date_time::month()		const
{
	return	month_;
}

int		ffl_date_time::day()			const
{
	return	day_;
}

int		ffl_date_time::hour()		const
{
	return	hour_;
}

int		ffl_date_time::minute()		const
{
	return	minute_;
}

int		ffl_date_time::second()		const
{
	return	second_;
}

int		ffl_date_time::millisecond()	const
{
	return	millisecond_;
}

int		ffl_date_time::day_of_week()	const
{
	return	day_of_week_;
}
