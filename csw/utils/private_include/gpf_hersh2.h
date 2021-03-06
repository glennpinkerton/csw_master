
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_hersh2.h

      Definition of vectors for the Hershey font number 2.
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
#error Illegal attempt to include private header file gpf_hersh2.h.
#endif


/*
    add nothing above this ifndef
*/

#ifndef GPF_HERSH2_H
#define GPF_HERSH2_H

#  include "csw/utils/include/csw_.h"

/*
    character width data for hershey font number 2
*/
const FONTINT      Hershey2Width[] = {

     -1,   -1,   -1,   30,   90,  160,  150,  190,  210,   10,
     80,   80,  110,  190,   30,  190,   30,  150,  150,   60,
    150,  150,  160,  150,  140,  150,  150,  140,   30,   30,
    170,  190,  170,  130,  140,  170,  150,  160,  150,  140,
    140,  160,  150,   10,  110,  150,  130,  170,  150,  170,
    150,  170,  150,  150,  150,  150,  170,  210,  150,  170,
    150,   80,  150,   80,  170,  220,   70,  130,  130,  130,
    130,  130,   90,  130,  120,   30,   70,  120,   10,  230,
    120,  140,  130,  130,   90,  120,   90,  120,  130,  170,
    120,  140,  120,   60,   10,   60,  190,   90,   -1,   -1

    };

/*
    character list for hershey font number 2
*/
const VFontchar    Hershey2List[] = {

      {   -1,    0, ' '},
      {   -1,    0, ' '},
      {   -1,    0, ' '},
      {    0,   14, '!'},
      {   14,    8, '"'},
      {   22,   16, '#'},
      {   38,   48, '$'},
      {   86,   58, '%'},
      {  144,   68, '&'},
      {  212,    4, '\''},
      {  216,   20, '('},
      {  236,   20, ')'},
      {  256,   12, '*'},
      {  268,    8, '+'},
      {  276,   16, ','},
      {  292,    4, '-'},
      {  296,   10, '.'},
      {  306,    4, '/'},
      {  310,   34, '0'},
      {  344,    8, '1'},
      {  352,   28, '2'},
      {  380,   30, '3'},
      {  410,   10, '4'},
      {  420,   34, '5'},
      {  454,   46, '6'},
      {  500,    8, '7'},
      {  508,   58, '8'},
      {  566,   46, '9'},
      {  612,   20, ':'},
      {  632,   26, ';'},
      {  658,    6, '<'},
      {  664,    8, '='},
      {  672,    6, '>'},
      {  678,   38, '?'},
      {  716,   54, '@'},
      {  770,   12, 'A'},
      {  782,   42, 'B'},
      {  824,   36, 'C'},
      {  860,   28, 'D'},
      {  888,   16, 'E'},
      {  904,   12, 'F'},
      {  916,   42, 'G'},
      {  958,   12, 'H'},
      {  970,    4, 'I'},
      {  974,   20, 'J'},
      {  994,   12, 'K'},
      { 1006,    8, 'L'},
      { 1014,   16, 'M'},
      { 1030,   12, 'N'},
      { 1042,   42, 'O'},
      { 1084,   24, 'P'},
      { 1108,   46, 'Q'},
      { 1154,   28, 'R'},
      { 1182,   40, 'S'},
      { 1222,    8, 'T'},
      { 1230,   20, 'U'},
      { 1250,    8, 'V'},
      { 1258,   16, 'W'},
      { 1274,    8, 'X'},
      { 1282,   10, 'Y'},
      { 1292,   12, 'Z'},
      { 1304,   16, '['},
      { 1320,    4, '\\'},
      { 1324,   16, ']'},
      { 1340,    8, '^'},
      { 1348,    4, '_'},
      { 1352,   10, '`'},
      { 1362,   32, 'a'},
      { 1394,   32, 'b'},
      { 1426,   28, 'c'},
      { 1454,   32, 'd'},
      { 1486,   34, 'e'},
      { 1520,   14, 'f'},
      { 1534,   42, 'g'},
      { 1576,   18, 'h'},
      { 1594,   14, 'i'},
      { 1608,   20, 'j'},
      { 1628,   12, 'k'},
      { 1640,    4, 'l'},
      { 1644,   32, 'm'},
      { 1676,   18, 'n'},
      { 1694,   34, 'o'},
      { 1728,   32, 'p'},
      { 1760,   32, 'q'},
      { 1792,   14, 'r'},
      { 1806,   34, 's'},
      { 1840,   14, 't'},
      { 1854,   18, 'u'},
      { 1872,    8, 'v'},
      { 1880,   16, 'w'},
      { 1896,    8, 'x'},
      { 1904,   16, 'y'},
      { 1920,   12, 'z'},
      { 1932,   74, '{'},
      { 2006,    4, '|'},
      { 2010,   74, '}'},
      { 2084,   44, '~'},
      { 2128,   26, ''},
      {   -1,    0, ' '},
      {   -1,    0, ' '}

      };

