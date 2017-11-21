package csw.j3d.src;

import csw.j3d.src.J3DSurfacePatch;

/**
 This class is used for the userData of a {@link csw.jutils.src.XYZPolyline}
 object that is shared by two surfaces.  The objects sharing the
 line array are identified here.
*/
public class PatchPair {
    public J3DSurfacePatch  sp1;
    public J3DSurfacePatch  sp2;
    public int              type;
}
