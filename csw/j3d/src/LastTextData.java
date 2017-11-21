
package csw.j3d.src;


/**
 This class is used only for getting and setting the cached 3d text data
 in the 3d drawing.  It is not visible to the public, but rather it is
 only used within the package.
 */
class LastTextData
{

    String         lastText = null;
    double         lastTextSize = -1.0;
    byte[]         lastBdata = null;
    int            lastNcol = 0;
    int            lastNrow = 0;
    int            lastWidth = 0;
    int            lastBcol = 0;
}
