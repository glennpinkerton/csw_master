
# For loops act on iterators.  Iterators an be a bunch of different
# things:  lita, ranges, tuples (read only), functions that use the
# yield statement to pause and send back an intermediate result.

# For loops are a big part of any programming language.  Only a tiny
# bit of them is shown here.




# Looks like separating strings with commas adds the extra space
# but concatenating with + does not add the space.  ?? 

def menu(list, question):

  while True:

    for entry in list:
        ichoice = list.index(entry)
        ichoice += 1
        print(str(ichoice) + ") " + entry)

    choice_to_return = 0
    try:
        choice_to_return = int(input(question))
        break
    except SyntaxError as e:
        print ("");
        print ("SyntaxError: " + str(e))
    except ValueError as e:
        print ("");
        print ("ValueError: " + str(e))
    finally:
        print ("")
        print ("Make sure to choose a number from the list.")
        print ("")

  return choice_to_return


tlist = ['do this', 'or do that'];

choice = menu (tlist, "Who cares: ")

print (" ")
print (" ")
print ("inum range loops")
print (" ")
for inum in range(10):
    inum *= inum
    print ("inum squared = " + str(inum))

print (" ")
for inum in range(2, 20, 3):
    inum *= inum
    print ("inum squared = " + str(inum))
