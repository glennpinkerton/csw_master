
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * This is the class definition for the DLSelectable class.  This class
 * is used to store a trimesh object in the display list.  The display
 * list has a list of pointers to objects of this class.
 */

#ifndef _DLSELECTABLE_H
#define _DLSELECTABLE_H

#  include "csw/utils/include/csw_.h"

class DLSelectable {

  public:

    DLSelectable ();
    ~DLSelectable ();

    int              index_num;
    int              deleted_flag;

    int              AddLine (int ival);
    int              AddFill (int ival);
    int              AddText (int ival);
    int              AddSymb (int ival);
    int              AddShape (int ival);
    int              AddContour (int ival);
    int              AddAxis (int ival);

    int              GetNumLines ();
    int              GetNumFills ();
    int              GetNumTexts ();
    int              GetNumSymbs ();
    int              GetNumShapes ();
    int              GetNumContours ();
    int              GetNumAxes ();

    int              *GetLines ();
    int              *GetFills ();
    int              *GetTexts ();
    int              *GetSymbs ();
    int              *GetShapes ();
    int              *GetContours ();
    int              *GetAxes ();

    int              GetSelectState (void);
    void             ToggleSelectState (void);
    void             SetSelectState (int ival);

    void             SetVisibleState (int ival);
    void             SetDeletedState (int ival);

  private:

    int              select_state;

    int              *lines,
                     nlines,
                     maxlines;
    int              *fills,
                     nfills,
                     maxfills;
    int              *texts,
                     ntexts,
                     maxtexts;
    int              *symbs,
                     nsymbs,
                     maxsymbs;
    int              *shapes,
                     nshapes,
                     maxshapes;
    int              *contours,
                     ncontours,
                     maxcontours;
    int              *axes,
                     naxes,
                     maxaxes;

};

/*  do not add anything after this endif  */
#endif
