import module_1

# If you want to not have to type the module name for each module variable
# you can make a local variable and use it.  Personally, I (Glenn) like
# the idea of explicitly using the module name.  It is visually unambiguous
# about what you are referring to.


# Use the form modulename.itemname

print (" ")
print (module_1.ageofqueen)
cfcpiano = module_1.Piano()
cfcpiano.printdetails()

# Use local copies

aq = module_1.ageofqueen

print (" ")
print (aq)



# making a local "copy" of the actual class definition
# seems to behave as expected in this example.  I am guessing
# the local pc variable acts as an alias for the module class.

pc = module_1.Piano
pco = pc()
pco.printdetails()
