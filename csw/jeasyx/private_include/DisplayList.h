
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * This is the definition (header) file for the CDisplayList
 * class.  The display list encapsulates lists of line, fill,
 * text, symbol, shape and image primitives.  The spatial index
 * grids used for rapid lookup are also kept here.  This class
 * also has the frame list, graph list and the selectable object
 * list.
 *
 * This class replaces the old assemblage of easyx C functions
 * that kept track of all these separate lists.  This class is
 * intended for use in the native side of a java/jni implementation
 * of 2d graphics.
 */
#include <stdio.h>
#include <vector>

#include <csw/utils/private_include/gpf_calcdraw.h>
#include <csw/utils/private_include/gpf_spline.h>
#include <csw/utils/private_include/ply_calc.h>
#include <csw/utils/private_include/gpf_shape.h>
#include <csw/utils/private_include/gpf_graphP.h>
#include <csw/utils/private_include/ply_utils.h>

#include <csw/surfaceworks/include/contour_api.h>
#include <csw/surfaceworks/include/grid_api.h>

#include <csw/jeasyx/private_include/DLSurf.h>
#include <csw/jeasyx/private_include/DLContour.h>
#include <csw/jeasyx/private_include/DLSelectable.h>
#include <csw/jeasyx/private_include/EZXJavaArea.h>
#include <csw/jeasyx/private_include/gtx_drawprim.h>

#include <csw/jeasyx/private_include/gpf_linePP.h>
#include <csw/jeasyx/private_include/gpf_fillP.h>
#include <csw/jeasyx/private_include/gpf_imagePP.h>
#include <csw/jeasyx/private_include/gpf_shapeP.h>
#include <csw/jeasyx/private_include/gpf_symbPP.h>
#include <csw/jeasyx/private_include/gpf_textP.h>

#include <csw/jeasyx/private_include/gtx_graphP.h>
#include <csw/jeasyx/private_include/gtx_frinfoP.h>
#include <csw/jeasyx/private_include/gtx_layerP.h>

#ifndef _DISPLAYLIST_H_
#define _DISPLAYLIST_H_

#define _BIG_CHUNK_SIZE_            100
#define _MEDIUM_CHUNK_SIZE_         100
#define _SMALL_CHUNK_SIZE_          10

#define _DL_MAX_WORK_               25000
#define _DL_NUM_COLOR_BAND_INDEX_   10000
#define _DL_NUM_SIZE_UNITS_LIST_    3

class CDisplayList {

private:

    CSWGrdAPI    *grdapi_ptr = NULL;

    GPFGraph     gpf_graph_obj;
    GPFCalcdraw  gpf_calcdraw_obj;
    GPFSpline    gpf_spline_obj;
    CSWPolyCalc  ply_calc_obj;
    GPFShape     gpf_shape_obj;
    CSWContourApi  conapi_obj;
    CSWPolyUtils   ply_utils_obj;
    GTXDrawPrim    gtx_drawprim_obj;
    EZXJavaArea    ezx_java_obj;


public:

    CDisplayList();
    CDisplayList (const CDisplayList &old);
    CDisplayList &operator=(const CDisplayList &old);
    virtual ~CDisplayList();

    void SetGrdAPIPtr (CSWGrdAPI *p) {grdapi_ptr = p;};

    void SetVoidJavaAreaPtrs (void *v_jenv_in, void *v_jobj_in) 
    {
         ezx_java_obj.SetJNIPtrs (v_jenv_in, v_jobj_in);
    }

  /*
   * Set and get various boundary stuff
   */
    void SetPageUnitsType (int type);
    void SetScreenDPI (int ival);
    int SetDrawingBoundsHint (double page_xmin_in,
                              double page_ymin_in,
                              double page_xmax_in,
                              double page_ymax_in);
    void SetScreenBounds (double screen_lower_left_x,
                          double screen_lower_left_y,
                          double screen_upper_right_x,
                          double screen_upper_right_y);

    void GetDrawingBoundsHint (double *page_xmin_out,
                               double *page_ymin_out,
                               double *page_xmax_out,
                               double *page_ymax_out);
    void GetDrawingBounds (double *page_xmin_out,
                           double *page_ymin_out,
                           double *page_xmax_out,
                           double *page_ymax_out);
    void GetScreenBounds (double *screen_lower_left_x,
                          double *screen_lower_left_y,
                          double *screen_upper_right_x,
                          double *screen_upper_right_y);

    void SetPageClipLimits (double x1, double y1,
                            double x2, double y2);
    void SetScreenClipLimits (double x1, double y1,
                              double x2, double y2);

  /*
   * Draw everything in the display list.
   */
    void Draw (void);
    void Draw (FILE *dfile);
    void DrawSelected (void);
    void UnselectAll (void);
    void DeleteSelected (void);
    void HideSelected (void);
    void UnhideAll (void);

  /*
   * Set various graphic attribute states.
   */
    int CreateFrame (int rescaleable,
                     int moveable,
                     int aspect_flag,
                     double xmin,
                     double ymin,
                     double xmax,
                     double ymax,
                     double fx1,
                     double fy1,
                     double fx2,
                     double fy2,
                     CSW_F px1,
                     CSW_F py1,
                     CSW_F px2,
                     CSW_F py2,
                     int borderflag,
                     int attach_position,
                     double extra_gap,
                     double perp_move,
                     char *cdata,
                     int scale_width_to_attach_frame,
                     int scale_height_to_attach_frame);

