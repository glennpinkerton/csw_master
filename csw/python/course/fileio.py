#  examples of formatted and unformatted file io.

import sys


# This function is an example of a "generator" which returns
# int values from a binary file one at a time.  The file must
# be open and positioned to where the read should start.

def next_int_from_file (tb, maxint):
    count = 0
    while True:
        b = tb.read(4)
        if not b:
            break
        ival = int.from_bytes (b, byteorder = sys.byteorder, signed = True)
        yield ival
        count += 1
        if count >= maxint:
            break



#  open a text file, write a few simple lines and then
#  read a couple of different ways

tf = open ("testfile.txt", "r+")
tf.seek (0)

for i in range(10):
    tf.write (str(i) + "\n")

tf.seek (0)
for line in tf:
    print (line)

tf.seek (0)
for i in range(10):
    print ("line " + str(i) + ": " + tf.readline())

tf.close()



# do this stuff using the with keyword
# This should close the file automatically
# after the with block is finished.  

print(" ")
print("Using a with open block")
print(" ")

with open("testfile.txt", "r+") as tf:
    for line in tf:
        print (line)



# Try writing and then reading from a binary file.

tb = open ("testfile.bin", "wb")
tb.seek (0)

for i in range(10):
    i2 = i + 1
    i2 *= i2
    b = i2.to_bytes(4, sys.byteorder, signed = True)
    tb.write (b)

tb.close ()

tb = open ("testfile.bin", "rb")
tb.seek (0)

print (" ")
for ival in next_int_from_file (tb, 10):
    print ("val = " + str(ival))


print (" ")
print ("read from 4 byte seek")
print (" ")

tb.seek (4)

print (" ")
for ival in next_int_from_file (tb, 10):
    print ("val = " + str(ival))

tb.close ()



# The tutorial also talks a bit about pickles, which looks
# conceptually like a serialization sort of mechanism for
# objects.  

import pickle

# lets create something to be pickled
# How about a list?
picklelist = ['one',2,'three','four',5,'can you count?']

# now create a file
# replace filename with the file you want to create
file = open('filename', 'wb')

# now let's pickle picklelist
pickle.dump(picklelist,file)

# close the file, and your pickling is complete
file.close()



# now open a file for reading
# replace filename with the path to the file you created in pickletest.py
unpicklefile = open('filename', 'rb')

# now load the list that we pickled into a new object
unpickledlist = pickle.load(unpicklefile)

# close the file, just for safety
unpicklefile.close()

print ("")
print ("Output from unpickle")
print ("")

# Try out using the list
for item in unpickledlist:
    print (item)

