
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
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

// Do not allow copys, moves, etc on DLSeletctable objects. = delete 
// methods should prevent the copy stuff.

    DLSelectable (const DLSelectable &other) = delete;
    const DLSelectable &operator= (const DLSelectable &other) = delete;
    DLSelectable (DLSelectable &&other) = delete;
    const DLSelectable &operator= (DLSelectable &&other) = delete;

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

    int              *lines = NULL,
                     nlines,
                     maxlines;
    int              *fills = NULL,
                     nfills,
                     maxfills;
    int              *texts = NULL,
                     ntexts,
                     maxtexts;
    int              *symbs = NULL,
                     nsymbs,
                     maxsymbs;
    int              *shapes = NULL,
                     nshapes,
                     maxshapes;
    int              *contours = NULL,
                     ncontours,
                     maxcontours;
    int              *axes = NULL,
                     naxes,
                     maxaxes;

};

/*  do not add anything after this endif  */
#endif
