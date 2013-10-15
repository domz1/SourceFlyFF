//############################################################################
//##                                                                        ##
//##  MilesPly.C                                                            ##
//##                                                                        ##
//##  MSS 4.0 Tool                                                          ##
//##                                                                        ##
//##  V1.00 of 18-Mar-96: Initial version                                   ##
//##                                                                        ##
//##  C source compatible with Microsoft C v9.0 or later                    ##
//##                                                                        ##
//##   Author: Jeff Roberts                                                 ##
//##                                                                        ##
//############################################################################
//##                                                                        ##
//##  Copyright (C) RAD Game Tools, Inc.                                    ##
//##                                                                        ##
//##  For technical support, contact RAD Game Tools at 425-893-4300.        ##
//##                                                                        ##
//############################################################################

#include "StdAfx.h"

#if __VER < 15 // __MUSIC2		//music2가 아닐경우만 

#define _MUSIC_LIB_MILES	0		//use Miles lib
#define _MUSiC_LIB_FMOD		1		//use FMod Lib

#ifdef  __USE_SOUND_LIB_FMOD
#define __MUSIC_LIB  ( _MUSiC_LIB_FMOD ) 
#else
#define __MUSIC_LIB  ( _MUSIC_LIB_MILES ) 
#endif

#if ( __MUSIC_LIB == _MUSIC_LIB_MILES )

#include "music.h"
/*
#include <windows.h>

#include <afxwin.h>      
#include <AFXTEMPL.H>        // MFC Automation classes  
#include <winuser.h>    
#include <basetsd.h>  
#include <memory.h>  
#include <mmsystem.h>   
#include <string.h>   
#include <stdio.h>  
#include <tchar.h>  
#include <time.h> 
#include <stdlib.h>
*/
#undef WAVE_ENTRY
#include "mss.h"
#include <string.h>

#ifdef IS_WIN32
#include <commctrl.h>
#else
#include <commdlg.h>
#define memcpy _fmemcpy
#endif

CMusicMng g_MusicMng;

/* @cdep pre 
  $DefaultsEXE 
  $requires(milesply.rc)
  $requiresbinary(mss32.lib)
  $requiresbinary(comdlg32.lib comctl32.lib)
*/
// @cdep post $BuildEXE

static char szAppName[] = "MILESPLY";

static HWND hwnd=0;
static HAUDIO audio=0;
static TCHAR filename[128];
static S32 openFile=0;
static HDIGDRIVER dig;
static HMDIDRIVER mdi;
static HDLSDEVICE dls;
static HDLSFILEID DLSfile;
static void FAR*  dlsdata;
static void FAR*  filedata=0;
static BOOL g_bLockMusic;
static BOOL g_bEnableMusic = TRUE;

static S32 rate=22050;
static S32 bits=16;
static S32 chans=2;

static S32 length;
static S32 audioType;
static S32 forceDLS=0;

static S32 filtered=1;
static S32 reverb=0;

static HWND cpuTextHwnd;
static HWND scrollHwnd;
static HWND comboHwnd;
static HWND dlsTextHwnd;
static HWND forceDLSHwnd;
static HWND midiTextHwnd;

static char DLSfilename[MAX_PATH];

static S32 X=100;
static S32 Y=100;

static UINT rate_adj[9]={0,0,0,1,2,2,3,3,4};
static S32 ind_to_rate[5]={11025,16000,22050,32000,44100};

#define btnClose     129
#define scrollBar    132
#define cpuText      134
#define cbFilter     136
#define cbReverb     137
#define pdRate       139
#define cb16bit      135
#define cbStereo     138
#define btnHelp      145
#define btnAbout     146
#define pdForceDLS   141
#define dlsText      143
#define btnBrowseDLS 142
#define midiText     147
#define btnOpen      148

#ifdef IS_WIN32
static char SECTION[]="Miles Sound Player";
#else
static char SECTION[]="Miles 16-bit Sound Player";
#endif

static S32 we_moved=0;


#ifdef IS_WIN32

#define COMBOCOMMANDWORD wParam

#define RADGETSCROLLPOS(hwnd) ((S32)SendMessage(hwnd,TBM_GETPOS,0,0))
#define RADSETSCROLLPOS(hwnd,val) {++we_moved; SendMessage(hwnd,TBM_SETPOS,TRUE,val); --we_moved;}
#define RADSETSCROLLRANGE(hwnd,low,high) SendMessage(hwnd,TBM_SETRANGE,FALSE,MAKELONG(low,high))

#else

#define COMBOCOMMANDWORD lParam

#define RADGETSCROLLPOS(hwnd) ((S32)GetScrollPos(hwnd,SB_CTL))
#define RADSETSCROLLPOS(hwnd,val) {++we_moved; SetScrollPos(hwnd,SB_CTL,val,TRUE); --we_moved;}
#define RADSETSCROLLRANGE(hwnd,low,high) SetScrollRange(hwnd,SB_CTL,low,high,FALSE)

#endif

float g_fMusicVolume = 1.0f;
float g_fCurrentVolume;
BOOL g_bFadeOutMusic = FALSE;
int  g_nLoopCount;
TCHAR g_szFileName[64];

static void close_if_open()
{
   //
   // close the library if already open
   //

   if (openFile) {

     if (audio) {
       AIL_quick_halt(audio);
       audio=0;
     }

     if (DLSfile) {
       AIL_DLS_unload(dls,DLSfile);
       DLSfile=0;
       AIL_mem_free_lock(dlsdata);
       dlsdata=0;
     }

     if (filedata) {
       AIL_mem_free_lock(filedata);
       filedata=0;
     }

     AIL_quick_shutdown();
     openFile=0;
     dls=0;
   }

}

