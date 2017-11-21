
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
 * Collection of static utility functions to use with other jutils classes.
 */
public class MathUtils 
{

/**
 * Return the minimum value of the specified double array or return
 * 1.e30 if the array is empty.
 */
	public static double getMin (double[] array)
	{
		double val = 1.e30;
		if (array == null) return val;

		int size = array.length;
		if (size == 0) return val;

		for (int i=0; i<size; i++) {
			if (array[i] < val) val = array[i];
		}

		return val;

	}

/**
 * Return the maximum value of the specified double array or return
 * -1.e30 if the array is empty.
 */
	public static double getMax (double[] array)
	{
		double val = -1.e30;
		if (array == null) return val;

		int size = array.length;
		if (size == 0) return val;

		for (int i=0; i<size; i++) {
			if (array[i] > val) val = array[i];
		}

		return val;

	}

/**
 * Fill the specified double array with the specified value.
 */
	public static void fill (double[] array, double val)
	{
		if (array == null) return;

		int size = array.length;

		for (int i=0; i<size; i++)
		{
			array[i] = val;
		}

	}

/**
 * Fill the specified int array with the specified value.
 */
	public static void fill (int[] array, int val)
	{
		if (array == null) return;

		int size = array.length;

		for (int i=0; i<size; i++)
		{
			array[i] = val;
		}

	}

}
