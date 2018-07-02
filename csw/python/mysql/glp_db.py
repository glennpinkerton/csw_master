
#  This is a class to access the glp mysql data base.  The 
#  purpose of this is to learn a bit more python in a somewhat
#  realistic problem.


#  Most of the web instructions for using mysql (at least those
#  which I looked at) say to import PyMySQL.  The PyMySQL does
#  not work on my linux box (fedora 26).  I found a suggestion
#  to use all lower case, and that works for me.

import pymysql

# regular expression stuff and timing stuff
import re
import time


#  The glp_fileio module has the GLPFileIO class, which reads
#  data from the random access points files associated with the
#  glp database.

import glp_fileio


# class to hold id values (object, file, file pos) that are
# returned by a "spatial" query of the database

class ObjFilePos:
  def __init__ (self, obid, f_id, f_pos):
    self.object_id = obid
    self.file_id = f_id
    self.file_pos = f_pos




#  Encapsulate the glp database stuff in class GLPDB
#  This is a very simple "spatial" data base which
#  can be populated with random "spatial" objects
#  so I (Glenn) can work with mysql in python.

class GLPDB:

#  The constructor needs to establish connection to the mysql dbase.

  def __init__ (self):

    self.dirname = ""
    self.connection = pymysql.connect (host = "localhost",\
                                       user = "glenn",\
                                       password = "Dog4-J3ssi3",\
                                       db = "glp")
    try:
      with self.connection.cursor() as curs:
        sq = "select * from global_stuff"
        curs.execute(sq)
        for (dirname, xmin, ymin, xmax, ymax, ixmin, iymin, ixmax, iymax) in curs:
          self.dirname = dirname
          self.xmin = xmin
          self.ymin = ymin
          self.xmax = xmax
          self.ymax = ymax
          self.ixmin = ixmin
          self.iymin = iymin
          self.ixmax = ixmax
          self.iymax = iymax
          self.fio = glp_fileio.GLPFileIO (xmin, ymin, xmax, ymax,\
                                ixmin, iymin, ixmax, iymax)
          break

    finally:
      pass

# !! end of __init__ constructor


# Method to execute the bbox select query

  def __do_bbox_query_ (self, sq):

    rlst = []

    with self.connection.cursor() as curs:
      curs.execute(sq)
      for (object_id, line_id, file_id, file_pos) in curs:
        ofp = ObjFilePos (int(object_id), int(file_id), int(file_pos))
        rlst.append(ofp)

      return rlst          

# !! end of __do_bbox_query_ method
    
    
#  Method to disconnect from the mysql server

  def disconnect (self):
    self.connection.close()

# !! end of disconnect method


# create an sql string to select objects intersecting
# the specified rectangle

  def __create_bbox_query_ (self, bxmin, bymin, bxmax, bymax):

    ix1 = int(bxmin * 1000000.0)
    iy1 = int(bymin * 1000000.0)
    ix2 = int(bxmax * 1000000.0)
    iy2 = int(bymax * 1000000.0)

    bbs =\
      "'polygon ((\n" +\
         str(ix1) + " " + str(iy1) + " , \n" +\
         str(ix1) + " " + str(iy2) + " , \n" +\
         str(ix2) + " " + str(iy2) + " , \n" +\
         str(ix2) + " " + str(iy1) + " , \n" +\
         str(ix1) + " " + str(iy1) + " \n" + "))'"

    s = "select objects.object_id,pline_points_lookup.* from \n" +\
          "objects_geom,objects,pline_id_lookup,pline_points_lookup \n" +\
          " where \n" +\
          "(MBRIntersects" + "\n" +\
             "(ST_GeomFromText\n(" +\
               bbs + "),\n objects_geom.bbox)) \n" +\
          "  and \n" +\
          "(objects.object_id = objects_geom.object_id) \n" +\
          "  and \n" +\
          "(pline_id_lookup.object_id = objects.object_id) \n" +\
          "  and \n" +\
          "(pline_points_lookup.line_id = pline_id_lookup.line_id)\n" +\
          "order by pline_points_lookup.file_id, pline_points_lookup.file_pos"

    return s

# !! end of __create_bbox_query_ method



