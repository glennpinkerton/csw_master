
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*

    csw_fileio.cc

    Implement the methods of the CSWFileioUtil class.  This class
    is a refactor of the old csw_fileio.c functions.

    This file has functions to open, close, read and write data
    to files.  There are also ancillary functions for locking files,
    setting file permissions, deleting files etc.  All operations 
    on files are separated in this module so that porting to a 
    different type of file system in the future will be easier.

    The functions in this file are as generic as possible.  In
    some cases, this generality produces lower performance or possibly
    incorrect performance (see csw_FileSync).  If you are building 
    on UNIX, you can use csw_ux_fio.c.  This gives better 
    performance and the csw_FileSync function works properly.

*/

#include <stdlib.h>

#ifdef SOLARIS
#define __EXTENSIONS__
#endif
/*#include <stdio.h>*/
#ifdef SOLARIS
#undef __EXTENSIONS__
#endif

#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>

/*
    system headers needed for windows NT compile
*/
#ifdef WINNT

#include <direct.h>
/*#include <process.h>*/
#include <io.h>

#else

/*
#include <unistd.h>
*/

#endif

/*
    private header files
*/
#include "csw/utils/include/csw_.h"

#include "csw/utils/private_include/csw_fileio.h"



/*
    ***************************************************************

                 c s w _ N e w F i l e P e r m i t

    ***************************************************************

    function name:     csw_NewFilePermit      (int)

    calling syntax:    csw_NewFilePermit (owner, group, world)

    synopsis:          Set the permissions for new files created by
                       subsequent calls to csw_CreateFile, 
                       csw_LockCreateIfNeeded, or csw_LockCreate.
                       The permission strings are in the following
                       form:

                         "rwxna"  where each character defines 
                                  permissions as follows

                          r       read permission
                          w       write and delete permission
                          x       execute permission
                          n       no permission
                          a       all permission

                       Before this function is called, the new file
                       permissions are set to all for owner, group
                       and world.  This permissions mode will be
                       modified by the current umask of the process
                       at the time a new file is actually created.

    return status:     always returns 1

    parameters:

      owner      r    char*       permission string for owner
      group      r    char*       permission string for group
      world      r    char*       permission string for world

*/

int CSWFileioUtil::csw_NewFilePermit (const char *owner,
                       const char *group,
                       const char *world)
{
    int          mode, tmp;
    char         local[100];

/*
    initialize file permissions mode to 0
*/
    mode = 0;

/*
    interpret owner string
*/
    csw_StrClean2 (local, owner, 100);
    csw_StrLeftJust (local);
    tmp = 0;
    ParsePermissionString (local, &tmp);
    tmp *= 0100;
    mode += tmp;

/*
    interpret group string
*/    
    csw_StrClean2 (local, group, 100);
    csw_StrLeftJust (local);
    tmp = 0;
    ParsePermissionString (local, &tmp);
    tmp *= 010;
    mode += tmp;

/*
    interpret world string
*/
    csw_StrClean2 (local, world, 100);
    csw_StrLeftJust (local);
    tmp = 0;
    ParsePermissionString (local, &tmp);
    mode += tmp;

    mode = mode;

    return 1;

}  /*  end of function csw_NewFilePermit  */




/*
    ****************************************************************

            P a r s e P e r m i s s i o n S t r i n g

    ****************************************************************

      Static function called only from csw_NewFilePermit.  This
    function interprets a permission string and returns an integer
    representation of that string in the mode parameter.

*/

int CSWFileioUtil::ParsePermissionString (const char *text,
                                  int *mode)
{
    int             eflag, rflag, wflag, i, len, tmp;
    char            c1;

/*
    set flags with execute, write and read bits
*/
    eflag = 1;
    wflag = 2;
    rflag = 4;

    tmp = 0;
    len = strlen (text);

/*
    loop through text, interpreting each character
    and building the permissions mask
*/
    for (i=0; i<len; i++) {
        
        c1 = text[i];
        switch (c1) {
            
            case XCHAR:
                tmp |= eflag;
                break;

            case RCHAR:
                tmp |= rflag;
                break;

            case WCHAR:
                tmp |= wflag;
                break;

            case ACHAR:
                tmp = eflag | wflag | rflag;
                *mode = tmp;
                return 1;

            case NCHAR:
                tmp = 0;
                *mode = tmp;
                return 1;

            default:
                break;

        }
    }

    *mode = tmp;

    return 1;

}  /*  end of private ParsePermissionString function  */



/*
  ******************************************************

               c s w _ C r e a t e F i l e

  ******************************************************

  function name:    csw_CreateFile      (int)

  call sequence:    csw_CreateFile (name, mode)

  synopsis:         Create a new file for writing.

  return values:    Returns the file number or -1 on error.

  calling parameters:

    name     r    char*    file name to create
    mode     r    char*    read write flags
                           "w" = write only
                           anything else = read and write

*/ 

int CSWFileioUtil::CSWFileioUtil::csw_CreateFile (const char *name,
                    const char *mode)
{
    char        local_name[1000];
    int         file;

    ValidFileName (local_name, name, 999);

    if (FirstOpen) {
        InitOpenFileList();
    }

    if (!strcmp(mode, "w")) {
        Ftmp = fopen (local_name, "wb");
    }
    else {
        Ftmp = fopen (local_name, "wb+");
    }

    file = NextFileNumber (Ftmp);

    return file;

}  /*  end of function csw_CreateFile  */
    