   int SetFrameAxisValues (
                    char   *cdata,
                    int    axis_id,
                    int    label_flag,
                    int    tick_flag,
                    int    tick_direction,
                    double major_interval,
                    int    line_red,
                    int    line_green,
                    int    line_blue,
                    int    text_red,
                    int    text_green,
                    int    text_blue,
                    double line_thickness,
                    double text_size,
                    double text_thickness,
                    int    text_font,
                    int    caption_flag);

    int AddAxis (
                     char   *cdata,
                     int    label_dir,
                     int    label_flag,
                     int    tick_flag,
                     int    tick_direction,
                     double major_interval,
                     int    line_red,
                     int    line_green,
                     int    line_blue,
                     int    text_red,
                     int    text_green,
                     int    text_blue,
                     double line_thickness,
                     double text_size,
                     double text_thickness,
                     int    text_font,
                     int    caption_flag,
                     double x1,
                     double y1,
                     double x2,
                     double y2,
                     double firstValue,
                     double lastValue,
                     int    label_anchor,
                     double label_angle
                );

    int SetFrame (char *name);
    void SetFrameClip (int ival);
    void SetFrameNum (int ival);
    void GetFrameClipLimits (int fnum,
                             double *fx1,
                             double *fy1,
                             double *fx2,
                             double *fy2);

    void SetMinimumFrameSeparation (double fval);
    int ZoomOut (int frame_num);
    int ZoomExtents (int frame_num);
    int ZoomExtentsForFrameName (
        char *name,
        double top_marg,
        double left_marg,
        double bottom_marg,
        double right_marg);
    int RescaleFrame (char *name, int resize_border_flag,
                      double fx1, double fy1, double fx2, double fy2);
    int RescaleFrame (int frame_num,
                      int resize_border_flag,
                      int ix1, int iy1, int ix2, int iy2);
    int PanFrame (int frame_num,
                  int ix1, int iy1, int ix2, int iy2);

    int CreateGraph (GRaphRec *grec_ptr, char *name);
    int SetGraphName (char *name);
    void SetGraphNum (int ival);

    int CreateSelectable (int number, char *name);
    void SetSelectableNum (int ival);
    void EraseSelectableNum (int ival);

    void SetXaxisNum (int ival);
    void SetYaxisNum (int ival);

    int CreateLayer (int number, int selectable_flag, char *name);
    int SetLayerName (char *name);
    void SetLayerNum (int ival);
    void SetLayerSelectableFlag (int layer_num, int flag);
    void SetLayerSelectableFlag (char *layer_name, int flag);

    int CreateItem (int number, char *name);
    int SetItemName (char *name);
    void SetItemNum (int ival);

    int UnsetFrame (void);
    int UnsetLayer (void);
    int UnsetItem (void);

    void SetDefaultGraphicAttributes (void);

    int SetSizeUnits (char *name);
    void SetSizeUnitsNum (int ival);

    void SetTriMeshNum (int ival);
    void SetGridNum (int ival);

    void SetLineThickness (double dvalue);
    void SetLinePattern (int ival, double dvalue);
    void SetLineArrow (int ival);
    void SetLineSymbol (int ival);
    void SetTextFont (int ival);
    void SetTextBackground (int ival);
    void SetTextThickness (double dvalue);
    void SetTextAnchor (int ival);
    void SetTextOffsets (double xoff, double yoff);
    void SetFillPattern (int patnum, double psize);

    void SetImageColors (double *low, double *high,
                         int *red, int *green, int *blue, int *alpha, int ncolors);
    void SetImageOptions (int origin_flag, int column1, int row1,
                          int column_space, int row_space);
    void SetImageName (char *name);

    void SetBackgroundColor (int red, int green, int blue, int alpha);
    void SetForegroundColor (int red, int green, int blue, int alpha);
    void SetColor (int red, int green, int blue, int alpha);
    void SetLineColor (int red, int green, int blue, int alpha);
    void SetFillColor (int red, int green, int blue, int alpha);
    void SetSymbolColor (int red, int green, int blue, int alpha);
    void SetTextColor (int red, int green, int blue, int alpha);
    void SetTextFillColor (int red, int green, int blue, int alpha);
    void SetFillPatternColor (int red, int green, int blue, int alpha);
    void SetFillBorderColor (int red, int green, int blue, int alpha);

    void SetLineSmoothFlag (int ival);
    void SetEditableFlag (int ival);
    void SetSelectableFlag (int ival);

  /*
   * Get various graphic attribute states.
   */
    void GetFrame (char *name, int maxlen);
    int GetFrameClip (void);
    int GetFrameNum (void);
    int GetGraphName (char *name, int maxlen);
    int GetGraphNum (void);
    int GetSelectableName (char *name, int maxlen);
    int GetSelectableNum (void);
    int GetXaxisName (char *name, int maxlen);
    int GetXaxisNum (void);
    int GetYaxisName (char *name, int maxlen);
    int GetYaxisNum (void);

    void GetLayerName (char *name, int maxlen);
    int GetLayerNum (void);
    void GetItemName (char *name, int maxlen);
    int GetItemNum (void);

    int GetSizeUnits (void);
    void GetSizeUnits (char *name, int maxlen);

    double GetLineThickness (void);
    void GetLinePattern (int *ival, double *dvalue);
    int GetLineArrow (void);
    int GetTextFont (void);
    double GetTextThickness (void);
    void GetFillPattern (int *ipat, double *psize, double *pthick);

    void GetImagecolors (double *low, double *high,
                         int *red, int *green, int *blue, int *ncolors);
    void GetImageOptions (int *origin_flag, int *column1, int *row1,
                          int *column_space, int *row_space);
    void GetImageName (char *name, int maxlen);

