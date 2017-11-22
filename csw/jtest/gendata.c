#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "csw/surfaceworks/include/grid_api.h"

#include "csw/utils/include/csw_.h"

#define  MAX_POINTS  400000
#define  MAX_PROF    1000
#define  MAX_CLUST   20
#define  MAX_SOURCE  100

typedef struct {
    CSW_Float        x1, y1, x2, y2;
} FourFloat;


static int           DataDistFlag = 1;
static CSW_Float     NoiseFact = 0.0f;
static int           FlatFlag = 0;
static int           NumCluster = 0,
                     NumProfile = 0,
                     TrendOrder = 3,
                     ThickFlag = 0,
                     Neven = 0;
static int           ClustPts[MAX_CLUST],
                     ProfPts[MAX_PROF];
static CSW_Float     Xsource[MAX_SOURCE],
                     Ysource[MAX_SOURCE],
                     SourceStrength[MAX_SOURCE],
                     Zsource[MAX_SOURCE];

static int           Iseed;
static int           HemisphereFlag = 0,
                     FaultFlag = 0,
                     IlineFlag = 0,
                     NumAnomaly = 0,
                     BustSign = 0,
                     NumBusts = 0;
static CSW_Float     LogBase;

static CSW_Float     X[MAX_POINTS], Y[MAX_POINTS], Z[MAX_POINTS];
static int           Ndata;

static CSW_Float     Xmin, Ymin, Xmax, Ymax;
static FourFloat     ProfLoc[MAX_PROF], ClustLoc[MAX_CLUST];

static char          Ans[200];

static CSW_Float     AnisoAngle = -1000.f,
                     AnisoExcen = 1.0f,
                     Zfudge = 1.0f;
static int           Nxyrow, Nxycol, GridXY;

CSW_Float            csw_FloatRandom ();

static int           GenerateXYData ();
static int           GenerateStepData ();
static int           AddAnomalies ();


int GenerateZData ();
int WriteData ();


