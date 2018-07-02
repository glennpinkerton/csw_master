# calculator program

import sys

# define functions for menu and arithmetic


# Note that in python 3, the input methos always returns a string
# This is like rawinput i python 2.  Thus, in the code below, the
# return from input is "cast" with the int function.

#print what options you have
def menu():
    print (" ")
    print ("Welcome to calculator.py")
    print ("your options are:")
    print (" ")
    print ("1) Addition")
    print ("2) Subtraction")
    print ("3) Multiplication")
    print ("4) Division")
    print ("5) Quit calculator.py")
    print (" ")
    opt = int (input ("Choose your option: "))
    print (" ")
    print ("opt = ", opt)
    return opt
    
# this adds two numbers given
def add(a,b):
    print (" ")
    print (a, "+", b, "=", a + b)
    print (" ")
    
# this subtracts two numbers given
def sub(a,b):
    print (" ")
    print (b, "-", a, "=", b - a)
    print (" ")
    
# this multiplies two numbers given
def mul(a,b):
    print (" ")
    print (a, "*", b, "=", a * b)
    print (" ")
    
# this divides two numbers given
def div(a,b):
    print (" ")
    try:
        print (a, "/", b, "=", a / b)
    except ZeroDivisionError:
        print ("Cannot divide by zero")
    print (" ")
    
# NOW THE PROGRAM REALLY STARTS, AS CODE IS RUN
loop = 1
choice = 0
while loop == 1:
    choice = menu()
    print ("choice from menu = ", choice)
    if choice == 1:
        add(int(input("Add this: ")),int(input("to this: ")))
    elif choice == 2:
        sub(int(input("Subtract this: ")),int(input("from this: ")))
    elif choice == 3:
        mul(int(input("Multiply this: ")),int(input("by this: ")))
    elif choice == 4:
        div(int(input("Divide this: ")),int(input("by this: ")))
    elif choice == 5:
        loop = 0

print ("Thank you for using calculator.py!")

# NOW THE PROGRAM REALLY FINISHES