    void GetBackgroundColor (int *red, int *green, int *blue);
    void GetForegroundColor (int *red, int *green, int *blue);
    void GetLineColor (int *red, int *green, int *blue);
    void GetFillColor (int *red, int *green, int *blue);
    void GetSymbolColor (int *red, int *green, int *blue);
    void GetTextColor (int *red, int *green, int *blue);
    void GetTextFillColor (int *red, int *green, int *blue);
    void GetFillPatternColor (int *red, int *green, int *blue);
    void GetFillBorderColor (int *red, int *green, int *blue);

    int GetLineSmoothFlag (void);
    int GetEditableFlag (void);
    int GetSelectableFlag (void);

  /*
   * functionality from old line primitive C code.
   */
    int DrawAllLines (void);
    int SetSpatialIndexForLine (int index_of_line);
    int CalcLineBounds (LInePrim *lptr);
    int CalcLineBounds (int index_of_line,
                        double *x1, double *y1, double *x2, double *y2);
    int AddLine (CSW_F *x, CSW_F *y, int npts);

    int DrawAllContourLines (void);
    int SetSpatialIndexForContourLine (int index_of_line);
    int CalcContourLineBounds (int index_of_line,
                        double *x1, double *y1, double *x2, double *y2);
    int AddContourLine (CSW_F *x, CSW_F *y, int npts);

  /*
   * functionality from old fill primitive C code.
   */
    int DrawAllFills (void);
    int SetSpatialIndexForFill (int index_of_fill);
    int CalcFillBounds (int index_of_fill,
                        double *x1, double *y1, double *x2, double *y2);
    int AddFill (double *x, double *y,
                 int *points_per_component, int num_components,
                 int outline_flag);

  /*
   * functionality from old text primitive C code.
   */
    int DrawAllTexts (void);
    int SetSpatialIndexForText (int index_of_text);
    int CalcTextBounds (int index_of_text,
                        double *x1, double *y1, double *x2, double *y2);
    int AddText (double x, double y, double size,
                 double angle, char *text);
    int AddNumber (double x, double y, double size,
                   double angle, double value, int ndec,
                   int comma_flag);

  /*
   * functionality from old symb primitive C code.
   */
    int DrawAllSymbs (void);
    int SetSpatialIndexForSymb (int index_of_symb);
    int CalcSymbBounds (int index_of_symb,
                        double *x1, double *y1, double *x2, double *y2);
    int AddSymb (double x, double y, double size,
                     double angle, int symbol_num);

  /*
   * functionality from old shape primitive C code.
   */
    int DrawAllShapes (void);
    int SetSpatialIndexForShape (int index_of_shape);
    int CalcShapePageBounds (CSW_F *fval, int type, int frame_num,
                             double *x1, double *y1, double *x2, double *y2);
    int CalcShapePageBounds (int index_of_shape,
                             double *x1, double *y1, double *x2, double *y2);
    int CalcShapeBounds (int index_of_shape,
                         double *x1, double *y1, double *x2, double *y2);
    int AddShape (int shape_type,
                  double *shape_values);

  /*
   * functionality from old image primitive C code.
   */
    int DrawAllImages (void);

    int DrawFrameBorders (void);

  /*
   * Overloaded functions for images of different data types.
   */
    int AddDataImage (double *data, int ncol, int nrow, double nullval,
                      double xmin, double ymin, double xmax, double ymax);
    int AddDataImage (float *data, int ncol, int nrow, float nullval,
                      double xmin, double ymin, double xmax, double ymax);
    int AddDataImage (int *data, int ncol, int nrow, int nullval,
                      double xmin, double ymin, double xmax, double ymax);
    int AddDataImage (short int *data, int ncol, int nrow, short int nullval,
                      double xmin, double ymin, double xmax, double ymax);
    int AddDataImage (unsigned char *data, int ncol, int nrow, char nullval,
                      double xmin, double ymin, double xmax, double ymax);
    int AddColorImage (unsigned char *red_data,
                       unsigned char *green_data,
                       unsigned char *blue_data,
                       unsigned char *transparency_data,
                       int ncol, int nrow,
                       double xmin, double ymin, double xmax, double ymax);

  /*
   * Access to contour list.
   */
    int AddContour (COntourOutputRec *crec,
                    int grid_num,
                    int image_id);
    int AddContour (double *x, double *y, int npts,
                    double zvalue, char *label);
    int SetSpatialIndexForContour (int index_of_contour);

  /*
   * Setup colors and options used in both grids and trimeshes.
   */
    int SetTmpImageBands (double *low, double *high,
                          int *red, int *green, int *blue, int *alpha, int ncolors);

    int SetTmpContourProperties (int *idata, double *ddata);

    int SetTmpFaultLines (int nline, int ntot,
                          int *npts,
                          double *x, double *y, double *z);

  /*
   * Add to trimesh list.
   */
    int AddTriMesh (char *name,
                    double *xnode,
                    double *ynode,
                    double *znode,
                    int    *flagnode,
                    int numnode,
                    int *n1edge,
                    int *n2edge,
                    int *t1edge,
                    int *t2edge,
                    int *flagedge,
                    int numedge,
                    int *e1tri,
                    int *e2tri,
                    int *e3tri,
                    int *flagtri,
                    int numtri);

    int SetImageFaultData (
        double *xf, double *yf,
        int    *npf, int nf, int ntot);

