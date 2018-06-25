/*
 *  This example uses unique pointer objects in various ways.
 *  It is hard to know exactly when the C++ compiler makes
 *  something an r value or an l value.  When dealing with
 *  unique pointers, which cannot be copied, it is better to
 *  explicitly use the std::move() syntax when you want the 
 *  move copy constructor or move assinment operator to be used.
 *
 *  Unique pointers do not have an "is a" relationship to the
 *  undwerlying raw pointer.  The unique ppointer has a "has a"
 *  relationship to the raw pointer.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <memory.h>

class RLTest
{
    private:

      static const int SIZE = 200;
      int *rldata = nullptr;

  // The RLTest object is not a unique_ptr of itself.  A unique
  // pointer object has a raw pointer of some type.  Thus, the
  // RLTest class has a copy constrictor and assignment operator.

      void init ()
      {
        try {
          if (rldata != NULL) {
            delete [] rldata;
            rldata = NULL;
          }
          // In C++11, the {} sets the int array to zeros after allocation
          rldata = new int[SIZE]{};
        }
        catch (...) {
          std::cout << "exception constructing RLTest for size = " <<
            SIZE << std::endl;
        }
      };

    public:

  // The three constructors are just for variety.

      RLTest () : rldata (new int[SIZE])
      {
        std::cout << "RLTest default constructor" << std::endl;
        init ();
      }

      RLTest (int idum) : rldata (new int[SIZE])
      {
        std::cout << "RLTest int param constructor" << std::endl;
        init ();
        if (idum > SIZE) idum = SIZE;
        for (int i=0; i<idum; i++) {
            rldata[i] = i * i * 3;
        }
      };

      RLTest (int idum, int ival) : rldata (new int[SIZE])
      {
        std::cout << "RLTest two int param constructor" << std::endl;
        init ();
        if (idum > SIZE) idum = SIZE;
        for (int i=0; i<idum; i++) {
            rldata[i] = i * i * ival;
        }
      };

  // copy constructor

      RLTest (const RLTest &other) {
        std::cout << "RLTest copy constructor" << std::endl;
        init ();
      }  

  // copy assignment operator

      RLTest &operator= (const RLTest &other) {
        std::cout << "RLTest copy assignment" << std::endl;
        init ();
        return *this;
      }

   // move constructor

      RLTest (RLTest &&other) {
        std::cout << "RLTest move constructor" << std::endl;
        if (rldata != other.rldata) {
          rldata = other.rldata;
          other.rldata = nullptr;
        }
      }  

  // move assignment operator

      RLTest &operator= (RLTest &&other) {
        std::cout << "RLTest move assignment" << std::endl;
        delete[] rldata;
        rldata = other.rldata;
        other.rldata = nullptr;
        return *this;
      }

  // destructor

      virtual ~RLTest ()
      {
        std::cout << std::endl << "RLTest destructor run" << std::endl;
        delete [] rldata;
        rldata = nullptr;
      };

      friend std::ostream &operator<<(std::ostream &out, const RLTest &other);

      int GetDataPoint (int idx)
      {
        return rldata[idx];
      }

};



std::ostream &operator<<(std::ostream &out, const RLTest &other)
{
    out << "from RLTest << operator" << std::endl;
    out << "    rldata[0] = " << other.rldata[0] << std::endl;
    out << "    rldata[1] = " << other.rldata[1] << std::endl;
    out << "    rldata[2] = " << other.rldata[2] << std::endl;
    return out;
}



// two overloaded functions for lvalue and rvalue refs
// The && syntax has nothing to do with a reference to a
// reference.  It specifies an rvalue reference.

void rlfunc (RLTest &lvref) {
    std::cout << "l value ref function" << std::endl;
}
void rlfunc (RLTest &&lvref) {
    std::cout << "r value ref function" << std::endl;
}


static std::vector< std::shared_ptr<RLTest> > shrlvecstat;

int main() {

    std::unique_ptr<RLTest[]> p_unq(new RLTest[2]);
    std::cout << "from p_unq " << p_unq[0] << std::endl;

    std::unique_ptr<RLTest> p_unq2(new RLTest(10));
    std::cout << "from p_unq2 " << *(p_unq2.get()) << std::endl;

  // pushing unique pointers onto a vector seems to work well
  // when the std::move syntax is used.  The push triggers the
  // unique pointer move constructor.  Experimentation shows
  // that the raw pointer in the original pushed unique_ptr 
  // object has been set to invalid after the push.  The vector
  // takes ownership.  Subsequent vector elements reset the raw
  // pointer and push the reset unique_ptr object onto the vector.
    std::cout << std::endl;
    std::vector< std::unique_ptr<RLTest> > rlvec;

    std::unique_ptr<RLTest> p_unq3(new RLTest(10));
    rlvec.push_back (std::move(p_unq2));
    p_unq2.reset (new RLTest(10));
    rlvec.push_back (std::move(p_unq2));
    p_unq2.reset (new RLTest());
    rlvec.push_back (std::move(p_unq2));
    p_unq2.reset (new RLTest(10, 9));
    rlvec.push_back (std::move(p_unq2));
    p_unq2.reset (new RLTest(10, -5));
    rlvec.push_back (std::move(p_unq2));
    

  // To use a range loop through a unique_ptr collection, use references
  // to the collection elements.  A const reference can pinch hit for
  // using a value in many cases.

    for (const auto &p : rlvec) {
        std::cout << "In rlvec loop:  " << std::endl << *p << std::endl;
    }


// shared pointers get "emptied" when moved to vector.
// try regular copy constructor.  So unique_ptr objects
// must be moved into a collection and shared_ptr objects
// should be copied into the collection.

    std::vector< std::shared_ptr<RLTest> > shrlvec1;
    std::vector< std::shared_ptr<RLTest> > shrlvec2;

    std::shared_ptr<RLTest> shp1 = std::make_shared<RLTest> (10);
    std::shared_ptr<RLTest> shp2 = std::make_shared<RLTest> (10, 10);
    std::shared_ptr<RLTest> shp3 = std::make_shared<RLTest> (10, -10);
    std::shared_ptr<RLTest> shp4 = std::make_shared<RLTest> ();

    shrlvec1.push_back ((shp1));
    shrlvec1.push_back ((shp1));
    shrlvec1.push_back ((shp1));
    shrlvec2.push_back ((shp1));
    shrlvec2.push_back ((shp1));
    shrlvec2.push_back ((shp1));

    shrlvec1.push_back ((shp2));
    shrlvec1.push_back ((shp2));
    shrlvec1.push_back ((shp2));
    shrlvec2.push_back ((shp2));
    shrlvec2.push_back ((shp2));
    shrlvec2.push_back ((shp2));

    shrlvec1.push_back ((shp3));
    shrlvec1.push_back ((shp3));
    shrlvec1.push_back ((shp3));
    shrlvec2.push_back ((shp3));
    shrlvec2.push_back ((shp3));
    shrlvec2.push_back ((shp3));

    shrlvec1.push_back ((shp4));
    shrlvec1.push_back ((shp4));
    shrlvec1.push_back ((shp4));
    shrlvec2.push_back ((shp4));
    shrlvec2.push_back ((shp4));
    shrlvec2.push_back ((shp4));

    shrlvecstat.push_back ((shp4));
    shrlvecstat.push_back ((shp4));
    shrlvecstat.push_back ((shp4));
    shrlvecstat.push_back ((shp4));
    shrlvecstat.push_back ((shp4));
    shrlvecstat.push_back ((shp4));

    for (const auto &p : shrlvec1) {
      if (p) {
        std::cout << "In shrlvec1 loop:  " << std::endl << *p << std::endl;
      }
    }

    for (const auto &p : shrlvec2) {
      if (p) {
        std::cout << "In shrlvec2 loop:  " << std::endl << *p << std::endl;
      }
    }

    return 0;
}
