package csw.j3d.src;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.Toolkit;
import java.awt.font.FontRenderContext;
import java.awt.geom.AffineTransform;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.nio.DoubleBuffer;
import java.util.HashMap;


import com.jogamp.opengl.GL;
import com.jogamp.opengl.GL2;
import com.jogamp.opengl.glu.GLU;

//import static com.jogamp.opengl.GL.*;
//import static com.jogamp.opengl.GL2.*;


/**
 Wraps around the jogl package.  Trying to isolate opengl specific
 (or sort or specific) stuff to this class.
 */
public class JGL
{

    private static final double MAX_DEPTH_32 = 0xffffffff;

    private static final double PERSPECTIVE_ANGLE = 65.0;
    private static final double PERSPECTIVE_CMIN = 0.001;
    private static final double PERSPECTIVE_CMAX = 100000.0;
    private static final int PERSPECTIVE_MAX_AFACT = 100;

    public static final int DG_WIREFRAME = 20;
    public static final int DG_MESH = 30;
    public static final int DG_TRANSPARENT = 40;
    public static final int DG_POINTS   = 100;
    public static final int DG_TRIANGLES = 101;
    public static final int DG_TRIANGLE_STRIP = 102;
    public static final int DG_TRIANGLE_FAN = 103;
    public static final int DG_LINES    = 104;
    public static final int DG_QUADS    = 105;
    public static final int DG_LINESTRIP = 106;
    public static final int DG_POLYGON = 107;

    public static final int TRAP_NAME = 60;

    private static final long MAX_LDIST = 4294967295L;

    private double           color_r, color_g, color_b;
    private double           bg_r, bg_g, bg_b, bg_a;

    private int              stack_size;

    private int      selection_window_size;

    private int      draw_mode;

    private double   max_depth_value;
    private double   hit_z_value;

    private double   xscale, yscale, zscale;
    private double   xtrans, ytrans, ztrans;

    private double   near_slice, far_slice;
    private int      slicing_enabled;

    private double[] model_matrix = new double[16];
    private double[] projection_matrix = new double[16];

    private int    viewport_matrix[] = new int[4];

    static private ByteBuffer select_buff = ByteBuffer.allocateDirect(
        100000 * 4
    ).order(ByteOrder.nativeOrder());
    static private IntBuffer select_buff_int_view =
        select_buff.asIntBuffer();

    static private ByteBuffer feed_buffer = ByteBuffer.allocateDirect(
        10000 * 4
    ).order(ByteOrder.nativeOrder());
    static private FloatBuffer feed_buffer_float_view =
        feed_buffer.asFloatBuffer();

    private int     selected_name_stack[] = new int[100];

    private GL2        gl = null;
    private GLU       glu = null;


// Method to convert old float arrays to the FloatBuffer objects
// needed by the GL2 object.

    private ByteBuffer toBB(byte[] v) {
      ByteBuffer buffer = ByteBuffer.allocate(v.length);
      buffer.put(v);
      buffer.position(0);
      return buffer;
    }

    private DoubleBuffer toDB(double[] v) {
      DoubleBuffer buffer = DoubleBuffer.allocate(v.length);
      buffer.put(v);
      buffer.position(0);
      return buffer;
    }

    private FloatBuffer toFB(float[] v) {
      FloatBuffer buffer = FloatBuffer.allocate(v.length);
      buffer.put(v);
      buffer.position(0);
      return buffer;
    }

    private IntBuffer toIB(int[] v) {
      IntBuffer buffer = IntBuffer.allocate(v.length);
      buffer.put(v);
      buffer.position(0);
      return buffer;
    }

    void setGL (GL gll, GLU gllu)
    {
        gl = gll.getGL2();
        glu = gllu;
    }

    JGL()
    {
        draw_mode = GL.GL_POINTS;

        color_r = 0.0;
        color_g = 0.0;
        color_b = 0.0;

        bg_r = 0.0;
        bg_g = 0.0;
        bg_b = 0.0;
        bg_a = 1.0;

        xscale = 1.0;
        yscale = 1.0;
        zscale = 1.0;

        stack_size = 0;

        selection_window_size = 3;

        max_depth_value = (double)MAX_LDIST;
        hit_z_value = -1.0;

        near_slice = PERSPECTIVE_CMIN;
        far_slice = PERSPECTIVE_CMAX;

        slicing_enabled = 1;

    }

    void AdjustSlicing (int zdelta, int fine_tuning)
    {
        double       dtmp, dslice;

        if (zdelta < -90000) {
            near_slice = PERSPECTIVE_CMIN;
            far_slice = PERSPECTIVE_CMAX;
            return;
        }

        if (slicing_enabled == 0) {
            return;
        }

        dslice = 0.1;
        if (fine_tuning != 0) {
            dslice = 0.01;
        }

        dtmp = xscale;
        if (dtmp < 1.0) dtmp = 1.0;

        if (zdelta > 0) {
            if (near_slice < 1.0 / dtmp) {
                near_slice = 1.0 / dtmp;
            }
            near_slice += dslice;
        }
        else {
            near_slice -= dslice;
        }
        if (near_slice < PERSPECTIVE_CMIN) {
            near_slice = PERSPECTIVE_CMIN;
        }

        return;
    }


    void SetDrawMode(int dmode)
    {
        //Map to OpenGL equivalent
        switch (dmode)
        {
        case DG_POINTS:
            draw_mode = GL.GL_POINTS;
            break;

        case DG_TRIANGLES:
            draw_mode = GL.GL_TRIANGLES;
            break;

        case DG_TRIANGLE_STRIP:
            draw_mode = GL.GL_TRIANGLE_STRIP;
            break;

        case DG_TRIANGLE_FAN:
            draw_mode = GL.GL_TRIANGLE_FAN;
            break;

        case DG_LINES:
            draw_mode = GL2.GL_LINES;
            break;

        case DG_QUADS:
            draw_mode = GL2.GL_QUADS;
            break;

        case DG_LINESTRIP:
            draw_mode = GL.GL_LINE_STRIP;
            break;

        case DG_POLYGON:
            draw_mode = GL2.GL_POLYGON;
            gl.glPolygonMode (GL.GL_FRONT_AND_BACK, GL2.GL_FILL);
            break;

        default:
            draw_mode = GL.GL_TRIANGLES;
            break;
        }

    }

    void SetDrawStyle(int dstyle)
    {
        switch (dstyle)
        {
        case DG_WIREFRAME:
            gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL2.GL_LINE);
            break;

        case DG_MESH:
            gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL2.GL_FILL);
            break;

        case DG_TRANSPARENT:
            //Not Used at the moment
            break;

        default:

