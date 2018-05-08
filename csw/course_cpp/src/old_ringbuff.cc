#include <iostream>
#include <string>
#include <vector>


template <typename T>
class old_ringbuff
{
    private:

      int maxsize = 3;
      int current = 0;
      T   *ringdata = NULL;

      void init ()
      {
        try {
          if (ringdata != NULL) {
            delete [] ringdata;
            ringdata = NULL;
          }
          else {
            ringdata = new T [maxsize];
          }
        }
        catch (...) {
          std::cout << "exception constructing old_ringbuff for size = " <<
            maxsize << std::endl;
        }
      };

    public:

      old_ringbuff ()
      {
        init ();
      };

      old_ringbuff (int max) : maxsize (max)
      {
        init ();
      };

      virtual ~old_ringbuff ()
      {
        try {
          delete[] ringdata;
        }
        catch (...) {
        }
      };

      void add (T val)
      {
          ringdata[current] = val;
          current++;
          if (current == maxsize) current = 0;
      };

      T& get (int idx) {
          idx %= maxsize;
          return ringdata[idx];
      };

};


int main() {

// To use the default constructor, do not use any ()
// even if I have defined a "defaul" constructor

    old_ringbuff <std::string> rbuff;

// Parentheses and parameter for non default constructor
//    old_ringbuff <std::string> rbuff(3);

    rbuff.add ("one");
    rbuff.add ("two");
    rbuff.add ("three");
    rbuff.add ("four");

    for (int i=0; i<3; i++) {
        std::cout << "old_ringbuff i = " << i << " string = " <<
                rbuff.get(i) << std::endl;
    }

    return 0;
}