void CSWFileioUtil::ValidFileName (char *nameout,
                           const char *namein,
                           int maxlen)
{
    int             i, nc;
    unsigned char   uc;

    nameout[0] = '\0';
    if (namein == NULL) {
        return;
    }
    
    nc = strlen (namein);
    if (nc > maxlen-1) nc = maxlen-1;
    if (nc < 0) return;

/*
 * Invalid characters are control characters (e.g. alert, 
 * backspace, carriage return, form feed, horizontal tab,
 * newline and vertical tab), white space characters, and
 * punctuation excepting .(dot), /(forward slash), 
 * \(back slash), and -(hyphen).  A colon (:) is also allowed.
 */
    for (i=0; i<nc; i++) {
        nameout[i] = namein[i];
        uc = nameout[i];
        if (iscntrl(uc)  ||
            isspace(uc)) {
            nameout[i] = '_';
            continue;
        }
        if (ispunct(uc)) {
            if (nameout[i] == '.'  ||
                nameout[i] == '/'  ||
                nameout[i] == '\\' ||
                nameout[i] == '-'  ||
                nameout[i] == '_'  ||
                nameout[i] == ':') {
                    continue;
            }
            nameout[i] = '_';
            continue;
        }
    }

    nameout[nc] = '\0';

    strncpy (ValidName, nameout, 999);
    ValidName[999] = '\0';

    return;
}

/*
 *******************************************************

        c s w _ G e t V a l i d F i l e N a m e

 *******************************************************

  Return the ValidName string of the most recent file that
  was opened or closed.

*/

void CSWFileioUtil::csw_GetValidFileName (char *fname, int maxlen)
{
    if (fname == NULL) {
        return;
    }
    if (maxlen < 1) {
        return;
    }

    if (maxlen > 999) {
        maxlen = 999;
    }
    strncpy (fname, ValidName, maxlen-1);
    fname[maxlen-1] = '\0';

    return;
}

/*
  ******************************************************

                c s w _ O p e n F i l e

  ******************************************************

  function name:    csw_OpenFile      (int)

  call sequence:    csw_OpenFile (name, mode)

  synopsis:         Open an existing file for read only,
                    write only, or read/write.

  return values:    Returns file number or -1 on error.

  calling parameters:

    name      r    char*    file name to open
    mode      r    char*    read/write flag
                            "r" = read only
                            "w" = write only
                            anything else = read/write

*/ 

int CSWFileioUtil::csw_OpenFile (const char *name,
                  const char *mode)
{
    int       file;
    char      local[10];
    char      local_name[1000];

    if (FirstOpen) {
        InitOpenFileList();
    }

    csw_StrTruncate (local, mode, 10);
    csw_StrLeftJust (local);
    csw_StrLowerCase (local);

    ValidFileName (local_name, name, 999);

    /*
     * force binary file i/o to work on windows platforms
     */
    if (!strcmp(local, "r")) {
      Ftmp = fopen (local_name, "rb");
    } else if (!strcmp(local, "w")) {
      Ftmp = fopen (local_name, "wb");
    } else if (!strcmp(local, "a")) {
      Ftmp = fopen(local_name, "ab");
    } else if (!strcmp(local, "a+")) {
      Ftmp = fopen(local_name, "ab+");
    } else {
      Ftmp = fopen (local_name, "rb+");
    }

    file = NextFileNumber (Ftmp);

    if (file < 0  &&  Ftmp != NULL) {
        fclose (Ftmp);
    }

    return file;

}  /*  end of function csw_FileOpen  */

/*
  ******************************************************

                c s w _ A p p e n d F i l e

  ******************************************************

  function name:    csw_AppendFile      (int)

  call sequence:    csw_AppendFile (name, mode)

  synopsis:         Append an existing file for write only,
                    or read/write.

  return values:    Returns file number or -1 on error.

  calling parameters:

    name      r    char*    file name to open
    mode      r    char*    read/write flag
                            "w" = write only
                            anything else = read/write
    position       long*    position of EOF prior to appending
                            (store this value and use fseek
                            to find position of appended block
                            later)

*/ 

int CSWFileioUtil::csw_AppendFile (const char *name,
                  const char *mode,
                  long *position)
{
    int       file;
    char      local[10];
    char      local_name[1000];

    *position = 0L;
    
    if (FirstOpen) {
        InitOpenFileList();
    }

    csw_StrTruncate (local, mode, 10);
    csw_StrLeftJust (local);
    csw_StrLowerCase (local);

    ValidFileName (local_name, name, 999);

    /*
     * force binary file i/o to work on windows platforms
     */
    if (!strcmp(local, "a")) {
      Ftmp = fopen(local_name, "ab");
    } else {
      Ftmp = fopen(local_name, "ab+");
    }
    /* set position return parameter */
    if (Ftmp != NULL) {
      fseek(Ftmp, 0L, SEEK_END);
      *position = ftell(Ftmp);
    }

    file = NextFileNumber(Ftmp);

    if (file < 0  &&  Ftmp != NULL) {
      fclose(Ftmp);
    }

    return file;

}  /*  end of function csw_FileAppend  */


/*
  ******************************************************

                c s w _ C l o s e F i l e

  ******************************************************

    Close a file without deleting it.
    
*/ 

int CSWFileioUtil::csw_CloseFile (int filenum)
{
    if (filenum < 0) {
        return -1;
    }

    if (FirstOpen) {
        InitOpenFileList();
    }

/*
    close (filenum);
*/
    Ftmp = OpenFileList[filenum];
    if (Ftmp == NULL)
        return 1;

    fclose (Ftmp);
    OpenFileList[filenum] = NULL;
    Ftmp = NULL;

    return 1;

}  /*  end of function csw_CloseFile  */




/*
  ******************************************************

          c s w _ O p e n S c r a t c h F i l e

  ******************************************************

  function name:    csw_OpenScratchFile    (int)

  call sequence:    csw_OpenScratchFile (mode)

  synopsis:         Open a scratch file that will be deleted
                    when closed.  The function names the file.

  return values:    Returns a file descriptor or -1 on error.

  calling parameters:

    mode    r    char*   string of characters defining the
                         file mode. Valid characters are
                         r w t and b.  
                         r = read
                         w = write
                         t = text
                         b = binary
                         (text and binary are mutually exclusive)

*/ 