/*
static char FAR* get_filename(char FAR* pathname)
{
   char FAR* f=pathname+lstrlen(pathname)-1;
   while ((f>=pathname) && (*f!='\\') && (*f!=':')) --f;
   return(f+1);
}
*/
/*
static char FAR* extract_filename(char FAR* dest,char FAR* cmdline)
{
  char FAR* s;
  char FAR* e;

  s=cmdline;

  //check for null
  if (s==0) {
    *dest=0;
    return(0);
  }

  //skip whitespace
  while ((*s<=32) && (*s!=0))
    s++;

  //is empty?
  if (*s==0) {
    *dest=0;
    return(0);
  }

  //is it a quoted name?
  if (*s=='"') {
    //scan until we hit the end or the last quote
    e=(++s)+1;
    while ((*e!='"') && (*e!=0))
      e++;

    //copy the string
    memcpy(dest,s,e-s);
    dest[e-s]=0;

    //skip over the end quote
    if (*e='"')
      ++e;
    goto finish;
  } else {
    //scan until we hit the end or another space
    e=s+1;
    while (*e!=0)
      e++;

    //copy the string
    memcpy(dest,s,e-s);
    dest[e-s]=0;

   finish:
    //skip over any remaining whitespace
    while ((*e<=32) && (*e!=0))
      e++;

    return(*e?e:0);
  }
}


static void handle_filenames(char FAR*cmdline)
{
  char FAR*end;

  // copy the first filename
  end=extract_filename(filename,cmdline);

  if (end) {
    char exe[256];
    char FAR* e;

    GetModuleFileName(0,exe,256);
    e=exe+lstrlen(exe);
    *e++=' ';

    // now launch other instances for multiple files
    while (end) {
      end=extract_filename(e,end);
      if (*e)
        WinExec(exe,SW_SHOW);
    }
  }
}
*/
/*
static void set_title(char FAR* pathname)
{
  char title[128];
  lstrcpy(title,"Miles Sound Player - ");
  lstrcat(title,get_filename(pathname));
  SetWindowText(hwnd,title);
}
*/
/*
static void set_device_strings()
{
  char desc[128];

  if (audioType==AIL_QUICK_DIGITAL_TYPE) {
    lstrcpy(desc,"Using ");
    AIL_digital_configuration(dig,0,0,desc+lstrlen(desc));
  } else if (audioType==AIL_QUICK_XMIDI_TYPE) {
    lstrcpy(desc,"Using the system MIDI device.");
    if (forceDLS==3)
      lstrcpy(desc+lstrlen(desc)-1," with Vortex DLS.");
    else if (forceDLS==4)
      lstrcpy(desc+lstrlen(desc)-1," with SonicVibes DLS.");
  } else if (audioType==AIL_QUICK_DLS_XMIDI_TYPE)
    lstrcpy(desc,"Using the Miles DLS software-synthesizer.");

  SetWindowText(midiTextHwnd,desc);
}

*/
static void set_reverb( void )
{
  AIL_quick_set_reverb_levels( audio, 1.0f, ( reverb ) ? 1.0f : 0.0f );
}

//############################################################################
//##                                                                        ##
//## Get a sound filename                                                   ##
//##                                                                        ##
//############################################################################

S32 get_sound_file()
{
	/*
   OPENFILENAME fn;

   memset(&fn, 0, sizeof(fn));

   fn.lStructSize       = sizeof(fn);
   fn.hwndOwner         = hwnd;
   fn.lpstrFilter       = "Sound files (*.mss;*.xmi;*.mid;*.rmi;*.wav;*.mp?)\0*.mid;*.mss;*.rmi;*.xmi;*.wav;*.mp?\0All files\0*.*\0";
   fn.nFilterIndex      = 1;
   fn.lpstrFile         = filename;
   fn.nMaxFile          = 126;
   fn.lpstrTitle        = "Select Sound File...";
   fn.Flags             = OFN_FILEMUSTEXIST |
#ifdef IS_WIN32
                          OFN_EXPLORER | OFN_LONGNAMES |
#endif
                          OFN_NOCHANGEDIR |
                          OFN_PATHMUSTEXIST |
                          OFN_HIDEREADONLY;
   fn.lpstrDefExt       = "xmi";
*/
  // return( GetOpenFileName(&fn)?1:0 );
   return 1;

}


