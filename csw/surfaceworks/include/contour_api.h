
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    contour_api.h

    This header defines the CSWContourApi class.  This class is the
    class that should be used by application code to access the 
    contouring algorithms and functions.  By and large, the methods 
    of the CSWContourApi class are thin wraps around methods in a
    "private" object which is part of this Api class.  An application
    should not directly use a "private" object.
*/

/*
    Add nothing above this ifndef
*/
#ifndef CONTOUR_API_H
#define CONTOUR_API_H


/*
    Include the contour drawing header file
    and the csw utilities header file.
*/
#ifndef PRIVATE_HEADERS_OK
#define PRIVATE_HEADERS_OK
#endif

#  include "csw/utils/include/csw_.h"

#  include "csw/surfaceworks/include/con_shared_structs.h"
#  include "csw/surfaceworks/include/grd_shared_structs.h"

#  include "csw/surfaceworks/private_include/con_calc.h"
#  include "csw/surfaceworks/private_include/con_xsect.h"
#  include "csw/surfaceworks/private_include/con_draw.h"
#  include "csw/surfaceworks/private_include/con_triangle_class.h"

#  include "csw/surfaceworks/private_include/grd_fault.h"
#  include "csw/surfaceworks/private_include/grd_arith.h"
#  include "csw/surfaceworks/private_include/grd_utils.h"
#  include "csw/surfaceworks/private_include/grd_calc.h"
#  include "csw/surfaceworks/private_include/grd_image.h"
#  include "csw/surfaceworks/private_include/grd_triangle_class.h"
#  include "csw/surfaceworks/private_include/grd_fileio.h"
#  include "csw/surfaceworks/private_include/grd_stats.h"
#  include "csw/surfaceworks/private_include/grd_tsurf.h"
#  include "csw/surfaceworks/private_include/grd_constraint.h"




    class CSWContourApi
    {
    
      private:

        CSWConCalc    con_calc_obj;
        CSWConDraw    con_draw_obj;
        CSWConTriangle  con_triangle_obj;
        CSWConXsect   con_xsect_obj;

        CSWGrdUtils   grd_utils_obj;
        CSWGrdCalc    grd_calc_obj;
        CSWGrdFault   grd_fault_obj;
        CSWGrdArith   grd_arith_obj;
        CSWGrdImage   grd_image_obj;
        CSWGrdTriangle  grd_triangle_obj;
        CSWGrdFileio  grd_fileio_obj;
        CSWGrdStats   grd_stats_obj;
        CSWGrdTsurf   grd_tsurf_obj;
        CSWGrdConstraint  grd_constraint_obj;

        void SetContourAPIPtrs () {

            con_calc_obj.SetGrdFaultPtr (&grd_fault_obj);
            con_calc_obj.SetGrdArithPtr (&grd_arith_obj);
            con_calc_obj.SetGrdUtilsPtr (&grd_utils_obj);

            con_draw_obj.SetGrdUtilsPtr (&grd_utils_obj);

            con_triangle_obj.SetConCalcPtr (&con_calc_obj);
            con_triangle_obj.SetGrdFaultPtr (&grd_fault_obj);
            con_triangle_obj.SetGrdUtilsPtr (&grd_utils_obj);
            con_triangle_obj.SetGrdFileioPtr (&grd_fileio_obj);
            con_triangle_obj.SetGrdTrianglePtr (&grd_triangle_obj);

            con_xsect_obj.SetGrdArithPtr (&grd_arith_obj);
            con_xsect_obj.SetGrdUtilsPtr (&grd_utils_obj);

            grd_utils_obj.SetGrdArithPtr (&grd_arith_obj);
            grd_utils_obj.SetGrdCalcPtr (&grd_calc_obj);
            grd_utils_obj.SetGrdFaultPtr (&grd_fault_obj);
            grd_utils_obj.SetGrdTrianglePtr (&grd_triangle_obj);

            grd_calc_obj.SetGrdArithPtr (&grd_arith_obj);
            grd_calc_obj.SetGrdFaultPtr (&grd_fault_obj);
            grd_calc_obj.SetGrdFileioPtr (&grd_fileio_obj);
            grd_calc_obj.SetGrdStatsPtr (&grd_stats_obj);
            grd_calc_obj.SetGrdTrianglePtr (&grd_triangle_obj);
            grd_calc_obj.SetGrdTsurfPtr (&grd_tsurf_obj);
            grd_calc_obj.SetGrdUtilsPtr (&grd_utils_obj);

            grd_fault_obj.SetGrdArithPtr (&grd_arith_obj);
            grd_fault_obj.SetGrdFileioPtr (&grd_fileio_obj);
            grd_fault_obj.SetGrdTrianglePtr (&grd_triangle_obj);
            grd_fault_obj.SetGrdUtilsPtr (&grd_utils_obj);

            grd_arith_obj.SetGrdFaultPtr (&grd_fault_obj);
            grd_arith_obj.SetGrdUtilsPtr (&grd_utils_obj);

        CSWGrdTsurf   grd_tsurf_obj;
        CSWGrdConstraint  grd_constraint_obj;

            grd_image_obj.SetGrdArithPtr (&grd_arith_obj);
            grd_image_obj.SetGrdFileioPtr (&grd_fileio_obj);
            grd_image_obj.SetGrdUtilsPtr (&grd_utils_obj);

            grd_triangle_obj.SetGrdArithPtr (&grd_arith_obj);
            grd_triangle_obj.SetGrdCalcPtr (&grd_calc_obj);
            grd_triangle_obj.SetGrdConstraintPtr (&grd_constraint_obj);
            grd_triangle_obj.SetGrdFaultPtr (&grd_fault_obj);
            grd_triangle_obj.SetGrdTsurfPtr (&grd_tsurf_obj);
            grd_triangle_obj.SetGrdUtilsPtr (&grd_utils_obj);

            grd_fileio_obj.SetGrdFaultPtr (&grd_fault_obj);
            grd_fileio_obj.SetGrdUtilsPtr (&grd_utils_obj);

            grd_stats_obj.SetGrdFaultPtr (&grd_fault_obj);
            grd_stats_obj.SetGrdUtilsPtr (&grd_utils_obj);

            grd_tsurf_obj.SetGrdFileioPtr (&grd_fileio_obj);
            grd_tsurf_obj.SetGrdUtilsPtr (&grd_utils_obj);

            grd_constraint_obj.SetGrdFaultPtr (&grd_fault_obj);
            grd_constraint_obj.SetGrdFileioPtr (&grd_fileio_obj);
            grd_constraint_obj.SetGrdTrianglePtr (&grd_triangle_obj);
            grd_constraint_obj.SetGrdUtilsPtr (&grd_utils_obj);

        };


      public:
        
        CSWContourApi () {SetContourAPIPtrs ();};
        //~CSWContourApi () {};

        int con_SetCalcOption (int, int, CSW_Float);
        int con_SetCalcOptions (COntourCalcOptions *);
        int con_DefaultCalcOptions (COntourCalcOptions *);
        int con_SetContourIntervals (CSW_Float, int, CSW_Float, CSW_Float,
                                     CSW_Float*, int, CSW_Float*, int);
        int con_CalcContoursFromDouble
                             (CSW_Float*, int, int,
                              double, double, double, double, CSW_Float,
                              COntourOutputRec **, int*,
                              FAultLineStruct*, int,
                              COntourCalcOptions*);
        int con_CalcContours (CSW_Float*, int, int,
                              CSW_Float, CSW_Float, CSW_Float, CSW_Float, CSW_Float,
                              COntourOutputRec **, int*,
                              FAultLineStruct*, int,
                              COntourCalcOptions*);
        int con_FreeContours (COntourOutputRec *, int);

        int con_BuildColorBands (CSW_Float, CSW_Float, CSW_Float, int,
                                 CSW_Float*, CSW_Float*, int*);
        int con_SetColorBands (CSW_Float*, CSW_Float*, int*, int);
        int con_SetZeroFillColor (int);
        int con_CalcColorFillsFromDouble
                               (CSW_Float*, int, int,
                                double, double, double, double, CSW_Float,
                                COntourFillRec **, int*,
                                FAultLineStruct*, int,
                                COntourCalcOptions*);
        int con_CalcColorFills (CSW_Float*, int, int,
                                CSW_Float, CSW_Float, CSW_Float, CSW_Float, CSW_Float,
                                COntourFillRec **, int*,
                                FAultLineStruct*, int,
                                COntourCalcOptions*);
        int con_FreeColorFills (COntourFillRec *, int);

        int con_GetErr (void);

        int con_SetDrawScaleFromDouble
                             (double, double, double, double,
                              CSW_Float, CSW_Float, CSW_Float, CSW_Float);
        int con_SetDrawScale (CSW_Float, CSW_Float, CSW_Float, CSW_Float,
                              CSW_Float, CSW_Float, CSW_Float, CSW_Float);
        int con_SetDrawOption (int, int, CSW_Float);
        int con_SetDrawOptions (COntourDrawOptions *);
        int con_DefaultDrawOptions (COntourDrawOptions *);
        int con_DrawLine (COntourOutputRec*, CSW_Float*,
                          COntourLinePrim**, int*,
                          COntourTextPrim**, int*, COntourDrawOptions*);
        int con_DrawFill (COntourFillRec*,
                          COntourFillPrim**, int*);
        int con_FreeDrawing (COntourLinePrim*, int,
                             COntourTextPrim*, int,
                             COntourFillPrim*, int);
        int con_DrawText (COntourTextPrim*,
                          COntourLinePrim**, int*);
        int con_ResampleForSmoothing (CSW_F*, char*, int, int,
                                      CSW_F, CSW_F, CSW_F, CSW_F,
                                      CSW_F**, char**, int*, int*);
        int con_CalcCrossSection (GRidStruct*, int,
                                  CRossSectionTraceStruct*, int,
                                  CRossSectionLineStruct**, int*,
                                  CRossSectionFillStruct**, int*,
                                  CSW_F page_units_per_inch);
        int con_CalcCrossSectionPolygons (CRossSectionLineStruct*,
                                          CRossSectionLineStruct*,
                                          CRossSectionFillStruct**,
                                          int*,
                                          CSW_F page_units_per_inch);
        int con_DrawCrossSection (CRossSectionLineStruct*, int,
                                  CRossSectionFillStruct*, int,
                                  int, int,
                                  char*, char*,
                                  CSW_F, CSW_F,
                                  CRossSectionLinePrimitive**, int*,
                                  CRossSectionFillPrimitive**, int*);
        int con_FreeCrossSection (CRossSectionLineStruct*, int,
                                  CRossSectionFillStruct*, int);
        int con_FreeCrossSectionDrawing
                                 (CRossSectionLinePrimitive*, int,
                                  CRossSectionFillPrimitive*, int);

        int con_AutoGridLimits (CSW_F*, int, int, CSW_F*, CSW_F*);
        int con_ClipContoursToPolygon (int inside_outside_flag,
                                       COntourOutputRec *contours, int ncontours,
                                       double *xpoly, double *ypoly, int *ipoly, int npoly,
                                       COntourOutputRec **conclip, int *nconclip);

        COntourOutputRec *con_CopyOutputRec (COntourOutputRec *cp);

        int con_SmoothTriMesh (NOdeStruct **nodes, int *numnodes,
                               EDgeStruct **edges, int *numedges,
                                   TRiangleStruct **tris, int *numtris,
                           int sflag);

        int con_CalcTriMeshContours (NOdeStruct*, int,
                                     EDgeStruct*, int,
                                     TRiangleStruct*, int,
                                     double,
                                     COntourOutputRec**, int*,
                                     COntourCalcOptions*);

    }; // end of main class

/*
    End of the header file, add nothing below this line.
*/
#endif
