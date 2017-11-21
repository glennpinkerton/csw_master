
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;


public class TriangleIndex3DResultList
{
    int[] tmeshid;
    int[] trinum;
    int   size = 0;
    int   nout = 0;
    int   nin = 0;

    /*
     * The constructor and all writing accessors have 
     * package scope.  The public only has read access to this class.
     */

    TriangleIndex3DResultList (int s)
    {
        tmeshid = new int[s];
        trinum = new int[s];
        size = s;
    }


    void addTriangle (int tmid, int tnum)
    {
        tmeshid[nin] = tmid;
        trinum[nin] = tnum;
        nin++;
    }

  /**
   Return the number of results in this list.
   */
    public int getSize()
    {
        return size;
    }

  /**
   * Get the next result in the list.  When the last item has been 
   * retrieved, this returns null.
   */
    public TriangleIndex3DResult getNext ()
    {
        TriangleIndex3DResult  tr;
       
        tr = new TriangleIndex3DResult ();
        tr.tmeshid = tmeshid[nout];
        tr.trinum = trinum[nout];
        nout++;

        return tr;

    }

   /**
    Reset the output iterator for the list.  After this is called, the getNext
    method will go back to returning the first item in the list.
   */
    public void resetOutputIterator ()
    {
        nout = 0;
    }
        
}