main (argc, argv)

    int         argc;
    char        *argv[];
{
    int              istat, i, xytype, ztype;
    char             comment[200], filename[200];
    FILE             *outfile;
    CSW_Float        f1, f2, f3, f4, zfudge;

/*
    set up log file to write to
*/
    if (argc >= 3) {
        if (!strcmp ("-log", argv[1])) {
            istat = csw_CreateLogFile (argv[2]);
            if (istat == -1) {
                fprintf (stderr,"\nError creating log file\n");
                exit (0);
            }
        }
    }

    Iseed = 123457;

    while (1) {

        NumCluster = 0;
        NumProfile = 0;
        Neven = 0;

        printf ("\n\nEnter output file name [Default, Exit Program]: ");
        csw_gets (Ans);
        if (Ans[0] == '\0') break;
        csw_StrTruncate (filename, Ans, 200);

        outfile = fopen (filename, "w");
        if (!outfile) {
            printf ("I cannot write to this file, try again.\n");
            continue;
        }

        printf ("Enter x1, y1, x2, y2 for data area [default 0,0,100,100]: ");
        Xmin = 0.0;
        Ymin = 0.0;
        Xmax = 100.0;
        Ymax = 100.0;
        csw_gets (Ans);
        if (Ans[0] != '\0') {
            sscanf (Ans, "%f %f %f %f", &Xmin, &Ymin, &Xmax, &Ymax);
        }

        zfudge = (Xmax - Xmin + Ymax - Ymin) / 200.0;
        if (zfudge < 1.0) zfudge = 1.0;

        printf ("Data patterns available:\n\
1 = Fairly evenly scattered [Default]\n\
2 = Clustered\n\
3 = Profiled\n\
4 = Combined clustered and even\n\
5 = Combined profile and even\n\
6 = Combined cluster and profile\n\
7 = Combine all 3\n\
8 = regular grid\n\
9 = interpretation line test\n\
\n");
        printf ("Enter the pattern you want for the file: ");
        csw_gets (Ans);
        if (Ans[0] == '\0') Ans[0] = '1';
        xytype = 1;
        sscanf (Ans, "%d", &xytype);
        if (xytype < 1  ||  xytype > 9) xytype = 1;

        if (xytype == 1  ||  xytype == 4  ||  xytype == 5  ||  xytype == 7) {
            printf ("Enter number of fairly evenly scattered points [Default = 100]: ");
            csw_gets (Ans);
            Neven = 100;
            if (Ans[0] != '\0') {
                sscanf (Ans, "%d", &Neven);
            }
        }

        if (xytype == 2  ||  xytype == 4  ||  xytype == 6  ||  xytype == 7) {
            printf ("Enter number of clusters [default = 2]: ");
            csw_gets (Ans);
            NumCluster = 2;
            if (Ans[0] != '\0') {
                sscanf (Ans, "%d", &NumCluster);
            }
            if (NumCluster < 1  ||  NumCluster > MAX_CLUST) NumCluster = 2;

            for (i=0; i<NumCluster; i++) {
                printf ("Enter number of points for cluster %d [default = 100]: ", i+1);
                csw_gets (Ans);
                ClustPts[i] = 100;
                if (Ans[0] != '\0') {
                    sscanf (Ans, "%d", ClustPts+i);
                }
                printf ("Enter x, y, width and height for cluster %d: ", i+1);
                csw_gets (Ans);
                sscanf (Ans, "%f %f %f %f\n", &f1, &f2, &f3, &f4);
                ClustLoc[i].x1 = f1;
                ClustLoc[i].y1 = f2;
                ClustLoc[i].x2 = f3;
                ClustLoc[i].y2 = f4;
            }
        }

        if (xytype == 3  ||  xytype == 5  ||  xytype == 7  ||  xytype == 9) {
            printf ("Enter number of profiles [Default = 10]: ");
            csw_gets (Ans);
            IlineFlag = 0;
            if (xytype == 9) {
                IlineFlag = 1;
            }
            NumProfile = 10;
            if (Ans[0] != '\0') {
                sscanf (Ans, "%d", &NumProfile);
            }
            if (NumProfile < 0  ||  NumProfile > MAX_PROF) NumProfile = 10;
            for (i=0; i<NumProfile; i++) {
                printf ("Enter number of points for profile %d [Default = 100]: ", i+1);
                csw_gets (Ans);
                ProfPts[i] = 100;
                if (Ans[0] != '\0') {
                    sscanf (Ans, "%d", ProfPts+i);
                }
                printf ("Enter x1, y1, x2, y2 for profile %d: ", i+1);
                csw_gets (Ans);
                sscanf (Ans, "%f %f %f %f\n", &f1, &f2, &f3, &f4);
                ProfLoc[i].x1 = f1;
                ProfLoc[i].y1 = f2;
                ProfLoc[i].x2 = f3;
                ProfLoc[i].y2 = f4;
            }
        }

        Nxyrow = -1;
        Nxycol = -1;
        GridXY = 0;
        if (xytype == 8) {
            printf ("Enter rows and columns for grid [10,10]: ");
            csw_gets (Ans);
            Nxyrow = 10;
            Nxycol = 10;
            if (Ans[0] != '\0') {
                sscanf (Ans, "%d %d", &Nxyrow, &Nxycol);
            }
            GridXY = 1;
            Xmin = 0.0f;
            Ymin = 0.0f;
            Xmax = (CSW_F)(Nxycol - 1);
            Ymax = (CSW_F)(Nxyrow - 1);
        }

        printf ("Surface Models available\n\
1 = Polynomial Surface\n\
2 = Concentric Circles\n\
3 = Hemisphere\n\
4 = Inverse Square Field [Default]\n\
5 = Step Grid Data\n\
\nTake a pick: ");
        
        csw_gets (Ans);
        ztype = 4;
        if (Ans[0] != '\0') {
            sscanf (Ans, "%d", &ztype);
        }

        if (ztype == 5) {
            GenerateXYData ();
            GenerateStepData ();
            LogBase = 0;
            NoiseFact = -1.0;
            NumBusts = 0;
            FaultFlag = 0;
            ThickFlag = 0;
            WriteData (outfile, comment);
            continue;
        }

        Zfudge = 1.0;
        if (ztype == 4) {
            printf ("Enter anisotropy angle and excentricity [Default isotropic]: ");
            csw_gets (Ans);
            AnisoAngle = -1000;
            AnisoExcen = 1;
            if (Ans[0] != '\0') {
                sscanf (Ans, "%f %f", &AnisoAngle, &AnisoExcen);
                if (AnisoExcen < 1.5) AnisoExcen = 1.5;
            }
            Zfudge = zfudge;
        }

        if (ztype == 1) {
            printf ("Enter polynomial order [Default = 3]: ");
            csw_gets (Ans);
            TrendOrder = 3;
            if (Ans[0] != '\0') {
                sscanf (Ans, "%d", &TrendOrder);
            }
            NumAnomaly = 0;
            if (TrendOrder < 4) {
                printf ("Enter number of anomalies: ");
                csw_gets (Ans);
                if (Ans[0] != '\0') {
                    sscanf (Ans, "%d", &NumAnomaly);
                }
            }
        }

        printf ("Enter Noise Factor [Default = 0.0]: ");
        csw_gets (Ans);
        NoiseFact = 0.0;
        if (Ans[0] != '\0') {
            sscanf (Ans, "%f", &NoiseFact);
        }
        FlatFlag = 0;
        if (NoiseFact < -100.0) FlatFlag = 1;
        if (NoiseFact > .5) NoiseFact = .5;
        if (NoiseFact < 0.) NoiseFact = 0.;

        printf ("Enter FaultFlag [Default = 0]: ");
        csw_gets (Ans);
        FaultFlag = 0;
        if (Ans[0] != '\0') {
            sscanf (Ans, "%d", &FaultFlag);
        }
        if (FaultFlag != 1) FaultFlag = 0;

        printf ("Enter ThickFlag [Default = 0]: ");
        csw_gets (Ans);
        ThickFlag = 0;
        if (Ans[0] != '\0') {
            sscanf (Ans, "%d", &ThickFlag);
        }
        if (ThickFlag != 1) ThickFlag = 0;

        printf ("Enter Number of Data Busts [Default = 0]: ");
        csw_gets (Ans);
        NumBusts = 0;
        if (Ans[0] != '\0') {
            sscanf (Ans, "%d", &NumBusts);
        }

        BustSign = 0;
        if (NumBusts < 0) {
            NumBusts = -NumBusts;
            BustSign = 1;
        }

        printf ("Enter log base for z data [default = linear]: ");
        csw_gets (Ans);
        LogBase = 0;
        if (Ans[0] != '\0') {
            sscanf (Ans, "%f", &LogBase);
        }
        if (LogBase < 1.1) LogBase = 0.0;

        GenerateXYData ();
        GenerateZData (ztype);

        WriteData (outfile, comment);

        fclose (outfile);

    }  /*  end of loop through files  */

    exit (0);

    return 1;

}



    

