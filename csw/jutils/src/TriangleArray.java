
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jutils.src;

import java.util.List;

/**
 * A class that represents an array of triangles.
 */
public class TriangleArray {

    protected int[] node1Array;
    protected int[] node2Array;
    protected int[] node3Array;

    public TriangleArray(int size) {
        node1Array = new int[size];
        node2Array = new int[size];
        node3Array = new int[size];
    }

  /**
   * Construct a triangle array from the specified node arrays.  The
   * node arrays may be of different lengths, but only the minimal length
   * of the 3 arrays is used for the NodeArray.
   * @param node1 node 1 array
   * @param node2 node 2 array
   * @param node3 node 3 array
   */
    public TriangleArray(
        int[] node1,
        int[] node2,
        int[] node3)
   	{
        int size = Integer.MAX_VALUE;
        if (size > node1.length) size = node1.length;
        if (size > node2.length) size = node2.length;
        if (size > node3.length) size = node3.length;

        node1Array = new int[size];
        node2Array = new int[size];
        node3Array = new int[size];

        System.arraycopy(node1, 0, node1Array, 0, size);
        System.arraycopy(node2, 0, node2Array, 0, size);
        System.arraycopy(node3, 0, node3Array, 0, size);
    }

  /**
   * Constructor from lists of node numbers.  These are passed as 
   * lists of Integer objects.
   */
    public TriangleArray(List<Integer> node1, 
			             List<Integer> node2,
						 List<Integer> node3)
   	{

        int size = node1.size();

        if (size == node2.size() && size == node3.size()) {

            int[] tempNode1Array = new int[size];
            int[] tempNode2Array = new int[size];
            int[] tempNode3Array = new int[size];
            for(int i = 0; i < size; i++)
            {
                tempNode1Array[i] = node1.get(i).intValue();
                tempNode2Array[i] = node2.get(i).intValue();
                tempNode3Array[i] = node3.get(i).intValue();
            }

            node1Array = tempNode1Array;
            node2Array = tempNode2Array;
            node3Array = tempNode3Array;
        } 
		else {
            throw new IllegalArgumentException(
              "node1,node2,node3 lists must be of same size"
            );
		}
    }

  /*
   * Accessors for the individual node arrays.
   */
    public int[] getNode1Array(){ return node1Array; }
    public int[] getNode2Array(){ return node2Array; }
    public int[] getNode3Array(){ return node3Array; }

    public int getSize(){ return node1Array.length; }

    public TriangleArray getCopy() 
	{
        TriangleArray copy = new TriangleArray(getSize());

        int[] node1 = copy.getNode1Array();
        int[] node2 = copy.getNode2Array();
        int[] node3 = copy.getNode3Array();

        for(int i = 0; i < getSize(); i++) {
            node1[i] = node1Array[i];
            node2[i] = node2Array[i];
            node3[i] = node3Array[i];
        }

        return copy;
    }

    private void dump() 
	{
        System.out.println("TriangleArray:");
        for (int index=0; index<getSize(); index++) {
            int n1 = node1Array[index];
            int n2 = node2Array[index];
            int n3 = node3Array[index];
            System.out.println("  ["+index+"]"+n1+","+n2+","+n3);
        }
    }

}
