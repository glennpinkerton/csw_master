from tkinter import font

import tkinter as tk


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

import glp_db


##
##  Class to display and run a gui for selecting a bounding box
##  for retgrieving spatial objects from the glp database
##

class DBGUI:

    def __init__(self, top_master):

#  Instance of the glp db class to use mysql for retrieving
#  spatial data.
        self.db = glp_db.GLPDB ()

#  Label strings are immutable and thus are in a tuple
#  The label widgets themselves are in a list

        self.lstr = ("Min X:", "Min Y:", "Max X:", "Max Y:")
        self.widgets = []
        self.svars = []

        labelfont = font.Font (family="Courier", size=13)
        entryfont = font.Font (family="Courier", size=13)
        buttonfont = font.Font (family="Courier", size=13, weight="bold", slant="italic")
        errorfont = font.Font (family="Courier", size=14, weight="bold")

        top_master.title("Select From DB Using Bounding Box")
        top_master.grid_columnconfigure (0, weight=1)

        self.master = tk.Frame (top_master)
        self.master.grid (padx=10, pady=10)
        
#  Create the min max bbox labels and entries

        for irow in range (2):

          for jcol in range (2):
          
            lsidx = irow * 2 + jcol
            px = 3
            if jcol > 0:
                px = 20
            tmp_label = tk.Label(self.master,\
                           font=labelfont, \
                           text=self.lstr[lsidx])
            tmp_label.grid(row=irow,column=jcol * 2,\
                             sticky=tk.E,\
                             padx=(px, 0), pady=10)

            self.widgets.append (tmp_label)

            tmp_svar = tk.StringVar ()
            tmp_entry = tk.Entry (self.master, \
                               font=entryfont, \
                               textvariable = tmp_svar)
            tmp_entry.grid(row=irow,column=2*jcol+1,\
                                 padx=0, pady=10)
            tmp_entry.bind ('<FocusOut>', self.entry_validate)


            self.widgets.append (tmp_entry)
            self.svars.append (tmp_svar)

        self.widgets[1].focus()

#  Frame with single label the width of the entire form .
#  This label will have error messages when needed.

        self.emsg_frame = tk.Frame (top_master)
        self.emsg_frame.grid (row=1, padx=10, pady=(10,0), sticky="ew")
        self.emsg_frame.grid_columnconfigure (0, weight=1)
        self.emsg_svar = tk.StringVar ()
        self.emsg_label = tk.Label (self.emsg_frame,  \
                             text="", \
                             textvariable=self.emsg_svar, \
                             font=errorfont,\
                             fg="#ff0000")
        self.emsg_label.grid(row=0, column=0,\
                             sticky="ew", \
                             padx=(10,0),\
                             pady=0)
        
#  Create the buttons for select and exit below the min max entry fields.
#  These buttons are not centered in their cells.  I have tried many things
#  suggested in tutorials and questions on the internet, but nothing works.

        self.master2 = tk.Frame (top_master)
        self.master2.grid (row=2, padx=10, pady=10, sticky="ew")
        self.master2.grid_columnconfigure (0, weight=1)
        self.master2.grid_columnconfigure (1, weight=1)
        self.master2.grid_columnconfigure (2, weight=1)
        
        self.clear_button = tk.Button(self.master2,\
                            text="Clear",\
                            font=buttonfont, \
                            bg="#bbbbbb", \
                            activebackground="#bbbbbb", \
                            bd=5, \
                            command=self.clear)
        self.clear_button.grid(row=0, column=0,\
                             sticky=tk.W, \
                             padx=(60,0),\
                             pady=(10,0))

        self.bbox_button = tk.Button(self.master2,\
                            text="Bbox Select",\
                            font=buttonfont, \
                            bg="#bbbbbb", \
                            activebackground="#00bbbb", \
                            bd=5, \
                            command=self.bbox)
        self.bbox_button.grid(row=0, column=1, pady=(10,0))

        self.exit_button = tk.Button(self.master2,\
                            text="Exit",\
                            font=buttonfont, \
                            bg="#bbbbbb", \
                            activebackground="#ff2222", \
                            bd=5, \
                            command=self.master.quit)
        self.exit_button.grid(row=0, column=2,\
                             sticky=tk.E, \
                             padx=(0,60),\
                             pady=(10,0))

