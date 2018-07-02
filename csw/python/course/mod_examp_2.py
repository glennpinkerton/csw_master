# In this sample program, import only bits of the module used.
# This can be good or bad.  It mkes it possible to use the bits 
# without the module_1. preface.  Maybe less wordy.  But, I (Glenn)
# think using the module name long form is more explicitly definative
# and I plan on doing module reference the long way.


from module_1 import ageofqueen
from module_1 import Piano

### USING AN IMPORTED MODULE
# Use the form modulename.itemname
# Examples:
print (ageofqueen)
cfcpiano = Piano()
cfcpiano.printdetails()
