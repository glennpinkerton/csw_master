
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import java.util.ArrayList;


/**
 This class holds the collection of {@link AttributeDef} objects that
 sre currently used for the 3d view.
 */

public class AttributeDefList 
{

  private ArrayList<AttributeDef>     list;
  private AttributeDef                currentAtDef;
  private AttributeDef                lastAtDef;


/**
 Create an empty list.
*/
  public AttributeDefList ()
  {
    list = new ArrayList<AttributeDef> ();
    currentAtDef = null;
    lastAtDef = null;
  }

/**
 * Add an attribute def to the list.
 */
  public void add (AttributeDef adef)
  {
    list.add (adef);
  }

/**
 * Remove an attribute def from the list.
 */
  public void remove (AttributeDef adef)
  {
    int size = list.size ();
    int index = list.indexOf (adef);
    if (index < 0  ||  index >= size) {
      return;
    }
    list.remove (index);
  }

/**
 Get the attribute def for the index.
*/
  public AttributeDef get (int index)
  {
    AttributeDef atdef;
    atdef = list.get (index);
    return atdef;
  }

/**
 Get the number of attribute defs in the list.
*/
  public int size ()
  {
    return list.size ();
  }

/**
 Remove all items from the list.
*/
  public void clear ()
  {
    list.clear ();
  }

/**
 Get the current active attribute definition.
*/
  public AttributeDef getCurrentAtDef ()
  {
    return currentAtDef;
  }


  void resetCurrent (int index)
  {
    int size = list.size ();
    if (index < 0  ||  index >= size) {
      currentAtDef = null;
      return;
    }

    currentAtDef = list.get (index);

    if (currentAtDef == lastAtDef) {
      return;
    }
    lastAtDef = currentAtDef;

  }

} /* end of class */
