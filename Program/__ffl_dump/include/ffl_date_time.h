#ifndef __FFL_DATE_TIME_H__
#define __FFL_DATE_TIME_H__


#include "ffl_dump_common.h"


class	ffl_date_time
{
	public:
		ffl_date_time();
		~ffl_date_time();

		void	update();

		int		year()			const;
		int		month()			const;
		int		day()			const;

		int		hour()			const;
		int		minute()		const;
		int		second()		const;
		int		millisecond()	const;

		int		day_of_week()	const;


	private:
		int		year_;
		int		month_;
		int		day_;

		int		hour_;
		int		minute_;
		int		second_;
		int		millisecond_;

		int		day_of_week_;
};



#endif	//__FFL_DATE_TIME_H__