  /*
   * AddGrid
   */
    int AddGrid (char *name,
                 double *data,
                 int ncol,
                 int nrow,
                 double xmin,
                 double ymin,
                 double width,
                 double height,
                 double angle);
    int AddGridImage (int fnum, float *data, int ncol, int nrow, float nullval,
                      double xmin, double ymin, double xmax, double ymax);
    int AddGridImage (int fnum, int image_id, float *data,
                      int ncol, int nrow, float nullval,
                      double xmin, double ymin, double xmax, double ymax);
    int AddGridImage (int fnum, int image_id, CSW_F *data,
                      int ncol, int nrow, float nullval,
                      double xmin, double ymin, double xmax, double ymax);
    int AddGridColorImage (int fnum,
                       unsigned char *red_data,
                       unsigned char *green_data,
                       unsigned char *blue_data,
                       unsigned char *transparency_data,
                       int ncol, int nrow,
                       double xmin, double ymin, double xmax, double ymax);
    int AddGridColorImage (int fnum,
                       int image_id,
                       unsigned char *red_data,
                       unsigned char *green_data,
                       unsigned char *blue_data,
                       unsigned char *transparency_data,
                       int ncol, int nrow,
                       double xmin, double ymin, double xmax, double ymax);

    double GetFrameUnitsPerPixel (int fnum);

  /*
   * Access to frame list.
   */
    int AddFrame (FRameStruct *frame_ptr);
    int GetFrameCopyForIndex (int index_of_frame, FRameStruct *frame_ptr);
    FRameStruct const *GetFrameForIndex (int index_of_frame);
    int GetCurrentFrameIndex (void);

  /*
   * Access the graph list.
   */
    int AddGraph (GRaphRec *graph_ptr);
    int GetGraphCopyForIndex (int index_of_graph, GRaphRec *graph_ptr);
    GRaphRec const *GetGraphForIndex (int index_of_graph, GRaphRec *graph_ptr);
    int GetCurrentGraphIndex (void);

  /*
   * Access the selectable object list
   */
    void AddSelectableObject (int object_index);
    DLSelectable const *GetSelectableObjectForIndex (int object_index);
    int GetCurrentSelectableObjectIndex (void);
    int SetSelectableState (int index, int ival);

  /*
   * Access to layer list.
   */
    int AddLayer (LAyerStruct *layer_ptr);
    int AddLayer (char *name);
    LAyerStruct const *GetLayerForIndex (int index_of_layer);
    int GetCurrentLayerIndex (void);

  /*
   * Access to item list.
   */
    int AddItem (ITemStruct *item_ptr);
    int AddItem (char *name);
    ITemStruct const *GetItemForIndex (int index_of_item);
    int GetCurrentItemIndex (void);

  /*
   * Picking and display list read functions.
   */
    int PickFrameObject (int frame_num,
                         int ix,
                         int iy);

    int GetFrameObject (int frame_num,
                        int ix,
                        int iy);

    int ConvertToFrame (int frame_num,
                        int ix,
                        int iy);

    int ReadSelectableObject (int ival);

    void SetImageID (int id);


private:

  /*
   * private functions
   */
    template<typename T>
    void ZeroInit (T p, int n);

    int AddLine (CSW_F *x, CSW_F *y, int npts, bool from_graph);
    int AddText (double x, double y, double size,
                 double angle, char *text, bool from_graph);

    void delete_surf_contours (int surf_num);
    void delete_surf_contour_lines (int surf_num);

    bool IsFrameSynced (int pick_fnum, int fnum);
    bool CheckInsideFrame (int frame_num,
                           double xmin, double ymin,
                           double xmax, double ymax);

    void backscalef (CSW_F sx, CSW_F sy, CSW_F *px, CSW_F *py);
    void ReturnSelected (void);

    void return_selected_lines (DLSelectable *dls);
    void return_selected_fills (DLSelectable *dls);
    void return_selected_texts (DLSelectable *dls);
    void return_selected_symbs (DLSelectable *dls);
    void return_selected_rectangles (DLSelectable *dls);
    void return_selected_arcs (DLSelectable *dls);
    void return_selected_contours (DLSelectable *dls);
    void return_selected_axes (DLSelectable *dls);

    int ProcessFramePick (int frame_num,
                          CSW_F x,
                          CSW_F y);
    int GetSelectableIndex (int frame_num,
                          CSW_F x,
                          CSW_F y);
    int ClosestPickPrim (int frame_num,
                         CSW_F x,
                         CSW_F y,
                         int *index,
                         int *type);
    void closest_frame_axis (int fnum, CSW_F xin, CSW_F yin,
                             int *indexout, CSW_F *pdistout);
    void closest_frame_contour (int fnum, CSW_F xin, CSW_F yin,
                                int *indexout, CSW_F *pdistout);
    void closest_frame_line (int fnum, CSW_F xin, CSW_F yin,
                             int *indexout, CSW_F *pdistout);
    void closest_frame_fill_border (int fnum, CSW_F xin, CSW_F yin,
                                    int *indexout, CSW_F *pdistout);
    void closest_frame_symb (int fnum, CSW_F xin, CSW_F yin,
                             int *indexout, CSW_F *pdistout);
    void closest_frame_text (int fnum, CSW_F xin, CSW_F yin,
                             int *indexout, CSW_F *pdistout);
    void closest_frame_shape_border (int fnum, CSW_F xin, CSW_F yin,
                                     int *indexout, CSW_F *pdistout);
    void closest_frame_fill (int fnum, CSW_F xin, CSW_F yin,
                             int *indexout);
    void closest_frame_shape (int fnum, CSW_F xin, CSW_F yin,
                              int *indexout);

