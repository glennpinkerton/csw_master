
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

import java.util.ArrayList;

/**
Store the native primitives needed to draw a symbol.  This is only
uses as the return value from the {@link JDisplayList#getSymbolParts}
static method.
*/
public class SymbolParts {

    public ArrayList<NativePrim.Line>    lineList;
    public ArrayList<NativePrim.Fill>    fillList;
    public ArrayList<NativePrim.Text>    textList;
    public ArrayList<NativePrim.Arc>     arcList;
    public ArrayList<NativePrim.FilledArc>    filledArcList;

}