            break;
        }


    }

    void SetDrawCulling(int cmode)
    {
        if (cmode == 1) {
            gl.glEnable(GL.GL_CULL_FACE);
        }
        else {
            gl.glDisable(GL.GL_CULL_FACE);
        }
    }

    void SetDrawLighting(int lstate)
    {
        if (lstate == 1) {
            gl.glEnable (GL2.GL_LIGHTING);
        }
        else {
            gl.glDisable (GL2.GL_LIGHTING);
        }
    }

    void SetDrawShading(int sstate)
    {
        if (sstate == 0) {
            gl.glShadeModel (GL2.GL_FLAT);
        }
        else {
            gl.glShadeModel (GL2.GL_SMOOTH);
        }
    }

    void SetDrawLineWidth(double width)
    {
        gl.glLineWidth((float)width);
    }

    void SetDrawLineDash (boolean flag)
    {
        if (flag == false) {
            gl.glDisable (GL2.GL_LINE_STIPPLE);
        }
        else {
            SetDrawLinePattern (2, (short)0x00FF);
        }
    }

    void SetDrawLinePattern (int ifact, short ipat)
    {
        if (ifact > 0) {
            gl.glEnable (GL2.GL_LINE_STIPPLE);
            gl.glLineStipple (ifact, ipat);
        }
        else {
            gl.glDisable (GL2.GL_LINE_STIPPLE);
        }
    }

    void SetDrawPointSize(double psize)
    {
        gl.glPointSize((float)psize);
    }

    void SetDrawTransparency(int tstate)
    {
        /* tstate = tstate; */
        gl.glEnable(GL.GL_BLEND);
        gl.glBlendFunc (GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA);
    }

    void SetDrawAliasing(int astate)
    {
        /* astate = astate; */
        gl.glDisable(GL2.GL_POINT_SMOOTH);
        gl.glDisable(GL.GL_LINE_SMOOTH);
        gl.glDisable(GL2.GL_POLYGON_SMOOTH);
        gl.glHint (GL2.GL_POINT_SMOOTH_HINT, GL.GL_FASTEST);
        gl.glHint (GL.GL_LINE_SMOOTH_HINT, GL.GL_FASTEST);
        gl.glHint (GL2.GL_POLYGON_SMOOTH_HINT, GL.GL_FASTEST);
    }

    void SetDrawDepthTest(int dstate)
    {
        if (dstate == 1) {
            gl.glEnable(GL.GL_DEPTH_TEST);
        }
        else {
            gl.glDisable(GL.GL_DEPTH_TEST);
        }
    }

    void SetDepthRange (double dnear, double dfar)
    {
        gl.glDepthRange((float)dnear, (float)dfar);
    }

    void SetDepthMask(int flag)
    {
        if (flag == 1) {
            gl.glDepthMask (true);
        }
        else {
            gl.glDepthMask (false);
        }
    }

    void SetColorMask(int flag)
    {
        if (flag == 1) {
            gl.glColorMask (true, true, true, true);
        }
        else {
            gl.glColorMask (false, false, false, false);
        }
    }

    void SetDrawTranslatePosition(double x, double y, double z)
    {
        gl.glTranslated(x, y, z);
    }


    private int first = 1;

    void StartDrawMode()
    {
        if (first == 1) {
            gl.glEnable (GL.GL_BLEND);
            gl.glBlendFunc (GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA);
            gl.glDisable(GL2.GL_POINT_SMOOTH);
            gl.glDisable(GL.GL_LINE_SMOOTH);
            gl.glDisable(GL2.GL_POLYGON_SMOOTH);
            gl.glShadeModel (GL2.GL_SMOOTH);
            first = 0;
        }
        gl.glBegin(draw_mode);
    }



    void StopDrawMode()
    {
        gl.glEnd();
    }


    /*
     * Functions used to enable draping of lines over surfaces.
     */
    void SetOffsetFill ()
    {
        gl.glEnable(GL2.GL_POLYGON_OFFSET_FILL);
        gl.glPolygonOffset(0.0f, 2.0f);
    }

    void SetOffsetFill (double val)
    {
        gl.glEnable(GL2.GL_POLYGON_OFFSET_FILL);
        gl.glPolygonOffset(0.0f, (float)val);
    }

    void UnsetOffsetFill ()
    {
        gl.glDisable(GL2.GL_POLYGON_OFFSET_FILL);
    }


    //Draw Functions
    //Overriden base class functions for OpenGL
    //Do not swap y and z at this level as that is done at the abstract
    //level above. These functions just provide a direct mapping to the
    //OpenGL equivalents
    void DGNodeXYZ(double x, double y, double z)
    {
        gl.glVertex3d(x,y,z);
    }

    void DGNodeColor(Color color)
    {
        float [] rgba = color.getRGBComponents(null);
        gl.glColor4f(rgba[0], rgba[1], rgba[2], rgba[3]);
        color_r = rgba[0];
        color_g = rgba[1];
        color_b = rgba[2];
    }

    void DGNodeColor(double r, double g, double b, double alpha)
    {
        gl.glColor4f((float)r, (float)g, (float)b, (float)alpha);
        color_r = r;
        color_g = g;
        color_b = b;
    }

    void DGBackColor (double r, double g, double b, double a)
    {
        bg_r = r;
        bg_g = g;
        bg_b = b;
        bg_a = a;
    }

    void DGNodeNormal(double nx, double ny, double nz)
    {
        gl.glNormal3d(nx,ny,nz);
    }


    /*
     * Functions to remove the opengl calls from MviewerView
     */
    void FlushGX ()
    {
        gl.glFlush ();
    }

    void FinishGX ()
    {
        gl.glFinish ();
    }


    void ClearColorAndDepthBuffers ()
    {
        ClearColorBuffer ();
        ClearDepthBuffer ();
    }

    void ClearColorBuffer ()
    {
        gl.glClearColor (
                      (float)bg_r,
                      (float)bg_g,
                      (float)bg_b,
                      (float)bg_a
                     );
        gl.glClear (GL.GL_COLOR_BUFFER_BIT);
    }

    void ClearDepthBuffer ()
    {
        gl.glClearDepth (1.0);
        gl.glClear (GL.GL_DEPTH_BUFFER_BIT);
    }

    void PushMatrix ()
    {
        gl.glPushMatrix ();
    }

    void PopMatrix ()
    {
        gl.glPopMatrix ();
    }

    void LoadIdentity ()
    {
        gl.glLoadIdentity ();
    }

    void MatrixModeModel ()
    {
        gl.glMatrixMode (GL2.GL_MODELVIEW);
    }

    void MatrixModeProjection ()
    {
        gl.glMatrixMode (GL2.GL_PROJECTION);
    }


    void RotateD (double dang, double xt, double yt, double zt)
    {
        gl.glRotated (dang, xt, yt, zt);
    }

    void TranslateD (double xt, double yt, double zt)
    {
        xtrans = xt;
        ytrans = yt;
        ztrans = zt;
        gl.glTranslated (xt, yt, zt);
    }

    void ScaleD (double xt, double yt, double zt)
    {
        xscale = xt;
        yscale = yt;
        zscale = zt;
        gl.glScaled (xt, yt, zt);
    }

    /*
     * Set the light position.  The lighting is always 2 sided and
     * never local.
     */
    void UpdateLighting (double x, double y, double z)
    {
        float light_pos[];

        light_pos = new float[4];

        light_pos[0] = (float)x;
        light_pos[1] = (float)y;
        light_pos[2] = (float)z;
        light_pos[3] = (float)0.0;

        FloatBuffer fb = toFB (light_pos);
        //gl.glLightfv(GL2.GL_LIGHT0, GL2.GL_POSITION, light_pos);
        gl.glLightfv(GL2.GL_LIGHT0, GL2.GL_POSITION, fb);

        gl.glLightModeli(GL2.GL_LIGHT_MODEL_LOCAL_VIEWER, GL.GL_FALSE);

    }

    void UpdateLighting2 (double x, double y, double z)
    {
        float light_pos[];

        light_pos = new float[4];

        light_pos[0] = (float)x;
        light_pos[1] = (float)y;
        light_pos[2] = (float)z;
        light_pos[3] = (float)0.0;

        gl.glLightfv(GL2.GL_LIGHT1, GL2.GL_POSITION, 
                     toFB (light_pos));

        gl.glLightModeli(GL2.GL_LIGHT_MODEL_LOCAL_VIEWER, GL.GL_FALSE);

    }

    void UpdateLighting3 (double x, double y, double z)
    {
        float light_pos[];

        light_pos = new float[4];

        light_pos[0] = (float)x;
        light_pos[1] = (float)y;
        light_pos[2] = (float)z;
        light_pos[3] = (float)0.0;

        gl.glLightfv(GL2.GL_LIGHT2, GL2.GL_POSITION, toFB (light_pos));

        gl.glLightModeli(GL2.GL_LIGHT_MODEL_LOCAL_VIEWER, GL.GL_FALSE);

    }

    void UpdateLighting4 (double x, double y, double z)
    {
        float light_pos[];

        light_pos = new float[4];

        light_pos[0] = (float)x;
        light_pos[1] = (float)y;
        light_pos[2] = (float)z;
        light_pos[3] = (float)0.0;

        gl.glLightfv(GL2.GL_LIGHT3, GL2.GL_POSITION, toFB (light_pos));

        gl.glLightModeli(GL2.GL_LIGHT_MODEL_LOCAL_VIEWER, GL.GL_FALSE);

    }


    /*
     * Initialize some OpenGL states that won't change.
     */
    void InitializeState()
    {

        gl.glClearDepth(1.0);
        gl.glDepthFunc(GL.GL_LEQUAL);

        gl.glEnable(GL.GL_DEPTH_TEST);
        gl.glDisable (GL2.GL_NORMALIZE);
        gl.glFrontFace(GL.GL_CCW);
        gl.glCullFace(GL.GL_BACK);
        gl.glShadeModel(GL2.GL_SMOOTH);
        gl.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA);

    }


    /*
     * Set up the lighting type.  The light position is changeable
     * and set via UpdateLighting.
     */
    void InitializeLighting()
    {
        float[] lmodel_ambient;
        float[] light0_ambient;
        float[] light0_diffuse;
        float[] light0_specular;

        lmodel_ambient = new float [4];
        light0_ambient = new float [4];
        light0_diffuse = new float [4];
        light0_specular = new float [4];

        lmodel_ambient[0] = .45f;
        lmodel_ambient[1] = .45f;
        lmodel_ambient[2] = .45f;
        lmodel_ambient[3] = 0.0f;

        light0_ambient[0] = .1f;
        light0_ambient[1] = .1f;
        light0_ambient[2] = .1f;
        light0_ambient[3] = 1.0f;

        light0_diffuse[0] = 0.8f;
        light0_diffuse[1] = 0.8f;
        light0_diffuse[2] = 0.8f;
        light0_diffuse[3] = 0.0f;

        light0_specular[0] = 0.3f;
        light0_specular[1] = 0.3f;
        light0_specular[2] = 0.3f;
        light0_specular[3] = 0.0f;

        gl.glLightfv(GL2.GL_LIGHT0, GL2.GL_AMBIENT, toFB (light0_ambient));
        gl.glLightfv(GL2.GL_LIGHT0, GL2.GL_DIFFUSE, toFB (light0_diffuse));
        gl.glLightfv(GL2.GL_LIGHT0, GL2.GL_SPECULAR, toFB (light0_specular));
        gl.glEnable(GL2.GL_LIGHT0);

        gl.glLightModelfv(GL2.GL_LIGHT_MODEL_AMBIENT, toFB (lmodel_ambient));
        gl.glLightModeli(GL2.GL_LIGHT_MODEL_LOCAL_VIEWER, GL.GL_FALSE);

        gl.glEnable(GL2.GL_LIGHTING);

    }


    /*
     * Set the shininess and other lighting properties of the
     * objects.  They are all the same for this application.
     */
    void InitializeMaterialProperties()
    {
        float mat_shininess[] = {40.0f};
        float mat_specular[] = {0.0f, 0.0f, 0.0f, 0.0f};

        gl.glMaterialfv(GL.GL_FRONT, GL2.GL_SHININESS, toFB (mat_shininess));
        gl.glMaterialfv(GL.GL_FRONT, GL2.GL_SPECULAR, toFB (mat_specular));

        gl.glColorMaterial(GL.GL_FRONT, GL2.GL_AMBIENT_AND_DIFFUSE);
        gl.glEnable(GL2.GL_COLOR_MATERIAL);
    }


    void SetLightBrightness (int brightness)
    {
        float        fmult,
                     ambient[],
                     diffuse[],
                     specular[];

        ambient = new float[4];
        diffuse = new float[4];
        specular = new float[4];

        fmult = (float)brightness * .01f;
        if (fmult < 0.0) fmult = 0.0f;
        if (fmult > 1.0) fmult = 1.0f;

        specular[0] = fmult * .3f;
        specular[1] = fmult * .3f;
        specular[2] = fmult * .3f;
        specular[3] = 0.0f;
        ambient[0] = fmult * .1f;
        ambient[1] = fmult * .1f;
        ambient[2] = fmult * .1f;
        ambient[3] = 1.0f;
        diffuse[0] = fmult;
        diffuse[1] = fmult;
        diffuse[2] = fmult;
        diffuse[3] = 0.0f;

        gl.glLightfv(GL2.GL_LIGHT0, GL2.GL_AMBIENT, toFB (ambient));
        gl.glLightfv(GL2.GL_LIGHT0, GL2.GL_DIFFUSE, toFB (diffuse));
        gl.glLightfv(GL2.GL_LIGHT0, GL2.GL_SPECULAR, toFB (specular));
        gl.glEnable(GL2.GL_LIGHT0);

        return;
    }

    void SetLight2Brightness (int brightness)
    {
        float        fmult,
                     ambient[],
                     diffuse[],
                     specular[];

        ambient = new float[4];
        diffuse = new float[4];
        specular = new float[4];

        fmult = (float)brightness * .01f;
        if (fmult < 0.0) fmult = 0.0f;
        if (fmult > 1.0) fmult = 1.0f;

        specular[0] = fmult * .3f;
        specular[1] = fmult * .3f;
        specular[2] = fmult * .3f;
        specular[3] = 0.0f;
        ambient[0] = fmult * .1f;
        ambient[1] = fmult * .1f;
        ambient[2] = fmult * .1f;
        ambient[3] = 1.0f;
        diffuse[0] = fmult;
        diffuse[1] = fmult;
        diffuse[2] = fmult;
        diffuse[3] = 0.0f;

        gl.glLightfv(GL2.GL_LIGHT1, GL2.GL_AMBIENT, toFB (ambient));
        gl.glLightfv(GL2.GL_LIGHT1, GL2.GL_DIFFUSE, toFB (diffuse));
        gl.glLightfv(GL2.GL_LIGHT1, GL2.GL_SPECULAR, toFB (specular));
        gl.glEnable(GL2.GL_LIGHT1);

        return;
    }


    void SetLight3Brightness (int brightness)
    {
        float        fmult,
                     ambient[],
                     diffuse[],
                     specular[];

        ambient = new float[4];
        diffuse = new float[4];
        specular = new float[4];

        fmult = (float)brightness * .01f;
        if (fmult < 0.0) fmult = 0.0f;
        if (fmult > 1.0) fmult = 1.0f;

        specular[0] = fmult * .3f;
        specular[1] = fmult * .3f;
        specular[2] = fmult * .3f;
        specular[3] = 0.0f;
        ambient[0] = fmult * .1f;
        ambient[1] = fmult * .1f;
        ambient[2] = fmult * .1f;
        ambient[3] = 1.0f;
        diffuse[0] = fmult;
        diffuse[1] = fmult;
        diffuse[2] = fmult;
        diffuse[3] = 0.0f;

        gl.glLightfv(GL2.GL_LIGHT2, GL2.GL_AMBIENT, toFB (ambient));
        gl.glLightfv(GL2.GL_LIGHT2, GL2.GL_DIFFUSE, toFB (diffuse));
        gl.glLightfv(GL2.GL_LIGHT2, GL2.GL_SPECULAR, toFB (specular));
        gl.glEnable(GL2.GL_LIGHT2);

        return;
    }


    void SetLight4Brightness (int brightness)
    {
        float        fmult,
                     ambient[],
                     diffuse[],
                     specular[];

        ambient = new float[4];
        diffuse = new float[4];
        specular = new float[4];

        fmult = (float)brightness * .01f;
        if (fmult < 0.0) fmult = 0.0f;
        if (fmult > 1.0) fmult = 1.0f;

        specular[0] = fmult * .3f;
        specular[1] = fmult * .3f;
        specular[2] = fmult * .3f;
        specular[3] = 0.0f;
        ambient[0] = fmult * .1f;
        ambient[1] = fmult * .1f;
        ambient[2] = fmult * .1f;
        ambient[3] = 1.0f;
        diffuse[0] = fmult;
        diffuse[1] = fmult;
        diffuse[2] = fmult;
        diffuse[3] = 0.0f;

        gl.glLightfv(GL2.GL_LIGHT3, GL2.GL_AMBIENT, toFB (ambient));
        gl.glLightfv(GL2.GL_LIGHT3, GL2.GL_DIFFUSE, toFB (diffuse));
        gl.glLightfv(GL2.GL_LIGHT3, GL2.GL_SPECULAR, toFB (specular));
        gl.glEnable(GL2.GL_LIGHT3);

        return;
    }


    void SetLightEnabled (int ival)
    {
        if (ival != 0) {
            gl.glEnable (GL2.GL_LIGHT0);
        }
        else {
            gl.glDisable (GL2.GL_LIGHT0);
        }
    }

    void SetLight2Enabled (int ival)
    {
        if (ival != 0) {
            gl.glEnable (GL2.GL_LIGHT1);
        }
        else {
            gl.glDisable (GL2.GL_LIGHT1);
        }
    }

    void SetLight3Enabled (int ival)
    {
        if (ival != 0) {
            gl.glEnable (GL2.GL_LIGHT2);
        }
        else {
            gl.glDisable (GL2.GL_LIGHT2);
        }
    }

    void SetLight4Enabled (int ival)
    {
        if (ival != 0) {
            gl.glEnable (GL2.GL_LIGHT3);
        }
        else {
            gl.glDisable (GL2.GL_LIGHT3);
        }
    }


    /*
     * Set the viewing volume depending on the center point,
     * the size of the window, and the projection mode.  This
     * is cloned from listing 12-3, page 367 of OpenGL Programming
     * Guide (the red book).
     */
    void SelectModelObjectSetup (int ix, int iy, int size,
                                 int mode, double aspect_ratio)
    {
        int       viewport[];
        double      afact;

        viewport = new int[4];

        /*mode = mode;*/

        IntBuffer ib = toIB (viewport);
        gl.glGetIntegerv (GL.GL_VIEWPORT, ib);
        viewport = ib.array();

        select_buff.clear();
        gl.glSelectBuffer(100000, select_buff_int_view);
        gl.glRenderMode(GL2.GL_SELECT);
        gl.glInitNames();

      /*
       * One name needs to be pushed onto the name stack to allow
       * gl.glLoadName to replace it.
       */
        gl.glPushName(0);

        ib = toIB (viewport);
        glu.gluPickMatrix ((double)ix, (double)(viewport[3]-iy),
                       (double)size, (double)size,
                       ib);
        viewport = ib.array();

        afact = (xscale + yscale + zscale) / 3.0;
        if (afact > 1.0) {
            afact = Math.sqrt (afact);
        }
        if (afact < 1.0) {
            afact = 1.0;
        }
        if (afact > PERSPECTIVE_MAX_AFACT) {
            afact = PERSPECTIVE_MAX_AFACT;
        }
        if (mode == 0) {
            glu.gluPerspective(PERSPECTIVE_ANGLE / afact, aspect_ratio,
                           near_slice, far_slice);
        }
        if (mode == 1)
        {
            gl.glOrtho(
                0.0, 3.0,
                0.0, 3.0 / aspect_ratio,
                -10.0, 10.0);
            gl.glTranslated(1.5, 1.5 / aspect_ratio, -3.0);
        }

    }


    /*
     * When the render mode is switched back to GL2.GL_RENDER, the number
     * of hits from the previous select mode is returned.
     */
    int SelectModelObject()
    {
        int        found;
        int       hits;

        hits = gl.glRenderMode(GL2.GL_RENDER);
        found = ProcessHits(hits, select_buff_int_view);

        return found;
    }

    /*
     * The select buffer has the numbers of all the primitives
     * drawn in the small select volume.  This list is examined
     * to find the primitive that is the nearest in z to the
     * front.  In the first pass, only line primitives are
     * looked for.  In a second pass, the fill primitives are
     * looked for if no line primitive has been selected.
     */
    int ProcessHits (int hits, IntBuffer buffer)
    {
        int         i, j, found, istat;
        int         zt1, zt2, names;
        int         nstack[];
        int         trap_stack[], trap_found, trap_stack_size;
        long        real_trap_dist, trap_dist, dist, zt, z1, d1, d2, zdelta;

        nstack = new int[100];
        trap_stack = new int[100];

        dist = MAX_LDIST;
        found = 0;
        trap_found = 0;
        trap_dist = dist;
        real_trap_dist = dist;
        trap_stack_size = 0;
        d1 = dist;
        d2 = 0;

    /*
     * Find the min and max depth buffer values for the
     * hits, and get a zdelta from them.
     */
        names = 0;
        buffer.rewind();
        for (i = 0; i < hits; i++)
        {
           if (!buffer.hasRemaining())
               break;
          /*
           * For each hit, get the number of names on the stack and the z limits.
           */
            names = buffer.get();
            zt1 = buffer.get();
            if (zt1 >= 0) {
                z1 = (long)zt1;
            }
            else {
                z1 = MAX_LDIST + (long)zt1;
                z1++;
            }
            buffer.get();

          /*
           * get each name into the nstack array.
           */
            for (j = 0; j < names; j++)
            {
                if (j < 100) {
                    nstack[j] = buffer.get();
                } else {
                    buffer.get();
                }
            }
            if (z1 >= 0) {
                if (z1 < d1) d1 = z1;
                if (z1 > d2) d2 = z1;
            }
        }

        zdelta = 0;
        if (d2 > d1) {
            zdelta = d2 - d1;
            zdelta /= 10;
            if (zdelta < 1) zdelta = 1;
        }

        buffer.rewind();
        for (i = 0; i < hits; i++)
        {
           if (!buffer.hasRemaining())
               break;
          /*
           * For each hit, get the number of names on the stack and the z limits.
           */
            names = buffer.get();
            zt1 = buffer.get();
            if (zt1 >= 0) {
                z1 = (long)zt1;
            }
            else {
                z1 = MAX_LDIST + (long)zt1;
                z1++;
            }
            buffer.get();

          /*
           * get each name into the nstack array.
           */
            for (j = 0; j < names; j++)
            {
                if (j < 100) {
                    nstack[j] = buffer.get();
                } else {
                    buffer.get();
                }
            }
            if (names > 99) names = 99;

          /*
           * Apply the current selection filter to see if
           * this hit should be checked for z also.  This
           * call will only return true if a line type primitive
           * is found.
           */
            istat = FilterSelection (nstack, names, 0);
            if (istat == 0) {
                continue;
            }

          /*
           * Assume a 32 bit depth buffer for the JOGL stuff.
           */
            max_depth_value = (double)MAX_LDIST;

          /*
           * If a trap is being processed, bring it forward artificially.
           */
            if (nstack[0] == TRAP_NAME) {
                zt = z1 - zdelta;
                if (zt < trap_dist) {
                    trap_dist = zt;
                    real_trap_dist = z1;
                    trap_stack_size = names;
                    for (int kk=0; kk<names; kk++) {
                        trap_stack[kk] = nstack[kk];
                    }
                }
                trap_found = 1;
                continue;
            }
            if (z1 > 0  &&  z1 < dist) {
                found = 1;
                for (int kk=0; kk<names; kk++) {
                    selected_name_stack[kk] = nstack[kk];
                }
                stack_size = names;
                dist = z1;
                hit_z_value = (double)z1 / max_depth_value;
                if (hit_z_value > 1.0) hit_z_value = 1.0;
                if (hit_z_value < 0.0) hit_z_value = 0.0;
            }
        }

        if (found == 1  &&  dist < trap_dist) {
            return 1;
        }

        dist = MAX_LDIST;
        buffer.rewind();
        for (i = 0; i < hits; i++)
        {
           if (!buffer.hasRemaining())
               break;
          /*
           * For each hit, get the number of names on the stack and the z limits.
           */
            names = buffer.get();
            zt1 = buffer.get();
            if (zt1 >= 0) {
                z1 = (long)zt1;
            }
            else {
                z1 = MAX_LDIST + (long)zt1;
                z1++;
            }
            buffer.get();

          /*
           * get each name into the nstack array.
           */
            for (j = 0; j < names; j++)
            {
                if (j < 100) {
                    nstack[j] = buffer.get();
                } else {
                    buffer.get();
                }
            }
            if (names > 99) names = 99;

          /*
           * Apply the current selection filter to see if
           * this hit should be checked for z also.
           * This call will only return true if a fill
           * type primitive is found.
           */
            istat = FilterSelection (nstack, names, 1);
            if (istat == 0) {
                continue;
            }

          /*
           * The closest trap is already known in the trap_stack
           * array, so no traps are processed here.
           */
            if (nstack[0] == TRAP_NAME) {
                continue;
            }

          /*
           * Assume a 32 bit depth buffer for the JOGL stuff.
           */
            max_depth_value = (double)MAX_LDIST;

            if (z1 > 0  &&  z1 < dist) {
                found = 1;
                for (int kk=0; kk<names; kk++) {
                    selected_name_stack[kk] = nstack[kk];
                }
                stack_size = names;
                dist = z1;
                hit_z_value = (double)z1 / max_depth_value;
                if (hit_z_value > 1.0) hit_z_value = 1.0;
                if (hit_z_value < 0.0) hit_z_value = 0.0;
            }
        }

    /*
     * If a trap was found and it is closer to the viewpoint,
     * use it as the selected object.
     */
        if (trap_found != 0  &&  trap_stack_size > 0) {
            if (trap_dist <= dist) {
                found = 1;
                for (int kk=0; kk<names; kk++) {
                    selected_name_stack[kk] = nstack[kk];
                }
                stack_size = trap_stack_size;
                hit_z_value = real_trap_dist / max_depth_value;
                if (hit_z_value > 1.0) hit_z_value = 1.0;
                if (hit_z_value < 0.0) hit_z_value = 0.0;
            }
        }

        return found;

    }

    int FilterSelection (int[] stack, int n, int type)
    {

      /*
       * If the number of stack items is not 5 or 6, the item is by definition
       * not selectable.
       */
        if (n < 5) {
            return 0;
        }
        if (n > 7) {
            return 0;
        }

      /*
       * Traps trump everything, so always return 1 if they are
       * identified in the stack.
       */
        if (stack[0] == TRAP_NAME) {
            return 1;
        }

      /*
       * If type is zero, line type primitives are sought.  If
       * type is 1, fill type primitives are sought.
       */
        if (stack[4] == type) {
            return 1;
        }

        return 0;
    }

    void SetSelectionWindowSize (int size)
    {
        selection_window_size = size;
        if (selection_window_size < 2) selection_window_size = 2;
        if (selection_window_size > 100) selection_window_size = 100;
    }


    /*
     * Setup the viewing parameters for the drawing.
     */
    void SetupViewingFrustum( double aspect_ratio,
                              int mode)
    {
        double cmin, cmax, afact;

        gl.glMatrixMode(GL2.GL_PROJECTION);
        gl.glLoadIdentity();

        //Perspective
        if (mode == 0)
        {
            cmin = PERSPECTIVE_CMIN;
            cmax = PERSPECTIVE_CMAX;
            //cmin = near_slice;
            //cmax = far_slice;

            afact = (xscale + yscale + zscale) / 3.0;
            if (afact > 1.0) {
                afact = Math.sqrt (afact);
            }
            if (afact < 1.0) {
                afact = 1.0;
            }
            if (afact > PERSPECTIVE_MAX_AFACT) {
                afact = PERSPECTIVE_MAX_AFACT;
            }
            glu.gluPerspective(PERSPECTIVE_ANGLE / afact, aspect_ratio, cmin, cmax);
        }

        //Orthographic
        if (mode == 1)
        {
            gl.glOrtho(
                0.0, 3.0,
                0.0, 3.0 / aspect_ratio,
                -10.0, 10.0);
            gl.glTranslated(1.5, 1.5 / aspect_ratio, -3.0);
        }

        gl.glMatrixMode (GL2.GL_MODELVIEW);

    }

  /*
   * Set up an ortho projection using the pixel bounds
   * of the viewport.
   */
    void SetupRasterView ()
    {
        UpdateMatrices ();
        double width = viewport_matrix[2];
        double height = viewport_matrix[3];
        double depth = 100;

        gl.glMatrixMode(GL2.GL_PROJECTION);
        gl.glLoadIdentity();

        gl.glOrtho (-width / 2.0, width / 2.0,
                    -height / 2.0, height / 2.0,
                    -500.0, 500.0);

        gl.glMatrixMode (GL2.GL_MODELVIEW);

    }

    void InitNames ()
    {
        gl.glInitNames ();
    }

    void PushName (int ival)
    {
        gl.glPushName (ival);
    }

    void LoadName (int ival)
    {
        gl.glLoadName (ival);
    }

    void PopName ()
    {
        gl.glPopName ();
    }