static int GenerateXYData ()

{
    int                i, j, n, ido, npts;
    CSW_Float          dx, dy;
    CSW_Float          x1, y1, x2, y2, xt, yt;

    x2 = Xmax - Xmin;
    y2 = Ymax - Ymin;

/*
    Regular grid of data.
*/
    if (GridXY == 1) {
        n = 0;
        for (i=0; i<Nxyrow; i++) {
            y1 = (CSW_F)i;
            for (j=0; j<Nxycol; j++) {
                Y[n] = y1;
                X[n] = (CSW_F)j;
                n++;
            }
        }
        Ndata = Nxyrow * Nxycol;
        return 1;
    }

/*
    evenly scattered data
*/
    for (i=0; i<Neven; i++) {
        X[i] = csw_FloatRandom (&Iseed);
        Y[i] = csw_FloatRandom (&Iseed);
        X[i] *= x2;
        Y[i] *= y2;
        X[i] += Xmin;
        Y[i] += Ymin;
    }

    Ndata = Neven;

/*
    clustered data
*/
    if (NumCluster > 0) {

        for (ido=0; ido<NumCluster; ido++) {
            x1 = ClustLoc[ido].x1;
            y1 = ClustLoc[ido].y1;
            x2 = ClustLoc[ido].x2;
            y2 = ClustLoc[ido].y2;

            npts = ClustPts[ido];
            if (npts < 1) npts = 1;
            for (i=0; i<npts; i++) {
                xt = csw_FloatRandom (&Iseed);
                yt = csw_FloatRandom (&Iseed);
                xt *= x2;
                yt *= y2;
                X[Ndata] = xt + x1;
                Y[Ndata] = yt + y1;
                Ndata++;
            }
        }
    }

/*
    profile data
*/
    if (NumProfile > 0) {
        for (ido = 0; ido <NumProfile; ido++) {
            x1 = ProfLoc[ido].x1;
            y1 = ProfLoc[ido].y1;
            x2 = ProfLoc[ido].x2;
            y2 = ProfLoc[ido].y2;

            npts = ProfPts[ido];
            if (npts < 2) npts = 2;
            dx = (x2 - x1) / (CSW_Float)npts;
            dy = (y2 - y1) / (CSW_Float)npts;

            for (i=0; i<npts; i++) {
                X[Ndata] = x1 + dx * i;
                Y[Ndata] = y1 + dy * i;
                Ndata++;
            }
            if (IlineFlag == 1) {
                X[Ndata] = 1.e30f;
                Y[Ndata] = 1.e30f;
                Ndata++;
            }
                
        }
    }

    return 1;

}