## Widgets for output (count and elapsed time)

        self.out_frame = tk.Frame (top_master)
        self.out_frame.grid (row=3, padx=10, pady=10, sticky="ew")
        self.out_frame.grid_columnconfigure (0, weight=1)
        self.out_frame.grid_columnconfigure (1, weight=1)
        self.out_frame.grid_columnconfigure (2, weight=1)
        self.out_frame.grid_columnconfigure (3, weight=1)

        self.count_label = tk.Label(self.out_frame,\
                       font=labelfont, \
                       fg="#0000ff",\
                       text="Count:")
        self.count_label.grid(row=0,column=0,\
                         sticky=tk.E,\
                         padx=(0, 3), pady=10)
        self.count_svar = tk.StringVar ()
        self.count_entry = tk.Label (self.out_frame, \
                           width=20,\
                           relief=tk.SUNKEN,\
                           font=entryfont, \
                           bg="#eeeeee",\
                           fg="#0000ff",\
                           textvariable = self.count_svar)
        self.count_entry.grid(row=0,column=1,\
                             sticky="ew",\
                             padx=(0,10), pady=10)

        self.etime_label = tk.Label(self.out_frame,\
                       font=labelfont, \
                       fg="#0000ff",\
                       text="Time:")
        self.etime_label.grid(row=0,column=2,\
                         sticky=tk.E,\
                         padx=(0, 3), pady=10)

        self.etime_svar = tk.StringVar ()
        self.etime_entry = tk.Label (self.out_frame, \
                           width=20,\
                           relief=tk.SUNKEN,\
                           font=entryfont, \
                           bg="#eeeeee",\
                           fg="#0000ff",\
                           textvariable = self.etime_svar)
        self.etime_entry.grid(row=0,column=3,\
                             sticky="ew",\
                             padx=(0,10), pady=10)


        top_master.bind ('<Return>', self.bbox_from_enter)

  ##  end of __init__ constreuctor



  ## Call bbox funtion via the enter key

    def bbox_from_enter(self, event):
        self.bbox ()

  ## The class bbox function is called when the "Bbox Select" buttom 
  ## is activated.  The bbox corner values are checked for validity.
  ## If not valid, an error message is put into the emsg_label widget.
  ## For a valis bbox, the database is queried and the count of objects
  ## and execution time of the database select is writtgen to the 
  ## output widgets.

    def bbox(self):

        print("Bbox select chosen.")

        try:
            fval_xmin = float(self.svars[0].get())
            self.widgets[1].configure (fg="#000000")
        except (SyntaxError, ValueError) as e:
            self.widgets[1].configure (fg="#ff0000")
            self.emsg_svar.set ("Error from Bbox Select");
            return

        try:
            fval_ymin = float(self.svars[1].get())
            self.widgets[1].configure (fg="#000000")
        except (SyntaxError, ValueError) as e:
            self.widgets[1].configure (fg="#ff0000")
            self.emsg_svar.set ("Error from Bbox Select");
            return

        try:
            fval_xmax = float(self.svars[2].get())
            self.widgets[1].configure (fg="#000000")
        except (SyntaxError, ValueError) as e:
            self.widgets[1].configure (fg="#ff0000")
            self.emsg_svar.set ("Error from Bbox Select");
            return

        try:
            fval_ymax = float(self.svars[3].get())
            self.widgets[1].configure (fg="#000000")
        except (SyntaxError, ValueError) as e:
            self.widgets[1].configure (fg="#ff0000")
            self.emsg_svar.set ("Error from Bbox Select");
            return

        if fval_xmax <= fval_xmin:
            self.emsg_svar.set ("Xmin must be less than Xmax")
            return

        if fval_ymax <= fval_ymin:
            self.emsg_svar.set ("Ymin must be less than Ymax")
            return

        self.emsg_svar.set ("");

        ret_pair = self.db.get_bbox_objects (fval_xmin, fval_ymin,\
                                             fval_xmax, fval_ymax)

        ret_count = ret_pair[0]
        e_time = ret_pair[1]

        self.count_svar.set (str(ret_count))
        self.etime_svar.set (str(e_time))

    ## end if bbox function


## Clear all the entry fields (make them blank)

    def clear(self):
        self.widgets[1].delete(0, tk.END)
        self.widgets[3].delete(0, tk.END)
        self.widgets[5].delete(0, tk.END)
        self.widgets[7].delete(0, tk.END)
        self.count_svar.set ("")
        self.etime_svar.set ("")
        self.widgets[1].focus()


## Check for a valid number in a field when it loses focus.

    def entry_validate (self, event):

        force_false = 0
        widg = event.widget
        if widg == self.widgets[1]:
            en = "MinX"
            sv = self.svars[0]
        elif widg == self.widgets[3]:
            en = "MinY"
            sv = self.svars[1]
        elif widg == self.widgets[5]:
            en = "MaxX"
            sv = self.svars[2]
        elif widg == self.widgets[7]:
            en = "MaxY"
            sv = self.svars[3]
        else:
            en = "Unknown"
            force_false = 1

        if force_false == 1:
            self.emsg_svar.set ("Force false return for unknown widget.")
            return False;
        
        try:
            fval = float(sv.get())
            widg.configure (fg="#000000")

        except (SyntaxError, ValueError) as e:
            widg.configure (fg="#ff0000")
            return False

        return True

##
## End of DBGUI class
##



if __name__  ==  "__main__":
## main part of script to run

#run with prompts from stdin
    #db.test_bbox()

# run with gui
    top = tk.Tk()
    dbgui = DBGUI (top)
    top.mainloop()



