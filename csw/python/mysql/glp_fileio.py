#  examples of formatted and unformatted file io.

import sys



# This UniversePoint class is a simple x,y pair that 
# stores a point in two floats.  The x and y are "public"
# and set and get functions are not used.

class UniversePoint:
  
  def __init__ (self, x, y):
    self.x = x
    self.y = y




class GLPFileIO:

# The constructor sets the "universe" to use for converting
# between float coordinates and the int coords put into
# the random access files.

  def __init__ (self, xmin, ymin, xmax, ymax,\
                      ixmin, iymin, ixmax, iymax):

    self.xmin = xmin;
    self.ymin = ymin;
    self.xmax = xmax;
    self.ymax = ymax;
    self.ixmin = ixmin;
    self.iymin = iymin;
    self.ixmax = ixmax;
    self.iymax = iymax;
    self.xscal = (xmax - xmin) / (float(ixmax) - float(ixmin))
    self.yscal = (ymax - ymin) / (float(iymax) - float(iymin))



# This method is an example of a "generator" which returns
# int values from a binary file one at a time.  The file must
# be open and positioned to where the read should start.  The
# naming convention (__ prefix and _ suffix) is supposed to
# trigger name mangling, making the method effectively private.
# My (Glenn) early experience the way to call these "private"
# variables from within the class is:  ret_val = self.__..._()
# Various web advice says to put the self inside the parameter
# list or both inside the parameter list and as "self.".  Neither
# of these 2 pieces of advice worked, but using only the self.
# syntax does work for me.

  def __next_int_from_file_ (self, bfile, maxint):
    count = 0
    while True:
        b = bfile.read(4)
        if not b:
            break
        ival = int.from_bytes (b, byteorder = "big", signed = True)
        yield ival
        count += 1
        if count >= maxint:
            break


  
# "Private" method to get a bunch of consecutive ints from
# the current position of the open bfile file

  def __next_blob_of_ints_ (self, bfile, maxint):

    iblob = []
    
    n = 0
    for ival in self.__next_int_from_file_ (bfile, maxint):
      iblob.append(ival)

# if the blob length is not equal to maxint
# a problem occurred.  One possibility is an attempt
# to read past the end of the file.  If this mismatch
# occurs, an IOError exception is raised.
    if len(iblob) != maxint:
      print ("")
      print ("  maxint = " + str(maxint) + \
             "  blob length = " + str(len(iblob)))
      print ("")
      raise IOError\
      ("Could not read enough data into next blob.")

    return iblob




# use the universe limits and scales to convert inyeger
# x,y to universe float x,y

  def __universe_from_int_ (self, ix, iy):
    dx = (float(ix) - float(self.ixmin)) * self.xscal + self.xmin
    dy = (float(iy) - float(self.iymin)) * self.yscal + self.ymin
    rval = UniversePoint (dx, dy)
    return rval



# Read "line" points from the specified position of the specified file.
# Return a list of the points in universe coordinates as floats.
# The list is ordered as x1,y1  x2,y2 etc.

  def read_points (self, bfile, ipos):
    
    try:
      bfile.seek (ipos)
      first3 = self.__next_blob_of_ints_ (bfile, 3)
      npts = first3[2]
      ptsxy = self.__next_blob_of_ints_ (bfile, npts * 2)

    except IOError:
      raise

    pts_latlon = []

    for i in range(0, npts * 2, 2):
      ix = ptsxy[i]
      iy = ptsxy[i+1]
      llpair = self.__universe_from_int_ (ix, iy)
      pts_latlon.append(llpair)

    return pts_latlon