    int SetupSpatialIndexForFrame (int fnum);
    void ClearFrameSpatialIndex (int fnum);

    int PopulateLinePatches (double x1, double y1, double x2, double y2);
    int PopulateFillPatches (double x1, double y1, double x2, double y2);
    int PopulateTextPatches (double x1, double y1, double x2, double y2);
    int PopulateSymbPatches (double x1, double y1, double x2, double y2);
    int PopulateShapePatches (double x1, double y1, double x2, double y2);
    int PopulateContourLinePatches (double x1, double y1, double x2, double y2);

    int CalcTextOutline (int text_prim_num,
                         double *xmin, double *ymin,
                         double *xmax, double *ymax,
                         double *xpoly, double *ypoly, int *npoly);

    int reindex_primitives (int fnum);
    void reindex_lines (int fnum);
    void reindex_fills (int fnum);
    void reindex_texts (int fnum);
    void reindex_symbs (int fnum);
    void reindex_shapes (int fnum);
    void reindex_contours (int fnum);

    int set_grid_for_vec (double x1, double y1, double x2, double y2,
                          LInePrim *prim);
    int set_in_extra (LInePrim *prim);

    int set_grid_for_contour_vec (double x1, double y1, double x2, double y2,
                                  int contour_num);
    int set_in_contour_extra (int contour_num);

    void convert_frame_array (CSW_F *xy, int npts);
    void convert_frame_array (int fnum, CSW_F *xy, int npts);
    void convert_frame_array (CSW_F *x, CSW_F *y, int npts);
    void unconvert_frame_array (CSW_F *xy, int npts);
    void unconvert_frame_array (int fnum, CSW_F *xy, int npts);
    void unconvert_frame_array (CSW_F *x, CSW_F *y, int npts);
    void convert_frame_point (CSW_F *x, CSW_F *y);
    void unconvert_frame_point (CSW_F *x, CSW_F *y);
    void convert_frame_point (int fnum, CSW_F *x, CSW_F *y);
    void unconvert_frame_point (int fnum, CSW_F *x, CSW_F *y);
    void convert_frame_dist (int fnum, CSW_F *dist);
    void unconvert_frame_dist (int fnum, CSW_F *dist);

    int set_grid_for_vec (double x1, double y1, double x2, double y2,
                          FIllPrim *prim);
    int set_grid_for_area (FIllPrim *prim);
    int set_in_extra (FIllPrim *prim);

    int set_grid_for_area (TExtPrim *prim);
    int set_in_extra (TExtPrim *prim);

    int set_grid_for_area (SYmbPrim *prim);
    int set_in_extra (SYmbPrim *prim);

    int set_grid_for_area (SHapePrim *prim);
    int set_in_extra (SHapePrim *prim);

    void free_patch_lists (void);
    void free_pick_lists (void);

    int add_contour_line_patch_prim (int prim_num);
    int add_line_patch_prim (int prim_num);
    int add_fill_patch_prim (int prim_num);
    int add_text_patch_prim (int prim_num);
    int add_symb_patch_prim (int prim_num);
    int add_shape_patch_prim (int prim_num);

    void free_available_lists (void);

    int add_available_line (int prim_num);
    int get_available_line (void);
    int add_available_fill (int prim_num);
    int get_available_fill (void);
    int add_available_text (int prim_num);
    int get_available_text (void);
    int add_available_symb (int prim_num);
    int get_available_symb (void);
    int add_available_shape (int prim_num);
    int get_available_shape (void);
    int add_available_axis (int prim_num);
    int get_available_axis (void);
    int add_available_image (int prim_num);
    int get_available_image (void);
    int add_available_contour (int prim_num);
    int get_available_contour(void);
    int add_available_contour_line (int prim_num);
    int get_available_contour_line(void);

    void free_hidden_lists (void);

    int add_hidden_line (int prim_num);
    int add_hidden_fill (int prim_num);
    int add_hidden_text (int prim_num);
    int add_hidden_symb (int prim_num);
    int add_hidden_shape (int prim_num);
    int add_hidden_axis (int prim_num);
    int add_hidden_image (int prim_num);
    int add_hidden_contour (int prim_num);

    void setup_color_band_index (void);
    void lookup_image_color (double val,
                             int *red,
                             int *green,
                             int *blue,
                             int *trans);

    void free_axes (void);
    void free_lines (void);
    void free_fills (void);
    void free_texts (void);
    void free_symbs (void);
    void free_shapes (void);
    void free_images (void);

    void update_frame_limits (void);

    void calc_frame_layout (void);
    void calc_frame_margins (FRameStruct *frptr);

    void format_border_label (char *label,
                              double value,
                              double range,
                              int maxlen);

    void default_frame_axes (FRameStruct *fp);

    void rescale_frame (int frame_num);
    void rescale_frame (FRameStruct *frptr);
    void reborder_frame (int frame_num);
    void reborder_frame (FRameStruct *frptr);
    void reaxis_frame (FRameStruct *frptr);

    int find_frame (char *name);

    void delete_frame_axis_lines (int fnum);
    void delete_frame_axis_fills (int fnum);
    void delete_frame_axis_texts (int fnum);

