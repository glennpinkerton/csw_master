
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*

    csw_portP.h

      This header file is used to define platform specific items for
    various Colorado Softworks products.  The comments below describe
    each platform specific item in detail.

*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file csw_portP.h.
#endif


/*
    add nothing above this ifndef
*/
#ifndef CSW_PORTP_H
#define CSW_PORTP_H

/*
  ------------------------------------------------------------

    Define the proper default keyboard.

    The default keyboard is set by the define statements below.
    Comment out all but the keyboard being installed.  This
    defines the default keyboard.  The actual keyboard used can
    be set at run time with the -keyboard command line option or
    with the EASYX_KEYBOARD environment variable.  The section
    on the EasyX server in the utils.ref file explains how to
    set the keyboard at run time.

    The UNKNOWN_KEYBOARD value uses only alphabetic keys and the 
    control key for keyboard shortcuts in the EasyX server.  The
    other keyboards make use of special keys for shortcuts.

  -------------------------------------------------------------
*/

/*#define ULTRIX_KEYBOARD*/
/*#define SUN_KEYBOARD*/
/*#define PC_KEYBOARD*/

#define UNKNOWN_KEYBOARD







/*
  ------------------------------------------------------------

    Define whether the target computer has TCP/IP
    software installed.  If the computer is a stand
    alone PC, then the TCP/IP flag can be commented
    out.  For any platforms that are in a network,
    the TCP/IP flag should be uncommented.

    This is only used to get the host name to keep track of
    multiple instances of the EasyX server running.  If no
    TCP/IP is available, I assume that only one host needs
    to be kept track of.

  ------------------------------------------------------------
*/

#define TCP_IP_FLAG




/*
  ------------------------------------------------------------

    Define the number of bytes to read or write in graphics
    files before catching up on X event processing.  The
    shipped value will allow a lot of event processing.
    This is good for a slow disk system.  For faster disk
    I/O you may want to increase this number to speed up 
    the total throughput of graphics file I/O.

  -------------------------------------------------------------
*/

#define FILECHUNK           5000




/*
  ------------------------------------------------------------

    Define the number of application requests to process before
    catching up on user requests from the X server.  The default
    number of 100 will generally insure that the user response
    time is quite good.  If you have a very fast machine, you
    might want to increase this.  If you have a very slow machine,
    you might want to decrease this.

  ------------------------------------------------------------
*/

#define NUMAPPLOOP          100



/*
  --------------------------------------------------------------

    Define whether the signal function on the system runs the 
  signal handler one time only or multiple times.  For older
  SYSV3.2 UNIX systems, just before the signal handler is called,
  the default signal disposition is set.  On Solaris 2.x the same
  behavior is seen in the signal call.  On BSD systems (SunOS 4.1.3
  for example), the signal handler is called whenever a signal 
  arrives unless the disposition of the signal is explicitly reset
  via another call to the signal function.

    If the system building the EasyX software redefines the signal
  disposition to the default disposition before calling the signal
  handler, the ONE_TIME_SIGNAL preprocessor directive must be set.
  If the signal handler is called multiple times, until disposition
  is explicitly reset, you must comment out the #define below.

  --------------------------------------------------------------
*/

#define ONE_TIME_SIGNAL


/*
  -----------------------------------------------------------------

    Define whether the system uses void or int signal handler
  functions.  Most UNIX systems use void signal handlers, but 
  some, (e.g. IRIX), use int functions.  Set the SIGNAL_HANDLER
  preprocessor variable to either void or int, to agree with
  the system you are building for.

  -----------------------------------------------------------------
*/

#define SIGNAL_HANDLER   void      /*  for solaris and most UNIX systems  */
/*#define SIGNAL_HANDLER    int          for IRIX and possibly some others  */


/*
  -----------------------------------------------------------------

    Define whether to use signals as part of asynchronous picking
  by default.  If the DEFAULT_NO_SIGNAL preprocessor variable is
  set, then no signal is used by default.  If it is not set, then
  SIGUSR1 is used by default.  In either case, the default can be
  changed when the application is run by the EASYX_ASYNC_SIGNAL
  environment variable.  See the gtx_SetAsyncSignalHandler reference
  for more detail.

  ------------------------------------------------------------------
*/

/*#define DEFAULT_NO_SIGNAL*/


/*
  ------------------------------------------------------------------

    Specify whether the system supports the editres protocol for 
  widgets.  This is used in the user interface widget set only.
  If you have no idea what this is, then leave the following if
  block commented out.  If you know what editres is, and you have
  it available, and you wish to use it, uncomment the if block.

  ------------------------------------------------------------------
*/

/*
#ifndef USE_EDITRES
#  define USE_EDITRES
#endif
*/


/*  add nothing below this endif  */

#endif
