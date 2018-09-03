
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_tiledata.h

    This header defines the CSWPolyTiledata class.  The methods of
    this class replace the old ply_tiledata.c functions.  The 
    primary reason for this refactor is to get c++ experience.  
    I also want to make the csw code thread safe, or at least
    closer to being thread safe.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_tiledata.h.
#endif



#ifndef PLY_TILEDATA_H
#define PLY_TILEDATA_H


class CSWPolyTiledata
{

  private:

    int                    PatNum = 0;
    CSW_F                  PatScale = 1.0f,
                           TileX0 = 0.0f,
                           TileY0 = 0.0f;

  public:

    CSWPolyTiledata () {};
    ~CSWPolyTiledata () {};

// It makes no sense to copy construct, move construct,
// assign or move assign an object of this class.  The
// various copy methods are flagged "delete" to prevent
// their use.

    CSWPolyTiledata (const CSWPolyTiledata &old) = delete;
    const CSWPolyTiledata &operator=(const CSWPolyTiledata &old) = delete;
    CSWPolyTiledata (CSWPolyTiledata &&old) = delete;
    const CSWPolyTiledata &operator=(CSWPolyTiledata &&old) = delete;

    int ply_SetTileNum (int num);
    int ply_SetTileScale (CSW_F num);
    int ply_SetTileOrigin (CSW_F x, CSW_F y);
    int ply_GetTileVecs (CSW_F *x, CSW_F *y, int *ic, int *nc);
    int ply_GetTileArcs (CSW_F *x, CSW_F *y, CSW_F *r, int *nc);
    int ply_GetTilePolys (CSW_F *x, CSW_F *y, int *ic, int *nc);
    int ply_GetTileDots (CSW_F *x, CSW_F *y, CSW_F *r, int *nc);
    int ply_CountTilePrims (int mult, int mult2,
                            int *nv1, int *nf1, int *nc1, int *nd1,
                            int *iv1, int *if1);

}; // end of main class definition


/*
    end of header file
    add nothing below this endif
*/
#endif
