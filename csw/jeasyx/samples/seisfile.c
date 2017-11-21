
#include <math.h>
#include <stdio.h>

typedef union {
    char        c1[4];
    short       i2[2];
}  DAta;

float csw_FloatRandom ();


main ()

{
    char        buff1[3200], buff2[400], buff3[240];
    int         i, j, fnum, seed;
    DAta        data;
    float       fdata[1000];
    double      di, omega, dstart;

    fnum = csw_CreateFile ("test.segy", "w");
    
    csw_BinFileWrite (buff1, 3200, 1, fnum);
    
    data.i2[0] = 4000;
    csw_memcpy (buff2+16, data.c1, 2);
    data.i2[0] = 1000;
    csw_memcpy (buff2+20, data.c1, 2);
    data.i2[0] = 1;
    csw_memcpy (buff2+24, data.c1, 2);

    csw_BinFileWrite (buff2, 400, 1, fnum);

    di = 3.14159 / (double)200 * 4.0;
    omega = 6.28319 / 100. * 4.0;
    seed = 12345;

    for (i=0; i<200; i++) {
        dstart = di * i + csw_FloatRandom (&seed) * .25;
        for (j=0; j<1000; j++) {
            fdata[j] = sin (dstart + (double)j * omega);
        }
        csw_BinFileWrite (buff3, 240, 1, fnum);
        csw_BinFileWrite (fdata, 1000 * sizeof(float), 1, fnum);
    }

    csw_CloseFile (fnum);

    return 1;

}  /*  end of static ReadSegyData function  */
