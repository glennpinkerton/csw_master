
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_hersh8.h

      Definition of vectors for the Hershey font number 8.
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
#error Illegal attempt to include private header file gpf_hersh8.h.
#endif


/*
    add nothing above this ifndef
*/

#ifndef GPF_HERSH8_H
#define GPF_HERSH8_H

#  include "csw/utils/include/csw_.h"

/*
    character width data for hershey font number 8
*/
const FONTINT      Hershey8Width[] = {

     -1,   -1,   -1,   80,  120,  190,  180,  190,  230,   30,
    130,  130,  110,  190,   30,  190,   30,  150,  160,   90,
    180,  170,  170,  180,  160,  160,  170,  160,   60,   70,
    190,  190,  200,  140,  220,  190,  190,  160,  230,  160,
    210,  200,  240,  140,  130,  240,  190,  320,  230,  180,
    220,  200,  210,  180,  210,  230,  200,  290,  230,  200,
    170,  130,  150,  130,  170,  280,   30,  170,  150,  120,
    170,  110,  140,  160,  160,   80,  160,  150,   90,  260,
    190,  150,  200,  160,  140,  120,  100,  160,  160,  220,
    170,  160,  150,  100,   70,  110,  190,   90,   -1,   -1

    };

/*
    character list for hershey font number 8
*/
const VFontchar    Hershey8List[] = {

      {   -1,    0, ' '},
      {   -1,    0, ' '},
      {   -1,    0, ' '},
      {    0,   26, '!'},
      {   26,   32, '"'},
      {   58,   16, '#'},
      {   74,   74, '$'},
      {  148,   58, '%'},
      {  206,   74, '&'},
      {  280,   14, '\''},
      {  294,   36, '('},
      {  330,   36, ')'},
      {  366,   12, '*'},
      {  378,    8, '+'},
      {  386,   16, ','},
      {  402,    4, '-'},
      {  406,   10, '.'},
      {  416,    4, '/'},
      {  420,   42, '0'},
      {  462,   12, '1'},
      {  474,   54, '2'},
      {  528,   58, '3'},
      {  586,   10, '4'},
      {  596,   46, '5'},
      {  642,   58, '6'},
      {  700,   32, '7'},
      {  732,   60, '8'},
      {  792,   58, '9'},
      {  850,   18, ':'},
      {  868,   24, ';'},
      {  892,    6, '<'},
      {  898,    8, '='},
      {  906,    6, '>'},
      {  912,   48, '?'},
      {  960,   84, '@'},
      { 1044,   38, 'A'},
      { 1082,   78, 'B'},
      { 1160,   46, 'C'},
      { 1206,   68, 'D'},
      { 1274,   54, 'E'},
      { 1328,   50, 'F'},
      { 1378,   56, 'G'},
      { 1434,   72, 'H'},
      { 1506,   48, 'I'},
      { 1554,   48, 'J'},
      { 1602,   72, 'K'},
      { 1674,   56, 'L'},
      { 1730,   84, 'M'},
      { 1814,   60, 'N'},
      { 1874,   56, 'O'},
      { 1930,   60, 'P'},
      { 1990,   62, 'Q'},
      { 2052,   74, 'R'},
      { 2126,   54, 'S'},
      { 2180,   46, 'T'},
      { 2226,   62, 'U'},
      { 2288,   62, 'V'},
      { 2350,   42, 'W'},
      { 2392,   68, 'X'},
      { 2460,   72, 'Y'},
      { 2532,   78, 'Z'},
      { 2610,   16, '['},
      { 2626,    4, '\\'},
      { 2630,   16, ']'},
      { 2646,    6, '^'},
      { 2652,    4, '_'},
      { 2656,   14, '`'},
      { 2670,   42, 'a'},
      { 2712,   44, 'b'},
      { 2756,   26, 'c'},
      { 2782,   44, 'd'},
      { 2826,   32, 'e'},
      { 2858,   46, 'f'},
      { 2904,   52, 'g'},
      { 2956,   54, 'h'},
      { 3010,   28, 'i'},
      { 3038,   36, 'j'},
      { 3074,   60, 'k'},
      { 3134,   34, 'l'},
      { 3168,   60, 'm'},
      { 3228,   42, 'n'},
      { 3270,   44, 'o'},
      { 3314,   42, 'p'},
      { 3356,   50, 'q'},
      { 3406,   28, 'r'},
      { 3434,   28, 's'},
      { 3462,   26, 't'},
      { 3488,   34, 'u'},
      { 3522,   30, 'v'},
      { 3552,   44, 'w'},
      { 3596,   36, 'x'},
      { 3632,   42, 'y'},
      { 3674,   44, 'z'},
      { 3718,   68, '{'},
      { 3786,    4, '|'},
      { 3790,   68, '}'},
      { 3858,   44, '~'},
      { 3902,   26, ''},
      {   -1,    0, ' '},
      {   -1,    0, ' '}

      };