int CSWFileioUtil::csw_OpenScratchFile (const char *mode)
{
    char       local[40], temp[40];
    int        i, istat, offset;
    int        file;

    if (FirstOpen) {
        InitOpenFileList();
    }

/*
    Initialize the temporary file id's the first time
    this function is called.
*/
    if (open_scratch_file_first == 1) {
        for (i=0; i<MAXTF; i++) {
            Tfpoint[i] = -1;
        }
        open_scratch_file_first = 0;
    }

/*
    compress and lower case the mode string
*/
    csw_StrTruncate (local, (char *)mode, 40);
    csw_StrLeftJust (local);
    csw_StrLowerCase (local);

/*
    get a temporary file name
*/
    istat = csw_TmpFileName (temp);
    if (!istat) {
        strcpy (temp, "temp_file");
    }

    file = -1;
    file = csw_CreateFile (temp, "b");

/*
    add to list of scratch files
*/
    istat = 0;
    for (i=0; i<MAXTF; i++) {
        if (Tfpoint[i] == -1) {
            Tfpoint[i] = file;
            offset = 21*i;
            strcpy(Tfstrings+offset, temp);
            Tfname[i] = Tfstrings+offset;
            istat = 1;
            break;
        }
    }

/*
    Return an error if the list is full.
*/
    if (!istat) {
        if (Ftmp != NULL)
            fclose (Ftmp);
        file = -1;
    }

    return file;

}  /*  end of function csw_OpenScratchFile  */




/*
  ******************************************************

         c s w _ C l o s e S c r a t c h F i l e

  ******************************************************

  function name:    csw_CloseScratchFile      (int)

  call sequence:    csw_CloseScratchFile (file)

  synopsis:         Close a scratch file previously opened by
                    OpenScratchFile and delete the file if
                    successfully closed.

  return values:    status code

                   -1 = file was not opened with OpenScratchFile
                        In this case, the file is closed but not deleted.
                    0 = error closing or deleting
                    1 = normal successful completion

  calling parameters:

        file      r    int     File descriptor to close 
                               and delete.

*/ 

int CSWFileioUtil::csw_CloseScratchFile (int file)
{
    int          i;

    if (FirstOpen) {
        InitOpenFileList();
    }

    Ftmp = OpenFileList[file];
    if (Ftmp != NULL)
        fclose (Ftmp);

    for (i=0; i<MAXTF; i++) {
        if (file == Tfpoint[i]) {
            Tfpoint[i] = -1;
            csw_DeleteFile (Tfname[i]);
            return 1;
        }
    }

    return -1;

}  /*  end of function csw_CloseScratchFile  */



/*
  ******************************************************

               c s w _ T m p F i l e N a m e

  ******************************************************

  function name:    csw_TmpFileName     (int)

  call sequence:    csw_TmpFileName (name)

  synopsis:         Create a unique temporary file name on the
                    current working directory.

  return values:    status code

                    0 = no unique name could be created
                    1 = normal successful completion

  calling parameters:

    name    w    char*    character string to accept file name
                          this should be at least 20 characters long

*/ 

int CSWFileioUtil::csw_TmpFileName (char *name)
{
    char      timetick[20], ext[10], tmp[50];
    int       i, istat, errnum;

    struct stat    statbuf;

/*
    get current time tick string down to the second
*/
    istat = GetTimeTick (timetick);

    if (!istat) {
        strcpy (timetick, "temp_file");
    }

/*
    Add a 4 character extension to timetick until a filename
    not on the current working directory is found
*/
    i = 0;
    while (i<1000) {
        sprintf(ext, "_%03d", i);
        ext[4] = '\0';
        strcpy(tmp, timetick);
        strcat (tmp, ext);
        istat = stat(tmp, &statbuf);
        if (istat == -1) {
            errnum = csw_errno();
            if (errnum == ENOENT) {
                strcpy(name, tmp);
                return 1;
            }
        }
        i++;
    }

    return 0;

}  /*  end of function csw_TmpFileName  */




/*
  ******************************************************

                G e t T i m e T i c k

  ******************************************************

  function name:    GetTimeTick     (int)

  call sequence:    GetTimeTick (tname)

  synopsis:         return a string with the decimal month, day, hour,
                    minute, and second for the current time.

  return values:    status code

                    0 = error getting the time
                    1 = normal successful completion

  calling parameters:

    tname     w    char*   character string to write the time into
                           this should be at least 11 characters

*/ 

int CSWFileioUtil::GetTimeTick (char *tname)
{
    char              *t1;
    int               i;
    struct tm         *tmstruct;
    time_t            seconds;

/*
    first get seconds since the time origin
*/
    seconds = time(NULL);
    if (seconds == -1) {
        return 0;
    }

/*
    Separate into year, month etc.
*/
    tmstruct = localtime(&seconds);
    if (!tmstruct) {
        return 0;
    }

/*
    Build a string combining the month, day, hour, minute, second
    in the returned tname parameter.
*/
    t1 = tname;
    i = tmstruct->tm_mon + 1;
    sprintf(t1, "%02d", i);

    t1 += 2;
    sprintf(t1, "%02d", tmstruct->tm_mday);

    t1 += 2;
    sprintf(t1, "%02d", tmstruct->tm_hour);

    t1 += 2;
    sprintf(t1, "%02d", tmstruct->tm_min);

    t1 += 2;
    sprintf(t1, "%02d", tmstruct->tm_sec);

    t1 += 2;
    *t1 = '\0';

    return 1;

}  /*  end of private GetTimeTick function  */



/*

  ******************************************************

            c s w _ S e t F i l e P o s i t i o n

  ******************************************************

     Position the file pointer based on the specified
   offset and origin.  This is just a thin wrap around
   the fseek library call for now, but it is modularized
   here in case a different system call needs to be
   supported some day.

*/ 

int CSWFileioUtil::csw_SetFilePosition (int file,
                         int offset,
                         int origin)
{

    int    istat;

    if (file < 0  ||  file > MAX_OPEN_FILES-1) return -1;
    Ftmp = OpenFileList[file];
    if (Ftmp == NULL) return -1;

    istat = fseek (Ftmp, offset, origin);

    return istat;

}  /*  end of function csw_SetFilePosition  */



/*

  ******************************************************

         c s w _ G e t F i l e P o s i t i o n

  ******************************************************

     Report the file pointer position from the start of
   the file.  This is just a thin wrap around the ftell
   library call for now, but it is modularized here in
   case a different system call needs to be supported
   some day.

*/ 