static BOOL open_file( LPCTSTR lpszFileName, S32 save_position, int nLoopCount )
{
   S32 usedig;
   S32 usemidi;
   S32 type;

// S32 pos=((open) && (audio) && (save_position))?AIL_quick_ms_position(audio):0;
   S32 pos=((openFile) && (audio) && (save_position))?AIL_quick_ms_position(audio):0;

   if( _tcscmp( lpszFileName, filename ) != 0 )
   {
	   _tcscpy( filename, lpszFileName );
	   close_if_open();
	   filedata = AIL_file_read( filename, FILE_READ_WITH_SIZE );
   }
   else
		AIL_quick_play( audio, nLoopCount );
  //loadagain:
   //set_title("Loading...");

   if (filedata==0) {
	   return FALSE;
    err:
     MessageBox(hwnd,AIL_last_error(),"Error opening file.",MB_OK);

     //if (get_sound_file())
       //goto loadagain;
     //else {
       //DestroyWindow(hwnd);
       //return FALSE;
     //}
   }

   //
   // get type of file
   //

   type=AIL_file_type(((U32 FAR*)filedata)+1,*((U32 FAR*)filedata));

   switch (type) {
     case AILFILETYPE_PCM_WAV:
     case AILFILETYPE_ADPCM_WAV:
     case AILFILETYPE_VOC:
     case AILFILETYPE_MPEG_L3_AUDIO:
       usedig=1;
       usemidi=0;
       break;
     case AILFILETYPE_MIDI:
     case AILFILETYPE_XMIDI:
       if (forceDLS>=2)
         goto usedls;
       usemidi=1;
       usedig=0;
       break;
     case AILFILETYPE_XMIDI_DLS:
     case AILFILETYPE_XMIDI_MLS:
      usedls:
       usedig=0;
       if (forceDLS==3)
         usemidi=AIL_QUICK_MIDI_AND_VORTEX_DLS;
       else if (forceDLS==4)
         usemidi=AIL_QUICK_MIDI_AND_SONICVIBES_DLS;
       else {
         if (forceDLS==1)
           usemidi=1;
         else {
           usemidi=AIL_QUICK_DLS_ONLY;
           usedig=1;
         }
       }
       break;
     default:
       AIL_set_error("Unplayable file type.");
       AIL_mem_free_lock(filedata);
       filedata=0;
       goto err;
   }

   //
   // Open MSS
   //

   if (AIL_quick_startup(usedig,usemidi,rate,bits,chans) == 0) {
	   return FALSE;
     MessageBox(hwnd,AIL_last_error(),"Couldn't open MSS.",MB_OK);

     // if we were using a custom MIDI mode, flip back normal and try again
     if ((forceDLS!=0) && (usemidi)) {
       forceDLS=0;
       SendMessage(forceDLSHwnd,CB_SETCURSEL,(UINT)forceDLS,0);
       AIL_mem_free_lock(filedata);
	   return FALSE;
       //goto loadagain;
     }

     //DestroyWindow(hwnd);

   } else {

     //
     // mark the library as open and get the handles
     //

     openFile=1;
     AIL_quick_handles(&dig,&mdi,&dls);

     //
     // Set the reverb settings
     //
     AIL_set_digital_master_room_type( dig, EAX_ENVIRONMENT_HALLWAY );

    tryagain:

     //
     // try to load the file
     //

     audio=AIL_quick_load_mem( ((U32 FAR*)filedata)+1,*((U32 FAR*)filedata));

     if (audio) {

       audioType=AIL_quick_type(audio);

       if (audioType==AIL_QUICK_MPEG_DIGITAL_TYPE)
         audioType=AIL_QUICK_DIGITAL_TYPE;

       //
       // did the user force DLS even for general MIDI files?
       //

       if ((audioType==AIL_QUICK_XMIDI_TYPE) && (dls) && (forceDLS>=2) && (*DLSfilename)) {

         U32 size;

         dlsdata=AIL_file_read(DLSfilename,0);
         size=AIL_file_size(DLSfilename);

         if (dlsdata==0) {
			 return FALSE;
          dlserr:
           MessageBox(hwnd,AIL_last_error(),"Error opening DLS file.",MB_OK);

         } else {

           if (AIL_file_type(dlsdata,size)!=AILFILETYPE_DLS) {
             void FAR*uncomp;

             if (AIL_extract_DLS(dlsdata,size,0,0,&uncomp,0,0)==0) {
               AIL_mem_free_lock(dlsdata);
               goto dlserr;
             }

             AIL_mem_free_lock(dlsdata);
             dlsdata=uncomp;
           }

           DLSfile=AIL_DLS_load_memory(dls,dlsdata,0);
           if (DLSfile==0) {
             AIL_mem_free_lock(dlsdata);
             goto dlserr;
           } else
             audioType=AIL_QUICK_DLS_XMIDI_TYPE;
         }
       }

       //
       // if we aren't using the soft synth, change the file type
       //

       if ((audioType==AIL_QUICK_DLS_XMIDI_TYPE) && (forceDLS!=2) && ((forceDLS!=0) || (dls==0)))
         audioType=AIL_QUICK_XMIDI_TYPE;

       //
       // if no dls, then disable the inappropriate controls
       //
/*
       if (audioType==AIL_QUICK_XMIDI_TYPE)
         SetWindowText(cpuTextHwnd,"CPU: SysMon");

       EnableWindow( GetDlgItem(hwnd,cbFilter), (audioType==AIL_QUICK_XMIDI_TYPE)?0:1 );
       EnableWindow( GetDlgItem(hwnd,cbReverb), (audioType==AIL_QUICK_XMIDI_TYPE)?0:1 );

       EnableWindow( GetDlgItem(hwnd,cb16bit), (audioType==AIL_QUICK_XMIDI_TYPE)?0:1 );
       EnableWindow( GetDlgItem(hwnd,cbStereo), (audioType==AIL_QUICK_XMIDI_TYPE)?0:1 );
       EnableWindow( comboHwnd, (audioType==AIL_QUICK_XMIDI_TYPE)?0:1 );

       EnableWindow( forceDLSHwnd, (audioType==AIL_QUICK_DIGITAL_TYPE)?0:1 );
       EnableWindow( dlsTextHwnd, ((audioType!=AIL_QUICK_DIGITAL_TYPE) && (forceDLS>=2))?1:0 );
       EnableWindow( GetDlgItem(hwnd,btnBrowseDLS), ((audioType!=AIL_QUICK_DIGITAL_TYPE) && (forceDLS>=2))?1:0 );
*/
       //
       // Set devices strings
       //

       //set_device_strings();

       //
       // set the initial reverb
       //

       set_reverb();

       //
       // start the playback
       //

       AIL_quick_play(audio, nLoopCount );

       //set_title(filename);

       length=AIL_quick_ms_length( audio );

       if (pos)
         AIL_quick_set_ms_position(audio,pos);

     } else {

		 return FALSE;
       MessageBox(hwnd,AIL_last_error(),"Error opening file.",MB_OK);

       if (get_sound_file())
         goto tryagain;
       else
         DestroyWindow(hwnd);

     }

   }
   return TRUE;
/*
   if (*DLSfilename) {
     SetWindowText(dlsTextHwnd,get_filename(DLSfilename));
   } else
     SetWindowText(dlsTextHwnd,"None selected.");
	 */
}