int CreateSourcePoints ()


{
    int           i;
    CSW_Float         zmax;
    CSW_Float         xmin, ymin, xmax, ymax;

    xmin = Xmin;
    ymin = Ymin;
    xmax = Xmax;
    ymax = Ymax;

    zmax = (xmax - xmin + ymax - ymin) / 4.0;
    Iseed = 123457;

    for (i=0; i<MAX_SOURCE; i++) {
        Xsource[i] = csw_FloatRandom (&Iseed) * (xmax - xmin) + xmin;
        Ysource[i] = csw_FloatRandom (&Iseed) * (ymax - ymin) + ymin;
        Zsource[i] = csw_FloatRandom (&Iseed) * zmax + zmax / 6.0;
        SourceStrength[i] = (csw_FloatRandom (&Iseed) - 0.5);
        if (SourceStrength[i] < 0.0) SourceStrength[i] -= 0.5;
        if (SourceStrength[i] >= 0.0) SourceStrength[i] += 0.5;
        SourceStrength[i] *= zmax;

        if (i%10 == 9) Zsource[i] /= 2.0;
        if (Zsource[i] < zmax / 8.0) Zsource[i] = zmax / 8.0;
    }

    return 1;

}





int PointInverseSquare ()

{
    int         i, j, k, npts, aflag, ndo;
    CSW_Float   *x, *y, *z;
    CSW_Float   xt, yt, sum, dt, xt2, yt2, zt2, xt3, yt3;
    CSW_Float   zmin, zmax, ang, 
                str, adiv, dx, dy, x1, y1, x2, y2, sang, cang;

    x = X;
    y = Y;
    z = Z;
    npts = Ndata;

    cang = 1.0;
    sang = 1.0;
    adiv = 1.0;
    ndo = 1;

    if (AnisoAngle >= -180.0  &&  AnisoAngle <= 180.0) {
        ang = AnisoAngle * .01745329;
        sang = (CSW_Float)sin ((double)ang);
        cang = (CSW_Float)cos ((double)ang);
        adiv = 10.0 * AnisoExcen;
        ndo = adiv + 1.1;
        aflag = 1;
    }
    else {
        aflag = 0;
    }

    zmax = -1.e30;
    zmin = 1.e30;
    for (i=0; i<npts; i++) {
        if (x[i] > 1.e10) {
            z[i] = 1.e30;
            continue;
        }
        xt = x[i];
        yt = y[i];
        sum = 0.0;

        for (j=0; j<MAX_SOURCE; j++) {
            xt2 = Xsource[j] - xt;
            yt2 = Ysource[j] - yt;
            zt2 = Zsource[j];
            if (aflag) {
                x1 = Xsource[j] + zt2 * AnisoExcen * cang;
                y1 = Ysource[j] + zt2 * AnisoExcen * sang;
                x2 = Xsource[j] - zt2 * AnisoExcen * cang;
                y2 = Ysource[j] - zt2 * AnisoExcen * sang;
                dx = (x2 - x1) / adiv;
                dy = (y2 - y1) / adiv;
                str = SourceStrength[j] / adiv;
                for (k=0; k<ndo; k++) {
                    xt3 = x1 + k * dx - xt;
                    yt3 = y1 + k * dy - yt;
                    dt = xt3 * xt3 + yt3 * yt3 + zt2 * zt2;
                    if (dt < 0.001) dt = 0.001;
                    dt = 1.0 / dt;
                    dt *= str;
                    sum += dt;
                }
            }

            else {
                dt = xt2 * xt2 + yt2 * yt2 + zt2 * zt2;
                if (dt < 0.001) dt = 0.001;
                dt = 1.0 / dt;
                dt *= SourceStrength[j];
                sum += dt;
            }
        }

        z[i] = sum * 100.;
        if (z[i] < zmin) zmin = z[i];
        if (z[i] > zmax) zmax = z[i];

    }

    return 1;

}