int CSWFileioUtil::csw_GetFilePosition (int file)
{
    int         n;

    if (file < 0  ||  file > MAX_OPEN_FILES-1) return -1;
    Ftmp = OpenFileList[file];
    if (Ftmp == NULL) return -1;

    n = ftell (Ftmp);
    if (n < 0) {
        n = -1;
    }
    return n;

}  /*  end of function csw_GetFilePosition  */




/* 
  ******************************************************

                c s w _ B i n F i l e R e a d

  ******************************************************

  function name:    csw_BinFileRead      (int)

  call sequence:    csw_BinFileRead (buffer, size, count, file)

  synopsis:         Read data from a binary file.  This function
                    is separated in order to maintain either a 
                    direct unix i/o interface or a c lib interface.

  return values:    status code

                    0 = error reading the file
                    >0 = normal successful completion
                         A positive return value indicates the
                         number of items actually read from the file.
                         If a positive return value is less than count, 
                         the end of file has been reached.
                    -1 = The end of file was found without reading any data. 
                         This is not an error.  

  calling parameters:

    buffer    w    void*       Pointer to the buffer which will accept
                               the data from the disk file.  No bounds 
                               checking is done here.  The calling function
                               is responsible for creating a large enough 
                               buffer for the data to be read.
    size      r    int         size of each data item to be read
    count     r    int         number of data items to be read
    file      r    int         file descriptor to read from

*/ 

int CSWFileioUtil::csw_BinFileRead (void *buffer,
                     int size,
                     int count,
                     int file)
{
    int         nread;

/*
    if attempting to read from a bad pipe, return error
*/
    if (file == BadPipeNum) {
        return 0;
    }

    Ftmp = OpenFileList[file];
    nread = fread (buffer, size, count, Ftmp);
    if (ferror (Ftmp)) {
        nread = 0;
    }
    else if (feof (Ftmp)){
        nread = -1;
    }

    return nread;

}  /*  end of function csw_BinFileRead  */




/*
  ******************************************************

              c s w _ B i n F i l e W r i t e

  ******************************************************

  function name:    csw_BinFileWrite     (int)

  call sequence:    csw_BinFileWrite (buffer, size, count, file)

  synopsis:         Write data to a binary file.  This is a call to
                    either the unix low level i/o or the c library
                    binary i/o depending on the platform.

  return values:    status code

                    0 = error writing to file
                    >0 = normal successful completion
                         The return value indicates the number 
                         of items actually written and should be 
                         equal to count.

  calling parameters:

    buffer     r    void*       Pointer to the buffer to write to disk
    size       r    int         Size in bytes of each data item.
    count      r    int         Number of data items
    file       r    int         File number to write to

*/ 

int CSWFileioUtil::csw_BinFileWrite (const void *buffer,
                      int size,
                      int count,
                      int file)
{
    int          nwrite;

/*
    if attempting to write to a bad pipe, return error
*/
    if (file == BadPipeNum) {
        return 0;
    }

    DiskFullFlag = 0;

    Ftmp = OpenFileList[file];
    nwrite = fwrite (buffer, size, count, Ftmp);
    if (ferror (Ftmp)) {
        DiskFullFlag = 1;
        nwrite = 0;
    }

    if (ForceDiskFull == 1) {
        DiskFullFlag = 1;
        nwrite = 0;
    }

    return nwrite;

}  /*  end of function csw_BinFileWrite  */


int CSWFileioUtil::csw_IsDiskFull (void)
{
    return DiskFullFlag;
}


int CSWFileioUtil::csw_ForceDiskFull (int val)
{
    DiskFullFlag = val;
	return val;
}


/*
  ******************************************************

                 c s w _ F i l e S i z e

  ******************************************************

  function name:    csw_FileSize    (int)

  call sequence:    csw_FileSize (file)

  synopsis:         returns the size in bytes of a binary file
                    the size is not accurate for text files

  return values:    file size in bytes or -1 on error

  calling parameters:

    file    r   int      file number for an opened file

                         The position indicator for the file will
                         be changed to the beginning of the file
                         as a side effect of this function.

*/ 

int CSWFileioUtil::csw_FileSize (int file)
{
    int              now;

    if (file < 0  ||  file > MAX_OPEN_FILES-1) return -1;
    Ftmp = OpenFileList[file];
    if (Ftmp == NULL) return -1;

    now  = fseek (Ftmp, 0L, SEEK_END);
    now = ftell (Ftmp);
    rewind (Ftmp);
    if (now < 0) {
        now = -1;
    }
    return (now);

}  /*  end of function csw_FileSize  */



/*
  ******************************************************

                  c s w _ F l u s h F i l e

  ******************************************************

  function name:    csw_FlushFile      (int)

  call sequence:    csw_FlushFile (filenum)

  synopsis:         Flush the output file buffer 

  return values:    always returns 1

  calling parameters:

    filenum     r    int    File number to flush

*/ 

int CSWFileioUtil::csw_FlushFile (int filenum)
{

    Ftmp = OpenFileList[filenum];
    fflush (Ftmp);

    return 1;

}  /*  end of function csw_FlushFile  */





/*
  ******************************************************

                c s w _ R e w i n d F i l e

  ******************************************************

  function name:    csw_RewindFile      (int)

  call sequence:    csw_RewindFile (filenum)

  synopsis:         Reposition the file pointer to the start of the file.

  return values:    status code

                    -1 = error
                     0 = normal successful completion
                    >0 = error

  calling parameters:

    filenum     r    int      file number to rewind

*/ 

int CSWFileioUtil::csw_RewindFile (int filenum)
{
    
    if (filenum < 0  ||  filenum > MAX_OPEN_FILES-1) return -1;
    Ftmp = OpenFileList[filenum];
    if (Ftmp == NULL) return -1;

    rewind (Ftmp);

    return 0;

}  /*  end of function csw_RewindFile  */