/*
    void GetScreenCoords (double *x, double *y, double *z,
                                       int npts, double *sx, double *sy)
    {
        int            i;
        boolean      ia[10];
        double         da[10];

        if (x == null  ||  y == null  ||  z == null  ||
            npts < 1  ||  sx == null  ||  sy == null) {
            return;
        }

        for (i=0; i<npts; i++) {
            gl.glRasterPos3d (x[i], y[i], z[i]);
            gl.glGetBooleanv (GL2.GL_CURRENT_RASTER_POSITION_VALID, ia);
            if (ia[0]) {
                gl.glGetDoublev (GL2.GL_CURRENT_RASTER_POSITION, da);
                sx[i] = da[0];
                sy[i] = da[1];
            }
            else {
                sx[i] = 1.e30;
                sy[i] = 1.e30;
            }
        }

        return;
    }
*/

    void ReadAllPixels (int width, int height,
                        byte[] data)
    {
        ByteBuffer  bb = toBB (data);
        gl.glReadPixels (0, 0, width, height,
                      GL.GL_RGB, GL.GL_UNSIGNED_BYTE,
                      bb);
        data = bb.array();
    }



/*-----------------------------------------------------------------------------*/

  /*
   * Draw text without move or anchor, using default font of the specified size.
   */
    private int dpi = 0;

    void DrawTextXYZ (double x,
        double y,
        double z,
        String str,
        double sizeInInches)
    {
      /*
       * Get the resolution of the screen in dots per inch.
       */
        if (dpi < 1) {
            Toolkit tk = Toolkit.getDefaultToolkit ();
            dpi = tk.getScreenResolution ();
        }

        int size = (int)(sizeInInches * dpi * 1.6 + .5);

        Font font = new Font ("SansSerif",
            Font.PLAIN,
            size);
        this.DrawTextXYZ(x, y, z, str, font);

    }

  /*
   * Draw text without move or anchor, specifying the font.
   */
    void DrawTextXYZ (double x,
                      double y,
                      double z,
                      String str,
//                      double sizeInInches,
                      Font font)
    {

    /*
     * Create a properly sized font and get the rectangular
     * bounds of the string using this font.
     */
        FontRenderContext frc =
          new FontRenderContext (
              new AffineTransform(),
              false,
              false);

        Rectangle2D bounds =
        font.getStringBounds (str, frc);

        float ascent = font.getLineMetrics(str, frc).getAscent();
        float descent = font.getLineMetrics(str, frc).getDescent();

    /*
     * Create a buffered image to draw the text into.
     */
        int height = (int)(ascent + descent + .5);
        int width = (int)(bounds.getWidth() + .5);
        BufferedImage
        bi = new BufferedImage (width,
                                height,
                                BufferedImage.TYPE_INT_ARGB);

    /*
     * Draw the string into the image, flipping it in y
     * since opengl starts in the lower left.
     */
        Graphics2D g = bi.createGraphics ();
        //HashMap map = new HashMap();

        // This makes the text a little brighter.
        g.setRenderingHint (RenderingHints.KEY_RENDERING,
          RenderingHints.VALUE_RENDER_QUALITY);

        g.setPaint (new Color (0, 0, 0, 0));
        g.fillRect (0, 0, width, height);

        AffineTransform gt = new AffineTransform ();
        gt.translate (0.0, (double)height);
        gt.scale (1.0, -1.0);
        g.transform (gt);

        g.setFont (font);
        g.setPaint (Color.white);
        g.drawString (str, 0.0f, ascent);

        int nrow = bi.getHeight();
        int ncol = bi.getWidth();
        int i, j, rgb, r, offset, k, nbit;

        int bcol = ncol / 32;
        bcol++;
        bcol *= 4;

        byte[] bdata = new byte[nrow * bcol];
        int    b1, b2, j2, k8;

    /*
     * Initialize the bitmap to all zero's.
     */
        for (i=0; i<nrow*bcol; i++) {
            bdata[i] = 0;
        }

    /*
     * The background was set to black and the text foreground
     * to white.  Check the red coordinate in the image.  If
     * it is not zero, then the bit for that pixel must be set.
     * The bits in each byte in the opengl bitmap are arranged
     * so that bit 7 represents the farthest left pixel.
     */
        for (i=0; i<nrow; i++) {
            offset = i * bcol;
            for (j=0; j<ncol; j++) {
                rgb = bi.getRGB (j, i);
                if (rgb == 0) {
                    continue;
                }
                j2 = j / 8;
                k8 = j % 8;
                k = offset + j2;
                b1 = bdata[k];
                nbit = 7 - k8;
                b2 = 1 << nbit;
                b1 |= b2;
                bdata[k] = (byte)b1;
            }
        }

        DrawBitmap (x, y, z,
                    bcol * 8, nrow,
                    bdata);

        return;

    }


    void DrawPixels (double x, double y, double z,
                     int width, int height,
                     byte[] data)
    {
        gl.glRasterPos3d (x, y, z);

        ByteBuffer bb = toBB (data);
        gl.glDrawPixels (width, height,
                         GL.GL_RGBA, GL.GL_UNSIGNED_BYTE,
                         bb);
    }

    void DrawBitmap (double x, double y, double z,
                     int width, int height,
                     byte[] data)
    {
        gl.glColor4f((float)color_r, (float)color_g, (float)color_b, (float)1.0);
        gl.glRasterPos3d (x, y, z);

        ByteBuffer    bb = toBB (data);
        gl.glBitmap (width, height,
                     0.0f, 0.0f,
                     0.0f, 0.0f,
                     bb);
    }


    int SetupViewport (int cx, int cy)
    {
        gl.glViewport (0, 0, cx, cy);
        return 1;
    }

    int SetupViewport (int x1, int y1, int x2, int y2)
    {
        gl.glViewport (x1, y1, x2, y2);
        return 1;
    }

    void SetOrtho (double left, double right,
                   double bottom, double top,
                   double dnear, double dfar)
    {
        gl.glOrtho (left, right, bottom, top, dnear, dfar);
        return;
    }


    void UpdateMatrices ()
    {
        DoubleBuffer  db = toDB (model_matrix);
        gl.glGetDoublev (GL2.GL_MODELVIEW_MATRIX, db);
        model_matrix = db.array();
        db = toDB (projection_matrix);
        gl.glGetDoublev (GL2.GL_PROJECTION_MATRIX, db);
        projection_matrix = db.array();
        IntBuffer  ib = toIB (viewport_matrix);
        gl.glGetIntegerv (GL.GL_VIEWPORT, ib);
        viewport_matrix = ib.array();
    }

  /*
   * Return the current matrices for viewing.  This is used
   * to store "home" matrices or "last" matrices to allow
   * the J3DPanel to quickly replicate a particular viewpoint.
   */
    void GetMatrixData (double[] model,
                        double[] projection)
    {
        UpdateMatrices ();

        int i;

        for (i=0; i<16; i++) {
            model[i] = model_matrix[i];
            projection[i] = projection_matrix[i];
        }

        return;
    }

  /*
   * Set the matrices to values previously saved by GetMatrixData.
   * Use this from J3DPanel to go back to a particular view.
   */
    void SetMatrixData (double[] model,
                        double[] projection)
    {
        MatrixModeModel ();

        DoubleBuffer  db = toDB (model);
        gl.glLoadMatrixd (db);

        MatrixModeProjection ();

        db = toDB (projection);
        gl.glLoadMatrixd (db);

        return;
    }

    void PrintMatrices ()
    {
/*
System.out.println ("viewport matrix");
System.out.println ( viewport_matrix[0]+" "+
                     viewport_matrix[1]+" "+
                     viewport_matrix[2]+" "+
                     viewport_matrix[3]);
System.out.println ("model matrix");
System.out.println (model_matrix[0]+" "+
                    model_matrix[1]+" "+
                    model_matrix[2]+" "+
                    model_matrix[3]);
System.out.println (model_matrix[4]+" "+
                    model_matrix[5]+" "+
                    model_matrix[6]+" "+
                    model_matrix[7]);
System.out.println (model_matrix[8]+" "+
                    model_matrix[9]+" "+
                    model_matrix[10]+" "+
                    model_matrix[11]);
System.out.println (model_matrix[12]+" "+
                    model_matrix[13]+" "+
                    model_matrix[14]+" "+
                    model_matrix[15]);
System.out.println ("projection matrix");
System.out.println (projection_matrix[0]+" "+
                    projection_matrix[1]+" "+
                    projection_matrix[2]+" "+
                    projection_matrix[3]);
System.out.println (projection_matrix[4]+" "+
                    projection_matrix[5]+" "+
                    projection_matrix[6]+" "+
                    projection_matrix[7]);
System.out.println (projection_matrix[8]+" "+
                    projection_matrix[9]+" "+
                    projection_matrix[10]+" "+
                    projection_matrix[11]);
System.out.println (projection_matrix[12]+" "+
                    projection_matrix[13]+" "+
                    projection_matrix[14]+" "+
                    projection_matrix[15]);
*/
    }


