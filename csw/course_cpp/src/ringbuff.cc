#include <iostream>
#include <string>
#include <vector>
#include <initializer_list>


template <typename T>
class ringbuff
{
    private:

      int maxsize = 3;
      int current = 0;

      std::vector<T>  tvec;

      void init ()
      {

        try {

// Using the vector is better than using an array of T objects.
// The syntax for dynamic T array new and delete would be:
// T *array = new T[maxsize]
// delete[] array

          tvec.resize (maxsize);
        }
        catch (...) {
          std::cout << "exception constructing ringbuff for size = " <<
            maxsize << std::endl;
        }
        current = 0;
      };

    public:

  // use a "forward declaration" of an inner class to keep
  // the ringbuff class a bit less messy.

      class iterator;

      ringbuff ()
      {
        init ();
      };

      ringbuff (int max) : maxsize (max)
      {
        init ();
      };


  // Constructor using C++11 initializer list

      ringbuff (std::initializer_list<T> ilist) {
        maxsize = (int) (ilist.size());
        if (maxsize < 1) {
          maxsize = 3;
        }
        init ();
        for (auto lval : ilist) {
          add (lval);
        }
      }

   // Reset the ring buffer data to the contents of a new initializer list

      void reset (std::initializer_list<T> ilist) {
        int newsize = (int)ilist.size();
        if (newsize < 1) return;

        maxsize = newsize;
        init ();
        for (auto lval : ilist) {
          add (lval);
        }

      }


      virtual ~ringbuff ()
      {
      };

      void add (T val)
      {
          tvec[current] = val;
          current++;
          if (current == maxsize) current = 0;
      };

      T& get (int idx) {
          idx %= maxsize;
          return tvec[idx];
      };

// begin and end iterator accessors  These create an iterator
// object rather than using a reference.

      iterator begin () {
          return iterator (0, *this);
      }

      iterator end () {
          return iterator (maxsize, *this);
      }

};


// The actual iterator class methods are defined here.  Note the
// syntax of the T stuff here.  This is all needed to allow the
// iterator to know the type of the ringbuff class.  This is not
// real clear as to the basic workings.  For now, just do it this
// way because it works.

// It appears that the <T> stuff is implied in the iterator
// class implementation.

template <typename T>
class ringbuff<T>::iterator
{
  private:

    int        position = 0;
    ringbuff   &rbuf;

  public:

    iterator (int pos, ringbuff &rb) :
      position (pos), rbuf(rb) {};
    
// overload ++ operator.  The method with the int param is 
// the postfix version.  The prefix version has no params.
// The int param for postfix is not used for anything other
// than differentiating postfix from prefix.

    iterator &operator++ (int) {
      position++;
      return *this;
    }

    iterator &operator++ () {
      ++position;
      return *this;
    }

// not equals operator

    bool operator!= (const ringbuff<T>::iterator &other) const {
      return position != other.position;
    }

// dereference (*) operator  Return a reference to an object of
// type T whee the object is at the position specified in this
// iterator object.

    T &operator* () {
        return rbuf.get (position);
    }

};


int main() {

    ringbuff <int> rbuff;

    rbuff.add (10);
    rbuff.add (20);
    rbuff.add (30);

    std::cout << "from old c++98 style" << std::endl;
    for (ringbuff<int>::iterator it = rbuff.begin();
         it != rbuff.end(); it++) {
      std::cout << *it << std::endl;
    }

    std::cout << "from new c++11 style" << std::endl;
    for (auto val : rbuff) {
        std::cout << val << std::endl;
    }


// Create a string ring buffer using an initializer list.

    ringbuff<std::string> rb_str{"sand", "wich", "ice", "cream", "yum"};

    std::cout << "from string initializer_list" << std::endl;
    for (auto val : rb_str) {
        std::cout << val << std::endl;
    }

    rb_str.add ("first yummy");
    for (auto val : rb_str) {
        std::cout << val << std::endl;
    }

    rb_str.reset ({"toe", "ankle", "knee", "hip"});
    rb_str.add ("neck");
    std::cout << std::endl;
    for (auto val : rb_str) {
        std::cout << val << std::endl;
    }
    std::cout << std::endl;

// Create an int ring buffer using an initializer list.

    ringbuff<double> rb_double{1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7};

    std::cout << "from double initializer_list" << std::endl;
    for (auto val : rb_double) {
        std::cout << val << std::endl;
    }

    rb_double.add (3.141592618);
    for (auto val : rb_double) {
        std::cout << val << std::endl;
    }

    rb_double.reset ({2.34, 3.45, 11.111, 34.97});
    rb_double.add (9.8765);
    std::cout << std::endl;
    for (auto val : rb_double) {
        std::cout << val << std::endl;
    }

    return 0;
}