/*
  ******************************************************

                c s w _ L o c k C r e a t e

  ******************************************************

  function name:    csw_LockCreate    (int)

  call sequence:    csw_LockCreate (name)

  synopsis:         Create a new file and lock it exclusively 

                    This function does not do any locking when
                    run on NT,  Locking is only performed on
                    UNIX.

  return values:    returns a file descriptor or -1 on error

  calling parameters:

    name        r    char*    file name

*/ 

int CSWFileioUtil::csw_LockCreate (const char *name)
{
    int              fd1, fd;

    if (FirstOpen) {
        InitOpenFileList();
    }

    fd = csw_CreateFile (name, "b");
    if (fd < 0) {
        return fd;
    }

    if (fd < 0  ||  fd > MAX_OPEN_FILES-1) return -1;
    Ftmp = OpenFileList[fd];
    if (Ftmp == NULL) return -1;
    fd1 = fd;

    return fd1;

}  /*  end of function csw_LockCreate  */



/*
  ******************************************************

      c s w _ L o c k C r e a t e I f N e e d e d

  ******************************************************

  function name:    csw_LockCreateIfNeeded    (int)

  call sequence:    csw_LockCreateIfNeeded (name)

  synopsis:         Create a new file and lock it exclusively. 
                    If the file already exists, open and lock.

                    Locking is not done on NT, only on UNIX.

  return values:    returns a file descriptor or -1 on error

  calling parameters:

    name        r    char*    file name

*/ 

int CSWFileioUtil::csw_LockCreateIfNeeded (const char *name)
{
    int             fd1, fd;

    if (FirstOpen) {
        InitOpenFileList();
    }

    fd = csw_OpenFile (name, "b");
    if (fd < 0) {
        fd = csw_CreateFile (name, "b");
    }
    if (fd < 0) {
        return fd;
    }

    if (fd < 0  ||  fd > MAX_OPEN_FILES-1) return -1;
    Ftmp = OpenFileList[fd];
    if (Ftmp == NULL) return -1;
    fd1 = fd;

    return fd1;

}  /*  end of function csw_LockCreateIfNeeded  */





/*
  ******************************************************

                c s w _ L o c k O p e n

  ******************************************************

  function name:    csw_LockOpen      (int)

  call sequence:    csw_LockOpen (name, mode)

  synopsis:         open a file and lock it exclusively 

                    Locking is not supported on NT, only on UNIX.
                    On NT, the file is opened but not locked.

  return values:    returns a file descriptor or -1 on error

  calling parameters:

    name        r    char*    file name
    mode        r    char*    mode to open the file in
                              "r" = read only
                              "w" = write only
                              anything else read and write

*/ 

int CSWFileioUtil::csw_LockOpen (const char *name,
                  const char *mode)
{
    int             fd1, fd;

    if (FirstOpen) {
        InitOpenFileList();
    }

    fd = csw_OpenFile (name, mode);
    if (fd < 0) {
        return fd;
    }

    if (fd < 0  ||  fd > MAX_OPEN_FILES-1) return -1;
    Ftmp = OpenFileList[fd];
    if (Ftmp == NULL) return -1;
    fd1 = fd;

    return fd1;

}  /*  end of function csw_LockOpen  */




/*
  ******************************************************

               c s w _ L o c k C l o s e
              
  ******************************************************

  function name:    csw_LockClose      (int)

  call sequence:    csw_LockClose (fd)

  synopsis:         Unlock and close a file

  return values:    always returns zero

  calling parameters:

    fd     r    int     file descriptor

*/ 

int CSWFileioUtil::csw_LockClose (int fd)
{

    if (fd < 0) {
        return 0;
    }

    Ftmp = OpenFileList[fd];
    if (Ftmp) {
        fclose (Ftmp);
        OpenFileList[fd] = NULL;
    }

    return 0;

}  /*  end of function csw_LockClose  */



/*
  ******************************************************

                 c s w _ F i l e S y n c

  ******************************************************

    make sure file update is written to disk

*/ 

int CSWFileioUtil::csw_FileSync (int fd)
{

/*
    Use the ANSI fflush function, even though it does not
    always insure a disk write.  
*/
    if (fd < 0  ||  fd > MAX_OPEN_FILES-1) return -1;
    Ftmp = OpenFileList[fd];
    if (Ftmp == NULL) return -1;
    fflush (Ftmp);

    return 1;

}  /*  end of function csw_FileSync  */




/*
  ****************************************************************

                       c s w _ g e t s

  ****************************************************************

    Emulates the c library gets function, which doesn't work
    correctly on ISC unix.  If the LogFile has been opened,
    the characters read from standard input are echoed to the
    log file.

*/

char *CSWFileioUtil::csw_gets (char *s)
{
    char      *s0, *sout, sline[500];
    int       ichar, len, freeflag;

/*
    initialize output strings to NULL strings
    in case a signal interrupts the read
*/
    sline[0] = '\0';
    s[0] = '\0';

/*
    Read from a file if FileIn is not NULL.  When the
    end of file is found, read from standard input.
*/
    if (FileIn) {
        sout = csw_fgets (sline, 500, FileIn);
        if (sout) {
            strcpy (s, sout);
            return s;
        }
        else {
            FileIn = NULL;
        }
    }

/*
    get a character at a time from standard input
    until a newline is seen
*/
    s0 = s;
    for (;;) {
        ichar = getchar ();
        if (ichar == EOF) {
            return NULL;
        }
        if ((char)ichar == '\n') {
            *s = '\0';
            break;
        }
        *s = (char)ichar;
        if (*s != '\r') {
            s++;
        }
    }

/*
    if the logfile is open output the string to it
*/
    if (LogFileId != -1) {
        len = strlen (s0) + 3;
        if (len < 500) {
            sout = sline;
            freeflag = 0;
        }
        else {
            sout = (char *)csw_Malloc (len * sizeof(char));
            if (!sout) {
                return s0;
            }
            freeflag = 1;
        }
        strcpy (sout, s0);
        strcat (sout, "\n");
        csw_BinFileWrite (sout, strlen(sout), 1, LogFileId);
        if (freeflag) csw_Free (sout);
    }

    return s0;

}  /*  end of function csw_gets  */