/*------------------------------------------------------------------------------*/

    /*
     * Return the near and far points for a line that projects from the
     * current view through the specified screen point.
     */
    void CalcHitRay (double sx,
                     double syin,
                     JPoint3D near,
                     JPoint3D far)
    {
        double       sy;
        boolean      status;

        double[] wxyz = new double[4];

        near.x = 1.e30;
        near.y = 1.e30;
        near.z = 1.e30;

        far.x = 1.e30;
        far.y = 1.e30;
        far.z = 1.e30;

        UpdateMatrices ();

        sy = (double)viewport_matrix[3] - syin - 1.0;

      /*
       * Get near point (z screen = 0.0)
       * Bug 494  It appears that z values of zero or
       * 1 can cause the unproject to return a failure code,
       * so I tweak them a little.
       */
        status = glu.gluUnProject (sx, sy, 0.001,
                              model_matrix, 0,
                              projection_matrix, 0,
                              viewport_matrix, 0,
                              wxyz, 0);

        if (!status) {
            return;
        }

        near.x = wxyz[0];
        near.y = wxyz[1];
        near.z = wxyz[2];

      /*
       * Get far point (z screen = 1.0)
       * Bug 494  It appears that z values of zero or
       * 1 can cause the unproject to return a failure code,
       * so I tweak them a little.
       */
        status = glu.gluUnProject (sx, sy, 0.999,
                              model_matrix, 0,
                              projection_matrix, 0,
                              viewport_matrix, 0,
                              wxyz, 0);

        if (!status) {
            near.x = 1.e30;
            near.y = 1.e30;
            near.z = 1.e30;
            return;
        }

        far.x = wxyz[0];
        far.y = wxyz[1];
        far.z = wxyz[2];

        return;
    }





    /*
     * Return the cube coordinates for the view point.
     */
    void CalcViewPoint (JPoint3D near)
    {
        double       sx, sy;
        boolean      status;

        near.x = 1.e30;
        near.y = 1.e30;
        near.z = 1.e30;

        UpdateMatrices ();

        sx = (double)viewport_matrix[2];
        sy = (double)viewport_matrix[3];
        sx /= 2.0;
        sy /= 2.0;

        double[] wxyz = new double[4];

      /*
       * Get near point (z screen = 0.0)
       */
        status = glu.gluUnProject (sx, sy, 0.001,
                              model_matrix, 0,
                              projection_matrix, 0,
                              viewport_matrix, 0,
                              wxyz, 0);

        if (!status) {
            return;
        }

        near.x = wxyz[0];
        near.y = wxyz[1];
        near.z = wxyz[2];

        return;
    }



    int[] GetViewPort ()
    {
        return viewport_matrix;
    }


    JPoint2D GetRasterPos (double xp, double yp, double zp)
    {
        double       params[];
        int          iparams[];
        JPoint2D     r = new JPoint2D ();

        params = new double[5];
        iparams = new int[5];

        IntBuffer  ib = toIB (iparams);
        gl.glGetIntegerv (GL2.GL_RENDER_MODE, ib);
        iparams = ib.array();

        feed_buffer.clear();
        gl.glFeedbackBuffer (10000, GL2.GL_2D, feed_buffer_float_view);
        gl.glRenderMode (GL2.GL_FEEDBACK);

        gl.glRasterPos3d (xp, yp, zp);

        DoubleBuffer db = toDB  (params);
        gl.glGetDoublev (GL2.GL_CURRENT_RASTER_POSITION,
                         db);
        params = db.array();

        gl.glRenderMode (iparams[0]);

        r.x = params[0];
        r.y = params[1];

        return r;
    }

    int GetSelectedStack (int[] istack, int nmax)
    {
        int          n;

        if (istack == null) return -1;
        if (nmax < 7) return -1;

        n = stack_size;
        if (n > nmax) n = nmax;

        for (int i=0; i<n; i++) {
            istack[i] = selected_name_stack[i];
        }

        return n;
    }

    void NewList (int list)
    {
        gl.glNewList (list, GL2.GL_COMPILE);
    }

    void CallList (int list)
    {
        gl.glCallList (list);
    }

    void DeleteList (int list)
    {
        gl.glDeleteLists (list, 1);
    }

    void EndList ()
    {
        gl.glEndList ();
    }

