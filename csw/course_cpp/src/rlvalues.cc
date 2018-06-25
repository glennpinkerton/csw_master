#include <iostream>
#include <string>
#include <vector>
#include <memory.h>


class RLTest
{
    private:

      static const int SIZE = 200;
      int *rldata = NULL;

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

      RLTest ()
      {
        init ();
      };

      RLTest (int idum)
      {
        init ();
        if (idum > SIZE) idum = SIZE;
        for (int i=0; i<idum; i++) {
            rldata[i] = i * i * 3;
        }
      };

      RLTest (const RLTest &other) {
        std::cout << "RLTest copy constructor" << std::endl;
        init ();
        memcpy (rldata, other.rldata, SIZE * sizeof(int));
      }  

      RLTest &operator= (const RLTest &other) {
        std::cout << "RLTest copy assignment" << std::endl;
        init ();
        memcpy (rldata, other.rldata, SIZE * sizeof(int));
        return *this;
      }

      RLTest (RLTest &&other) {
        std::cout << "RLTest move constructor" << std::endl;
        rldata = other.rldata;
        other.rldata = nullptr;
      }  

      RLTest &operator= (RLTest &&other) {
        std::cout << "RLTest move assignment" << std::endl;
        rldata = other.rldata;
        other.rldata = nullptr;
        return *this;
      }

      virtual ~RLTest ()
      {
        try {
          delete[] rldata;
        }
        catch (...) {
        }
      };

      friend std::ostream &operator<<(std::ostream &out, const RLTest &other);

};

std::ostream &operator<<(std::ostream &out, const RLTest &other)
{
    out << "from RLTest << operator" << std::flush;
    return out;
}


RLTest GetRLTest ()
{
    return RLTest();
}


// two overloaded functions for lvalue and rvalue refs
// The && syntax has nothing to do with a reference to a
// reference.  It specifies an rvalue reference.

void rlfunc (RLTest &lvref) {
    std::cout << "l value ref function" << std::endl;
}
void rlfunc (RLTest &&rvref) {
    std::cout << "r value ref function" << std::endl;
}


int main() {

    RLTest rbuff;

    std::cout << rbuff << std::endl;

// "normal" lvalue reference

    std::cout << std::endl;
    std::cout << "should be l value refs" << std::endl;
    RLTest &lvref = rbuff;
    rlfunc (lvref);
    rlfunc (rbuff);

// r value reference specified with &&

    std::cout << std::endl;
    std::cout << "should be r value refs" << std::endl;
    RLTest &&rvref = GetRLTest ();
    rlfunc (RLTest ());
    rlfunc (GetRLTest());
    rlfunc (rvref);


// the vector below will use the move consteructor to push back
// an rvalue of RLTest.  Should get two cout lines.

    std::cout << std::endl;
    std::cout << "move constructors hopefully below" << std::endl;

    std::vector<RLTest> rlvec;
    rlvec.push_back (RLTest());
    rlvec.push_back (GetRLTest());

    std::cout << std::endl;
    std::cout << "which constructors below" << std::endl;

// It appears that using rvref as defined earlier here is considered an
// l value.  The copy constructor is used.  The cast in the next line
// causes the move constructor to be used
std::cout << "straight rvref push back" << std::endl;
    rlvec.push_back (rvref);
std::cout << "cast rvref push back" << std::endl;
    rlvec.push_back (static_cast<RLTest &&>(rvref));

    RLTest &&rvref2 = GetRLTest ();
std::cout << "straight rvref2 push back" << std::endl;
    rlvec.push_back (rvref2);
std::cout << "std::move rvref2 push back" << std::endl;
    rlvec.push_back (std::move(rvref2));

    return 0;
}
