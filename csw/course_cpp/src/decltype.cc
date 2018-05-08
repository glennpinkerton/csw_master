
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <typeinfo>
#include <vector>
#include <string>


// auto return type for a function.

template <typename T, typename U>
auto test (T v1, U v2) ->decltype(v1 + v2)
{
    return v1 + v2;
}


// vectors of mypair and yourpair objects are populated
// in the main program.

class yourpair {

  private:

    double   val = 0.0;
    int   idx = 0;

  public:

    yourpair (double d, int i) : val(d), idx(i) {};

    void print () const {
        std::cout << "yourpair:  " << std::flush;
        std::cout << "ival = " << idx  << " dval = " <<
                 val << std::endl;
    }

};


class mypair {

  private:

    int   idx = 0;
    double   val = 0.0;

  public:

    mypair (int i, double d) : idx(i), val(d) {};

    void print () const {
        std::cout << "mypair:  " << std::flush;
        std::cout << "ival = " << idx  << " dval = " <<
                 val << std::endl;
    }

};


// This templated function is called to show the mypair and
// lists without mypair or yourpair ever being mentioned in the
// loops or in this function

template <typename T>
void print_item_type (T tval)
{
    std::cout << std::endl;

// decltype seems to do the same as auto in this usage

    decltype(tval) tval2 = tval;
    tval2.print ();

    std::cout << typeid(tval).name() << std::endl;
    std::cout << typeid(tval2).name() << std::endl;

// auto seems to do the same as decltype in this usage

    auto tval3 = tval;
    std::cout << typeid(tval3).name() << std::endl;

    tval3.print ();

    std::cout << std::endl;
}



int main() {

    std::string   name = "Maxwell";

    decltype(name) name2 = name;

    std::cout << typeid(name).name() << std::endl;
    std::cout << typeid(name2).name() << std::endl;

    std::cout << "name 2 from decltype = " << name2 << std::endl;

    auto retval = test (5, 6);

    std::cout << "retval = " << retval << std::endl;

    std::srand (12345);

// mypair and yourpair are specifically mentioned in the
// list population loops.  auto does not work in the 
// push_back parameter.  The explicit type is needed.

    std::vector<mypair> mplist;
    std::vector<yourpair> yplist;
    for (int i=0; i<20; i++) {
        int  ival;
        double dval;
        ival = std::rand() % 1000;
        dval = (double)ival / (double)(i+1);
        dval = sqrt (dval);
        mplist.push_back (mypair(ival, dval));
        ival = std::rand() % 10000;
        dval = (double)ival / (double)(i+1);
        dval = sqrt (dval * 3.1415926);
        yplist.push_back (yourpair(dval, ival));
    }

// The use of decltype in this loop is overkill but it
// serves to demonstrate the syntax of using decltype
// in an auto loop.

    for (const auto &item : mplist) {
        print_item_type (item);
    }
    
    for (const auto &item : yplist) {
        print_item_type (item);
    }
    
    return 0;

}