/*-----------------------------------------------------------------------------*/

    private TextPositionFilter    textPositionFilter = null;

    void setTextPositionFilter (TextPositionFilter filter)
    {
        textPositionFilter = filter;
    }

    String         lastText = null;
    double         lastTextSize = -1.0;
    byte[]         lastBdata = null;
    int            lastNcol = 0;
    int            lastNrow = 0;
    int            lastWidth = 0;
    int            lastBcol = 0;

    void ResetTextCache ()
    {
        lastText = null;
        lastTextSize = -1.0;
        lastBdata = null;
    }

    LastTextData getLastTextData ()
    {
        LastTextData lt = new LastTextData ();
        lt.lastBdata = lastBdata;
        lt.lastNcol = lastNcol;
        lt.lastNrow = lastNrow;
        lt.lastWidth = lastWidth;
        lt.lastBcol = lastBcol;
        lt.lastText = lastText;
        lt.lastTextSize = lastTextSize;
        return lt;
    }

    void setLastTextData (LastTextData lt)
    {
        if (lt != null) {
            lastBdata = lt.lastBdata;
            lastNcol = lt.lastNcol;
            lastNrow = lt.lastNrow;
            lastWidth = lt.lastWidth;
            lastBcol = lt.lastBcol;
            lastText = lt.lastText;
            lastTextSize = lt.lastTextSize;
        }
        else {
            lastText = null;
            lastTextSize = -1.0;
            lastBdata = null;
            lastNcol = 0;
            lastNrow = 0;
            lastWidth = 0;
            lastBcol = 0;
        }

        return;
    }

