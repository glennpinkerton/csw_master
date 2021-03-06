
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_hersh6.h

      Definition of vectors for the Hershey font number 6.
    This is only used in the gpf_font.c file.  This header file
    depends on some constant and structure definitions in the 
    gpf_font.c file.

*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_hersh6.h.
#endif


/*
    add nothing above this ifndef
*/

#ifndef GPF_HERSH6_H
#define GPF_HERSH6_H

#  include "csw/utils/include/csw_.h"

/*
    character width data for hershey font number 6
*/
const FONTINT      Hershey6Width[] = {

     -1,   -1,   -1,   80,  120,  190,  180,  190,  230,   30,
    130,  130,  110,  190,   40,  190,   30,  270,  160,   90,
    180,  170,  170,  180,  160,  160,  170,  160,   60,   70,
    190,  190,  200,  140,  220,  190,  220,  180,  210,  220,
    220,  180,  270,  140,  180,  260,  180,  280,  260,  170,
    230,  170,  220,  200,  180,  220,  190,  230,  250,  200,
    210,  130,  150,  130,  170,  280,   30,  180,  130,  130,
    180,  130,  190,  170,  190,  120,  140,  160,   80,  320,
    220,  130,  200,  150,  160,  140,  100,  220,  170,  260,
    170,  190,  150,  100,   70,  110,  190,   90,   -1,   -1

    };

/*
    character list for hershey font number 6
*/
const VFontchar    Hershey6List[] = {

      {   -1,    0, ' '},
      {   -1,    0, ' '},
      {   -1,    0, ' '},
      {    0,   26, '!'},
      {   26,   32, '"'},
      {   58,   16, '#'},
      {   74,   74, '$'},
      {  148,   58, '%'},
      {  206,  104, '&'},
      {  310,   14, '\''},
      {  324,   36, '('},
      {  360,   36, ')'},
      {  396,   12, '*'},
      {  408,    8, '+'},
      {  416,   14, ','},
      {  430,    4, '-'},
      {  434,   10, '.'},
      {  444,    4, '/'},
      {  448,   78, '0'},
      {  526,   22, '1'},
      {  548,   76, '2'},
      {  624,   92, '3'},
      {  716,   14, '4'},
      {  730,   68, '5'},
      {  798,   86, '6'},
      {  884,   50, '7'},
      {  934,  114, '8'},
      { 1048,   86, '9'},
      { 1134,   18, ':'},
      { 1152,   24, ';'},
      { 1176,    6, '<'},
      { 1182,    8, '='},
      { 1190,    6, '>'},
      { 1196,   62, '?'},
      { 1258,  104, '@'},
      { 1362,   24, 'A'},
      { 1386,   70, 'B'},
      { 1456,   64, 'C'},
      { 1520,   52, 'D'},
      { 1572,   32, 'E'},
      { 1604,   28, 'F'},
      { 1632,   76, 'G'},
      { 1708,   36, 'H'},
      { 1744,   16, 'I'},
      { 1760,   36, 'J'},
      { 1796,   36, 'K'},
      { 1832,   20, 'L'},
      { 1852,   40, 'M'},
      { 1892,   28, 'N'},
      { 1920,   78, 'O'},
      { 1998,   44, 'P'},
      { 2042,  112, 'Q'},
      { 2154,   70, 'R'},
      { 2224,   66, 'S'},
      { 2290,   24, 'T'},
      { 2314,   42, 'U'},
      { 2356,   20, 'V'},
      { 2376,   32, 'W'},
      { 2408,   28, 'X'},
      { 2436,   28, 'Y'},
      { 2464,   24, 'Z'},
      { 2488,   16, '['},
      { 2504,    4, '\\'},
      { 2508,   16, ']'},
      { 2524,    6, '^'},
      { 2530,    4, '_'},
      { 2534,   14, '`'},
      { 2548,   70, 'a'},
      { 2618,   62, 'b'},
      { 2680,   46, 'c'},
      { 2726,   74, 'd'},
      { 2800,   48, 'e'},
      { 2848,   64, 'f'},
      { 2912,   78, 'g'},
      { 2990,   52, 'h'},
      { 3042,   46, 'i'},
      { 3088,   58, 'j'},
      { 3146,   56, 'k'},
      { 3202,   30, 'l'},
      { 3232,   92, 'm'},
      { 3324,   66, 'n'},
      { 3390,   58, 'o'},
      { 3448,   74, 'p'},
      { 3522,   56, 'q'},
      { 3578,   46, 'r'},
      { 3624,   52, 's'},
      { 3676,   30, 't'},
      { 3706,   66, 'u'},
      { 3772,   48, 'v'},
      { 3820,   74, 'w'},
      { 3894,   76, 'x'},
      { 3970,   74, 'y'},
      { 4044,   52, 'z'},
      { 4096,   68, '{'},
      { 4164,    4, '|'},
      { 4168,   68, '}'},
      { 4236,   44, '~'},
      { 4280,   26, ''},
      {   -1,    0, ' '},
      {   -1,    0, ' '}
      };

