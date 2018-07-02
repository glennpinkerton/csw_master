
# This has some very trivial examples of defining and using classes.


# trivial geometric shape class

class Shape:

#  conceptually __init__ is like a constructor
#  member variables (called attributes in python lingo?)
#  are implicitly defined by the assignments in this "constructor"

# I (Glenn) have read that starting an attribute name with a single
# _ (underscore) is a convention for a protected variable.  Also,
# starting a variable name with __ (two underscores) and ending with
# one or less undwerscores will trigger name mangling and for practical
# purposes make the attribute or method "private".

    def __init__(self, x, y):

        self._x = x
        self._y = y
        self.description = "This shape has not been described yet"
        self.author = "Nobody has claimed to make this shape yet"


# the __str__ is called when str(object_of_shape) is explicitly
# or implicitly called.
# note \ to continue to next line

    def __str__(self):
        return self.description + \
            '  x: ' + str(self._x) + \
            '  y: ' + str(self._y) 


#  Other class methods

    def area(self):
        return self._x * self._y

    def perimeter(self):
        return 2 * self._x + 2 * self._y

    def describe(self, text):
        self.description = text

    def authorName(self, text):
        self.author = text

    def scaleSize(self, scale):
        self._x = self.x * scale
        self._y = self.y * scale

    def setXY(self, x, y):
        self._x = x
        self._y = y


# use Shape as base for derived Square
# All of the shape members are avaiable via
# the self object in this derived class

class Square(Shape):
    def __init__(self,x):
        super().__init__(x, x)

    def setXY (self, x, y):
        pass


# Another level of derived class.  This level adds a new method

class DoubleSquare(Square):
    def __init__(self,y):
        super().__init__(y)
        self._x = 2 * y

    def perimeter(self):
        return 2 * self._x + 3 * self._y


# use Shape, Square, DoubleSquare a little

rect1 = Shape(100, 200)
rect2 = Shape(200, 50)

# direct assignment of class variable (attribute)
rect1.x = 25

# assign via class method
rect2.setXY (25, 35)

print (" ")
print ("rect 1 = ", rect1)
print (" ")
print ("rect 2 = ", rect2)

rect1.setXY(1.5, 1.9)
rect1.describe('rect1 description')
print (" ")
print ("after sets ")
print ("rect 1 = ", rect1)

sq1 = Square (20)
sq2 = Square (40)

print (" ")
print ("square 1 = ", sq1)
print (" ")
print ("square 2 = ", sq2)

sq1.setXY(1.5, 1.9)
sq1.describe('square 1 description')
print (" ")
print ("after sets ")
print ("square 1 = ", sq1)

# The following direct assign does not trigger an error,
# but it has no effect on the stuff I am doing with the class
# since the class uses a varible named _x
sq1.x = 1.5
print (" ")
print ("after direct assign ")
print ("square 1 = ", sq1)

dsq = DoubleSquare (2000)
print (" ")
print ("double square = ", dsq)

# same direct assign comment as above.
dsq.x = 1.5
print (" ")
print ("after direct assign ")
print ("double square = ", dsq)



# try collections of class objects
dictionary = {}

# Then, create some instances of classes in the dictionary:
dictionary["DoubleSquare 1"] = DoubleSquare(5)
dictionary["long rectangle"] = Shape(600,45)

#You can now use them like a normal class:
print(dictionary["long rectangle"])
print(dictionary["long rectangle"].area())

dictionary["DoubleSquare 1"].authorName("The Gingerbread Man")
print(dictionary["DoubleSquare 1"])

print (dictionary["long rectangle"]);
