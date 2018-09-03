
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_fileio.h
*/


/*
 *  This header defines the CSWFileioUtil class.
 *  
 *  This class refactors the functions found in the old 
 *  csw_fileio.c file.
 *
 */

#ifndef CSW_FILEIO_H
#define CSW_FILEIO_H


#include    <stdio.h>

/*
    constant definitions needed only by this file
*/
#define MAXTF             100
#define MAXTFCHARS        2200
#define MAX_OPEN_FILES    1000

/*
    constants used to parse file protection strings
*/
#define XCHAR             'x'
#define WCHAR             'w'
#define RCHAR             'r'
#define ACHAR             'a'
#define NCHAR             'n'



class CSWFileioUtil
{

  private:

    int             open_scratch_file_first = 1;

// Old file static variables are now class private variables.
    char            Tfstrings[MAXTFCHARS], *Tfname[MAXTF];
    int             Tfpoint[MAXTF];

    int             LogFileId = -1;
    int             BadPipeNum = -1;

    FILE            *FileIn = NULL,
                    *Ftmp = NULL,
                    *OpenFileList[MAX_OPEN_FILES];
    int             FirstOpen = 1;

    int             DiskFullFlag = 0;

    int             ForceDiskFull = 0;

    char            ValidName[1000];

    int             ByteSwapFlag = 0;

// Old file static functions become private class functions.
    int             ParsePermissionString (const char *, int *);
    int             GetTimeTick (char *);
    int             InitOpenFileList (void);
    int             NextFileNumber (FILE *);
    void            ValidFileName (char *nameout,
                                   const char *namein,
                                   int maxlen);
  public:

    CSWFileioUtil () {};
    ~CSWFileioUtil () {};

// It makes no sense to copy construct, move construct,
// assign or move assign an object of this class.  The
// various copy methods are flagged "delete" to prevent
// their use.

    CSWFileioUtil (const CSWFileioUtil &old) = delete;
    const CSWFileioUtil &operator=(const CSWFileioUtil &old) = delete;
    CSWFileioUtil (CSWFileioUtil &&old) = delete;
    const CSWFileioUtil &operator=(CSWFileioUtil &&old) = delete;

    int csw_NewFilePermit (const char *, const char *, const char *);
    int csw_CreateFile (const char *, const char *);
    void csw_GetValidFileName (char *filename, int maxlen);
    int csw_OpenFile (const char *, const char *);
    int csw_AppendFile (const char *, const char *, long *);
    int csw_CloseFile (int);
    int csw_OpenScratchFile (const char *);
    int csw_CloseScratchFile (int);
    int csw_TmpFileName (char *);
    int csw_SetFilePosition (int, int, int);
    int csw_GetFilePosition (int);
    int csw_BinFileRead (void *, int, int, int);
    int csw_BinFileWrite (const void *, int, int, int);
    int csw_IsDiskFull (void);
    int csw_ForceDiskFull (int);
    int csw_FileSize (int);
    int csw_FlushFile (int);
    int csw_RewindFile (int);
    int csw_LockCreate (const char *);
    int csw_LockCreateIfNeeded (const char *);
    int csw_LockOpen (const char *, const char *);
    int csw_LockClose (int);
    int csw_FileSync (int);
    char *csw_gets (char *);
    char *csw_gets_comment (char *);
    int csw_SetStandardInputFile (FILE *);
    FILE *csw_GetStandardInputFile (void);
    char *csw_fgets (char *, int, FILE *);
    int csw_DeleteFile (const char *);
    int csw_DeleteDirectory (const char *);
    int csw_DoesFileExist (const char *);
    int csw_IsFileNameValid (const char *);
    int csw_CreateLogFile (const char *);
    int csw_CloseLogFile (void);
    int csw_SetBadPipeNum (int);
    int csw_GetBadPipeNum (void);
    int csw_CreateDirectory (const char *);
    int csw_TmpDir (char*, int);

    int csw_SetSwapFlag (short int, char*);
    int csw_ReadI2 (int, short int*);
    int csw_ReadI4 (int, int*);
    int csw_ReadI4String (char*, int*);
    int csw_ReadC4String (char*, char*);
    int csw_ReadR4 (int, float*);
    int csw_ReadR8 (int, double*);




}; // end of class definition

#endif

/*
    end of header file
    add nothing below this endif
*/