int CSWFileioUtil::csw_SetStandardInputFile (FILE *filein)
{
    FileIn = filein;
    return 1;
}





FILE *CSWFileioUtil::csw_GetStandardInputFile (void)
{
    return FileIn;
}




/*
  ****************************************************************

                   c s w _ f g e t s

  ****************************************************************

    Emulate the fgets c library function, which does not work
    correctly on ISC UNIX.

*/

char *CSWFileioUtil::csw_fgets (char *s,
                 int n,
                 FILE *file)
{
    int       i, ichar, j;

/*
    Get one character at a time from a file
    until the newline or end of file is seen
    or until the buffer is full.
*/
    j = 0;
    for (i=0; i<n-1; i++) {
        ichar = getc (file);
        if ((char)ichar == '\n') {
            s[j] = '\0';
            return s;
        }
        if (ichar == EOF) {
            s[i] = '\0';
            return NULL;
        }
        s[j] = (char)ichar;
        if (s[j] == '\r') {
            continue;
        }
        j++;
    }

    s[j] = '\0';
    return s;

}  /*  end of function csw_fgets  */



/*
  ****************************************************************

                      c s w _ D e l e t e F i l e

  ****************************************************************

    Use a system specific call to delete a file name.  The full
  path name should be supplied to be safe.  No confirmation is
  done by this function.  The return status is 0 on successful 
  deletion or -1 if the file could not be deleted.

*/

int CSWFileioUtil::csw_DeleteFile (const char *path)
{
    int              istat;

    istat = remove (path);

    return istat;

}  /*  end of function csw_DeleteFile  */




/*
  ******************************************************

            c s w  _ D o e s F i l e E x i s t

  ******************************************************

  function name:    csw_DoesFileExist      (int)

  call sequence:    csw_DoesFileExist (name)

  synopsis:         report whether a file exists or not

  return values:    0 = file does not exist
                    1 = file exists

  calling parameters:

    name    r    char*     file name

*/ 

int CSWFileioUtil::csw_DoesFileExist (const char *name)
{
    int            istat, errnum;
    FILE           *fptr;
    struct stat    statbuf;

    if (!name) {
        return 0;
    }
    if (name[0] == '\0') {
        return 0;
    }

    fptr = fopen (name, "r");
    if (fptr == NULL) {
        return 0;
    }
    fclose (fptr);

    istat = stat (name, &statbuf);

    if (istat == -1) {
        errnum = csw_errno();
        if (errnum == ENOENT) {
            return 0;
        }
        return 1;
    }

    return 1;

}  /*  end of function csw_DoesFileExist  */




/*
  ******************************************************

            c s w _ I s F i l e N a m e V a l i d

  ******************************************************

  function name:    csw_IsFileNameValid      (int)

  call sequence:    csw_IsFileNameValid (name)

  synopsis:         report whether a file name is valid or not

  return values:    0 = file name is not valid
                    1 = file name is valid

  calling parameters:

    name    r    char*     file name

*/ 

int CSWFileioUtil::csw_IsFileNameValid (const char *name)
{
    int            istat, len, errnum;
    struct stat    statbuf;

    if (!name) {
        return 0;
    }
    if (name[0] == '\0') {
        return 0;
    }

    len = strlen (name);
    if (len > 256) {
        return 0;
    }
    
    istat = stat (name, &statbuf);

    if (istat == -1) {
        errnum = csw_errno();
        if (errnum == EFAULT  ||  
            errnum == ENAMETOOLONG  ||  
            errnum == ENOTDIR) {
            return 0;
        }
        return 1;
    }

    return 1;

}  /*  end of function csw_IsFileNameValid  */




/*
  ****************************************************************

                 c s w _ C r e a t e L o g F i l e

  ****************************************************************

    Create a new log file to which all characters read by csw_gets will
  be output.  If a file exists with this name it will be overwritten.
  On success, the log file number is returned.  On failure, -1 is
  returned.

*/

int CSWFileioUtil::csw_CreateLogFile (const char *fname)
{
    int            istat;

    if (!fname) {
        return -1;
    }
    if (LogFileId != -1) {
        return -1;
    }

    istat = csw_CreateFile (fname, "b");
    if (istat == -1) {
        return -1;
    }

    LogFileId = istat;

    return istat;

}  /*  end of function csw_CreateLogFile  */



/*
  ****************************************************************

               c s w _ C l o s e L o g F i l e

  ****************************************************************

    Close a file that was previously opened with csw_OpenLogFile.

*/

int CSWFileioUtil::csw_CloseLogFile (void)
{
    if (FirstOpen) {
        InitOpenFileList();
    }

    if (LogFileId == -1) {
        return 1;
    }

    csw_CloseFile (LogFileId);

    return 1;

}  /*  end of function csw_CloseLogFile  */





/*
  ****************************************************************

                 c s w _ S e t B a d P i p e N u m

  ****************************************************************

    Set the bad pipe number to the number of a file that has been
  flagged as a bad pipe.  Any reads or writes to the file number
  will return an error.

*/

int CSWFileioUtil::csw_SetBadPipeNum (int fd)
{

    BadPipeNum = fd;
    return 1;

}  /*  end of function csw_SetBadPipeNum  */



/*
  ****************************************************************

                 c s w _ G e t B a d P i p e N u m

  ****************************************************************

    Return the bad pipe number currently in use.

*/

int CSWFileioUtil::csw_GetBadPipeNum (void)
{

    return BadPipeNum;

}  /*  end of function csw_GetBadPipeNum  */



/*
  ****************************************************************

             c s w _ C r e a t e D i r e c t o r y

  ****************************************************************

    This is a thin wrap around the system mkdir function.  The 
  directory is created with full write permissions for the world.

*/

int CSWFileioUtil::csw_CreateDirectory (const char *name)
{
    int         istat;

#ifdef WINNT

    istat = mkdir (name);

#else

    istat = mkdir (name, 0777);

#endif

    if (istat == -1) {
        return -1;
    }

    return 1;

}  /*  end of function csw_CreateDirectory  */



