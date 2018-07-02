print ()
print ("While and If stuff")
print ()

count = 0
while count < 10:
    print ("    Inside while loop, count = " + str(count))
    count += 1

print ("Loop finished, count = " + str(count))

print ()

count = 12
while count > 0:
    if count % 4 == 0:
        print ("    Inside while loop, by 4 count = " + str(count))
    elif count % 2 == 0:
        print ("    Inside while loop, by 2 count = " + str(count))
    else:
        print ("    Inside while loop, not by 3 or by 2 count = " + str(count))
    count -= 1

print ("Loop finished, count = " + str(count))

print ()