int ConcentricCircles ()


{
    CSW_Float      *x, *y, *z;
    int            npts;
    CSW_Float      xmin, ymin, xmax, ymax;
    int            i;
    CSW_Float      x0, y0, xt, yt, dt, r1, r2, r0, dr;
    CSW_Float      xw, yw;
    double         ang;

    x = X;
    y = Y;
    z = Z;
    npts = Ndata;
    xmin = Xmin;
    ymin = Ymin;
    xmax = Xmax;
    ymax = Ymax;

    xw = xmax - xmin;
    yw = ymax - ymin;

    x0 = (xmin + xmax) / 2.0;
    y0 = (ymin + ymax) / 2.0;
    r2 = x0;
    if (y0 < x0) r2 = y0;
    r2 /= 2.0;
    r0 = r2;
    r2 *= r2;
    dr = r2;

    if (HemisphereFlag == 1) {
        for (i=0; i<npts; i++) {
            while (1) {
                x[i] = csw_FloatRandom (&Iseed) + .05;
                if (x[i] > .999) x[i] = .999;
                x[i] *= xmax / 2.0;
                x[i] += xmax / 4.0;
                y[i] = csw_FloatRandom (&Iseed) + .05;
                if (y[i] > .999) y[i] = .999;
                y[i] *= ymax / 2.0;
                y[i] += ymax / 4.0;
                xt = x0 - x[i];
                yt = y0 - y[i];
                dt = xt * xt + yt * yt;
                if (dt <= dr) break;
            }
            r1 = (CSW_Float)sqrt ((double)dt);
            r1 *= r1;
            dt = r2 - r1;
            if (dt < 0.0) dt = 0.0;
            z[i] = (CSW_Float)sqrt ((double)dt);
        }
    }

    else {
        for (i=0; i<npts; i++) {
            x[i] = csw_FloatRandom (&Iseed) * xw + xmin;
            y[i] = csw_FloatRandom (&Iseed) * yw + ymin;
            xt = x[i] - x0;
            yt = y[i] - y0;
            ang = xt * xt + yt * yt;
            z[i] = (CSW_Float)sqrt (ang);
        }
    }

    return 1;

}





int GenerateZData (ztype)

    int          ztype;

