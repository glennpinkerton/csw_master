
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/


package csw.j3d.src;

/**
 * Digitizing Type
 * 
 * @author Glenn Pinkerton
 */
public class DigitizingType {
  private DigitizingType() {}
  
  public static final DigitizingType POINT = new DigitizingType();
  public static final DigitizingType POLYLINE = new DigitizingType();
  public static final DigitizingType POLYGON = new DigitizingType();
  public static final DigitizingType POLYLINE_OR_POLYGON = new DigitizingType();
}