/*
  ****************************************************************

                    c s w _ T m p D i r

  ****************************************************************

    Return a temporary directory name.

*/

int CSWFileioUtil::csw_TmpDir (char *name, int namelen)
{
    int        len1;
    char       *cenv;

    if (namelen < 5) return -1;

    cenv = csw_getenv ("TMPDIR");
    if (cenv) {
        if (csw_DoesFileExist (cenv)) {
            len1 = strlen (cenv);
            if (len1 > namelen) {
                strcpy (name, "/tmp");
            }
            else {
                strcpy (name, cenv);
            }
        }
    }
    else {
        strcpy (name, "/tmp");
    }

    return 1;

}  /*  end of function csw_TmpDir  */



/*
  ****************************************************************

             c s w _ D e l e t e D i r e c t o r y

  ****************************************************************

    Use a system specific call to delete an entire directory.  The full
  path name should be supplied to be safe.  No confirmation is
  done by this function.  The return status is 0 on successful 
  deletion or -1 if the directory could not be deleted.

*/

int CSWFileioUtil::csw_DeleteDirectory (const char *pathin)
{
    int              istat;
    char             command[1000];

    sprintf (command, "/bin/rm -rf %s", pathin);
    istat = system (command);

    return istat;

}  /*  end of function csw_DeleteDirectory  */




/*
  ****************************************************************

                I n i t O p e n F i l e L i s t

  ****************************************************************

*/

int CSWFileioUtil::InitOpenFileList (void)
{
    int          i;

    if (FirstOpen == 0) return 1;

    for (i=0; i<MAX_OPEN_FILES; i++) {
        OpenFileList[i] = NULL;
    }
    i = csw_fileno (stdin);
    if (i < 0)
        OpenFileList[0] = stdin;
    else 
        OpenFileList[i] = stdin;

    i = csw_fileno (stdout);
    if (i < 0)
        OpenFileList[1] = stdout;
    else 
        OpenFileList[i] = stdout;

    i = csw_fileno (stderr);
    if (i < 0)
        OpenFileList[2] = stderr;
    else 
        OpenFileList[i] = stderr;

    FirstOpen = 0;

    return 1;

}  /*  end of private InitOpenFileList function  */







/*
  ****************************************************************

                   N e x t F i l e N u m b e r

  ****************************************************************

    Return the next file number after setting the OpenFileList entry
  for that number to the specified file pointer.

*/

int CSWFileioUtil::NextFileNumber (FILE *ptr)
{
    int         i;

    if (ptr == NULL) return -1;

    for (i=0; i<MAX_OPEN_FILES; i++) {
        if (OpenFileList[i] == NULL) {
            OpenFileList[i] = ptr;
            return i;
        }
    }

    return -1;
    
}  /*  end of private NextFileNumber function  */




/*
    The following group of functions reads binary data types from
    a file and returns internal data type (e.g. int, float).  The
    csw_SetByteSwap function must be called prior to any read calls
    or you will only coincidentally get the correct results.
*/


/* 
  ****************************************************************************

                     c s w _ S e t S w a p F l a g

  ****************************************************************************

    A short int number is specified in both internal form (sint) and as it 
  was read from disk (2 bytes in cint).  This function determines whether
  the two forms are byte swapped from each other or not.  If they are 
  swapped, the ByteSwapFlag private variable is set to 1 and subsequent
  calls to csw_ReadI2, csw_ReadI4 and csw_ReadR4 will byte swap before
  returning their results.

*/

int CSWFileioUtil::csw_SetSwapFlag (short int sint, char *cint)
{
    I2Equiv            i2;

    i2.i2 = sint;
    if (i2.c2[0] == cint[0]  &&  i2.c2[1] == cint[1]) {
        ByteSwapFlag = 0;
    }
    else {
        ByteSwapFlag = 1;
    }

    return 1;

}  /*  end of function csw_SetSwapFlag  */





/* 
  ****************************************************************************

                          c s w _ R e a d I 2

  ****************************************************************************

    Read the next two bytes from the specified file and return them as a
  short integer.  If byte swapping is needed it is done here.

*/

int CSWFileioUtil::csw_ReadI2 (int filenum, short int *i2val)
{
    int             istat;
    char            c2[2], c1;
    I2Equiv         i2;

    *i2val = 0;
    istat = csw_BinFileRead (c2, 1, 2, filenum);
    if (istat < 2) {
        return -1;
    }

    i2.c2[0] = c2[0];
    i2.c2[1] = c2[1];
    if (ByteSwapFlag) {
        c1 = i2.c2[0];
        i2.c2[0] = i2.c2[1];
        i2.c2[1] = c1;
    }

    *i2val = i2.i2;

    return 1;

}  /*  end of function csw_ReadI2  */







/* 
  ****************************************************************************

                          c s w _ R e a d I 4

  ****************************************************************************

    Read the next four bytes from the specified file and return them as a
  long integer.  If byte swapping is needed it is done here.

*/

int CSWFileioUtil::csw_ReadI4 (int filenum, int *i4val)
{
    int             istat;
    char            c4[4], c1;
    I4Equiv         i4;

    *i4val = 0;
    istat = csw_BinFileRead (c4, 1, 4, filenum);
    if (istat < 4) {
        return -1;
    }

    i4.c4[0] = c4[0];
    i4.c4[1] = c4[1];
    i4.c4[2] = c4[2];
    i4.c4[3] = c4[3];
    if (ByteSwapFlag) {
        c1 = i4.c4[0];
        i4.c4[0] = i4.c4[3];
        i4.c4[3] = c1;
        c1 = i4.c4[1];
        i4.c4[1] = i4.c4[2];
        i4.c4[2] = c1;
    }

    *i4val = i4.i4;

    return 1;

}  /*  end of function csw_ReadI4  */






/* 
  ****************************************************************************

                          c s w _ R e a d R 4

  ****************************************************************************

    Read the next four bytes from the specified file and return them as a
  float.  If byte swapping is needed it is done here.

*/