/*
    vector strokes for hershey font number 2
*/
const FONTINT   Hershey2Vector[] = {

/*  character '!'  */

    -20,310,  20,170, 
    -20,120,  10,110,  20,100,  30,110,  20,120, 

/*  character '"'  */

    -10,310,  10,240, 
    -90,310,  90,240, 

/*  character '#'  */

    -90,350,  20, 30, 
    -150,350,  80, 30, 
    -20,220, 160,220, 
    -10,160, 150,160, 

/*  character '$'  */

    -60,350,  60, 60, 
    -100,350, 100, 60, 
    -150,280, 130,300, 100,310,  60,310,  30,300,  10,280,  10,260,  20,240, 
     30,230,  50,220, 110,200, 130,190, 140,180, 150,160, 150,130, 130,110, 
    100,100,  60,100,  30,110,  10,130, 

/*  character '%'  */

    -190,310,  10,100, 
    -60,310,  80,290,  80,270,  70,250,  50,240,  30,240,  10,260,  10,280, 
     20,300,  40,310,  60,310,  80,300, 110,290, 140,290, 170,300, 190,310, 
    -150,170, 130,160, 120,140, 120,120, 140,100, 160,100, 180,110, 190,130, 
    190,150, 170,170, 150,170, 

/*  character '&'  */

    -210,220, 210,230, 200,240, 190,240, 180,230, 170,210, 150,160, 130,130, 
    110,110,  90,100,  50,100,  30,110,  20,120,  10,140,  10,160,  20,180, 
     30,190, 100,230, 110,240, 120,260, 120,280, 110,300,  90,310,  70,300, 
     60,280,  60,260,  70,230,  90,200, 140,130, 160,110, 180,100, 200,100, 
    210,110, 210,120, 

/*  character '''  */

    -10,310,  10,240, 

/*  character '('  */

    -80,350,  60,330,  40,300,  20,260,  10,210,  10,170,  20,120,  40, 80, 
     60, 50,  80, 30, 

/*  character ')'  */

    -10,350,  30,330,  50,300,  70,260,  80,210,  80,170,  70,120,  50, 80, 
     30, 50,  10, 30, 

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

    -10,190, 150,190, 

/*  character '.'  */

    -20,120,  10,110,  20,100,  30,110,  20,120, 

/*  character '/'  */

    -10, 70, 150,310, 

/*  character '0'  */

    -70,310,  40,300,  20,270,  10,220,  10,190,  20,140,  40,110,  70,100, 
     90,100, 120,110, 140,140, 150,190, 150,220, 140,270, 120,300,  90,310, 
     70,310, 

/*  character '1'  */

    -10,270,  30,280,  60,310,  60,100, 

/*  character '2'  */

    -20,260,  20,270,  30,290,  40,300,  60,310, 100,310, 120,300, 130,290, 
    140,270, 140,250, 130,230, 110,200,  10,100, 150,100, 

/*  character '3'  */

    -30,310, 140,310,  80,230, 110,230, 130,220, 140,210, 150,180, 150,160, 
    140,130, 120,110,  90,100,  60,100,  30,110,  20,120,  10,140, 

/*  character '4'  */

    -110,310,  10,170, 160,170, 
    -110,310, 110,100, 

/*  character '5'  */

    -130,310,  30,310,  20,220,  30,230,  60,240,  90,240, 120,230, 140,210, 
    150,180, 150,160, 140,130, 120,110,  90,100,  60,100,  30,110,  20,120, 
     10,140, 

/*  character '6'  */

    -130,280, 120,300,  90,310,  70,310,  40,300,  20,270,  10,220,  10,170, 
     20,130,  40,110,  70,100,  80,100, 110,110, 130,130, 140,160, 140,170, 
    130,200, 110,220,  80,230,  70,230,  40,220,  20,200,  10,170, 

/*  character '7'  */

    -150,310,  50,100, 
    -10,310, 150,310, 

/*  character '8'  */

    -60,310,  30,300,  20,280,  20,260,  30,240,  50,230,  90,220, 120,210, 
    140,190, 150,170, 150,140, 140,120, 130,110, 100,100,  60,100,  30,110, 
     20,120,  10,140,  10,170,  20,190,  40,210,  70,220, 110,230, 130,240, 
    140,260, 140,280, 130,300, 100,310,  60,310, 

/*  character '9'  */

    -140,240, 130,210, 110,190,  80,180,  70,180,  40,190,  20,210,  10,240, 
     10,250,  20,280,  40,300,  70,310,  80,310, 110,300, 130,280, 140,240, 
    140,190, 130,140, 110,110,  80,100,  60,100,  30,110,  20,130, 

/*  character ':'  */

    -20,240,  10,230,  20,220,  30,230,  20,240, 
    -20,120,  10,110,  20,100,  30,110,  20,120, 

/*  character ';'  */

    -20,240,  10,230,  20,220,  30,230,  20,240, 
    -30,110,  20,100,  10,110,  20,120,  30,110,  30, 90,  20, 70,  10, 60, 

/*  character '<'  */

    -170,280,  10,190, 170,100, 

/*  character '='  */

    -10,220, 190,220, 
    -10,160, 190,160, 

/*  character '>'  */

    -10,280, 170,190,  10,100, 

/*  character '?'  */

    -10,260,  10,270,  20,290,  30,300,  50,310,  90,310, 110,300, 120,290, 
    130,270, 130,250, 120,230, 110,220,  70,200,  70,170, 
    -70,120,  60,110,  70,100,  80,110,  70,120, 

/*  character '@'  */

    -100,210,  80,220,  60,220,  50,200,  50,190,  60,170,  80,170, 100,180, 
    -100,220, 100,180, 110,170, 130,170, 140,190, 140,200, 130,230, 110,250, 
     80,260,  70,260,  40,250,  20,230,  10,200,  10,190,  20,160,  40,140, 
     70,130,  80,130, 110,140, 

/*  character 'A'  */

    -90,310,  10,100, 
    -90,310, 170,100, 
    -40,170, 140,170, 

/*  character 'B'  */

    -10,310,  10,100, 
    -10,310, 100,310, 130,300, 140,290, 150,270, 150,250, 140,230, 130,220, 
    100,210, 
    -10,210, 100,210, 130,200, 140,190, 150,170, 150,140, 140,120, 130,110, 
    100,100,  10,100, 

/*  character 'C'  */

    -160,260, 150,280, 130,300, 110,310,  70,310,  50,300,  30,280,  20,260, 
     10,230,  10,180,  20,150,  30,130,  50,110,  70,100, 110,100, 130,110, 
    150,130, 160,150, 

/*  character 'D'  */

    -10,310,  10,100, 
    -10,310,  80,310, 110,300, 130,280, 140,260, 150,230, 150,180, 140,150, 
    130,130, 110,110,  80,100,  10,100, 

/*  character 'E'  */

    -10,310,  10,100, 
    -10,310, 140,310, 
    -10,210,  90,210, 
    -10,100, 140,100, 

/*  character 'F'  */

    -10,310,  10,100, 
    -10,310, 140,310, 
    -10,210,  90,210, 

/*  character 'G'  */

    -160,260, 150,280, 130,300, 110,310,  70,310,  50,300,  30,280,  20,260, 
     10,230,  10,180,  20,150,  30,130,  50,110,  70,100, 110,100, 130,110, 
    150,130, 160,150, 160,180, 
    -110,180, 160,180, 

/*  character 'H'  */

    -10,310,  10,100, 
    -150,310, 150,100, 
    -10,210, 150,210, 

/*  character 'I'  */

    -10,310,  10,100, 

/*  character 'J'  */

    -110,310, 110,150, 100,120,  90,110,  70,100,  50,100,  30,110,  20,120, 
     10,150,  10,170, 

/*  character 'K'  */

    -10,310,  10,100, 
    -150,310,  10,170, 
    -60,220, 150,100, 

/*  character 'L'  */

    -10,310,  10,100, 
    -10,100, 130,100, 

/*  character 'M'  */

    -10,310,  10,100, 
    -10,310,  90,100, 
    -170,310,  90,100, 
    -170,310, 170,100, 

/*  character 'N'  */

    -10,310,  10,100, 
    -10,310, 150,100, 
    -150,310, 150,100, 

/*  character 'O'  */

    -70,310,  50,300,  30,280,  20,260,  10,230,  10,180,  20,150,  30,130, 
     50,110,  70,100, 110,100, 130,110, 150,130, 160,150, 170,180, 170,230, 
    160,260, 150,280, 130,300, 110,310,  70,310, 

/*  character 'P'  */

    -10,310,  10,100, 
    -10,310, 100,310, 130,300, 140,290, 150,270, 150,240, 140,220, 130,210, 
    100,200,  10,200, 

/*  character 'Q'  */

    -70,310,  50,300,  30,280,  20,260,  10,230,  10,180,  20,150,  30,130, 
     50,110,  70,100, 110,100, 130,110, 150,130, 160,150, 170,180, 170,230, 
    160,260, 150,280, 130,300, 110,310,  70,310, 
    -100,140, 160, 80, 

/*  character 'R'  */

    -10,310,  10,100, 
    -10,310, 100,310, 130,300, 140,290, 150,270, 150,250, 140,230, 130,220, 
    100,210,  10,210, 
    -80,210, 150,100, 

/*  character 'S'  */

    -150,280, 130,300, 100,310,  60,310,  30,300,  10,280,  10,260,  20,240, 
     30,230,  50,220, 110,200, 130,190, 140,180, 150,160, 150,130, 130,110, 
    100,100,  60,100,  30,110,  10,130, 

/*  character 'T'  */

    -80,310,  80,100, 
    -10,310, 150,310, 

/*  character 'U'  */

    -10,310,  10,160,  20,130,  40,110,  70,100,  90,100, 120,110, 140,130, 
    150,160, 150,310, 

/*  character 'V'  */

    -10,310,  90,100, 
    -170,310,  90,100, 

/*  character 'W'  */

    -10,310,  60,100, 
    -110,310,  60,100, 
    -110,310, 160,100, 
    -210,310, 160,100, 

/*  character 'X'  */

    -10,310, 150,100, 
    -150,310,  10,100, 

/*  character 'Y'  */

    -10,310,  90,210,  90,100, 
    -170,310,  90,210, 

/*  character 'Z'  */

    -150,310,  10,100, 
    -10,310, 150,310, 
    -10,100, 150,100, 

/*  character '['  */

    -10,350,  10, 30, 
    -20,350,  20, 30, 
    -10,350,  80,350, 
    -10, 30,  80, 30, 

/*  character '\'  */

    -10,310, 150, 70, 

/*  character ']'  */

    -70,350,  70, 30, 
    -80,350,  80, 30, 
    -10,350,  80,350, 
    -10, 30,  80, 30, 

/*  character '^'  */

    -90,330,  10,190, 
    -90,330, 170,190, 

/*  character '_'  */

    -10,100, 220,100, 

/*  character '`'  */

    -20,310,  70,250, 
    -20,310,  10,300,  70,250, 

/*  character 'a'  */

    -130,240, 130,100, 
    -130,210, 110,230,  90,240,  60,240,  40,230,  20,210,  10,180,  10,160, 
     20,130,  40,110,  60,100,  90,100, 110,110, 130,130, 

/*  character 'b'  */

    -10,310,  10,100, 
    -10,210,  30,230,  50,240,  80,240, 100,230, 120,210, 130,180, 130,160, 
    120,130, 100,110,  80,100,  50,100,  30,110,  10,130, 

/*  character 'c'  */

    -130,210, 110,230,  90,240,  60,240,  40,230,  20,210,  10,180,  10,160, 
     20,130,  40,110,  60,100,  90,100, 110,110, 130,130, 

/*  character 'd'  */

    -130,310, 130,100, 
    -130,210, 110,230,  90,240,  60,240,  40,230,  20,210,  10,180,  10,160, 
     20,130,  40,110,  60,100,  90,100, 110,110, 130,130, 

/*  character 'e'  */

    -10,180, 130,180, 130,200, 120,220, 110,230,  90,240,  60,240,  40,230, 
     20,210,  10,180,  10,160,  20,130,  40,110,  60,100,  90,100, 110,110, 
    130,130, 

/*  character 'f'  */

    -90,310,  70,310,  50,300,  40,270,  40,100, 
    -10,240,  80,240, 

/*  character 'g'  */

    -130,240, 130, 80, 120, 50, 110, 40,  90, 30,  60, 30,  40, 40, 
    -130,210, 110,230,  90,240,  60,240,  40,230,  20,210,  10,180,  10,160, 
     20,130,  40,110,  60,100,  90,100, 110,110, 130,130, 

/*  character 'h'  */

    -10,310,  10,100, 
    -10,200,  40,230,  60,240,  90,240, 110,230, 120,200, 120,100, 

/*  character 'i'  */

    -10,310,  20,300,  30,310,  20,320,  10,310, 
    -20,240,  20,100, 

/*  character 'j'  */

    -50,310,  60,300,  70,310,  60,320,  50,310, 
    -60,240,  60, 70,  50, 40,  30, 30,  10, 30, 

/*  character 'k'  */

    -10,310,  10,100, 
    -110,240,  10,140, 
    -50,180, 120,100, 

/*  character 'l'  */

    -10,310,  10,100, 

/*  character 'm'  */

     10,240,  10,100,  10,200,  40,230,  60,240,  90,240, 110,230, 120,200, 
    120,100, 
    -120,200, 150,230, 170,240, 200,240, 220,230, 230,200, 230,100, 

/*  character 'n'  */

    -10,240,  10,100, 
    -10,200,  40,230,  60,240,  90,240, 110,230, 120,200, 120,100, 

/*  character 'o'  */

    -60,240,  40,230,  20,210,  10,180,  10,160,  20,130,  40,110,  60,100, 
     90,100, 110,110, 130,130, 140,160, 140,180, 130,210, 110,230,  90,240, 
     60,240, 

/*  character 'p'  */

    -10,240,  10, 30, 
    -10,210,  30,230,  50,240,  80,240, 100,230, 120,210, 130,180, 130,160, 
    120,130, 100,110,  80,100,  50,100,  30,110,  10,130, 

/*  character 'q'  */

    -130,240, 130, 30, 
    -130,210, 110,230,  90,240,  60,240,  40,230,  20,210,  10,180,  10,160, 
     20,130,  40,110,  60,100,  90,100, 110,110, 130,130, 

/*  character 'r'  */

    -10,240,  10,100, 
    -10,180,  20,210,  40,230,  60,240,  90,240, 

/*  character 's'  */

    -120,210, 110,230,  80,240,  50,240,  20,230,  10,210,  20,190,  40,180, 
     90,170, 110,160, 120,140, 120,130, 110,110,  80,100,  50,100,  20,110, 
     10,130, 

/*  character 't'  */

    -40,310,  40,140,  50,110,  70,100,  90,100, 
    -10,240,  80,240, 

/*  character 'u'  */

    -10,240,  10,140,  20,110,  40,100,  70,100,  90,110, 120,140, 
    -120,240, 120,100, 

/*  character 'v'  */

    -10,240,  70,100, 
    -130,240,  70,100, 

/*  character 'w'  */

    -10,240,  50,100, 
    -90,240,  50,100, 
    -90,240, 130,100, 
    -170,240, 130,100, 

/*  character 'x'  */

    -10,240, 120,100, 
    -120,240,  10,100, 

/*  character 'y'  */

    -20,240,  80,100, 
    -140,240,  80,100,  60, 60,  40, 40,  20, 30,  10, 30, 

/*  character 'z'  */

    -120,240,  10,100, 
    -10,240, 120,240, 
    -10,100, 120,100, 

/*  character '{'  */

    -60,350,  40,340,  30,330,  20,310,  20,290,  30,270,  40,260,  50,240, 
     50,220,  30,200, 
    -40,340,  30,320,  30,300,  40,280,  50,270,  60,250,  60,230,  50,210, 
     10,190,  50,170,  60,150,  60,130,  50,110,  40,100,  30, 80,  30, 60, 
     40, 40, 
    -30,180,  50,160,  50,140,  40,120,  30,110,  20, 90,  20, 70,  30, 50, 
     40, 40,  60, 30, 

/*  character '|'  */

    -10,350,  10, 30, 

/*  character '}'  */

    -10,350,  30,340,  40,330,  50,310,  50,290,  40,270,  30,260,  20,240, 
     20,220,  40,200, 
    -30,340,  40,320,  40,300,  30,280,  20,270,  10,250,  10,230,  20,210, 
     60,190,  20,170,  10,150,  10,130,  20,110,  30,100,  40, 80,  40, 60, 
     30, 40, 
    -40,180,  20,160,  20,140,  30,120,  40,110,  50, 90,  50, 70,  40, 50, 
     30, 40,  10, 30, 

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
    end of hershey font number 2 vector strokes
    add nothing below this endif
*/
#endif 