    void delete_frame_border_lines (int fnum);
    void delete_frame_border_fills (int fnum);
    void delete_frame_border_texts (int fnum);
    void calc_frame_border_prims (FRameStruct *frptr);
    int  calc_frame_axis_prims (FRameAxisStruct *axptr,
                                CSW_F px1,
                                CSW_F py1,
                                CSW_F px2,
                                CSW_F py2,
                                CSW_F axval1,
                                CSW_F axval2,
                                int position);
    int  calc_and_rotate_frame_axis_prims (
                                int fnum,
                                FRameAxisStruct *axptr,
                                CSW_F px1,
                                CSW_F py1,
                                CSW_F px2,
                                CSW_F py2,
                                CSW_F axval1,
                                CSW_F axval2,
                                int position,
                                CSW_F xorigin,
                                CSW_F yorigin,
                                CSW_F cang,
                                CSW_F sang);

    int  calc_and_rotate_frame_axis_prims_direct (
                                FRameAxisStruct *axptr,
                                CSW_F px1,
                                CSW_F py1,
                                CSW_F px2,
                                CSW_F py2,
                                CSW_F axval1,
                                CSW_F axval2,
                                int position,
                                CSW_F xorigin,
                                CSW_F yorigin,
                                CSW_F cang,
                                CSW_F sang);

    int CalcAxisBounds (AXisPrim *aptr,
                         double *x1, double *yt1, double *x2, double *y2);
    int calc_axis_limits (
        int fnum,
        FRameAxisStruct *axptr,
        CSW_F px1,
        CSW_F py1,
        CSW_F px2,
        CSW_F py2,
        CSW_F val1,
        CSW_F val2,
        int position,
        double *xmin,
        double *ymin,
        double *xmax,
        double *ymax);

    void erase_selected_lines (DLSelectable *dls);
    void erase_selected_fills (DLSelectable *dls);
    void erase_selected_texts (DLSelectable *dls);
    void erase_selected_symbs (DLSelectable *dls);
    void erase_selected_shapes (DLSelectable *dls);
    void erase_selected_contours (DLSelectable *dls);
    void erase_selected_axes (DLSelectable *dls);

    void delete_frame_cell_edges (int fnum);
    void delete_frame_lines (int fnum);
    void delete_frame_fills (int fnum);
    void delete_frame_symbs (int fnum);
    void delete_frame_texts (int fnum);
    void delete_frame_shapes (int fnum);
    void delete_frame_images (int fnum);
    void delete_frame_grid_images (int fnum);
    void delete_frame_ndp_images (int fnum);

    void reclip_frame_contours (int fnum);
    void reclip_frame_lines (int fnum);
    void reclip_frame_fills (int fnum);
    void reclip_frame_symbs (int fnum);
    void reclip_frame_texts (int fnum);
    void reclip_frame_shapes (int fnum);
    void reclip_frame_grid_images (int fnum);

    void reclip_and_draw_selected_axes (DLSelectable *dls);
    void reclip_and_draw_selected_lines (DLSelectable *dls);
    void reclip_and_draw_selected_fills (DLSelectable *dls);
    void reclip_and_draw_selected_texts (DLSelectable *dls);
    void reclip_and_draw_selected_symbs (DLSelectable *dls);
    void reclip_and_draw_selected_shapes (DLSelectable *dls);
    void reclip_and_draw_selected_contours (DLSelectable *dls);

    void force_frame_aspect (int aspect_flag, int resize_border_flag,
                             CSW_F *pxmin, CSW_F *pymin, CSW_F *pxmax, CSW_F *pymax,
                             double *frx1, double *fry1, double *frx2, double *fry2);

    void clean_frame_list (void);
    void test_frame_indexes (void *frptr);

    void find_frame_limits (int frame_num,
                            double *xmin,
                            double *ymin,
                            double *xmax,
                            double *ymax);

    void extract_sub_strings (char *buffer, int *sindex, int nstr);

    void shift_frame_prims (int fnum, CSW_F dx, CSW_F dy);
    void shift_xy (CSW_F *xy, int npts, CSW_F dx, CSW_F dy);

    void draw_frame_border_prims (int fnum);
    void draw_frame_axis_prims (int fnum);

    void save_current_graphic_attributes (void);
    void unsave_current_graphic_attributes (void);

    void recalc_surfaces (void);

    void delete_grid_prims (int gridnum);
    void delete_grid_contours (int gridnum);
    void delete_grid_lines (int gridnum);
    void delete_grid_symbs (int gridnum);
    void delete_grid_texts (int gridnum);
    void delete_grid_images (int gridnum);

    void delete_trimesh_prims (int trimeshnum);
    void delete_trimesh_contours (int trimeshnum);
    void delete_trimesh_lines (int trimeshnum);
    void delete_trimesh_symbs (int trimeshnum);
    void delete_trimesh_texts (int trimeshnum);
    void delete_trimesh_images (int trimeshnum);

    int pick_axis_text_prims (
        double xpick,
        double ypick,
        double *dmin,
        int fnum,
        FRameAxisStruct *axptr,
        CSW_F px1,
        CSW_F py1,
        CSW_F px2,
        CSW_F py2,
        CSW_F val1,
        CSW_F val2,
        int position,
        CSW_F cang,
        CSW_F sang);

    void distance_to_text (
        CSW_F xin,
        CSW_F yin,
        double xtext,
        double ytext,
        double tsize,
        double tang,
        int font_num,
        int anchor,
        char *chardata,
        CSW_F *pdistout);

    int axis_text_pick (
        AXisPrim *aptr,
        double *dmin,
        double xpick,
        double ypick);


  /*
   * private data members
   */

    const int  max_static_ppts = 1000;
    double   static_xy_local[2002];

    int      page_units_type;

    CSW_F    *xywork = NULL,
             *xywork2 = NULL;
    int      *iwork = NULL;

    int      ImageID;

