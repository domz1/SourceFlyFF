/* A simple function to send the playback time back to the main window and
   handle seeks. This operates using the MCI command strings, and assumes
   that a sound file aliased "sounder" is already loaded. Written by Jeff Tsay
   (ctsay@pasteur.eecs.berkeley.edu).

   Last modified : 01/28/97 */

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOMCX
#define NOIME
// #define NOGDI             GDI APIs and definitions
// #define NOUSER            USER APIs and definitions
#define NOSOUND
#define NOCOMM
#define NODRIVERS
#define OEMRESOURCE
#define NONLS
#define NOSERVICE
#define NOKANJI
#define NOMINMAX
#define NOLOGERROR
#define NOPROFILER
#define NOMEMMGR
#define NOLFILEIO
#define NOOPENFILE
#define NORESOURCE
#define NOATOM
#define NOLANGUAGE
#define NOLSTRING
#define NODBCS
#define NOKEYBOARDINFO
#define NOGDICAPMASKS
#define NOCOLOR
#define NOGDIOBJ
#define NODRAWTEXT
#define NOTEXTMETRIC
#define NOSCALABLEFONT
#define NOBITMAP
#define NORASTEROPS
#define NOMETAFILE
#define NOSYSMETRICS
#define NOSYSTEMPARAMSINFO
#define NOMSG
#define NOWINSTYLES
#define NOWINOFFSETS
#define NOSHOWWINDOW
#define NODEFERWINDOWPOS
#define NOVIRTUALKEYCODES
#define NOKEYSTATES
#define NOWH
#define NOMENUS
#define NOSCROLL
#define NOCLIPBOARD
#define NOICONS
#define NOMB
#define NOSYSCOMMANDS
#define NOMDI
#define NOCTLMGR
#define NOWINMESSAGES
#define NOHELP
// #define _WINUSER_
#define __oleidl_h__
#define _OLE2_H_

#include "stdafx.h"
#include "usrmsg.h"


#define MMNODRV
#define MMNOSOUND
// #define MMNOWAVE
#define MMNOMIDI
#define MMNOAUX
#define MMNOTIMER
#define MMNOJOY
//#define MMNOMCI
#define MMNOMMIO
#define MMNOMMSYSTEM
#include <mmsystem.h>

#include "my_str_lib.h"
#include "args.h"
//#include "resource.h"

DWORD mciplay (MCI_Args *args)
{
	char mci_buffer[1024];
	char mci_command[256];
	char ms_buffer[32];

	while(!args->stop_query()) {

		// Handle seeks

		if (args->position_change_query()) {
			mciSendString("stop sounder", mci_buffer, 1024, NULL);
			lstrcpy(mci_command, "seek sounder to ");
			lstrcat(mci_command, my_itoa(args->desired_position_query(),
                                      ms_buffer, 10));
			lstrcat(mci_command, " wait");
			mciSendString(mci_command, mci_buffer, 1024, NULL);

			if (args->playing)
				mciSendString("play sounder notify", mci_buffer, 1024, args->hWnd);

			args->set_position_change(FALSE);

//			SendMessage(args->hWnd, SEEK_ACK, 0, 0);

		} else
			Sleep(100);

      // Update the scroll bar
		mciSendString("status sounder position", mci_buffer, 1024, NULL);
//		SendMessage(args->hWnd, SCROLL_POS, my_atoi(mci_buffer), 0);
	}
   args->set_done(TRUE);
   ExitThread(0);
   return(0);
}


