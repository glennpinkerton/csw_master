
// This is strictly an example of the basic syntax use to define
// template classes.  It is very simple and contrived.  It is very
// strongly suggested that all template class methods are implemented
// in the same header file where the class is defined.

// I (Glenn) suspect that an existing stl class is probably available 
// for almost all template class needs.  I will be very discreet in
// creating template classes of my own in "real code"

#include <iostream>
#include <string>


// Using "typename" or "class" in the template line
// are almost always equivalent.  In some cases I am
// told that typename works better, so I am using it.

//template <class T, class K> 

template <typename T, typename K> 
class Tpsamp {

private:

    T   tobj;
    K   kobj;

public:

    Tpsamp (T tin, K kin) : tobj (tin), kobj (kin) {};

    void print () const {
        std::cout << tobj << " , " << kobj << std::flush;
    }

// defining the << operator inline seems more consistent with
// the advised template class syntax.  It keeps the T and K
// consistent between the construction and the << operation.
// Probably can be done with a non inline friend, but this 
// works and it seems pretty simple syntactically.

    friend std::ostream &operator<< (std::ostream &out,
                                     const Tpsamp<T, K> &tp)
    {
        out << "from << oper: " << tp.tobj << " , " <<
               tp.kobj << std::flush;
        return out;
    }

};


// templated function to print "any type"
template <typename T>
void gprintnl (T val)
{
    std::cout << "From templated gprintnl: " << val << " " << std::endl;
}

// non templated function with same name to print a specific type.
// If gprintnl is called with no <> and with an int parameter, this
// non templated version is inferenced.
void gprintnl (int val)
{
    std::cout << "From non-templated gprintnl: " << val << " " << std::endl;
}

// templated function with no call parameters.  If the call to this 
// function does not have a complete <typeneme> syntax, the compiler
// will not be able to figure out what T is in the body of the function.
// if the typename is defined in calling the function "gprintln<double> ()"
// then the resukt of the default constructor is printed.

template <typename T>
void gprintnl ()
{
    std::cout << "From templated gprintnl: " << T() << " " << std::endl;
}

// I (Glenn) am making up a "rule of 2" from this.  If an inferred 
// behavior has more than 2 options depending upon whatever, it is
// clearer and less error prone to explicitly limit the options with
// the correct explicit syntax rather than to rely upon the (in ny mind)
// equivocal and somewhat confusing "implicit" behavior.


int main() {

    Tpsamp <std::string, int> t1 ("three", 3);

    t1.print ();
    std::cout << std::endl;

    std::cout << t1 << std::endl;

/*
 * the templated function often can be called  with "type inference" and
 * not need the <> syntax.  There are times when type inference does not
 * work.
    gprintnl<string> ("string print");
    gprintnl<double> (1.234);
 */

    gprintnl ("string print");
    gprintnl (1.234);

// These 2 calls produce different results.  The first call will
// use the non templated print specifically for the int type.  The
// second call will use the templated function and infer int.

// In "real code" I (Glenn) will try to avoid this by making the 
// two different functions have different names,  For example
// gprintnl_template and gprintnl_int.  This is a case where
// polymorphism can be as confusing as it is helpful.

    gprintnl (123);
    gprintnl<> (456);

// These 2 lines should not compile
//    gprintnl ();
//    gprintnl<> ();

// This line should compile and print whatever the default
// double value is.
    gprintnl <double> ();

    return 0;
}