int CSWFileioUtil::csw_ReadR4 (int filenum, float *r4val)
{
    int             istat;
    char            c4[4], c1;
    R4Equiv         r4;

    *r4val = 0.0f;
    istat = csw_BinFileRead (c4, 1, 4, filenum);
    if (istat < 4) {
        return -1;
    }

    r4.c4[0] = c4[0];
    r4.c4[1] = c4[1];
    r4.c4[2] = c4[2];
    r4.c4[3] = c4[3];
    if (ByteSwapFlag) {
        c1 = r4.c4[0];
        r4.c4[0] = r4.c4[3];
        r4.c4[3] = c1;
        c1 = r4.c4[1];
        r4.c4[1] = r4.c4[2];
        r4.c4[2] = c1;
    }

    *r4val = r4.fval;

    return 1;

}  /*  end of function csw_ReadR4  */








/* 
  ****************************************************************************

                          c s w _ R e a d R 8

  ****************************************************************************

    Read the next four bytes from the specified file and return them as a
  double.  If byte swapping is needed it is done here.

*/

int CSWFileioUtil::csw_ReadR8 (int filenum, double *r8val)
{
    int             istat, i1;
    char            c8[8], c1;
    R8Equiv         r8;

    *r8val = 0;
    istat = csw_BinFileRead (c8, 1, 8, filenum);
    if (istat < 8) {
        return -1;
    }

    r8.c8[0] = c8[0];
    r8.c8[1] = c8[1];
    r8.c8[2] = c8[2];
    r8.c8[3] = c8[3];
    r8.c8[4] = c8[4];
    r8.c8[5] = c8[5];
    r8.c8[6] = c8[6];
    r8.c8[7] = c8[7];
    if (ByteSwapFlag) {
        c1 = r8.c8[0];
        r8.c8[0] = r8.c8[3];
        r8.c8[3] = c1;
        c1 = r8.c8[1];
        r8.c8[1] = r8.c8[2];
        r8.c8[2] = c1;
        c1 = r8.c8[4];
        r8.c8[4] = r8.c8[7];
        r8.c8[7] = c1;
        c1 = r8.c8[5];
        r8.c8[5] = r8.c8[6];
        r8.c8[6] = c1;
        i1 = r8.ival[0];
        r8.ival[0] = r8.ival[1];
        r8.ival[1] = i1;
    }

    *r8val = r8.dval;

    return 1;

}  /*  end of function csw_ReadR8  */




/*
  ****************************************************************************

                   c s w _ R e a d I 4 S t r i n g

  ****************************************************************************

    Read the next four bytes from the specified string and return them as a
  long integer.  If byte swapping is needed it is done here.

*/
 
int CSWFileioUtil::csw_ReadI4String (char *text, int *i4val)
{
    char            c4[4], c1;
    I4Equiv         i4;  

    *i4val = 0;
 
    memcpy (c4, text, 4); 
 
    i4.c4[0] = c4[0];
    i4.c4[1] = c4[1];
    i4.c4[2] = c4[2];
    i4.c4[3] = c4[3];
    if (ByteSwapFlag) {
        c1 = i4.c4[0];
        i4.c4[0] = i4.c4[3];
        i4.c4[3] = c1;
        c1 = i4.c4[1];
        i4.c4[1] = i4.c4[2];
        i4.c4[2] = c1;
    }
 
    *i4val = i4.i4;
 
    return 1;
 
}  /*  end of function csw_ReadI4String  */




/*
  ****************************************************************************

                   c s w _ R e a d C 4 S t r i n g

  ****************************************************************************

    Read the next four bytes from the specified string and return them as a
  byte swapped string if byte swapping is needed.

*/
 
int CSWFileioUtil::csw_ReadC4String (char *text, char *c4)
{
    char            c1;

    memcpy (c4, text, 4); 
 
    if (ByteSwapFlag) {
        c1 = c4[0];
        c4[0] = c4[3];
        c4[3] = c1;
        c1 = c4[1];
        c4[1] = c4[2];
        c4[2] = c1;
    }

    return 1;
 
}  /*  end of function csw_ReadC4String  */




/*
  ****************************************************************

               c s w _ g e t s _ c o m m e n t

  ****************************************************************

    Emulates the c library gets function, which doesn't work
    correctly on ISC unix.  If the LogFile has been opened,
    the characters read from standard input are echoed to the
    log file.  If the first char is a #, the next line is read,
    assuming the # line is a comment.

*/

char *CSWFileioUtil::csw_gets_comment (char *s)
{
    char      *s0, *sout, sline[500];
    int       ichar, len, freeflag;

/*
    initialize output strings to NULL strings
    in case a signal interrupts the read
*/
    sline[0] = '\0';
    s[0] = '\0';

/*
    Read from a file if FileIn is not NULL.  When the
    end of file is found, read from standard input.
*/
    if (FileIn) {
        sout = csw_fgets (sline, 500, FileIn);
        if (sout) {
            strcpy (s, sout);
            return s;
        }
        else {
            FileIn = NULL;
        }
    }

/*
    get a character at a time from standard input
    until a newline is seen
*/
    s0 = s;
    for (;;) {
        ichar = getchar ();
        if (ichar == EOF) {
            return NULL;
        }
        if ((char)ichar == '\n') {
            *s = '\0';
            if (*s0 != '#') {
                break;
            }
            s = s0;
            continue;
        }
        *s = (char)ichar;
        s++;
    }

/*
    if the logfile is open output the string to it
*/
    if (LogFileId != -1) {
        len = strlen (s0) + 3;
        if (len < 500) {
            sout = sline;
            freeflag = 0;
        }
        else {
            sout = (char *)csw_Malloc (len * sizeof(char));
            if (!sout) {
                return s0;
            }
            freeflag = 1;
        }
        strcpy (sout, s0);
        strcat (sout, "\n");
        csw_BinFileWrite (sout, strlen(sout), 1, LogFileId);
        if (freeflag) csw_Free (sout);
    }

    return s0;

}  /*  end of function csw_gets_comment  */
