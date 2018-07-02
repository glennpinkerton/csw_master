# Some simple tuple, list and dictionary code


# A tuple is a listg of immutable values.  It is defined 
# with () surrounding a comma separated value list.  The
# tuple cannot be changed at run time.

months = ('January','February','March','April','May','June',\
'July','August','September','October','November','December')


# A list is a list of "single" values.  The [] square brackets
# implicitly make this a list.  Values can be added, removed
# etc from a list.  Also, empty brackets like listname[] can
# be used and values can be added later.

cats = ['Tom', 'Snappy', 'Kitty', 'Jessie', 'Chester']

# declare an empty list.  Populate it with stuff from the
# immutable months collection andthe cats list.  Print the
# mlist values after they are filled in.
mlist = []

im = 0
while im < 10:
    mt = months[im % 12] + " " + cats[im % 5]
    mlist.append (mt)
    im += 1

print (" ")
print (" ")
print (mlist)
print (" ")


# A dictionary has key, value pairs.  Curly brackets {} imply
# the variable is a dictionary.  Empty curly braces are allowed
# and the values can be filled in later. 

phonebook = {'Andrew Parson':8806336, \
             'Emily Everett':6784346, \
             'Peter Power':7658344, \
             'Lewis Lame':1122345}


#First we define the dictionary
#it will have nothing in it this time

ages = {}


#Add a couple of names to the dictionary

ages['Sue'] = 23
ages['Peter'] = 19
ages['Andrew'] = 78
ages['Karren'] = 45

#Use the in keyword 
#It returns TRUE
#if the dictionary has key-name in it
#but returns FALSE if it doesn't.

if 'Sue' in ages:
    print ("Sue is in the dictionary. She is", \
ages['Sue'], "years old")

else:
    print ("Sue is not in the dictionary")

if 'Suee' in ages:
    print ("Suee is in the dictionary. She is", \
ages['Sue'], "years old")

else:
    print ("Suee is not in the dictionary")




# get keys iterator and values iterator for the dictionary

keys = ages.keys()
values = ages.values()


# Looks like dictionaries are not implicitly iterables?  Need to
# use lists like below.  Some other dictionary things can be done
# as well.  Not shown here.

# The sorted function returns a new version of the list specified
# without changing the original list.  The sorted function can be
# used for any iterator and it always returns a list.  To sort a
# list in place, use list.sort()

# Note that the output for the unsorted and sorted look different.
# The keys variable is an iterator and the sorted(keys) variable
# is a list.  The iterator has a bit different toString behavior.

print (" ")
print (keys)
print(sorted(keys))

print (" ")
print (values)
print(sorted(values))

#You can find the number of entries with the len() function:
print (" ")
print ("The dictionary has", len(ages), "entries in it")
print (" ")



#  The dictionary update method requirea a dictionary input parameter
#  Either literal as follows

#  Note the curly braces around the key, value pair
#  to brand it as a dictionary.

ages.update ({'Glenn':64})
print (ages['Glenn'])

# or a variable as shown here

d2 = {'John':45}
ages.update(d2)
print (ages['John'])