/*-------------------------------------------------------------------*/

    private boolean moveAdjust = false;
    private int     textPositionLock = -1;

    void SetMoveAdjust (boolean bval)
    {
        moveAdjust = false;
    }

    void setTextPositionLock (int val)
    {
        if (val < 1  ||  val > 9) {
            val = -1;
        }
        textPositionLock = val;
    }

  /*
   * Draw text if the text position filter allows it.  The text is anchored
   * and possibly offset in this version.  Specify the size in inches and
   * use the default font.
   */
    int DrawTextXYZ (double x,
        double y,
        double z,
        String str,
        double sizeInInches,
        int    anchor,
        int    movex,
        int    movey)
    {
      /*
       * Get the resolution of the screen in dots per inch.
       */
        if (dpi < 1) {
            Toolkit tk = Toolkit.getDefaultToolkit ();
            dpi = tk.getScreenResolution ();
        }

        int size = (int)(sizeInInches * dpi * 1.6 + .5);

        /*
         * Create a properly sized font and get the rectangular
         * bounds of the string using this font.
         */
        Font font = new Font ("SansSerif",
                              Font.PLAIN,
                              size);

        int istatus = DrawTextXYZ(x, y, z, str, font, anchor, movex, movey);
        return istatus;
    }

  /*
   * Draw text if the text position filter allows it.  The text is anchored
   * and possibly offset in this version.  Specify the font.
   */
    int DrawTextXYZ (double x,
                     double y,
                     double z,
                     String str,
                     Font font,
                     int    anchor,
                     int    movex,
                     int    movey)
    {
        byte[]       bdata;
        int          width, height, ncol, nrow;
        int          i, j, rgb, r, offset, k, nbit, bcol;

        if (textPositionLock != -1  &&  anchor != textPositionLock) {
            return 0;
        }

        int size = font.getSize();

    /*
     * If the text or text size has changed, remake the bitmap.
     */
        if (!str.equals(lastText)  ||  size != lastTextSize) {

            FontRenderContext frc =
              new FontRenderContext (
                  new AffineTransform(),
                  false,
                  false);

            Rectangle2D bounds =
            font.getStringBounds (str, frc);

            float ascent = font.getLineMetrics(str, frc).getAscent();
            float descent = font.getLineMetrics(str, frc).getDescent();

        /*
         * Create a buffered image to draw the text into.
         */
            height = (int)(ascent + descent + .5);
            width = (int)(bounds.getWidth() + .5);
            BufferedImage
            bi = new BufferedImage (width,
                                    height,
                                    BufferedImage.TYPE_INT_ARGB);

        /*
         * Draw the string into the image, flipping it in y
         * since opengl starts in the lower left.
         */
            Graphics2D g = bi.createGraphics ();

            // This makes the text a little brighter.
            g.setRenderingHint (RenderingHints.KEY_RENDERING,
              RenderingHints.VALUE_RENDER_QUALITY);

            g.setPaint (new Color (0, 0, 0, 0));
            g.fillRect (0, 0, width, height);

            AffineTransform gt = new AffineTransform ();
            gt.translate (0.0, (double)height);
            gt.scale (1.0, -1.0);
            g.transform (gt);

            g.setFont (font);
            g.setPaint (Color.white);
            g.drawString (str, 0.0f, ascent);

            nrow = bi.getHeight();
            ncol = bi.getWidth();

            bcol = ncol / 32;
            bcol++;
            bcol *= 4;

            bdata = new byte[nrow * bcol];
            int    b1, b2, j2, k8;

        /*
         * Initialize the bitmap to all zero's.
         */
            for (i=0; i<nrow*bcol; i++) {
                bdata[i] = 0;
            }

        /*
         * The background was set to black and the text foreground
         * to white.  Check the red coordinate in the image.  If
         * it is not zero, then the bit for that pixel must be set.
         * The bits in each byte in the opengl bitmap are arranged
         * so that bit 7 represents the farthest left pixel.
         */
            for (i=0; i<nrow; i++) {
                offset = i * bcol;
                for (j=0; j<ncol; j++) {
                    rgb = bi.getRGB (j, i);
                    if (rgb == 0) {
                        continue;
                    }
                    j2 = j / 8;
                    k8 = j % 8;
                    k = offset + j2;
                    b1 = bdata[k];
                    nbit = 7 - k8;
                    b2 = 1 << nbit;
                    b1 |= b2;
                    bdata[k] = (byte)b1;
                }
            }
            lastBdata = bdata;
            lastText = str;
            lastTextSize = size;
            lastNcol = ncol;
            lastNrow = nrow;
            lastWidth = width;
            lastBcol = bcol;
        }

    /*
     * No changes, so use the last bitmap.
     */
        else {
            bdata = lastBdata;
            ncol = lastNcol;
            nrow = lastNrow;
            width = lastWidth;
            bcol = lastBcol;
        }

        if (anchor < 1  ||  anchor > 9) {
            return 1;
        }

    /*
     * Draw the bitmap with appropriate x and y moves for the
     * specified anchor and specified move value.
     */
        int               xmove, ymove;

        xmove = 0;
        ymove = 0;

        if (anchor == 1) {
            xmove = movex;
            ymove = movey;
        }
        else if (anchor == 2) {
            xmove = -ncol / 2 + movex;
            ymove = movey;
        }
        else if (anchor == 3) {
            xmove = movex - ncol;
            ymove = movey;
        }
        else if (anchor == 4) {
            xmove = movex;
            ymove = -nrow / 2 + movey;
        }
        else if (anchor == 5) {
            xmove = -ncol / 2 + movex;
            ymove = -nrow / 2 + movey;
        }
        else if (anchor == 6) {
            xmove = movex - ncol;
            ymove = -nrow / 2 + movey;
        }
        else if (anchor == 7) {
            xmove = movex;
            ymove = movey - nrow;
        }
        else if (anchor == 8) {
            xmove = -ncol / 2 + movex;
            ymove = movey - nrow;
        }
        else if (anchor == 9) {
            xmove = movex - ncol;
            ymove = movey - nrow;
        }

    /*
     * Check the text position filter.
     */
        if (textPositionFilter != null) {
            JPoint2D p2 = GetRasterPos (x, y, z);
            double    x1, y1, x2, y2;
            x1 = p2.x + xmove;
            y1 = p2.y + ymove;
            x2 = x1 + width;
            if (anchor == 1  ||  anchor == 4  ||  anchor == 7) {
                if (width < nrow * 2) {
                    x2 = x1 + nrow * 2;
                }
            }
            y2 = y1 + nrow;
            boolean b =
            textPositionFilter.checkTextPosition (x1, y1, x2, y2);
            if (b == false) {
                return 0;
            }
        }

        if (moveAdjust) {
            xmove /= 2;
            ymove /= 2;
        }

        DrawBitmap (x, y, z,
                    bcol * 8, nrow,
                    xmove, ymove,
                    bdata);

        return 1;

    }

    void DrawBitmap (double x, double y, double z,
                     int width, int height,
                     int xmove, int ymove,
                     byte[] data)
    {
        gl.glColor4f((float)color_r, (float)color_g, (float)color_b, (float)1.0);
        gl.glRasterPos3d (x, y, z);
        gl.glBitmap (0, 0,
                     0.0f, 0.0f,
                     (float)xmove, (float)ymove,
                     (ByteBuffer) null);
        ByteBuffer  bb = toBB (data);
        gl.glBitmap (width, height,
                     0.0f, 0.0f,
                     0.0f, 0.0f,
                     bb);
    }