//############################################################################
//##                                                                        ##
//## Get a DLS filename                                                     ##
//##                                                                        ##
//############################################################################
	/*

void get_DLS_file()
{
   OPENFILENAME fn;
   char         fn_buff[128];

   memset(&fn, 0, sizeof(fn));
   lstrcpy(fn_buff,DLSfilename);

   fn.lStructSize       = sizeof(fn);
   fn.hwndOwner         = hwnd;
   fn.lpstrFilter       = "Downloadable sample files (*.dls;*.mls)\0*.dls;*.mls\0All files\0*.*\0";
   fn.nFilterIndex      = 1;
   fn.lpstrFile         = fn_buff;
   fn.nMaxFile          = sizeof(fn_buff);
   fn.lpstrTitle        = "Select DLS File...";
   fn.Flags             = OFN_FILEMUSTEXIST |
#ifdef IS_WIN32
                          OFN_EXPLORER | OFN_LONGNAMES |
#endif
                          OFN_NOCHANGEDIR |
                          OFN_PATHMUSTEXIST |
                          OFN_HIDEREADONLY;
   fn.lpstrDefExt       = "dls";


   if (GetOpenFileName(&fn))
      {
      lstrcpy(DLSfilename,fn_buff);
      open_file(1);
      }
}
*/
//############################################################################
//##                                                                        ##
//## Show the help window                                                   ##
//##                                                                        ##
//############################################################################
/*
void show_help()
{
   MessageBox(hwnd,

   "Thank you for using RAD Game Tools!\n\n"
   "The Miles player can play MP3, XMIDI, MIDI, or wave files. Simply choose\n"
   "the file you'd like to play, or drop the file onto the player icon.\n\n"
   "Controls:\n"
   "\tCPU - how much CPU time the DLS synthesizer is using.\n\n"
   "\tFilter - this lets you turn on and off sound filtering in the DLS\n"
   "\tsoftware synthesizer and the digital mixer. Filtering takes a little\n"
   "\textra CPU time (so little that it should usually just be left on).\n\n"
   "\tReverb - this lets you turn on and off reverb in the DLS software\n"
   "\tsynthesizer and digital mixer. Reverb takes extra CPU time.\n\n"
   "\tRate - this lets you set the rate to use for digital output (both\n"
   "\tfor wave file and the DLS soft-synth). The faster rates require\n"
   "\tmore CPU time for the DLS software synthesizer.\n\n"
   "\t16-bit - this lets you set the output quality to 16-bit. 16-bit\n"
   "\tsound is a little slower for the software synthesizer to build.\n\n"
   "\tStereo - this lets you set the output to stereo. Stereo sound is\n"
   "\tslower for the software synthesizer to build.\n\n"
   "\tMIDI Device - this option lets you choose the MIDI output\n"
   "\tdevice.  Your choices are: 1) auto-choose, that is, play\n"
   "\tplain MIDI files on the MIDI hardware, and DLS MIDI files\n"
   "\ton the software synthesizer;  2) always use the MIDI\n"
   "\thardware; 3) always use the software synthesizer; 4) use the\n"
   "\tAureal Vortex DLS hardware; and, 4) use the S3 SonicVibes\n"
   "\tDLS hardware.\n\n"
   "\tIf you choose to always use the soft-synth, then you must\n"
   "\talso choose a DLS file for playing plain MIDI files. You can\n"
   "\tobtain a Roland (TM) General MIDI DLS file (called GM.DLS)\n"
   "\tby downloading Microsoft's 'Interactive Music Control'\n"
   "\t(available on the Microsoft web site)."

   ,"Help...", MB_OK);
}

*/
//############################################################################
//##                                                                        ##
//## Show the about window                                                  ##
//##                                                                        ##
//############################################################################
/*
void show_about()
{
   char text[1024];
   char version[8];
   AIL_MSS_version(version,8);
   lstrcpy(text,"Version ");
   lstrcat(text,version);
   lstrcat(text," " MSS_COPYRIGHT "\n\n"
                "Miles Sound System XMIDI, MIDI, MPEG and wave player.");

   if (dig) {
     lstrcat(text,"\n\nDigital driver: ");
     AIL_digital_configuration(dig,0,0,text+lstrlen(text));
   }

   lstrcat(text,"\n\nFor questions or comments, please contact RAD Game Tools at:\n\n"
               "\tRAD Game Tools\n"
               "\t401 Park Place - Suite 103\n"
               "\tKirkland, WA  98033\n"
               "\t425-893-4300\n"
               "\tFAX: 425-893-9111\n\n"
               "\tWeb: http://www.radgametools.com\n"
               "\tE-mail: sales@radgametools.com");
   MessageBox(hwnd,text,"About the Miles Sound Player...", MB_OK);
}

*/
//############################################################################
//##                                                                        ##
//## Main window procedure                                                  ##
//##                                                                        ##
//############################################################################
/*
LRESULT AILEXPORT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   char buffer[128];
   S32 i;

   switch (message)
      {
      case WM_COMMAND:

        switch (LOWORD(wParam))
        {
          case btnOpen:
            if (get_sound_file())
              open_file(0);
            break;

          case btnHelp:
            show_help();
            break;

          case btnAbout:
            show_about();
            break;

          case btnBrowseDLS:
            get_DLS_file();
            break;

          case IDCANCEL:
          case btnClose:
            DestroyWindow(hwnd);
            break;

          case cb16bit:
            bits=(bits==8)?16:8;
            open_file(1);
            break;

          case cbStereo:
            chans=(chans==1)?2:1;
            open_file(1);
            break;

          case cbFilter:
            filtered=!filtered;
            AIL_set_preference(DLS_ENABLE_FILTERING,filtered);
            AIL_set_preference(DIG_ENABLE_RESAMPLE_FILTER,filtered);
            break;

          case cbReverb:
            reverb=!reverb;
            set_reverb();
            break;

          case pdForceDLS:
            if (HIWORD(COMBOCOMMANDWORD) == CBN_SELENDOK) {
              forceDLS=SendMessage(forceDLSHwnd,CB_GETCURSEL,0,0);
              EnableWindow( dlsTextHwnd, (forceDLS==2)?1:0 );
              EnableWindow( GetDlgItem(hwnd,btnBrowseDLS), (forceDLS==2)?1:0 );
              if (forceDLS==2) {
                if (*DLSfilename)
                  open_file(1);
              } else
                open_file(1);
            }
            break;

          case pdRate:
            if (HIWORD(COMBOCOMMANDWORD) == CBN_SELENDOK) {
              i=ind_to_rate[SendMessage(comboHwnd,CB_GETCURSEL,0,0)];
              if (i!=rate) {
                rate=i;
                open_file(1);
              }
            }
            break;

        }
        return 0;

      case WM_HSCROLL:
#ifdef IS_WIN32
        if (!we_moved)
          AIL_quick_set_ms_position(audio,(RADGETSCROLLPOS(scrollHwnd)*length)/100L);
#else
        {  // have to handle 16-bit scroll bars manually
        S32 n=RADGETSCROLLPOS(scrollHwnd);
        switch (LOWORD(wParam)) {
          case SB_THUMBPOSITION:
          case SB_THUMBTRACK:
            n=LOWORD(lParam);
            break;
          case SB_BOTTOM:
          case SB_PAGEDOWN:
          case SB_LINEDOWN:
            n++;
            break;
          case SB_TOP:
          case SB_PAGEUP:
          case SB_LINEUP:
            n--;
            break;
        }

        if (n<0) n=0; else if (n>100) n=100;

        RADSETSCROLLPOS(scrollHwnd,(int)n);
        AIL_quick_set_ms_position(audio,(n*length)/100L);
        }
#endif
        break;

      case WM_TIMER:

        //
        // update the cpu hit
        //

        if ((dls) && (audioType==AIL_QUICK_DLS_XMIDI_TYPE)) {
          AIL_DLS_get_info(dls,0,&i);
          wsprintf(buffer,"CPU: %i%%",i);
          SetWindowText(cpuTextHwnd,buffer);
        } else
          if ((dig) && (audioType==AIL_QUICK_DIGITAL_TYPE)) {
            wsprintf(buffer,"CPU: %i%%",AIL_digital_CPU_percent(dig));
            SetWindowText(cpuTextHwnd,buffer);
          }

        //
        // update the position
        //

        if (audio) {
          i=AIL_quick_ms_position(audio);
          if (length)
            RADSETSCROLLPOS(scrollHwnd,(UINT)(i*100L/length));
        }
        break;

      case WM_WINDOWPOSCHANGED:
        if (!IsIconic(hwnd)) {
          X=((LPWINDOWPOS)lParam)->x;
          Y=((LPWINDOWPOS)lParam)->y;
        }
        break;

      case WM_SETFOCUS:    // deal with the focus in this weird dialog-window
        SetFocus(GetDlgItem(hwnd,btnClose));
        break;

      case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

#ifndef IS_WIN32
      case WM_ENDSESSION:   // for 16-bit windows
        close_if_open();
        break;

      case WM_CTLCOLOR:
        if (HIWORD(lParam)!=CTLCOLOR_LISTBOX) {
          SetBkColor((HDC)wParam,RGB(192,192,192));
          return((LRESULT)GetStockObject(LTGRAY_BRUSH));
        } else
          break;
#endif
      }

   return DefWindowProc(hwnd,message,wParam,lParam);
}


static void WriteProfileInt(char FAR* sec,char FAR* key,U32 value)
{
  char buf[16];
  wsprintf(buf,"%i",value);
  WriteProfileString(sec,key,buf);
}

//############################################################################
//##                                                                        ##
//## WinMain()                                                              ##
//##                                                                        ##
//############################################################################

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, int nCmdShow)
{
   MSG      msg;
   WNDCLASS wndclass;

   //
   // set the redist directory and statup the system
   //

   AIL_set_redist_directory( MSS_DIR_UP_TWO "redist" );

   AIL_startup();

   //
   // create the window class
   //

   if (!hPrevInstance)
      {
      wndclass.lpszClassName = szAppName;
      wndclass.lpfnWndProc   = (WNDPROC) WndProc;
      wndclass.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
      wndclass.hInstance     = hInstance;
      wndclass.hIcon         = LoadIcon(hInstance,szAppName);
      wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
#ifdef _WIN32
      wndclass.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
#else
      wndclass.hbrBackground = GetStockObject(LTGRAY_BRUSH);
#endif
      wndclass.cbClsExtra    = 0;
      wndclass.cbWndExtra    = DLGWINDOWEXTRA;
      wndclass.lpszMenuName  = NULL;

      RegisterClass(&wndclass);
      }

   hwnd = CreateDialog(hInstance,szAppName,0,NULL);

   if (hwnd==0) {
     MessageBox(0,"Couldn't create dialog box.","Error",MB_OK);
     AIL_quick_shutdown();
     return(0);
   }

   //
   // read the INI file
   //

   filtered=GetProfileInt(SECTION,"Filtered",(UINT)filtered);
   reverb=GetProfileInt(SECTION,"Reverb",(UINT)reverb);
   rate=GetProfileInt(SECTION,"Rate",(UINT)rate);
   bits=GetProfileInt(SECTION,"Bits",(UINT)bits);
   chans=GetProfileInt(SECTION,"Channels",(UINT)chans);
   forceDLS=GetProfileInt(SECTION,"ForceDLS",(UINT)forceDLS);
   GetProfileString(SECTION,"DLSFilename","",DLSfilename,128);
   X=GetProfileInt(SECTION,"X",(UINT)X);
   Y=GetProfileInt(SECTION,"Y",(UINT)Y);


   //
   // Main message loop
   //

   SetWindowPos(hwnd,0,(UINT)X,(UINT)Y,0,0,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOSIZE);
   ShowWindow(hwnd,nCmdShow);

#ifdef IS_WIN32
 //  InitCommonControls();
#endif

   //
   // copy the filename and if blank, ask for one
   //

   handle_filenames(lpszCmdLine);

   if (*filename==0) {
     get_sound_file();
     if (*filename==0)
       return(0);
   }


   //
   // setup the dialog
   //

   cpuTextHwnd=GetDlgItem(hwnd,cpuText);
   scrollHwnd=GetDlgItem(hwnd,scrollBar);
   comboHwnd=GetDlgItem(hwnd,pdRate);
   dlsTextHwnd=GetDlgItem(hwnd,dlsText);
   forceDLSHwnd=GetDlgItem(hwnd,pdForceDLS);
   midiTextHwnd=GetDlgItem(hwnd,midiText);

   SendMessage(comboHwnd,CB_ADDSTRING,0,(U32)(char FAR*)"44 Khz");
   SendMessage(comboHwnd,CB_ADDSTRING,0,(U32)(char FAR*)"32 Khz");
   SendMessage(comboHwnd,CB_ADDSTRING,0,(U32)(char FAR*)"22 Khz");
   SendMessage(comboHwnd,CB_ADDSTRING,0,(U32)(char FAR*)"16 Khz");
   SendMessage(comboHwnd,CB_ADDSTRING,0,(U32)(char FAR*)"11 Khz");
   SendMessage(comboHwnd,CB_SETCURSEL,rate_adj[rate/5000],0);

   SendMessage(forceDLSHwnd,CB_ADDSTRING,0,(U32)(char FAR*)"Auto-choose MIDI device.");
   SendMessage(forceDLSHwnd,CB_ADDSTRING,0,(U32)(char FAR*)"Use MIDI hardware.");
   SendMessage(forceDLSHwnd,CB_ADDSTRING,0,(U32)(char FAR*)"Use Miles software synth.");
   SendMessage(forceDLSHwnd,CB_ADDSTRING,0,(U32)(char FAR*)"Use Vortex DLS hardware.");
   SendMessage(forceDLSHwnd,CB_ADDSTRING,0,(U32)(char FAR*)"Use SonicVibes DLS hardware.");
   SendMessage(forceDLSHwnd,CB_SETCURSEL,(UINT)forceDLS,0);

   if (filtered)
     SendMessage(GetDlgItem(hwnd,cbFilter),BM_SETCHECK,1,0);

   if (reverb)
     SendMessage(GetDlgItem(hwnd,cbReverb),BM_SETCHECK,1,0);

   if (bits==16)
     SendMessage(GetDlgItem(hwnd,cb16bit),BM_SETCHECK,1,0);

   if (chans==2)
     SendMessage(GetDlgItem(hwnd,cbStereo),BM_SETCHECK,1,0);

   if (forceDLS!=2) {
     EnableWindow( dlsTextHwnd, 0 );
     EnableWindow( GetDlgItem(hwnd,btnBrowseDLS), 0 );
   }

   RADSETSCROLLRANGE(scrollHwnd,0,100);

   UpdateWindow(hwnd);

   //
   // open the system and the MIDI file
   //

   open_file(0);

   //
   // create a timer to update the position and CPU usage
   //

   SetTimer(hwnd,1,1000,0);

   //
   // start the message loop
   //

   SetFocus(GetDlgItem(hwnd,btnClose));

   while (GetMessage(&msg, 0, 0, 0)) {
     if (!IsDialogMessage(hwnd,&msg)) {
       TranslateMessage(&msg);
       DispatchMessage(&msg);
     }
   }

   //
   // write the INI file
   //

   WriteProfileInt(SECTION,"Filtered",filtered);
   WriteProfileInt(SECTION,"Reverb",reverb);
   WriteProfileInt(SECTION,"Rate",rate);
   WriteProfileInt(SECTION,"Bits",bits);
   WriteProfileInt(SECTION,"Channels",chans);
   WriteProfileInt(SECTION,"ForceDLS",forceDLS);
   WriteProfileString(SECTION,"DLSFilename",DLSfilename);
   WriteProfileString(SECTION,"DLSFilename",DLSfilename);
   WriteProfileInt(SECTION,"X",X);
   WriteProfileInt(SECTION,"Y",Y);

   close_if_open();

   AIL_shutdown();

   return msg.wParam;
}

*/

