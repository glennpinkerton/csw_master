
// Examples of various initialization syntax for C++ 98 and C++ 11

#include <iostream>
#include <string>
#include <initializer_list>

/*
 * Initializing the members in the struct definitions
 * makes the "aggregate" initializers fail.

struct ST1
{
    double  dval = 0.0;
    int     ival = 0;
};

struct ST2
{
    std::string   txt = "";
    int     id = 0;
};

struct ST3
{
    std::string   txt = "";
    int     id = 0;
    ST1   s1;
};

*/




// example of using an initializer_list object in a constructor
// The IList object has copies of the items in the initializer
// list.  The IList owns these and is responsible for deleting
// the items.  The deletion happens implicitly when the default
// constructor is called.

template <typename T>
class IList
{
  private:

    T ldata[10];
    int  ndata = 0;

  public:
   
// In C++ 11, can use = default for "devault" constructor
    IList () = default;
 
//    IList () {};

// If we do not want to allow copy constructor or assignment
// operator, use = delete keyword.  In this case the compiler
// makes no methods and if code calls for them, a compile 
// error is generated.

    IList (const IList &other) = delete;
    IList &operator= (const IList &other) = delete;

    IList(std::initializer_list<T> ilst) {
      for (auto item : ilst) {
        ldata[ndata] = item;
        ndata++;
        if (ndata == 10) break;
      }
    }

// Return a reference to the IList item at the specified idx.
// if idx is out of range, throw exception blindly for 
// this example.  real code needs to do much better.

    T &get_item (int idx)
    {
      if (idx < 0  ||  idx >= ndata) {
        throw (1);
      }
      return ldata[idx];
    }

};




// Making a constructor with the 2 variables
// makes the "aggregate" curly brace initialization work
// Also need an explicit "default" constructor to make
// the empty curly brace initialization work.

struct SST1
{
    SST1 () {};
    SST1 (double dv, int iv) : dval(dv), ival(iv) {};
    double  dval = 0.0;
    int     ival = 0;
};



struct ST1
{
    double  dval;
    int     ival;
};

struct ST2
{
    std::string   txt;
    int     id;
};

struct ST3
{
    std::string   txt;
    int     id;
    ST1   s1;
};


    
int main() {

// In C++ 98, public members can supposedly be initialized like this
// Since I am only using C++ 11, I don't know if this stuff works
// in C++ 98.  It does work in C++ 11.

// The = versions are 98 and no = are 11 supposedly

    ST1  s1 = { 1.234, 777 };
    std::cout << s1.dval << " , " << s1.ival << std::endl;

    ST2  s2{ "Who cares", 999 };
    std::cout << s2.txt << " , " << s2.id << std::endl;

    ST3  s3{"s3 string", 543, s1};
    std::cout << "from s3" << std::endl;
    std::cout << s3.txt << " , " << s3.id << std::endl;
    std::cout << s3.s1.dval << " , " << s3.s1.ival << std::endl;

    ST3  s4 = {"s4 string", 987, ST1{1.234, 777}};
    std::cout << "from s4" << std::endl;
    std::cout << s4.txt << " , " << s4.id << std::endl;
    std::cout << s4.s1.dval << " , " << s4.s1.ival << std::endl;

    SST1  s5 = {3.1415926, 753};
    std::cout << "from s5" << std::endl;
    std::cout << s5.dval << " , " << s5.ival << std::endl;

// empty curly braces are supposed to initialize with "default" values

    SST1  s6{};
    std::cout << "from s6" << std::endl;
    std::cout << s6.dval << " , " << s6.ival << std::endl;

// Use initializer_list constructor for IList objects
// I have not bothered to enable range based for stuff
// in IList.

    IList<int> int_ilist{123, 456, 234, 345};
    IList<std::string> str_ilist{"one", "abc", "two", "def", "nss"};

    std::cout << "int ilist: " << int_ilist.get_item(2) << std::endl;
    std::cout << "str ilist: " << str_ilist.get_item(3) << std::endl;

    return 0;
}