{
    CSW_Float        coef[50];
    int          i;

    if (IlineFlag == 1) {
        ztype = 4;
    }

/*
    polynomial trend surface
*/
    if (ztype == 1) {
        for (i=0; i<50; i++) {
            coef[i] = csw_FloatRandom (&Iseed);
            coef[i] -= 0.5f;
            coef[i] *= 0.1f;
        }

        grd_EvalTrendSurface (X, Y, Z, Ndata, TrendOrder, coef);
        AddAnomalies (X, Y, Z, Ndata);
    }

/*
    concentric circle radii for z
*/
    else if (ztype == 2) {
        HemisphereFlag = 0;
        ConcentricCircles ();
    }

/*
    hemisphere model
*/
    else if (ztype == 3) {
        HemisphereFlag = 1;
        ConcentricCircles ();
    }

/*
    Inverse square field
*/
    else if (ztype == 4) {
        CreateSourcePoints ();
        PointInverseSquare ();
    }

    return 1;

}





int WriteData (fptr, comment)

    FILE      *fptr;
    char      *comment;

{
    int       i, n;
    CSW_Float zt, zmin, zmax, zn, zb, zf, pct,
              yt1, yt2, zt2, zt4, zavg, tiny;
    char      obuf[200];

    comment = comment;

/*
    find z range
*/
    zmin = 1.e30;
    zmax = -1.e30;
    zavg = 0.0;
    for (i=0; i<Ndata; i++) {
        if (Z[i] < zmin) zmin = Z[i];
        if (Z[i] > zmax) zmax = Z[i];
        zavg += Z[i];
    }

    zn = 11.0 - ThickFlag;
    zt = zmax - zmin;
    tiny = zt / 100.0f;
    zt2 = zt / 4.;

    zavg /= Ndata;
    zt4 = zt / zn;
    zt4 += zavg;

/*
    output numbers reasonable for logarithm data
*/
    if (LogBase > 1.1) {
        for (i=0; i<Ndata; i++) {
            Z[i] -= zmin;
            Z[i] += tiny;
            Z[i] /= zt2;
            Z[i] = (float)(pow ((double)LogBase, (double)Z[i]));
        }
        zt /= zt2;
        zt = (float)(pow ((double)LogBase, (double)zt));
    }

/*
    Noise, data bust and fault stuff based on the z range
*/
    zn = zt *NoiseFact;
    zb = zt / 4.0;
    zf = zt / 2.0;
  
/*
    Put noise into data
*/
    if (NoiseFact > 0.0) {
        for (i=0; i<Ndata; i++) {
            pct = csw_FloatRandom (&Iseed);
            pct -= 0.5;
            Z[i] += NoiseFact * pct * zt;
        }
    }

/*
    Put in data busts.
*/
    if (NumBusts > Ndata) NumBusts = Ndata;

    for (i=0; i<NumBusts; i++) {
        pct = csw_FloatRandom (&Iseed);
        n = Ndata * pct;
        if (BustSign == 0) {
            Z[n] += zb;
        }
        else {
            Z[n] -= zb;
        }
    }

/*
    Fault the surface in 2 places if desired.
*/
    if (FaultFlag == 1) {

        yt1 = Ymax - (Ymax - Ymin) / 2.0;
        yt2 = Ymin - (Ymax - Ymin) / 4.0;

        for (i=0; i<Ndata; i++) {
            if (Y[i] < X[i] + yt2) {
                Z[i] -= zf;
            }
            else if (Y[i] > X[i] + yt1) {
                Z[i] += zf;
            }
        }

    }

/*
    set negatives to zero if ThickFlag = 1
*/
    if (ThickFlag == 1) {
        for (i=0; i<Ndata; i++) {
            Z[i] -= zt4;
            if (Z[i] < 0.0) Z[i] = 0.0;
        }
    }

/*
    write one point per line
*/
    for (i=0; i<Ndata; i++) {
        if (IlineFlag == 1  &&  X[i] > 1.e10) {
            sprintf (obuf, "end\n");
        }
        else {
            if (FlatFlag == 1) {
                sprintf (obuf, "%8.3f  %8.3f  0.0\n", X[i], Y[i]);
            }
            else {
                sprintf (obuf, "%8.3f  %8.3f  %8.3f\n", X[i], Y[i], Z[i]*Zfudge);
            }
        }
        fputs (obuf, fptr);
    }

    return 1;

}





