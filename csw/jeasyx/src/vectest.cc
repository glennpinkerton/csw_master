/*
 ***************************************************************************

  This file is used to test the native side of the jeasyx system separately
  from the Java side.  The main simply reads from a command file and calls
  ezx_process_command as if the command came from Java.

  You need to edit the EZXJavaArea.cc file to enable debugging in order to
  run this program.  If you don't the program will either not link or it
  will fail miserably.

  This program reads from standard input, which will almost always be a file
  redirected via the < operator on the command line.

 ***************************************************************************
*/
#include <stdio.h>

#include <csw/jeasyx/private_include/gtx_msgP.h>
#include <csw/utils/include/csw_.h>

#include <vector>

FILE      *dfile = NULL;

int main (int argc, char *argv[])
{
    int              ntimes, itimes, i, j, size;
    int              ioffset, k, i1, i2, j1, j2;

    static double    ddata[1000000];

    std::vector<double>  dvec(1000000);

    argc = argc;
    argv = argv;

    ntimes = 100;
    size = 1000000;

    printf ("\n\nstarting vector loops\n");
    csw_CpuDisp ();

    double sum = 0.0;
    for (j=0; j<size; j++) {
        dvec[j] = (double)j;
    }
    for (itimes=0; itimes<ntimes; itimes++) {
        i1 = 200;
        i2 = 600;
        j1 = 300;
        j2 = 700;
        for (i=i1; i<i2; i++) {
            ioffset = 1000 * i;
            for (j=j1; j<j2; j++) {
                k = ioffset + i;
                sum += dvec[k] / (j - j1);
            }
        }
    }    
    
    printf ("end of vector loops\n");
    csw_CpuDisp ();


    printf ("\n\nstarting array loops\n");
    csw_CpuDisp ();

    for (j=0; j<size; j++) {
        ddata[j] = (double)j;
    }
    for (itimes=0; itimes<ntimes; itimes++) {
        i1 = 200;
        i2 = 600;
        j1 = 300;
        j2 = 700;
        for (i=i1; i<i2; i++) {
            ioffset = 1000 * i;
            for (j=j1; j<j2; j++) {
                k = ioffset + i;
                sum += ddata[k] / (j - j1);
            }
        }
    }    

    sum = sum;
    
    printf ("end of array loops\n");
    csw_CpuDisp ();

    return 1;

}
