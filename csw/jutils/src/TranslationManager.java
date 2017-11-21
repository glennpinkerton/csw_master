
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jutils.src;

/**
 * Currently a do nothing class which encapsulates text translation 
 * for possible future use.
 */
public class TranslationManager {

  /**
   * Translates the given text using the currently set dictionary.  If no
   * dictionary is defined, return the sentence as is.
   * @param sentence the text to translate
   * @return the translated sentence
   */
  public static String translate(String sentence) {
    return sentence;
  }

}