    double   *ImageXFault = NULL,
             *ImageYFault = NULL;
    int      *ImageNFaultPoints = NULL,
             ImageNFaults,
             ImageNFaultTotal;

  /*
   * graphic attribute state members
   */
    int             current_frame_clip_flag;
    int             current_border_num;
    int             current_axis_num;
    int             current_grid_num;
    int             current_frame_num;

    int             current_graph_num;
    int             current_selectable_object_num;
    DLSelectable    *current_selectable_object = NULL;
    int             current_xaxis_num;
    int             current_yaxis_num;

    int             current_layer_num;
    int             current_item_num;
    int             current_size_units;

    CSW_F           current_line_thickness;
    int             current_line_pattern;
    CSW_F           current_line_pattern_size;
    int             current_line_symbol;
    int             current_line_arrow_style;

    int             current_text_font;
    CSW_F           current_text_thickness;
    int             current_text_anchor;
    CSW_F           current_text_x_offset;
    CSW_F           current_text_y_offset;
    int             current_text_background;

    int             current_fill_outline_flag;
    int             current_fill_pattern;
    CSW_F           current_fill_pattern_size;

    CSW_F           current_image_low_values[1000];
    CSW_F           current_image_high_values[1000];
    int             current_image_red_values[1000];
    int             current_image_green_values[1000];
    int             current_image_blue_values[1000];
    int             current_image_transparency_values[1000];
    int             current_number_image_values;

    char            current_image_name[100];
    int             current_image_id;

    int             current_image_origin;
    int             current_image_column1;
    int             current_image_row1;
    int             current_image_colspace;
    int             current_image_rowspace;

    int             current_background_red, current_background_green,
                    current_background_blue, current_background_alpha;
    int             current_foreground_red, current_foreground_green,
                    current_foreground_blue, current_foreground_alpha;
    int             current_global_red, current_global_green,
                    current_global_blue, current_global_alpha;
    int             current_line_red, current_line_green,
                    current_line_blue, current_line_alpha;
    int             current_fill_red, current_fill_green,
                    current_fill_blue, current_fill_alpha;
    int             current_symbol_red, current_symbol_green,
                    current_symbol_blue, current_symbol_alpha;
    int             current_text_red, current_text_green,
                    current_text_blue, current_text_alpha;
    int             current_text_fill_red, current_text_fill_green,
                    current_text_fill_blue, current_text_fill_alpha;
    int             current_fill_pattern_red, current_fill_pattern_green,
                    current_fill_pattern_blue, current_fill_pattern_alpha;
    int             current_fill_border_red, current_fill_border_green,
                    current_fill_border_blue, current_fill_border_alpha;

    int             current_line_smooth_flag;
    int             current_editable_flag;
    int             current_selectable_flag;

    double          current_image_null_value;

    int             current_contour_index;

  /*
   * lists of grids, trimeshes and contours.
   */
    std::vector<DLContour *>   contour_list;

    std::vector<DLSurf *>      surf_list;

    DLContourProperties   tmp_contour_props;
    void            default_contour_properties (void);

    FAultLineStruct *tmp_contour_faults = NULL;
    int             num_tmp_contour_faults;

  /*
   * members with various lists of primitives
   */
    std::vector<LInePrim>   contour_line_prim_list;

    std::vector<LInePrim>   line_prim_list;

    std::vector<FIllPrim>   fill_prim_list;

    std::vector<SYmbPrim>   symb_prim_list;

    std::vector<TExtPrim>   text_prim_list;
    int                     num_selectable_text = 0;

    std::vector<SHapePrim>  shape_prim_list;

    std::vector<IMagePrim>  image_prim_list;

    std::vector<AXisPrim>   axis_prim_list;

  /*
   * lists with indexes of available primitives
   * primitives can become available when they are
   * deleted, rescaled from a frame, etc.
   */
    std::vector<int>      line_available_list;
    std::vector<int>      contour_available_list;
    std::vector<int>      contour_line_available_list;
    std::vector<int>      fill_available_list;
    std::vector<int>      text_available_list;
    std::vector<int>      symb_available_list;
    std::vector<int>      shape_available_list;
    std::vector<int>      axis_available_list;
    std::vector<int>      image_available_list;

  /*
   * lists with indexes of hidden primitives
   * primitives can become hidden when they are
   * deleted, rescaled from a frame, etc.
   */
    std::vector<int>    line_hidden_list;
    std::vector<int>    fill_hidden_list;
    std::vector<int>    symb_hidden_list;
    std::vector<int>    text_hidden_list;
    std::vector<int>    shape_hidden_list;
    std::vector<int>    axis_hidden_list;
    std::vector<int>    contour_hidden_list;

  /*
   * patch drawing lists.
   */
    std::vector<int> contour_line_patch_list;
    std::vector<int> line_patch_list;
    std::vector<int> fill_patch_list;
    std::vector<int> symb_patch_list;
    std::vector<int> text_patch_list;
    std::vector<int> shape_patch_list;

    int             patch_draw_flag = 0;

  /*
   * patch picking lists
   */
    std::vector<int> contour_line_pick_list;
    std::vector<int> line_pick_list;
    std::vector<int> fill_pick_list;
    std::vector<int> symb_pick_list;
    std::vector<int> text_pick_list;
    std::vector<int> shape_pick_list;

    int             patch_pick_flag = 0;

  /*
   * spatial indexing arrays.  These are kept on a per frame
   * basis and the data from the active frame is referenced
   * by the display list via these variables.
   */
    int             **line_spatial_index = NULL;
    int             **fill_spatial_index = NULL;
    int             **symb_spatial_index = NULL;
    int             **text_spatial_index = NULL;
    int             **shape_spatial_index = NULL;
    int             **contour_spatial_index = NULL;