#  function to prompt for bbox limits, get the obkects intersecting
#  the bbox and wite them to a file.

  def test_bbox (self):

    BBERR = "Illegal bbox definition, try again"
    while True:

      try:
        print ("")
        c = input("Enter lower left x y and upper right x y: ")
        if len(c) < 1:
          break

        try:
          bblst = re.split(" ", c)
        except Exception as e:
          print ("Exception from re.split: " + str(e))
          continue

        rpair = self.get_bbox_objects (float(bblst[0]), float(bblst[1]),
                                       float(bblst[2]), float(bblst[3]))

      except Exception as e:
        print ("Exception in test_bbox: " + str(e))
        raise

      sel_count = rpair[0]
      delta_time = rpair[1]

      print ("")
      print ("")
      print ("number of objects selected = " + str(sel_count))
      print ("")
      print ("elapsed time for query = " + str(delta_time))
      print ("")

  #  ! end of while true loop to prompt for bbox limits and process them


  # comes here if no exception happened 
  # still may need to close the output and point files

    try:
      if not fout.closed:
        fout.close()
      if not fpts.closed:
        fpts.close()
    except:
      pass

    return
      

# !! end of test_bbox method




#  function to prompt for bbox limits, get the obkects intersecting
#  the bbox and wite them to a file.

  def get_bbox_objects (self, xmin, ymin, xmax, ymax):

    try:
      fout = open ("py_bbox_sel.out", "w")
    except IOError:
      print ("****** Cannot open file: py_bbox_sel.out ******")
      return

    return_pair = []

    BBERR = "Illegal bbox definition, try again"
    try:

        tstart = time.time()

        sq = self.__create_bbox_query_ (xmin, ymin, xmax, ymax)
        file_pos_list = self.__do_bbox_query_ (sq)
        sel_count = len(file_pos_list)
        sl = str(sel_count)

        return_pair.append(sel_count)

        c = str(xmin) + " " + str(ymin) + " " + str(xmax) + " " + str(ymax)
        fout.write ("\n==== new search bbox =  " + c + "  ====\n")
        fout.write ("     number of rows found = " + sl + "\n")
  #      print ("\n     number of rows found = " + sl + "\n")

        last_file_id = -1
        for ofp in file_pos_list:
          fout.write ("\n")
          fout.write ("object_id = " + str(ofp.object_id))
          fout.write ("\n")
          fout.write ("file_id = " + str(ofp.file_id))
          fout.write ("\n")
          fout.write ("file_pos = " + str(ofp.file_pos))
          fout.write ("\n")

          if last_file_id != ofp.file_id:
            try:
              if last_file_id != -1  and  not fpts.closed:
                fpts.close()

              fpts = open (self.dirname + "/glp_points_" +\
                           str(ofp.file_id) + ".dat", "rb")
              last_file_id = ofp.file_id
            except IOError:
              print ("")
              print ("Error opening points random access file " +\
                      str(ofp.file_id))
              print ("")
              raise

       # end of open file_id block

          plist = self.fio.read_points (fpts, ofp.file_pos)
          fout.write (" npts = " + str(len(plist)))
          fout.write ("\n")
          for upt in plist:
       # the f just prior to the quote signals a "formatted" string
       # the curly braces surround value:format pairs.
            fout.write(f"  x = {upt.x:.5f}  y = {upt.y:.5f}")
            fout.write ("\n")


    except Exception as e:
        print ("Exception in test_bbox: " + str(e))
    # close output file and point before raising the exception
        if not fout.closed:
          fout.close()
        if not fpts.closed:
          fpts.close()
        raise

    tend = time.time() 
    delta_time = round(tend - tstart, 2)

    return_pair.append(delta_time)

  # comes here if no exception happened 
  # still probably need to close the output and point files
    if not fout.closed:
      fout.close()

    try:
      if not fpts.closed:
        fpts.close()

    except:
      pass

    return return_pair
      

# !! end of get_bbox_objects method



## !!! end of GLPDB class definition




# make a class and print the first "line" points in file
# This code is not part of the GLPDB class

if __name__ == "__main__":
  db = GLPDB ()
  db.test_bbox()



