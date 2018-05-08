
// This example explores various aspects of using a base class
// refefrence or pointer when the actual object is of a derived
// class.  If you do not know how the base and derived classes
// work with each other, this can be perilous.

// A use for this that outweighs it's confusion (in my opinion)
// is to build a collection (i.e. std::vector) of similar objects
// that share a base class.  Using a pointer to the base class as
// the actual collection (std::vector<Base *> will allow
// virtual base class functions to be called on the retrieved
// pointer from the vector.

// The file named abstract_base.cc is a simple example of using
// an abstract base class in a vector.


#include <iostream>


class BaseOne {

  private:

    int    base_one_val = 0;

  public:

    BaseOne () {};
    BaseOne (int val) : base_one_val (val) {};

    void printnl () {
        std::cout << std::endl;
        std::cout << "From BaseOne class: val = " << base_one_val;
        std::cout << std::endl << std::flush;
    }

};


class BaseTwo {

  private:

    int    base_two_val = 31415926;

  public:

    BaseTwo () {};
    BaseTwo (int val) : base_two_val (val) {};

    BaseTwo (const BaseTwo &other) {
      base_two_val = other.base_two_val;
      std::cout << "BaseTwo copy constructor" << std::endl;
    };

    virtual void printnl () {
        std::cout << std::endl;
        std::cout << "From BaseTwo class: val = " << base_two_val;
        std::cout << std::endl << std::flush;
    }

};


class DerivedOne : public BaseOne {

  private:
 
    int    derived_one_val = 12345;

  public:

    DerivedOne() {};
    DerivedOne(int val) : derived_one_val (val) {};

    void printnl () {
        std::cout << std::endl;
        std::cout << "From DerivedOne class: val = " << derived_one_val;
        std::cout << std::endl << std::flush;
    }

};


class DerivedTwo : public BaseTwo {

  private:
 
    int    derived_two_val = 54321;

  public:

    DerivedTwo() {};
    DerivedTwo(int val) : derived_two_val (val) {};

    virtual void printnl () {
        std::cout << std::endl;
        std::cout << "From DerivedTwo class: val = " << derived_two_val;
        std::cout << std::endl << std::flush;
    }

};





int main() {

    DerivedOne  d_one;
    DerivedTwo  d_two;

    BaseOne  &b_one = d_one;
    BaseTwo  &b_two = d_two;

// The BaseOne class printnl method is not virtual, so calling
// printnl on b_one runs the BaseOne printnl method.

    b_one.printnl ();

// The BaseTwo class printnl method is virtual, so calling
// printnl on b_two runs the BaseTwo printnl method.

    b_two.printnl ();

// The same results apply to pointers to the "sliced objects"

    BaseOne  *b1p = &(d_one);
    BaseTwo  *b2p = &(d_two);

    b1p->printnl ();
    b2p->printnl ();

// I (Glenn) find the intentional use of object slicing to be a bad
// idea.  It requires a knowledge of the internal worings of the
// class being "sliced".  In this example, the extremely simple
// classes are easily understood.  In "real code" the class workings
// may (and probably will) be spread amongst header files nested
// like a den of snakes.  The "encapsulation" philosophy seems to be
// non trivially compromised by intentional use of "object slicing".


// Using the copy constructor in the following line is, in my
// opinion a bad thing.  It is confusing and it works differently
// than the assignments above.  In the copy constructor case,
// only the base class constructor is known, and all of the
// derived class stuff is "sliced" off.  Even with the virtual
// printnl in the BaseTwo class, the base class method is always run.

    BaseTwo b2 = DerivedTwo ();
    b2.printnl ();

    return 0;
}