void InitCustomSound( BOOL bEnable )
{
	g_bEnableMusic = bEnable;
	filtered=1;
	reverb=0;
	rate=44100;//GetProfileInt(SECTION,"Rate",(UINT)rate);
	bits=16;//GetProfileInt(SECTION,"Bits",(UINT)bits);
	chans=2;//GetProfileInt(SECTION,"Channels",(UINT)chans);
	forceDLS=0;//GetProfileInt(SECTION,"ForceDLS",(UINT)forceDLS);
//	GetProfileString(SECTION,"DLSFilename","",DLSfilename,128);
}
void UnInitCustomSound()
{
	close_if_open();
}
BOOL PlayMusic_( LPCTSTR lpszFileName, int nLoopCount )
{
	if( g_bEnableMusic == FALSE )		// 사운드 카드가 없거나 껐을때
		return FALSE;
	// 음악이 연주중인데, 연주중인 파일과 동일한 파일이 플레이를 요청했다면 캔슬 
	if( _tcscmp( lpszFileName, filename ) == 0 && IsPlayingMusic() )
		return FALSE;
	filtered=1;
	reverb=0;
	rate=44100;//GetProfileInt(SECTION,"Rate",(UINT)rate);
	bits=16;//GetProfileInt(SECTION,"Bits",(UINT)bits);
	chans=2;//GetProfileInt(SECTION,"Channels",(UINT)chans);
	forceDLS=0;//GetProfileInt(SECTION,"ForceDLS",(UINT)forceDLS);
	
	// 아직 음악이 연주중이라면, 볼륨을 서서히 낮춰줄 세팅을 한다.
	if( IsPlayingMusic() )
	{
		_tcscpy( g_szFileName, lpszFileName );
		g_nLoopCount = nLoopCount;
		g_bFadeOutMusic = TRUE;
	}
	else
	//get_sound_file();
	if( open_file( lpszFileName, 0, nLoopCount ) ) 
	{
		//SetTimer(hwnd,1,1000,0);
		g_fCurrentVolume = g_fMusicVolume;
		AIL_set_digital_master_volume_level( dig, g_fMusicVolume );
		F32 fVolume = AIL_digital_master_volume_level( dig );
		g_bFadeOutMusic = FALSE;
		return TRUE;
	}
	return FALSE;
}
BOOL PlayMusic( LPCTSTR lpszFileName, int nLoopCount )
{
	if( g_bLockMusic )
		return FALSE;
	return PlayMusic_( lpszFileName, nLoopCount );
}	
// 주로 중간에 곡이 끊겼을 때 끊기는 곡을 부드럽게 끊기 위해서.
void ProcessFadeMusic()
{
	if( g_bFadeOutMusic )
	{
		g_fCurrentVolume -= 0.01f;
		if( dig )
			AIL_set_digital_master_volume_level( dig, g_fCurrentVolume );
		if( g_fCurrentVolume < 0.0f )
		{
		    StopMusic();
			g_bFadeOutMusic = FALSE;
			g_bLockMusic = FALSE;
			if( PlayMusic_( g_szFileName, g_nLoopCount ) )
				g_bLockMusic = TRUE;
		}
	}
	else
	{
		S32 status = AIL_quick_status( audio );
		if( status != QSTAT_PLAYING ) 
			g_bLockMusic = FALSE;
	}
}
// 이 함수를 호출하면 다른 어떤 음악이 플레이가 시도 되더라도 무시한다.
// 즉 완주를 보장하는 것이다.
void LockMusic()
{
	g_bLockMusic = TRUE;
}
BOOL PlayMusic( DWORD dwIdMusic, int nLoopCount )
{
	if( dwIdMusic == NULL_ID || dwIdMusic == 0 ) //BGM_NONE )
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName( dwIdMusic );
	if( lpFileName[ 0 ] )
	{	
		g_bLockMusic = FALSE;
		if( PlayMusic( lpFileName, nLoopCount ) )
		{
			g_bLockMusic = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL PlayBGM( DWORD dwIdMusic )
{
	if( dwIdMusic == NULL_ID || dwIdMusic == 0 ) //BGM_NONE )
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName( dwIdMusic );
	if( lpFileName[ 0 ] )
		return PlayMusic( lpFileName, 1 );
	return FALSE;
}
void StopMusic()
{
    AIL_quick_halt(audio);
	//close_if_open();
}
void SetVolume( FLOAT fVolume )
{
//	if( fVolume ) 
//		fVolume = 0.25f;
	if( fVolume > 1.0f )
		fVolume = 1.0f;

	g_fMusicVolume = fVolume;
	g_fCurrentVolume = fVolume;
	if( dig )
		AIL_set_digital_master_volume_level( dig, fVolume );
}
FLOAT GetVolume()
{
	float fVolume = 0.0f;
	
	if( dig )
		fVolume = AIL_digital_master_volume_level( dig );

	//if( fVolume ) return 0.25f;
	return fVolume;
	return 0.0f;
}
BOOL IsPlayingMusic()
{
	S32 status = AIL_quick_status( audio );
	if( g_bLockMusic )
		return TRUE;
	if( status == QSTAT_PLAYING ) 
		return TRUE;
	return FALSE;
}
BOOL IsStopMusic()
{
	S32 status = AIL_quick_status( audio );
	if( g_bLockMusic )
		return FALSE;
	if( status != QSTAT_PLAYING ) 
		return TRUE;
	return FALSE;
}

//X=GetProfileInt(SECTION,"X",(UINT)X);
//Y=GetProfileInt(SECTION,"Y",(UINT)Y);

/*
void AIL_set_digital_master_volume_level(
  HDIGDRIVER dig,
  F32 master_volume
);
 F32 AIL_digital_master_volume_level(
  HDIGDRIVER dig
);


*/
//CFixedArray< TCHAR* > m_aMusic;

#elif ( __MUSIC_LIB == _MUSiC_LIB_FMOD )

//------------------------------------------------------------------------------------------------------------------------------
// FMOD here 
// gmpbigsun : FMOD , 기존의 Sound Interface를 그대로 유지 
//------------------------------------------------------------------------------------------------------------------------------

#include "fmod.h"
#include "fmoddyn.h"
#include "fmod_errors.h"

#pragma comment ( lib, "fmodvc.lib" )

CMusicMng g_MusicMng;

static BOOL g_bLockMusic = FALSE;
static BOOL g_bEnableMusic = TRUE;
static BOOL g_bFadeOutMusic = FALSE;
static float g_fMusicVolume = 1.0f;

struct FModData
{
	FModData( ) { _stream = NULL; _iChannel = -1; }
	FSOUND_STREAM* _stream;
	std::string _filename;
	int _iChannel;
};

FModData g_FModData;		// bgm을 2곡이상 동시에 플레이시킬일이 없으므로...


void ProcessFadeMusic()
{
	// 이기능이 필요한가?
/*	static CTime kOldTime;
	static bool bFirst = true;
	if( g_bFadeOutMusic )
	{
		if( bFirst )
		{
			kOldTime = CTime::GetCurrentTime( );
			bFirst = false;
		}

		CTime kCurTime = CTime::GetCurrentTime( );
		CTime kDelta = kCurTime - kOldTime;

		FLOAT fVol = GetVolume( );
		fVol -= ( kDelta * kDelta.GetSecond( ) );

		if( fVol <= 0.0f )
		{
			//ok finished the processing
			g_bFadeOutMusic = FALSE
			bFirst = true;
		}else
	} */
}

BOOL PlayMusic_( LPCTSTR lpszFileName, int nLoopCount )
{
	if( g_bEnableMusic == FALSE )		// 사운드 카드가 없거나 껐을때
		return FALSE;

	// 이미 로드된 Stream이 있다면..
	if( g_FModData._stream )
	{
		if( g_FModData._filename == string( lpszFileName ) && IsPlayingMusic( ) )
			return FALSE;
	}

	// 제거 !
	FSOUND_Stream_Close( g_FModData._stream );

	// 새로운 스트림으로!
	g_FModData._stream = FSOUND_Stream_Open( lpszFileName,  FSOUND_NORMAL | FSOUND_MPEGACCURATE, 0, 0);
	if( !g_FModData._stream )
	{
		assert( g_FModData._stream && "cannot find music file!!" );
		return FALSE;
	}

	g_FModData._filename = string( lpszFileName );

	// Loop!
	assert( nLoopCount > -1 );
	if( nLoopCount > 0 )
		FSOUND_Stream_SetLoopCount( g_FModData._stream, nLoopCount );

	g_FModData._iChannel = FSOUND_Stream_Play( FSOUND_FREE, g_FModData._stream );

	// 볼륨은 원래대로
	SetVolume( g_fMusicVolume );
	
	return FALSE;
}

BOOL PlayMusic( LPCTSTR lpszFileName, int nLoopCount )
{
	if( g_bLockMusic )
		return FALSE;
	return PlayMusic_( lpszFileName, nLoopCount );
}

BOOL PlayMusic( DWORD dwIdMusic, int nLoopCount )
{
	if( dwIdMusic == NULL_ID || dwIdMusic == 0 ) //BGM_NONE )
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName( dwIdMusic );
	if( lpFileName[ 0 ] )
	{	
		g_bLockMusic = FALSE;
		if( PlayMusic( lpFileName, nLoopCount ) )
		{
			g_bLockMusic = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL PlayBGM( DWORD dwIdMusic )
{
	if( dwIdMusic == NULL_ID || dwIdMusic == 0 ) //BGM_NONE )
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName( dwIdMusic );
	if( lpFileName[ 0 ] )
		return PlayMusic( lpFileName, 1 );
	
	return FALSE;
}

void LockMusic()
{
	g_bLockMusic = TRUE;
}

void StopMusic()
{
	FSOUND_StopSound( g_FModData._iChannel );
}

void SetVolume( FLOAT fVolume )
{
	g_fMusicVolume = fVolume;	//Miles는 0.0 ~ 1.0f 를 쓰는관계로 이값을 255로 곱해줘야 실제 FMOD volum 수치가 된다.
	int iFModVolum = g_fMusicVolume * 255.0f;
	FSOUND_SetVolume( g_FModData._iChannel, iFModVolum );
}

FLOAT GetVolume()
{
	return g_fMusicVolume;
	//return (FLOAT)FSOUND_GetVolume( g_FModData._iChannel );
}

BOOL IsPlayingMusic()
{
	return FSOUND_IsPlaying( g_FModData._iChannel );
}

BOOL IsStopMusic()
{
	if( g_FModData._iChannel == -1 )
		return TRUE;

	return ( !FSOUND_IsPlaying( g_FModData._iChannel ) );
}

void InitCustomSound( BOOL bEnable )
{
	g_bEnableMusic = bEnable;
	
	if (FSOUND_GetVersion() < FMOD_VERSION)
	{
		assert( 0 );
		UnInitCustomSound( );
		return;
	}

	if (!FSOUND_Init(44100, 32, FSOUND_NONBLOCKING ))
    {
		assert( 0 && "FSOUNT_Init failed" );
		UnInitCustomSound( );
		return;
    }

	FSOUND_Stream_SetBufferSize(1000);
}

void UnInitCustomSound()
{
	FSOUND_Stream_Close( g_FModData._stream );

	FSOUND_Close();
}

#endif 


BOOL CMusicMng::LoadScript( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	CString string;
	MUSIC music;
	int id = script.GetNumber();
	do
	{
		script.GetToken(); // fileName
		_tcscpy( music.szMusicFileName, script.token );
		m_aMusic.SetAtGrow( id, &music );
		id = script.GetNumber();
	} while( script.tok != FINISHED );
	m_aMusic.Optimize();

	return TRUE;
}

LPTSTR CMusicMng::GetFileName( DWORD dwId )
{
	MUSIC* pMusic = m_aMusic.GetAt( dwId );
	if( pMusic )
		return (LPTSTR)pMusic->szMusicFileName;
	return "";
}

#endif //#ifndef __MUSIC2