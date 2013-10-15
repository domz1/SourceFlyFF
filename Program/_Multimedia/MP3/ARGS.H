#ifndef __ARGS_H
#define __ARGS_H

#include "all.h"
#include "crc.h"
#include "ibitstr.h"
#include "header.h"
#include "subband.h"

//#include <wtypes.h>

/* A abstract class to pass values to maplay. From the Win32 API, we can only
	pass one 32-bit value to a thread. This will be a pointer to a object
	of this class. The Args class contains the seeking control variables
	needed for both MPEG and MCI files. */

/* Added stop and done variables and functions for terminating the
   threads from outside. */

class Args_
{
	public:

	Args_()
	{
   		hWnd = NULL;
   		stop = FALSE;
		done = FALSE;
		desired_position = 0;
		position_change = FALSE;
	}
   
	virtual ~Args_() {}

	void set_stop(BOOL flag) { stop = flag; }
	void set_done(BOOL flag) { done = flag; }

	void set_desired_position(int32 frame)
	{
      	desired_position = frame;
	}
	
	void set_position_change(BOOL flag)
	{
		position_change = flag;
	}
   
	int32 desired_position_query() { return(desired_position); }
	BOOL  position_change_query()  { return(position_change);  }

	volatile BOOL stop_query() { return(stop); }
	volatile BOOL done_query() { return(done); }

	HWND hWnd;

   private:

	volatile int32 desired_position;
	volatile BOOL position_change;
	volatile BOOL stop;
	volatile BOOL done;
};

// A class to contain arguments for maplay.
class MPEG_Args : public Args_
{

public:
	Ibitstream		*stream;
	Header			*MPEGheader;
	HWAVEOUT		*phwo;
	char			*huffdec_path;
	enum e_channels which_c;

	MPEG_Args()
	{
		stream = NULL;
		MPEGheader = NULL;
		phwo = NULL;
		huffdec_path = NULL;
		which_c = both;
  	}

	~MPEG_Args() { }
};

// A class to hold the arguments for MCI playing.
class MCI_Args : public Args_ {

public:
	MCI_Args()
	{
		playing = FALSE;
	}

   ~MCI_Args() { }

	volatile BOOL playing;
};

#endif