/*
    vector strokes for hershey font number 8
*/
const FONTINT   Hershey8Vector[] = {

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

    -60,250,  60,130, 
    -10,220, 110,160, 
    -110,220,  10,160, 

/*  character '+'  */

    -100,280, 100,100, 
    -10,190, 190,190, 

/*  character ','  */

    -30,110,  20,100,  10,110,  20,120,  30,110,  30, 90,  20, 70,  10, 60, 

/*  character '-'  */

    -10,190, 190,190, 

/*  character '.'  */

    -20,120,  10,110,  20,100,  30,110,  20,120, 

/*  character '/'  */

    -10, 70, 150,310, 

/*  character '0'  */

    -100,310,  70,300,  50,280,  30,250,  20,220,  10,180,  10,150,  20,120, 
     30,110,  50,100,  70,100, 100,110, 120,130, 140,160, 150,190, 160,230, 
    160,260, 150,290, 140,300, 120,310, 100,310, 

/*  character '1'  */

    -90,310,  30,100, 
    -90,310,  60,280,  30,260,  10,250, 

/*  character '2'  */

    -70,270,  80,260,  70,250,  60,260,  60,270,  70,290,  80,300, 110,310, 
    140,310, 170,300, 180,280, 180,260, 170,240, 150,220, 120,200,  80,180, 
     50,160,  30,140,  10,100, 
    -20,120,  30,130,  50,130,  80,110, 100,100, 130,100, 150,110, 160,140, 

/*  character '3'  */

    -50,260,  50,270,  60,290,  70,300, 100,310, 130,310, 160,300, 170,280, 
    170,260, 160,240, 130,220, 100,210,  80,210, 100,210, 130,200, 140,190, 
    150,170, 150,140, 140,120, 130,110, 100,100,  60,100,  30,110,  20,120, 
     10,140,  10,150,  20,160,  30,150,  20,140, 

/*  character '4'  */

    -160,310, 100,100, 
    -160,310,  10,160, 170,160, 

/*  character '5'  */

    -80,310,  30,210, 
    -80,310, 180,310, 
    -30,210,  40,220,  70,230, 100,230, 130,220, 140,210, 150,190, 150,160, 
    140,130, 120,110,  90,100,  60,100,  30,110,  20,120,  10,140,  10,150, 
     20,160,  30,150,  20,140, 

/*  character '6'  */

    -150,280, 140,270, 150,260, 160,270, 160,280, 150,300, 130,310, 100,310, 
     70,300,  50,280,  30,250,  20,220,  10,180,  10,140,  20,120,  30,110, 
     50,100,  80,100, 110,110, 130,130, 140,150, 140,180, 130,200, 120,210, 
    100,220,  70,220,  50,210,  30,190,  20,170, 

/*  character '7'  */

    -30,310,  10,250, 
    -160,310, 150,280, 130,250,  70,190,  50,160,  40,140,  30,100, 
    -30,290,  50,310,  70,310, 120,280, 140,280, 150,290, 160,310, 

/*  character '8'  */

    -100,310,  70,300,  60,290,  50,270,  50,240,  60,220,  80,210, 110,210, 
    150,220, 160,230, 170,250, 170,280, 160,300, 130,310, 100,310, 
    -80,210,  40,200,  20,180,  10,160,  10,130,  20,110,  50,100,  90,100, 
    130,110, 140,120, 150,140, 150,170, 140,190, 130,200, 110,210, 

/*  character '9'  */

    -150,240, 140,220, 120,200, 100,190,  70,190,  50,200,  40,210,  30,230, 
     30,260,  40,280,  60,300,  90,310, 120,310, 140,300, 150,290, 160,270, 
    160,230, 150,190, 140,160, 120,130, 100,110,  70,100,  40,100,  20,110, 
     10,130,  10,140,  20,150,  30,140,  20,130, 

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
    -30,120,  20,110,  30,100,  40,110,  30,120, 

/*  character '@'  */

    -160,230, 150,250, 130,260, 100,260,  80,250,  70,240,  60,210,  60,180, 
     70,160,  90,150, 120,150, 140,160, 150,180, 
    -160,260, 150,180, 150,160, 170,150, 190,150, 210,170, 220,200, 220,220, 
    210,250, 200,270, 180,290, 160,300, 130,310, 100,310,  70,300,  50,290, 
     30,270,  20,250,  10,220,  10,190,  20,160,  30,140,  50,120,  70,110, 
    100,100, 130,100, 160,110, 180,120, 190,130, 

/*  character 'A'  */

    -10,100,  30,110,  60,140,  90,180, 130,250, 160,310, 160,100, 150,130, 
    130,160, 110,180,  80,200,  60,200,  50,190,  50,170,  60,150,  80,130, 
    110,110, 140,100, 190,100, 

/*  character 'B'  */

    -110,290, 120,280, 120,250, 110,210, 100,180,  90,160,  70,130,  50,110, 
     30,100,  20,100,  10,110,  10,140,  20,190,  30,220,  40,240,  60,270, 
     80,290, 100,300, 130,310, 160,310, 180,300, 190,280, 190,260, 180,240, 
    170,230, 150,220, 120,210, 
    -110,210, 120,210, 150,200, 160,190, 170,170, 170,140, 160,120, 150,110, 
    130,100, 100,100,  80,110,  70,130, 

/*  character 'C'  */

    -100,250, 100,240, 110,230, 130,230, 150,240, 160,260, 160,280, 150,300, 
    130,310, 100,310,  70,300,  50,280,  30,250,  20,230,  10,190,  10,150, 
     20,120,  30,110,  50,100,  70,100, 100,110, 120,130, 130,150, 

/*  character 'D'  */

    -150,310, 130,300, 120,280, 110,240, 100,180,  90,150,  80,130,  60,110, 
     40,100,  20,100,  10,110,  10,130,  20,140,  40,140,  60,130,  80,110, 
    110,100, 140,100, 170,110, 190,130, 210,170, 220,220, 220,260, 210,290, 
    200,300, 180,310, 150,310, 130,290, 130,270, 140,240, 160,210, 180,190, 
    210,170, 230,160, 

/*  character 'E'  */

    -120,270, 120,260, 130,250, 150,250, 160,260, 160,280, 150,300, 120,310, 
     80,310,  50,300,  40,280,  40,250,  50,230,  60,220,  90,210,  60,210, 
     30,200,  20,190,  10,170,  10,140,  20,120,  30,110,  60,100,  90,100, 
    120,110, 140,130, 150,150, 

/*  character 'F'  */

    -100,250,  80,250,  60,260,  50,280,  60,300,  90,310, 120,310, 160,300, 
    190,300, 210,310, 
    -160,300, 140,230, 120,170, 100,130,  80,110,  60,100,  40,100,  20,110, 
     10,130,  10,150,  20,160,  40,160,  60,150, 
    -90,210, 180,210, 

/*  character 'G'  */

    -10,100,  30,110,  70,150, 100,200, 110,230, 120,270, 120,300, 110,310, 
    100,310,  90,300,  80,280,  80,250,  90,230, 110,220, 150,220, 180,230, 
    190,240, 200,260, 200,200, 190,150, 180,130, 160,110, 130,100,  90,100, 
     60,110,  40,130,  30,150,  30,170, 

/*  character 'H'  */

    -70,240,  50,250,  40,270,  40,280,  50,300,  70,310,  80,310, 100,300, 
    110,280, 110,260, 100,220,  80,160,  60,120,  40,100,  20,100,  10,110, 
     10,130, 
    -70,190, 160,220, 180,230, 210,250, 230,270, 240,290, 240,300, 230,310, 
    220,310, 200,290, 180,250, 160,190, 150,140, 150,110, 160,100, 170,100, 
    190,110, 200,120, 220,150, 

/*  character 'I'  */

    -140,150, 120,170, 100,200,  90,220,  80,250,  80,280,  90,300, 100,310, 
    120,310, 130,300, 140,280, 140,250, 130,200, 110,150, 100,130,  80,110, 
     60,100,  40,100,  20,110,  10,130,  10,150,  20,160,  40,160,  60,150, 

/*  character 'J'  */

    -100, 70,  80,100,  60,150,  50,210,  50,270,  60,300,  80,310, 100,310, 
    110,300, 120,270, 120,240, 110,190,  80,100,  60, 40,  50, 10,  40,
    -10, 20, 
    -20, 10, 
    -10, 10,  10, 20,  40, 40,  70, 60,  90, 90, 110,130, 130,

/*  character 'K'  */

    -70,240,  50,250,  40,270,  40,280,  50,300,  70,310,  80,310, 100,300, 
    110,280, 110,260, 100,220,  80,160,  60,120,  40,100,  20,100,  10,110, 
     10,130, 
    -240,280, 240,300, 230,310, 220,310, 200,300, 180,280, 160,250, 140,230, 
    120,220, 100,220, 
    -120,220, 130,200, 130,130, 140,110, 150,100, 160,100, 180,110, 190,120, 
    210,150, 

/*  character 'L'  */

    -50,190,  70,190, 110,200, 140,220, 160,240, 170,260, 170,290, 160,310, 
    140,310, 130,300, 120,280, 110,230, 100,180,  90,150,  80,130,  60,110, 
     40,100,  20,100,  10,110,  10,130,  20,140,  40,140,  60,130,  90,110, 
    120,100, 140,100, 170,110, 190,130, 

/*  character 'M'  */

    -40,240,  20,250,  10,270,  10,280,  20,300,  40,310,  50,310,  70,300, 
     80,280,  80,260,  70,210,  60,170,  40,100, 
    -60,170,  90,250, 110,290, 120,300, 140,310, 150,310, 170,300, 180,280, 
    180,260, 170,210, 160,170, 140,100, 
    -160,170, 190,250, 210,290, 220,300, 240,310, 250,310, 270,300, 280,280, 
    280,260, 270,210, 250,140, 250,110, 260,100, 270,100, 290,110, 300,120, 
    320,150, 

/*  character 'N'  */

    -40,240,  20,250,  10,270,  10,280,  20,300,  40,310,  50,310,  70,300, 
     80,280,  80,260,  70,210,  60,170,  40,100, 
    -60,170,  90,250, 110,290, 120,300, 140,310, 160,310, 180,300, 190,280, 
    190,260, 180,210, 160,140, 160,110, 170,100, 180,100, 200,110, 210,120, 
    230,150, 

/*  character 'O'  */

    -100,310,  70,300,  50,280,  30,250,  20,230,  10,190,  10,150,  20,120, 
     30,110,  50,100,  70,100, 100,110, 120,130, 140,160, 150,180, 160,220, 
    160,260, 150,290, 140,300, 120,310, 100,310,  80,290,  80,260,  90,230, 
    110,200, 130,180, 160,160, 180,150, 

/*  character 'P'  */

    -110,290, 120,280, 120,250, 110,210, 100,180,  90,160,  70,130,  50,110, 
     30,100,  20,100,  10,110,  10,140,  20,190,  30,220,  40,240,  60,270, 
     80,290, 100,300, 130,310, 180,310, 200,300, 210,290, 220,270, 220,240, 
    210,220, 200,210, 180,200, 150,200, 130,210, 120,220, 

/*  character 'Q'  */

    -140,250, 130,230, 120,220, 100,210,  80,210,  70,230,  70,250,  80,280, 
    100,300, 130,310, 160,310, 180,300, 190,280, 190,240, 180,210, 160,180, 
    120,140,  90,120,  70,110,  40,100,  20,100,  10,110,  10,130,  20,140, 
     40,140,  60,130,  90,110, 120,100, 150,100, 180,110, 200,130, 

/*  character 'R'  */

    -110,290, 120,280, 120,250, 110,210, 100,180,  90,160,  70,130,  50,110, 
     30,100,  20,100,  10,110,  10,140,  20,190,  30,220,  40,240,  60,270, 
     80,290, 100,300, 130,310, 170,310, 190,300, 200,290, 210,270, 210,240, 
    200,220, 190,210, 170,200, 140,200, 110,210, 120,200, 130,180, 130,130, 
    140,110, 160,100, 180,110, 190,120, 210,150, 

/*  character 'S'  */

    -10,100,  30,110,  50,130,  80,170, 100,200, 120,240, 130,270, 130,300, 
    120,310, 110,310, 100,300,  90,280,  90,260, 100,240, 120,220, 150,200, 
    170,180, 180,160, 180,140, 170,120, 160,110, 130,100,  90,100,  60,110, 
     40,130,  30,150,  30,170, 

/*  character 'T'  */

    -100,250,  80,250,  60,260,  50,280,  60,300,  90,310, 120,310, 160,300, 
    190,300, 210,310, 
    -160,300, 140,230, 120,170, 100,130,  80,110,  60,100,  40,100,  20,110, 
     10,130,  10,150,  20,160,  40,160,  60,150, 

/*  character 'U'  */

    -40,240,  20,250,  10,270,  10,280,  20,300,  40,310,  50,310,  70,300, 
     80,280,  80,260,  70,220,  60,190,  50,150,  50,130,  60,110,  80,100, 
    100,100, 120,110, 130,120, 150,160, 180,240, 200,310, 
    -180,240, 170,200, 160,140, 160,110, 170,100, 180,100, 200,110, 210,120, 
    230,150, 

/*  character 'V'  */

    -40,240,  20,250,  10,270,  10,280,  20,300,  40,310,  50,310,  70,300, 
     80,280,  80,260,  70,220,  60,190,  50,150,  50,120,  60,100,  80,100, 
    100,110, 130,140, 150,170, 170,210, 180,240, 190,280, 190,300, 180,310, 
    170,310, 160,300, 150,280, 150,260, 160,230, 180,210, 200,200, 

/*  character 'W'  */

    -40,240,  20,250,  10,270,  10,280,  20,300,  40,310,  50,310,  70,300, 
     80,280,  80,250,  70,100, 
    -170,310,  70,100, 
    -170,310, 150,100, 
    -290,310, 270,300, 240,270, 210,230, 180,170, 150,100, 

/*  character 'X'  */

    -80,250,  60,250,  50,260,  50,280,  60,300,  80,310, 100,310, 120,300, 
    130,280, 130,250, 110,160, 110,130, 120,110, 140,100, 160,100, 180,110, 
    190,130, 190,150, 180,160, 160,160, 
    -230,280, 230,300, 220,310, 200,310, 180,300, 160,280, 140,250, 100,160, 
     80,130,  60,110,  40,100,  20,100,  10,110,  10,130, 

/*  character 'Y'  */

    -40,240,  20,250,  10,270,  10,280,  20,300,  40,310,  50,310,  70,300, 
     80,280,  80,260,  70,220,  60,190,  50,150,  50,130,  60,110,  70,100, 
     90,100, 110,110, 130,130, 150,160, 160,180, 180,240, 
    -200,310, 180,240, 150,140, 130, 80, 110, 30,  90,
    -10, 70, 
    -20, 60, 
    -10, 60,  10, 70,  40, 90,  70,120, 100,150, 120,200, 150,

/*  character 'Z'  */

    -120,250, 110,230, 100,220,  80,210,  60,210,  50,230,  50,250,  60,280, 
     80,300, 110,310, 140,310, 160,300, 170,280, 170,240, 160,210, 140,170, 
    110,140,  70,110,  50,100,  20,100,  10,110,  10,130,  20,140,  50,140, 
     70,130,  80,120,  90,100,  90, 70,  80, 40,  70, 20,  50,
    -10, 30, 
    -20, 20, 
    -10, 20,  10, 30,  40, 50,  70, 80, 100,110, 120,170, 150,

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

    -100,160,  90,180,  70,190,  50,190,  30,180,  20,170,  10,150,  10,130, 
     20,110,  40,100,  60,100,  80,110,  90,130, 110,190, 100,140, 100,110, 
    110,100, 120,100, 140,110, 150,120, 170,150, 

/*  character 'b'  */

    -10,150,  30,180,  60,230,  70,250,  80,280,  80,300,  70,310,  50,300, 
     40,280,  30,240,  20,170,  20,110,  30,100,  40,100,  60,110,  80,130, 
     90,160,  90,190, 100,150, 110,140, 130,140, 150,150, 

/*  character 'c'  */

    -80,170,  80,180,  70,190,  50,190,  30,180,  20,170,  10,150,  10,130, 
     20,110,  40,100,  70,100, 100,120, 120,150, 

/*  character 'd'  */

    -100,160,  90,180,  70,190,  50,190,  30,180,  20,170,  10,150,  10,130, 
     20,110,  40,100,  60,100,  80,110,  90,130, 150,310, 
    -110,190, 100,140, 100,110, 110,100, 120,100, 140,110, 150,120, 170,150, 

/*  character 'e'  */

    -20,120,  40,130,  50,140,  60,160,  60,180,  50,190,  40,190,  20,180, 
     10,160,  10,130,  20,110,  40,100,  60,100,  80,110,  90,120, 110,150, 

/*  character 'f'  */

    -60,150, 100,200, 120,230, 130,250, 140,280, 140,300, 130,310, 110,300, 
    100,280,  80,200,  50,110,  20, 40,  10, 10,  10,
    -10, 20, 
    -20, 40, 
    -10, 50,  20, 60, 110, 70, 100, 90, 100,110, 110,120, 120,140, 150,

/*  character 'g'  */

    -100,160,  90,180,  70,190,  50,190,  30,180,  20,170,  10,150,  10,130, 
     20,110,  40,100,  60,100,  80,110,  90,120, 
    -110,190,  90,120,  50, 10,  40,
    -10, 20, 
    -20, 10, 
    -10, 10,  10, 20,  40, 50,  70, 80,  90,100, 100,130, 120,160, 150,

/*  character 'h'  */

    -10,150,  30,180,  60,230,  70,250,  80,280,  80,300,  70,310,  50,300, 
     40,280,  30,240,  20,180,  10,100, 
    -10,100,  20,130,  30,150,  50,180,  70,190,  90,190, 100,180, 100,160, 
     90,130,  90,110, 100,100, 110,100, 130,110, 140,120, 160,150, 

/*  character 'i'  */

    -40,240,  40,230,  50,230,  50,240,  40,240, 
    -10,150,  30,190,  10,130,  10,110,  20,100,  30,100,  50,110,  60,120, 
     80,150, 

/*  character 'j'  */

    -120,240, 120,230, 130,230, 130,240, 120,240, 
    -90,150, 110,190,  50, 10,  40,
    -10, 20, 
    -20, 10, 
    -10, 10,  10, 20,  40, 50,  70, 80,  90,100, 100,130, 120,160, 150,

/*  character 'k'  */

    -10,150,  30,180,  60,230,  70,250,  80,280,  80,300,  70,310,  50,300, 
     40,280,  30,240,  20,180,  10,100, 
    -10,100,  20,130,  30,150,  50,180,  70,190,  90,190, 100,180, 100,160, 
     80,150,  50,150, 
    -50,150,  70,140,  80,110,  90,100, 100,100, 120,110, 130,120, 150,150, 

/*  character 'l'  */

    -10,150,  30,180,  60,230,  70,250,  80,280,  80,300,  70,310,  50,300, 
     40,280,  30,240,  20,170,  20,110,  30,100,  40,100,  60,110,  70,120, 
     90,150, 

/*  character 'm'  */

    -10,150,  30,180,  50,190,  60,180,  60,170,  50,130,  40,100, 
    -50,130,  60,150,  80,180, 100,190, 120,190, 130,180, 130,170, 120,130, 
    110,100, 
    -120,130, 130,150, 150,180, 170,190, 190,190, 200,180, 200,160, 190,130, 
    190,110, 200,100, 210,100, 230,110, 240,120, 260,150, 

/*  character 'n'  */

    -10,150,  30,180,  50,190,  60,180,  60,170,  50,130,  40,100, 
    -50,130,  60,150,  80,180, 100,190, 120,190, 130,180, 130,160, 120,130, 
    120,110, 130,100, 140,100, 160,110, 170,120, 190,150, 

/*  character 'o'  */

    -70,190,  50,190,  30,180,  20,170,  10,150,  10,130,  20,110,  40,100, 
     60,100,  80,110,  90,120, 100,140, 100,160,  90,180,  70,190,  60,180, 
     60,160,  70,140,  90,130, 120,130, 140,140, 150,150, 

/*  character 'p'  */

    -50,150,  70,180,  80,200,  70,160,  10,
    -20,
    -70,160,  80,180, 100,190, 120,190, 140,180, 150,160, 150,140, 140,120, 
    130,110, 110,100, 
    -70,110,  90,100, 120,100, 150,110, 170,120, 200,150, 

/*  character 'q'  */

    -100,160,  90,180,  70,190,  50,190,  30,180,  20,170,  10,150,  10,130, 
     20,110,  40,100,  60,100,  80,110, 
    -110,190, 100,160,  80,110,  50, 40,  40, 10,  40,
    -10, 50, 
    -20, 70, 
    -10, 80,  20, 80,  90,100, 100,130, 120,160, 150,

/*  character 'r'  */

    -10,150,  30,180,  40,200,  40,180,  70,180,  80,170,  80,150,  70,120, 
     70,110,  80,100,  90,100, 110,110, 120,120, 140,150, 

/*  character 's'  */

    -10,150,  30,180,  40,200,  40,180,  60,150,  70,130,  70,110,  50,100, 
    -10,110,  30,100,  70,100,  90,110, 100,120, 120,150, 

/*  character 't'  */

    -10,150,  30,180,  50,220, 
    -80,310,  20,130,  20,110,  30,100,  50,100,  70,110,  80,120, 100,150, 
    -20,230,  90,230, 

/*  character 'u'  */

    -10,150,  30,190,  10,130,  10,110,  20,100,  40,100,  60,110,  80,130, 
    100,160, 
    -110,190,  90,130,  90,110, 100,100, 110,100, 130,110, 140,120, 160,150, 

/*  character 'v'  */

    -10,150,  30,190,  20,140,  20,110,  30,100,  40,100,  70,110,  90,130, 
    100,160, 100,190, 
    -100,190, 110,150, 120,140, 140,140, 160,150, 

/*  character 'w'  */

    -40,190,  20,170,  10,140,  10,120,  20,100,  40,100,  60,110,  80,130, 
    -100,190,  80,130,  80,110,  90,100, 110,100, 130,110, 150,130, 160,160, 
    160,190, 
    -160,190, 170,150, 180,140, 200,140, 220,150, 

/*  character 'x'  */

    -10,150,  30,180,  50,190,  70,190,  80,180,  80,110,  90,100, 120,100, 
    150,120, 170,150, 
    -140,180, 130,190, 110,190, 100,180,  60,110,  50,100,  30,100,  20,110, 

/*  character 'y'  */

    -10,150,  30,190,  10,130,  10,110,  20,100,  40,100,  60,110,  80,130, 
    100,160, 
    -110,190,  50, 10,  40,
    -10, 20, 
    -20, 10, 
    -10, 10,  10, 20,  40, 50,  70, 80,  90,100, 100,130, 120,160, 150,

/*  character 'z'  */

    -10,150,  30,180,  50,190,  70,190,  90,170,  90,150,  80,130,  60,110, 
     30,100,  50, 90,  60, 70,  60, 40,  50, 10,  40,
    -10, 20, 
    -20, 10, 
    -10, 10,  10, 20,  40, 50,  70, 80,  90,120, 120,150, 150,

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
    end of hershey font number 8 vector strokes
    add nothing below this endif
*/
#endif 