    int             num_line_index,
                    num_fill_index,
                    num_text_index,
                    num_symb_index,
                    num_shape_index,
                    num_contour_index;

  /*
   * members with display list geometry
   */
    double          raw_page_xmin,
                    raw_page_ymin,
                    raw_page_xmax,
                    raw_page_ymax;

    int             raw_from_hints_flag;

    double          page_xmin,
                    page_ymin,
                    page_xmax,
                    page_ymax;

    double          clip_xmin,
                    clip_ymin,
                    clip_xmax,
                    clip_ymax;

    double          screen_xmin,
                    screen_ymin,
                    screen_xmax,
                    screen_ymax;

    double          x_page_to_screen_scale,
                    y_page_to_screen_scale;

    double          index_xmin,
                    index_ymin,
                    index_xmax,
                    index_ymax,
                    index_xspace,
                    index_yspace;

    double          page_index_xmin,
                    page_index_ymin,
                    page_index_xmax,
                    page_index_ymax;

    int             index_ncol,
                    index_nrow;

  /*
   * Lists of various grouping objects.
   */
    FRameStruct     *frame_list = NULL;
    int             num_frame_list,
                    max_frame_list;

    GRaphStruct     *graph_list = NULL;
    int             num_graph_list,
                    max_graph_list;

    DLSelectable    **selectable_object_list = NULL;
    int             num_selectable_object_list,
                    max_selectable_object_list;

    LAyerStruct     *layer_list = NULL;
    int             num_layer_list,
                    max_layer_list;

    ITemStruct      *item_list = NULL;
    int             num_item_list,
                    max_item_list;

  /*
   * Current frame limits
   */
    CSW_F           Pickx1, Picky1, Pickx2, Picky2;
    CSW_F           Fx1, Fy1, Fx2, Fy2;
    CSW_F           Px1, Py1, Px2, Py2;

    CSW_F           page_units_per_inch;

    int             screen_dpi;

  /*
   * misc. variables for internal use.
   */
    int             color_band_index[_DL_NUM_COLOR_BAND_INDEX_];
    CSW_F           min_color_band_index,
                    max_color_band_index,
                    delta_color_band_index;

    char            *size_units_list[_DL_NUM_SIZE_UNITS_LIST_];
    char            size_units_buffer[20 * _DL_NUM_SIZE_UNITS_LIST_];

    int             spline_used_flag;

    int             frame_layout_needed;
    int             screen_was_resized;
    CSW_F           minimum_frame_separation;

    int             border_margin_only_flag;

    FILE            *dfile = NULL;

    int             HideFlag;

  /*
   * saved attribute state members
   */
    int             saved_frame_clip_flag;
    int             saved_border_num;
    int             saved_axis_num;
    int             saved_frame_num;
    int             saved_graph_num;
    int             saved_selectable_object_num;
    DLSelectable    *saved_selectable_object = NULL;
    int             saved_xaxis_num;
    int             saved_yaxis_num;

    int             saved_layer_num;
    int             saved_item_num;
    int             saved_size_units;

    CSW_F           saved_line_thickness;
    int             saved_line_pattern;
    CSW_F           saved_line_pattern_size;
    int             saved_line_symbol;
    int             saved_line_arrow_style;

    int             saved_text_font;
    CSW_F           saved_text_thickness;
    int             saved_text_anchor;
    CSW_F           saved_text_x_offset;
    CSW_F           saved_text_y_offset;
    int             saved_text_background;

    int             saved_fill_outline_flag;
    int             saved_fill_pattern;
    CSW_F           saved_fill_pattern_size;

    CSW_F           saved_image_low_values[1000];
    CSW_F           saved_image_high_values[1000];
    int             saved_image_red_values[1000];
    int             saved_image_green_values[1000];
    int             saved_image_blue_values[1000];
    int             saved_image_transparency_values[1000];
    int             saved_number_image_values;

    char            saved_image_name[100];
    int             saved_image_id;

    int             saved_image_origin;
    int             saved_image_column1;
    int             saved_image_row1;
    int             saved_image_colspace;
    int             saved_image_rowspace;

    int             saved_background_red, saved_background_green,
                    saved_background_blue, saved_background_alpha;
    int             saved_foreground_red, saved_foreground_green,
                    saved_foreground_blue, saved_foreground_alpha;
    int             saved_global_red, saved_global_green,
                    saved_global_blue, saved_global_alpha;
    int             saved_line_red, saved_line_green,
                    saved_line_blue, saved_line_alpha;
    int             saved_fill_red, saved_fill_green,
                    saved_fill_blue, saved_fill_alpha;
    int             saved_symbol_red, saved_symbol_green,
                    saved_symbol_blue, saved_symbol_alpha;
    int             saved_text_red, saved_text_green,
                    saved_text_blue, saved_text_alpha;
    int             saved_text_fill_red, saved_text_fill_green,
                    saved_text_fill_blue, saved_text_fill_alpha;
    int             saved_fill_pattern_red, saved_fill_pattern_green,
                    saved_fill_pattern_blue, saved_fill_pattern_alpha;
    int             saved_fill_border_red, saved_fill_border_green,
                    saved_fill_border_blue, saved_fill_border_alpha;

    int             saved_line_smooth_flag;
    int             saved_editable_flag;
    int             saved_selectable_flag;

    double          saved_image_null_value;

};

/*
 * add nothing below this endif
 */
#endif
