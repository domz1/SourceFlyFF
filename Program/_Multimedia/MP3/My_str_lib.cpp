#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOMCX
#define NOIME
#define NOGDI
#define NOUSER
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
// #define NOLSTRING
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
#define _WINUSER_

#include "stdafx.h"
#include "my_str_lib.h"

str_type my_itoa(int value, str_type str, int radix)
// Negative numbers not supported, or radices bigger than 10
{
   int i;
   int last_index;
   int remainder;
   char temp[256];

   if (value == 0) {
   	str[1] = '\0';
      str[0] = '0';

	} else {

	   temp[255]='\0';

   	for(i=254; i>=0, value>0; i--) {
      	remainder = value % radix;
	      value /= radix;
   	   temp[i] = (char) (remainder + '0');
      	last_index = i;
	   }

   	lstrcpy(str, temp + last_index);
   }

   return(str);
}

int my_atoi(str_type str)
// Negative numbers not supported
{
	int length = lstrlen(str);
   int base = 1;
   int i;
   int result = 0;


   for(i=length-1; i>=0; i--)
   {
		result += (str[i] - '0') * base;
      base = (base << 3) + (base << 1);  // base*=10;
   }

   return (result);
}