/*-------------------------------------------------------------------*/

    final static int MY_LINE_TOKEN = 1799;

    int GetRasterPoints (
        JPoint3D[] p3dArray,
        int        n3d,
        JPoint2D[] p2dArray,
        int        max2d)
    {
        double       params[];
        int          iparams[];
        int          i;

        params = new double[5];
        iparams = new int[5];

        IntBuffer  ib = toIB (iparams);
        gl.glGetIntegerv (GL2.GL_RENDER_MODE, toIB (iparams));
        iparams = ib.array();
        feed_buffer.clear();
        gl.glFeedbackBuffer (10000, GL2.GL_2D, feed_buffer_float_view);
        gl.glRenderMode (GL2.GL_FEEDBACK);

        i = 0;
        SetDrawMode (DG_LINES);
        for (;;) {
            StartDrawMode ();
            DGNodeXYZ (p3dArray[i].x, p3dArray[i].y, p3dArray[i].z);
            DGNodeXYZ (p3dArray[i+1].x, p3dArray[i+1].y, p3dArray[i+1].z);
            StopDrawMode ();
            i += 2;
            if (i >= n3d) {
                break;
            }
        }

        int size = gl.glRenderMode (GL2.GL_RENDER);

        feed_buffer_float_view.rewind();
        int n = 0;
        i = 0;
        int    type;
        while (feed_buffer_float_view.hasRemaining()) {

            float token = feed_buffer_float_view.get();
            type = (int) (token + 0.01);
            if (type == MY_LINE_TOKEN) {
                p2dArray[n] = new JPoint2D();
                p2dArray[n].x = feed_buffer_float_view.get();
                p2dArray[n].y = feed_buffer_float_view.get();
                n++;
                p2dArray[n] = new JPoint2D();
                p2dArray[n].x = feed_buffer_float_view.get();
                p2dArray[n].y = feed_buffer_float_view.get();
                n++;
                i += 5;
                if (i >= size) {
                    break;
                }
                if (n >= max2d) {
                    System.out.println
                     ("raster point buffer too small "+n+" points needed.");
                    break;
                }
            }
            else {
/*
                System.out.println (
                  "Non-line token (" + token + ") found in feedback buffer[" + i + "]"
                );
*/
                break;
            }
        }

        gl.glRenderMode (iparams[0]);

        return n;
    }

/*-------------------------------------------------------------------*/

}  // end of main JGL class