/*
    vector strokes for hershey font number 6
*/
const FONTINT   Hershey6Vector[] = {

/*  character '!'  */

    -70,310,  60,300,  40,180, 
    -70,300,  40,180, 
    -70,310,  80,300,  40,180, 
    -20,120,  10,110,  20,100,  30,110,  20,120, 

/*  character '"'  */

    -20,310,  10,300,  10,240, 
    -20,300,  10,240, 
    -20,310,  30,300,  10,240, 
    -110,310, 100,300, 100,240, 
    -110,300, 100,240, 
    -110,310, 120,300, 100,240, 

/*  character '#'  */

    -130,310,  10, 30, 
    -190,310,  70, 30, 
    -40,200, 180,200, 
    -10,140, 150,140, 

/*  character '$'  */

    -110,350,  30, 60, 
    -160,350,  80, 60, 
    -170,270, 160,260, 170,250, 180,260, 180,270, 170,290, 160,300, 130,310, 
     90,310,  60,300,  40,280,  40,260,  50,240,  60,230, 130,190, 150,170, 
    -40,260,  60,240, 130,200, 140,190, 150,170, 150,140, 140,120, 130,110, 
    100,100,  60,100,  30,110,  20,120,  10,140,  10,150,  20,160,  30,150, 
     20,140, 

/*  character '%'  */

    -190,310,  10,100, 
    -60,310,  80,290,  80,270,  70,250,  50,240,  30,240,  10,260,  10,280, 
     20,300,  40,310,  60,310,  80,300, 110,290, 140,290, 170,300, 190,310, 
    -150,170, 130,160, 120,140, 120,120, 140,100, 160,100, 180,110, 190,130, 
    190,150, 170,170, 150,170, 

/*  character '&'  */

    -220,230, 210,220, 220,210, 230,220, 230,230, 220,240, 210,240, 190,230, 
    170,210, 120,130, 100,110,  80,100,  50,100,  20,110,  10,130,  10,150, 
     20,170,  30,180,  50,190, 100,210, 120,220, 140,240, 150,260, 150,280, 
    140,300, 120,310, 100,300,  90,280,  90,250, 100,190, 110,160, 130,130, 
    150,110, 170,100, 190,100, 200,120, 200,130, 
    -50,100,  30,110,  20,130,  20,150,  30,170,  40,180, 100,210, 
    -90,250, 100,200, 110,170, 130,140, 150,120, 170,110, 190,110, 200,120, 

/*  character '''  */

    -20,290,  10,300,  20,310,  30,300,  30,280,  20,260,  10,250, 

/*  character '('  */

    -130,350,  90,320,  60,290,  40,260,  20,220,  10,170,  10,130,  20, 80, 
     30, 50,  40, 30, 
    -90,320,  60,280,  40,240,  30,210,  20,160,  20,110,  30, 60,  40, 30, 

/*  character ')'  */

    -100,350, 110,330, 120,300, 130,250, 130,210, 120,160, 100,120,  80, 90, 
     50, 60,  10, 30, 
    -100,350, 110,320, 120,270, 120,220, 110,170, 100,140,  80,100,  50, 60, 

/*  character '*'  */

    -60,310,  60,190, 
    -10,280, 110,220, 
    -110,280,  10,220, 

/*  character '+'  */

    -100,280, 100,100, 
    -10,190, 190,190, 

/*  character ','  */

    -30,100,  20,110,  30,120,  40,110,  40,100,  30, 80,  10, 60, 

/*  character '-'  */

    -10,190, 190,190, 

/*  character '.'  */

    -20,120,  10,110,  20,100,  30,110,  20,120, 

/*  character '/'  */

    -270,350,  10, 30, 

/*  character '0'  */

    -100,310,  70,300,  50,280,  30,250,  20,220,  10,180,  10,150,  20,120, 
     30,110,  50,100,  70,100, 100,110, 120,130, 140,160, 150,190, 160,230, 
    160,260, 150,290, 140,300, 120,310, 100,310, 
    -100,310,  80,300,  60,280,  40,250,  30,220,  20,180,  20,150,  30,120, 
     50,100, 
    -70,100,  90,110, 110,130, 130,160, 140,190, 150,230, 150,260, 140,290, 
    120,310, 

/*  character '1'  */

    -70,270,  20,100, 
    -90,310,  30,100, 
    -90,310,  60,280,  30,260,  10,250, 
    -80,280,  40,260,  10,250, 

/*  character '2'  */

    -70,270,  80,260,  70,250,  60,260,  60,270,  70,290,  80,300, 110,310, 
    140,310, 170,300, 180,280, 180,260, 170,240, 150,220, 120,200,  80,180, 
     50,160,  30,140,  10,100, 
    -140,310, 160,300, 170,280, 170,260, 160,240, 140,220,  80,180, 
    -20,120,  30,130,  50,130, 100,110, 130,110, 150,120, 160,140, 
    -50,130, 100,100, 130,100, 150,110, 160,140, 

/*  character '3'  */

    -60,270,  70,260,  60,250,  50,260,  50,270,  60,290,  70,300, 100,310, 
    130,310, 160,300, 170,280, 170,260, 160,240, 130,220, 100,210, 
    -130,310, 150,300, 160,280, 160,260, 150,240, 130,220, 
    -80,210, 100,210, 130,200, 140,190, 150,170, 150,140, 140,120, 130,110, 
    100,100,  60,100,  30,110,  20,120,  10,140,  10,150,  20,160,  30,150, 
     20,140, 
    -100,210, 120,200, 130,190, 140,170, 140,140, 130,120, 120,110, 100,100, 

/*  character '4'  */

    -150,300,  90,100, 
    -160,310, 100,100, 
    -160,310,  10,160, 170,160, 

/*  character '5'  */

    -80,310,  30,210, 
    -80,310, 180,310, 
    -80,300, 130,300, 180,310, 
    -30,210,  40,220,  70,230, 100,230, 130,220, 140,210, 150,190, 150,160, 
    140,130, 120,110,  90,100,  60,100,  30,110,  20,120,  10,140,  10,150, 
     20,160,  30,150,  20,140, 
    -100,230, 120,220, 130,210, 140,190, 140,160, 130,130, 110,110,  90,100, 

/*  character '6'  */

    -150,280, 140,270, 150,260, 160,270, 160,280, 150,300, 130,310, 100,310, 
     70,300,  50,280,  30,250,  20,220,  10,180,  10,140,  20,120,  30,110, 
     50,100,  80,100, 110,110, 130,130, 140,150, 140,180, 130,200, 120,210, 
    100,220,  70,220,  50,210,  30,190,  20,170, 
    -100,310,  80,300,  60,280,  40,250,  30,220,  20,180,  20,130,  30,110, 
    -80,100, 100,110, 120,130, 130,150, 130,190, 120,210, 

/*  character '7'  */

    -30,310,  10,250, 
    -160,310, 150,280, 130,250,  80,190,  60,160,  50,140,  40,100, 
    -130,250,  70,190,  50,160,  40,140,  30,100, 
    -20,280,  50,310,  70,310, 120,280, 
    -30,290,  50,300,  70,300, 120,280, 140,280, 150,290, 160,310, 

/*  character '8'  */

    -100,310,  70,300,  60,290,  50,270,  50,240,  60,220,  80,210, 110,210, 
    150,220, 160,230, 170,250, 170,280, 160,300, 130,310, 100,310, 
    -100,310,  80,300,  70,290,  60,270,  60,240,  70,220,  80,210, 
    -110,210, 140,220, 150,230, 160,250, 160,280, 150,300, 130,310, 
    -80,210,  40,200,  20,180,  10,160,  10,130,  20,110,  50,100,  90,100, 
    130,110, 140,120, 150,140, 150,170, 140,190, 130,200, 110,210, 
    -80,210,  50,200,  30,180,  20,160,  20,130,  30,110,  50,100, 
    -90,100, 120,110, 130,120, 140,140, 140,180, 130,200, 

/*  character '9'  */

    -150,240, 140,220, 120,200, 100,190,  70,190,  50,200,  40,210,  30,230, 
     30,260,  40,280,  60,300,  90,310, 120,310, 140,300, 150,290, 160,270, 
    160,230, 150,190, 140,160, 120,130, 100,110,  70,100,  40,100,  20,110, 
     10,130,  10,140,  20,150,  30,140,  20,130, 
    -50,200,  40,220,  40,260,  50,280,  70,300,  90,310, 
    -140,300, 150,280, 150,230, 140,190, 130,160, 110,130,  90,110,  70,100, 

/*  character ':'  */

    -50,240,  40,230,  50,220,  60,230,  50,240, 
    -20,120,  10,110,  20,100,  30,110, 

/*  character ';'  */

    -60,240,  50,230,  60,220,  70,230,  60,240, 
    -30,100,  20,110,  30,120,  40,110,  40,100,  30, 80,  10, 60, 

/*  character '<'  */

    -190,280,  10,200, 120,100, 

/*  character '='  */

    -10,220, 190,220, 
    -10,160, 190,160, 

/*  character '>'  */

    -60,280, 200,200,  10,100, 

/*  character '?'  */

    -20,270,  30,260,  20,250,  10,260,  10,270,  20,290,  30,300,  60,310, 
    100,310, 130,300, 140,280, 140,260, 130,240, 120,230,  60,210,  40,200, 
     40,180,  50,170,  70,170, 
    -100,310, 120,300, 130,280, 130,260, 120,240, 110,230,  90,220, 
    -30,120,  20,110,  30,100,  40,110,  30,120, 

/*  character '@'  */

    -160,230, 150,250, 130,260, 100,260,  80,250,  70,240,  60,210,  60,180, 
     70,160,  90,150, 120,150, 140,160, 150,180, 
    -100,260,  80,240,  70,210,  70,180,  80,160,  90,150, 
    -160,260, 150,180, 150,160, 170,150, 190,150, 210,170, 220,200, 220,220, 
    210,250, 200,270, 180,290, 160,300, 130,310, 100,310,  70,300,  50,290, 
     30,270,  20,250,  10,220,  10,190,  20,160,  30,140,  50,120,  70,110, 
    100,100, 130,100, 160,110, 180,120, 190,130, 
    -170,260, 160,180, 160,160, 170,150, 

/*  character 'A'  */

    -160,310,  30,100, 
    -160,310, 170,100, 
    -150,290, 160,100, 
    -70,160, 160,160, 
    -10,100,  70,100, 
    -130,100, 190,100, 

/*  character 'B'  */

    -100,310,  40,100, 
    -110,310,  50,100, 
    -70,310, 180,310, 210,300, 220,280, 220,260, 210,230, 200,220, 170,210, 
    -180,310, 200,300, 210,280, 210,260, 200,230, 190,220, 170,210, 
    -80,210, 170,210, 190,200, 200,180, 200,160, 190,130, 170,110, 130,100, 
     10,100, 
    -170,210, 180,200, 190,180, 190,160, 180,130, 160,110, 130,100, 

/*  character 'C'  */

    -160,290, 170,290, 180,310, 170,250, 170,270, 160,290, 150,300, 130,310, 
    100,310,  70,300,  50,280,  30,250,  20,220,  10,180,  10,150,  20,120, 
     30,110,  60,100,  90,100, 110,110, 130,130, 140,150, 
    -100,310,  80,300,  60,280,  40,250,  30,220,  20,180,  20,150,  30,120, 
     40,110,  60,100, 

/*  character 'D'  */

    -100,310,  40,100, 
    -110,310,  50,100, 
    -70,310, 160,310, 190,300, 200,290, 210,260, 210,220, 200,180, 180,140, 
    160,120, 140,110, 100,100,  10,100, 
    -160,310, 180,300, 190,290, 200,260, 200,220, 190,180, 170,140, 150,120, 
    130,110, 100,100, 

/*  character 'E'  */

    -100,310,  40,100, 
    -110,310,  50,100, 
    -150,250, 130,170, 
    -70,310, 220,310, 210,250, 210,310, 
    -80,210, 140,210, 
    -10,100, 160,100, 180,150, 150,100, 

/*  character 'F'  */

    -100,310,  40,100, 
    -110,310,  50,100, 
    -150,250, 130,170, 
    -70,310, 220,310, 210,250, 210,310, 
    -80,210, 140,210, 
    -10,100,  80,100, 

/*  character 'G'  */

    -160,290, 170,290, 180,310, 170,250, 170,270, 160,290, 150,300, 130,310, 
    100,310,  70,300,  50,280,  30,250,  20,220,  10,180,  10,150,  20,120, 
     30,110,  60,100,  80,100, 110,110, 130,130, 150,170, 
    -100,310,  80,300,  60,280,  40,250,  30,220,  20,180,  20,150,  30,120, 
     40,110,  60,100, 
    -80,100, 100,110, 120,130, 140,170, 
    -110,170, 180,170, 

/*  character 'H'  */

    -100,310,  40,100, 
    -110,310,  50,100, 
    -230,310, 170,100, 
    -240,310, 180,100, 
    -70,310, 140,310, 
    -200,310, 270,310, 
    -80,210, 200,210, 
    -10,100,  80,100, 
    -140,100, 210,100, 

/*  character 'I'  */

    -100,310,  40,100, 
    -110,310,  50,100, 
    -70,310, 140,310, 
    -10,100,  80,100, 

/*  character 'J'  */

    -150,310, 100,140,  90,120,  80,110,  60,100,  40,100,  20,110,  10,130, 
     10,150,  20,160,  30,150,  20,140, 
    -140,310,  90,140,  80,120,  60,100, 
    -110,310, 180,310, 

/*  character 'K'  */

    -100,310,  40,100, 
    -110,310,  50,100, 
    -240,310,  70,180, 
    -140,220, 180,100, 
    -130,220, 170,100, 
    -70,310, 140,310, 
    -200,310, 260,310, 
    -10,100,  80,100, 
    -140,100, 200,100, 

/*  character 'L'  */

    -100,310,  40,100, 
    -110,310,  50,100, 
    -70,310, 140,310, 
    -10,100, 160,100, 180,160, 150,100, 

/*  character 'M'  */

    -100,310,  40,100, 
    -100,310, 110,100, 
    -110,310, 120,120, 
    -240,310, 110,100, 
    -240,310, 180,100, 
    -250,310, 190,100, 
    -70,310, 110,310, 
    -240,310, 280,310, 
    -10,100,  70,100, 
    -150,100, 220,100, 

/*  character 'N'  */

    -100,310,  40,100, 
    -100,310, 170,130, 
    -100,280, 170,100, 
    -230,310, 170,100, 
    -70,310, 100,310, 
    -200,310, 260,310, 
    -10,100,  70,100, 

/*  character 'O'  */

    -100,310,  70,300,  50,280,  30,250,  20,220,  10,180,  10,150,  20,120, 
     30,110,  50,100,  80,100, 110,110, 130,130, 150,160, 160,190, 170,230, 
    170,260, 160,290, 150,300, 130,310, 100,310, 
    -100,310,  80,300,  60,280,  40,250,  30,220,  20,180,  20,150,  30,120, 
     50,100, 
    -80,100, 100,110, 120,130, 140,160, 150,190, 160,230, 160,260, 150,290, 
    130,310, 

/*  character 'P'  */

    -100,310,  40,100, 
    -110,310,  50,100, 
    -70,310, 190,310, 220,300, 230,280, 230,260, 220,230, 200,210, 160,200, 
     80,200, 
    -190,310, 210,300, 220,280, 220,260, 210,230, 190,210, 160,200, 
    -10,100,  80,100, 

/*  character 'Q'  */

    -100,310,  70,300,  50,280,  30,250,  20,220,  10,180,  10,150,  20,120, 
     30,110,  50,100,  80,100, 110,110, 130,130, 150,160, 160,190, 170,230, 
    170,260, 160,290, 150,300, 130,310, 100,310, 
    -100,310,  80,300,  60,280,  40,250,  30,220,  20,180,  20,150,  30,120, 
     50,100, 
    -80,100, 100,110, 120,130, 140,160, 150,190, 160,230, 160,260, 150,290, 
    130,310, 
    -30,120,  30,130,  40,150,  60,160,  70,160,  90,150, 100,130, 100, 60, 
    110, 50, 130, 50, 140, 70, 140, 80, 
    -100,130, 110, 70, 120, 60, 130, 60, 140, 70, 

/*  character 'R'  */

    -100,310,  40,100, 
    -110,310,  50,100, 
    -70,310, 180,310, 210,300, 220,280, 220,260, 210,230, 200,220, 170,210, 
     80,210, 
    -180,310, 200,300, 210,280, 210,260, 200,230, 190,220, 170,210, 
    -130,210, 150,200, 160,190, 170,110, 180,100, 200,100, 210,120, 210,130, 
    -160,190, 180,120, 190,110, 200,110, 210,120, 
    -10,100,  80,100, 

/*  character 'S'  */

    -180,290, 190,290, 200,310, 190,250, 190,270, 180,290, 170,300, 140,310, 
    100,310,  70,300,  50,280,  50,260,  60,240,  70,230, 140,190, 160,170, 
    -50,260,  70,240, 140,200, 150,190, 160,170, 160,140, 150,120, 140,110, 
    110,100,  70,100,  40,110,  30,120,  20,140,  20,160,  10,100,  20,120, 
     30,120, 

/*  character 'T'  */

    -100,310,  40,100, 
    -110,310,  50,100, 
    -40,310,  10,250,  30,310, 180,310, 170,250, 170,310, 
    -10,100,  80,100, 

/*  character 'U'  */

    -50,310,  20,200,  10,160,  10,130,  20,110,  50,100,  90,100, 120,110, 
    140,130, 150,160, 190,310, 
    -60,310,  30,200,  20,160,  20,130,  30,110,  50,100, 
    -20,310,  90,310, 
    -160,310, 220,310, 

/*  character 'V'  */

    -30,310,  40,100, 
    -40,310,  50,120, 
    -170,310,  40,100, 
    -10,310,  70,310, 
    -130,310, 190,310, 

/*  character 'W'  */

    -40,310,  20,100, 
    -50,310,  30,120, 
    -120,310,  20,100, 
    -120,310, 100,100, 
    -130,310, 110,120, 
    -200,310, 100,100, 
    -10,310,  80,310, 
    -170,310, 230,310, 

/*  character 'X'  */

    -90,310, 160,100, 
    -100,310, 170,100, 
    -230,310,  30,100, 
    -70,310, 130,310, 
    -190,310, 250,310, 
    -10,100,  70,100, 
    -130,100, 190,100, 

/*  character 'Y'  */

    -30,310,  70,210,  40,100, 
    -40,310,  80,210,  50,100, 
    -180,310,  80,210, 
    -10,310,  70,310, 
    -140,310, 200,310, 
    -10,100,  80,100, 

/*  character 'Z'  */

    -200,310,  10,100, 
    -210,310,  20,100, 
    -80,310,  50,250,  70,310, 210,310, 
    -10,100, 150,100, 170,160, 140,100, 

/*  character '['  */

    -60,350,  10, 30, 
    -70,350,  20, 30, 
    -60,350, 130,350, 
    -20, 30,  90, 30, 

/*  character '\'  */

    -10,350, 150, 70, 

/*  character ']'  */

    -120,350,  70, 30, 
    -130,350,  80, 30, 
    -60,350, 130,350, 
    -10, 30,  80, 30, 

/*  character '^'  */

    -10,190, 130,330, 170,190, 

/*  character '_'  */

    -10, 40, 280, 40, 

/*  character '`'  */

    -30,310,  20,300,  10,280,  10,260,  20,250,  30,260,  20,270, 

/*  character 'a'  */

    -140,240, 120,170, 110,130, 110,110, 120,100, 150,100, 170,120, 180,140, 
    -150,240, 130,170, 120,130, 120,110, 130,100, 
    -120,170, 120,200, 110,230,  90,240,  70,240,  40,230,  20,200,  10,170, 
     10,140,  20,120,  30,110,  50,100,  70,100,  90,110, 110,140, 120,170, 
    -70,240,  50,230,  30,200,  20,170,  20,130,  30,110, 

/*  character 'b'  */

    -50,310,  10,180,  10,150,  20,120,  30,110, 
    -60,310,  20,180, 
    -20,180,  30,210,  50,230,  70,240,  90,240, 110,230, 120,220, 130,200, 
    130,170, 120,140, 100,110,  70,100,  50,100,  30,110,  20,140,  20,180, 
    -110,230, 120,210, 120,170, 110,140,  90,110,  70,100, 
    -20,310,  60,310, 

/*  character 'c'  */

    -120,210, 120,200, 130,200, 130,210, 120,230, 100,240,  70,240,  40,230, 
     20,200,  10,170,  10,140,  20,120,  30,110,  50,100,  70,100, 100,110, 
    120,140, 
    -70,240,  50,230,  30,200,  20,170,  20,130,  30,110, 

/*  character 'd'  */

    -160,310, 120,170, 110,130, 110,110, 120,100, 150,100, 170,120, 180,140, 
    -170,310, 130,170, 120,130, 120,110, 130,100, 
    -120,170, 120,200, 110,230,  90,240,  70,240,  40,230,  20,200,  10,170, 
     10,140,  20,120,  30,110,  50,100,  70,100,  90,110, 110,140, 120,170, 
    -70,240,  50,230,  30,200,  20,170,  20,130,  30,110, 
    -130,310, 170,310, 

/*  character 'e'  */

    -20,150,  60,160,  90,170, 120,190, 130,210, 120,230, 100,240,  70,240, 
     40,230,  20,200,  10,170,  10,140,  20,120,  30,110,  50,100,  70,100, 
    100,110, 120,130, 
    -70,240,  50,230,  30,200,  20,170,  20,130,  30,110, 

/*  character 'f'  */

    -180,300, 170,290, 180,280, 190,290, 190,300, 180,310, 160,310, 140,300, 
    130,290, 120,270, 110,240,  80,100,  70, 60,  60, 40, 
    -160,310, 140,290, 130,270, 120,230, 100,140,  90,100,  80, 70,  70, 50, 
     60, 40,  40, 30,  20, 30,  10, 40,  10, 50,  20, 60,  30, 50,  20, 40, 
    -70,240, 170,240, 

/*  character 'g'  */

    -170,240, 130,100, 120, 70, 100, 40,  70, 30,  40, 30,  20, 40,  10, 50, 
     10, 60,  20, 70,  30, 60,  20, 50, 
    -160,240, 120,100, 110, 70,  90, 40,  70, 30, 
    -140,170, 140,200, 130,230, 110,240,  90,240,  60,230,  40,200,  30,170, 
     30,140,  40,120,  50,110,  70,100,  90,100, 110,110, 130,140, 140,170, 
    -90,240,  70,230,  50,200,  40,170,  40,130,  50,110, 

/*  character 'h'  */

    -70,310,  10,100, 
    -80,310,  20,100, 
    -40,170,  60,210,  80,230, 100,240, 120,240, 140,230, 150,220, 150,200, 
    130,140, 130,110, 140,100, 
    -120,240, 140,220, 140,200, 120,140, 120,110, 130,100, 160,100, 180,120, 
    190,140, 
    -40,310,  80,310, 

/*  character 'i'  */

    -90,310,  80,300,  90,290, 100,300,  90,310, 
    -10,200,  20,220,  40,240,  70,240,  80,230,  80,200,  60,140,  60,110, 
     70,100, 
    -60,240,  70,230,  70,200,  50,140,  50,110,  60,100,  90,100, 110,120, 
    120,140, 

/*  character 'j'  */

    -130,310, 120,300, 130,290, 140,300, 130,310, 
    -50,200,  60,220,  80,240, 110,240, 120,230, 120,200,  90,100,  80, 70, 
     70, 50,  60, 40,  40, 30,  20, 30,  10, 40,  10, 50,  20, 60,  30, 50, 
     20, 40, 
    -100,240, 110,230, 110,200,  80,100,  70, 70,  60, 50,  40, 30, 

/*  character 'k'  */

    -70,310,  10,100, 
    -80,310,  20,100, 
    -150,230, 140,220, 150,210, 160,220, 160,230, 150,240, 140,240, 120,230, 
     80,190,  60,180,  40,180, 
    -60,180,  80,170, 100,110, 110,100, 
    -60,180,  70,170,  90,110, 100,100, 120,100, 140,110, 160,140, 
    -40,310,  80,310, 

/*  character 'l'  */

    -60,310,  20,170,  10,130,  10,110,  20,100,  50,100,  70,120,  80,140, 
    -70,310,  30,170,  20,130,  20,110,  30,100, 
    -30,310,  70,310, 

/*  character 'm'  */

     10,200,  20,220,  40,240,  70,240,  80,230,  80,210,  70,170,  50,100, 
    -60,240,  70,230,  70,210,  60,170,  40,100, 
    -70,170,  90,210, 110,230, 130,240, 150,240, 170,230, 180,220, 180,200, 
    150,100, 
    -150,240, 170,220, 170,200, 140,100, 
    -170,170, 190,210, 210,230, 230,240, 250,240, 270,230, 280,220, 280,200, 
    260,140, 260,110, 270,100, 
    -250,240, 270,220, 270,200, 250,140, 250,110, 260,100, 290,100, 310,120, 
    320,140, 

/*  character 'n'  */

    -10,200,  20,220,  40,240,  70,240,  80,230,  80,210,  70,170,  50,100, 
    -60,240,  70,230,  70,210,  60,170,  40,100, 
    -70,170,  90,210, 110,230, 130,240, 150,240, 170,230, 180,220, 180,200, 
    160,140, 160,110, 170,100, 
    -150,240, 170,220, 170,200, 150,140, 150,110, 160,100, 190,100, 210,120, 
    220,140, 

/*  character 'o'  */

    -70,240,  40,230,  20,200,  10,170,  10,140,  20,120,  30,110,  50,100, 
     70,100, 100,110, 120,140, 130,170, 130,200, 120,220, 110,230,  90,240, 
     70,240, 
    -70,240,  50,230,  30,200,  20,170,  20,130,  30,110, 
    -70,100,  90,110, 110,140, 120,170, 120,210, 110,230, 

/*  character 'p'  */

    -30,200,  40,220,  60,240,  90,240, 100,230, 100,210,  90,170,  50, 30, 
    -80,240,  90,230,  90,210,  80,170,  40, 30, 
    -90,170, 100,200, 120,230, 140,240, 160,240, 180,230, 190,220, 200,200, 
    200,170, 190,140, 170,110, 140,100, 120,100, 100,110,  90,140,  90,170, 
    -180,230, 190,210, 190,170, 180,140, 160,110, 140,100, 
    -10, 30,  80, 30, 

/*  character 'q'  */

    -140,240,  80, 30, 
    -150,240,  90, 30, 
    -120,170, 120,200, 110,230,  90,240,  70,240,  40,230,  20,200,  10,170, 
     10,140,  20,120,  30,110,  50,100,  70,100,  90,110, 110,140, 120,170, 
    -70,240,  50,230,  30,200,  20,170,  20,130,  30,110, 
    -50, 30, 120, 30, 

/*  character 'r'  */

    -10,200,  20,220,  40,240,  70,240,  80,230,  80,210,  70,170,  50,100, 
    -60,240,  70,230,  70,210,  60,170,  40,100, 
    -70,170,  90,210, 110,230, 130,240, 150,240, 160,230, 160,220, 150,210, 
    140,220, 150,230, 

/*  character 's'  */

    -130,220, 130,210, 140,210, 140,220, 130,230, 100,240,  70,240,  40,230, 
     30,220,  30,200,  40,190, 110,150, 120,140, 
    -30,210,  40,200, 110,160, 120,150, 120,120, 110,110,  80,100,  50,100, 
     20,110,  10,120,  10,130,  20,130,  20,120, 

/*  character 't'  */

    -70,310,  30,170,  20,130,  20,110,  30,100,  60,100,  80,120,  90,140, 
    -80,310,  40,170,  30,130,  30,110,  40,100, 
    -10,240, 100,240, 

/*  character 'u'  */

    -10,200,  20,220,  40,240,  70,240,  80,230,  80,200,  60,140,  60,120, 
     80,100, 
    -60,240,  70,230,  70,200,  50,140,  50,120,  60,110,  80,100, 100,100, 
    120,110, 140,130, 160,170, 
    -180,240, 160,170, 150,130, 150,110, 160,100, 190,100, 210,120, 220,140, 
    -190,240, 170,170, 160,130, 160,110, 170,100, 

/*  character 'v'  */

    -10,200,  20,220,  40,240,  70,240,  80,230,  80,200,  60,140,  60,120, 
     80,100, 
    -60,240,  70,230,  70,200,  50,140,  50,120,  60,110,  80,100,  90,100, 
    120,110, 140,130, 160,160, 170,200, 170,240, 160,240, 170,220, 

/*  character 'w'  */

    -10,200,  20,220,  40,240,  70,240,  80,230,  80,200,  60,140,  60,120, 
     80,100, 
    -60,240,  70,230,  70,200,  50,140,  50,120,  60,110,  80,100, 100,100, 
    120,110, 140,130, 150,150, 
    -170,240, 150,150, 150,120, 160,110, 180,100, 200,100, 220,110, 240,130, 
    250,150, 260,190, 260,240, 250,240, 260,220, 
    -180,240, 160,150, 160,120, 180,100, 

/*  character 'x'  */

    -20,200,  40,230,  60,240,  90,240, 100,220, 100,190, 
    -80,240,  90,220,  90,190,  80,150,  70,130,  50,110,  30,100,  20,100, 
     10,110,  10,120,  20,130,  30,120,  20,110, 
    -80,150,  80,120,  90,100, 120,100, 140,110, 160,140, 
    -160,230, 150,220, 160,210, 170,220, 170,230, 160,240, 150,240, 130,230, 
    110,210, 100,190,  90,150,  90,120, 100,100, 

/*  character 'y'  */

    -10,200,  20,220,  40,240,  70,240,  80,230,  80,200,  60,140,  60,120, 
     80,100, 
    -60,240,  70,230,  70,200,  50,140,  50,120,  60,110,  80,100, 100,100, 
    120,110, 140,130, 160,170, 
    -190,240, 150,100, 140, 70, 120, 40,  90, 30,  60, 30,  40, 40,  30, 50, 
     30, 60,  40, 70,  50, 60,  40, 50, 
    -180,240, 140,100, 130, 70, 110, 40,  90, 30, 

/*  character 'z'  */

    -150,240, 140,220, 120,200,  40,140,  20,120,  10,100, 
    -20,200,  30,220,  50,240,  80,240, 120,220, 
    -30,220,  50,230,  80,230, 120,220, 140,220, 
    -20,120,  40,120,  80,110, 110,110, 130,120, 
    -40,120,  80,100, 110,100, 130,120, 140,140, 

/*  character '{'  */

    -100,350,  80,340,  60,330,  50,310,  50,290,  60,270,  70,250,  70,230, 
     60,210,  40,200, 
    -80,340,  70,330,  60,310,  60,300,  70,280,  80,260,  80,230,  60,210, 
     20,190,  40,170,  50,150,  50,140,  40,130,  20,120,  10, 90,  10, 60, 
     20, 40,  40, 30, 
    -40,170,  60,150,  60,130,  30,100,  20, 80,  20, 40, 

/*  character '|'  */

    -70,350,  10, 30, 

/*  character '}'  */

    -70,350,  90,340, 110,320, 110,310, 100,290,  80,260,  60,240,  60,230, 
     70,210,  90,190,  70,180,  50,160,  40,140,  40,130,  60, 90,  60, 70, 
     50, 50,  10, 30, 
    -90,340, 100,320, 100,310,  90,290,  60,260,  50,240,  50,230,  90,190, 
     50,170,  30,150,  30,120,  40,100,  50, 80,  50, 70,  40, 50,  30, 40, 

/*  character '~'  */

    -10,160,  10,180,  20,210,  40,220,  60,220,  80,210, 120,180, 140,170, 
    160,170, 180,180, 190,200, 
    -10,180,  20,200,  40,210,  60,210,  80,200, 120,170, 140,160, 160,160, 
    180,170, 190,200, 190,220, 

/*  character ''  */

    -40,310,  20,300,  10,280,  10,260,  20,240,  40,230,  60,230,  80,240, 
     90,260,  90,280,  80,300,  60,310,  40,310

};

/*
    end of hershey font number 6 vector strokes
    add nothing below this endif
*/
#endif 