static int GenerateStepData ()

{
    int               i, j, k, ns, nc;
    CSW_F             xt1, yt1, xt, yt, dt, dist;
    CSW_Float         xmin, ymin, xmax, ymax;

    printf ("Enter number of source points [default = 10]: ");
    csw_gets (Ans);
    ns = 10;
    if (Ans[0] != '\0') {
        sscanf (Ans, "%d", &ns);
    }

    printf ("Enter number of classifications [default = 5]: ");
    csw_gets (Ans);
    nc = 5;
    if (Ans[0] != '\0') {
        sscanf (Ans, "%d", &nc);
    }

    xmin = Xmin;
    ymin = Ymin;
    xmax = Xmax;
    ymax = Ymax;

    for (i=0; i<ns; i++) {
        Xsource[i] = csw_FloatRandom (&Iseed) * (xmax - xmin) + xmin;
        Ysource[i] = csw_FloatRandom (&Iseed) * (ymax - ymin) + ymin;
    }

    for (j=0; j<Ndata; j++) {

        dist = 1.e30;
        xt1 = X[j];
        yt1 = Y[j];

        for (k=0; k<ns; k++) {

            xt = xt1 - Xsource[k];
            yt = yt1 - Ysource[k];
            dt = xt * xt + yt * yt;
            if (dt < dist) {
                dist = dt;
                Z[j] = (CSW_F)((k % nc) + 1);
            }

        }

    }

    return 1;

}




static int AddAnomalies (x, y, z, ndata)

    CSW_F         *x, *y, *z;
    int           ndata;

{
    int           i, j, isign;
    CSW_F         xanom[100], yanom[100], zanom[100];
    CSW_F         dx, dy, dz, dt, xt, yt, zt,
                  zadd, zmin, zmax, dmax, x0, y0;

    if (NumAnomaly == 0)
        return 1;

    isign = 1;
    if (NumAnomaly < 0) {
        NumAnomaly = -NumAnomaly;
        isign = -1;
    }

    if (NumAnomaly > 100)
        NumAnomaly = 100;

    dx = Xmax - Xmin;
    dy = Ymax - Ymin;
    x0 = Xmin + dx * .15f;
    y0 = Ymin + dy * .15f;
    dx *= .7f;
    dy *= .7f;
    dmax = (dx + dy) / 20.0f;
    dmax *= dmax;

    zmin = 1.e30;
    zmax = -1.e30;
    for (j=0; j<ndata; j++) {
        if (z[j] < zmin) zmin = z[j];
        if (z[j] > zmax) zmax = z[j];
    }
    dz = zmax - zmin;
    dz /= 2.0f;

    for (i=0; i<NumAnomaly; i++) {
        xt = csw_FloatRandom (&Iseed);
        yt = csw_FloatRandom (&Iseed);
        zt = csw_FloatRandom (&Iseed) + 0.5f;
        xt *= dx;
        yt *= dy;
        xanom[i] = xt + x0;
        yanom[i] = yt + y0;
        zanom[i] = zt * dz;
    }

    for (j=0; j<ndata; j++) {

        xt = x[j];
        yt = y[j];
        
        zadd = 0.0f;
        for (i=0; i<NumAnomaly; i++) {

            dx = xanom[i] - xt;
            dx *= dx;
            dy = yanom[i] - yt;
            dy *= dy;

            dt = dx + dy;            
            if (dt >= dmax)
                continue;
            zt = zanom[i] * (dmax - dt) / dmax;
            zadd += zt;
        }

        zadd *= isign;

        z[j] += zadd;

    }

    return 1;

}
