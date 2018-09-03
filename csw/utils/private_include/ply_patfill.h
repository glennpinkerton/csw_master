
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_patfill.h

    Define the CSWPolyPatfill class.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_patfill.h.
#endif



#ifndef PLY_PATFILL_H
#define PLY_PATFILL_H

#include "csw/utils/include/csw_errnum.h"
#include "csw/utils/include/csw_timer.h"

#include "csw/utils/private_include/ply_fpatP.h"

#define MAXTILEVECS           2000
#define MAXTILEARCS           300
#define MAXPRIMNODES          100000

#define MINWORKVECS           50
#define MAXWORKVECS           500
#define MAXWORKCOMPS          50
#define MAXPRIMCOMPS          10


class CSWPolyPatfill
{

  private:

    int                    ClipPattPrimFlag {0};

    POlygonClipPrimRec     *CprimPtr {NULL},
                           *CprimW {NULL};
    int                    NcprimW {0},
                           NcprimPtr {0},
                           MaxCprimW {0},
                           MaxXcprimW {0},
                           NxcprimW{0};
    CSW_F                  *XcprimPtr {NULL},
                           *YcprimPtr {NULL},
                           *XcprimW {NULL},
                           *YcprimW {NULL};

    int                    IvecTile[MAXTILEARCS];

    CSW_F                  XvecTile[MAXTILEVECS],
                           YvecTile[MAXTILEVECS],
                           XarcTile[MAXTILEARCS],
                           YarcTile[MAXTILEARCS],
                           RarcTile[MAXTILEARCS];

    double                 XvecD[MAXWORKVECS],
                           YvecD[MAXWORKVECS],
                           Xvec2[MAXWORKVECS],
                           Yvec2[MAXWORKVECS];

    int                    Ivec2[MAXWORKCOMPS],
                           Ivec3[MAXWORKCOMPS];

    int                    PatNum {0};
    CSW_F                  PatScale {1.0f};

    CSW_F                  *XvecPoly {NULL},
                           *YvecPoly {NULL},
                           *XarcPoly {NULL},
                           *YarcPoly {NULL},
                           *RarcPoly {NULL};

    CSW_F                  *XfvecPoly {NULL},
                           *YfvecPoly {NULL},
                           *XfarcPoly {NULL},
                           *YfarcPoly {NULL},
                           *RfarcPoly {NULL};

    int                    MaxVptPoly {0},
                           MaxFptPoly {0},
                           MaxVecPoly {0},
                           MaxArcPoly {0},
                           MaxFvecPoly {0},
                           MaxFarcPoly {0};

    int                    *IvecPoly {NULL},
                           NvecPoly {0},
                           NptPoly {0},
                           NarcPoly {0};

    int                    *IfvecPoly {NULL},
                           NfvecPoly {0},
                           NfptPoly {0},
                           NfarcPoly {0};

    int                    NvecOut {0},
                           NarcOut {0},
                           NfvecOut {0},
                           NfarcOut {0},
                           NptOut {0},
                           NfptOut {0};

    double                 CellSize {0.1f},
                           PatX0{0.0},
                           PatY0{0.0};

    char                   *HgridP {NULL},
                           *VgridP {NULL},
                           *IogridP {NULL},
                           *BcellP {NULL};
    double                 Gx1{0.0},
                           Gy1{0.0},
                           Gx2{0.0},
                           Gy2{0.0};

    int                    NcolMax {0},
                           NrowMax {0};
    char                   *CellInside {NULL};

    int                    AllOrNothingFlag {1};

    double                 *Xvc {NULL},
                           *Yvc {NULL};
    double                 Fx0{0.0},
                           Fy0{0.0};
    int                    *Ivc {NULL},
                           *Ivc2 {NULL},
                           Nvc {0},
                           MaxIvc {0},
                           MaxXvc {0},
                           FirstClip {1};


  public:

    CSWPolyPatfill () {};
    ~CSWPolyPatfill ()
    {
        ply_PattWorkFree ();
    };

// It makes no sense to copy construct, move construct,
// assign or move assign an object of this class.  The
// various copy methods are flagged "delete" to prevent
// their use.

    CSWPolyPatfill (const CSWPolyPatfill &old) = delete;
    const CSWPolyPatfill &operator=(const CSWPolyPatfill &old) = delete;
    CSWPolyPatfill (CSWPolyPatfill &&old) = delete;
    const CSWPolyPatfill &operator=(CSWPolyPatfill &&old) = delete;

    int ply_SetPatternAllOrNothingFlag (int val);
    int ply_GetAllOrNothingFlag (void);
    int ply_SetupPatFill (CSW_F *xp, CSW_F *yp, int *ic, int nc,
                          int patnum, CSW_F patscale);
    int ply_SetupPatFill2 (double *xp, double *yp, int *ic, int nc,
                           int patnum, double patscale);
    int ply_OutputTile (double x0, double y0);
    int ply_NextPatVec (CSW_F *x, CSW_F *y, int *npts);
    int ply_NextPatVecFill (CSW_F *x, CSW_F *y, int *npts);
    int ply_NextPatArc (CSW_F *x, CSW_F *y, CSW_F *r);
    int ply_NextPatArcFill (CSW_F *x, CSW_F *y, CSW_F *r);
    int ply_ClipPrimToPoly (double *xp, double *yp, int np, int *ic, int *ih);
    int ply_ClipPrimToPoly1 (double *xp, double *yp, int *ic, int nc);
    int ply_DoesFillPrimNeedClipping (double x1, double y1, double x2, double y2);
    int ply_ClipVecsForTile (double *xp, double *yp, int *ic, int nc, int np);
    int ply_PatPrimLimits (CSW_F *x, CSW_F *y, int npts,
                           double *x1, double *y1, double *x2, double *y2);
    int ply_ClipPolysForTile (double *xp, double *yp, int *ic, int nc, int np);
    int ply_ClipArcsForTile (double *xp, double *yp, int *ic, int nc, int np);
    int ply_ClipDotsForTile (double *xp, double *yp, int *ic, int nc, int np);
    int ply_ClipLongTileLines (double *xp, double *yp, int *ic, int nc);
    int ply_setprimscangrids (double *xp, double *yp, int *ic, int nc);
    int ply_SetupTileClipVecs (double *xp, double *yp, int *ic, int nc);
    int ply_SetPattClipSaveFlag (int val);
    int ply_GetPattClipSaveFlag (void);
    int ply_TransferClipPrimData (void);
    int ply_AddSavedClipPrimData (void);
    int ply_PattWorkAlloc (int nv1, int nf1, int nc1, int nd1);
    int ply_PattWorkFree (void);

};  // end of main class definition

#endif
/*
    end of header file
    add nothing below this endif
*/
