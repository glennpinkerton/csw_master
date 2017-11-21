
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;

/**
 * Options available for CSW gridding.
 */
public class GridCalcOptions {
  /* directional flags */
  public static final int NON_DIRECTIONAL = 0;
  public static final int ANISOTROPIC = 1;
  public static final int PREFERRED_STRIKE = 2;

  boolean logFlag;
  int preferredStrikeAngle;
  int outsideMargin;
  int insideMargin;
  int fastFlag;
  double minValue;
  double maxValue;
  double hardMin;
  double hardMax;
  int anisotropyFlag;
  int distancePower;
  int strikePower;
  int numLocalPoints;
  int localSearchPattern;
  double maxSearchDistance;
  boolean thicknessFlag;
  boolean stepFlag;
  boolean userSpecifiedMaxSearchDistance;
  boolean userSpecifiedMinValue;
  boolean userSpecifiedMaxValue;
  int directionalFlag;

/*------------------------------------------------------------------------*/

 /**
 The empty constructor builds an object with default values.
 */
  public GridCalcOptions ()
  {
    setDefaults ();
  }

/*------------------------------------------------------------------------*/

  public void setDefaults ()
  {
  }

/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/


} // end of GridCalcOptions class
