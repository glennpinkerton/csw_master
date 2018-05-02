#include <iostream>

class Complex {
private:

    double  real = 0.0;
    double  imag = 0.0;

public:

    Complex () {}

    Complex ( double real, double imag) :
        real (real),
        imag (imag)
    {}

    Complex (const Complex& other) {
        real = other.real;
        imag = other.imag;
    }

    const Complex &operator= (const Complex &other) {
        real = other.real;
        imag = other.imag;
        return *this;
    }

    bool operator< (const Complex &other) const {
        double  dt1, dt2;
        dt1 = other.real;
        dt2 = other.imag;
        double d1 = dt1 * dt1 + dt2 * dt2;
        double d2 = real * real + imag * imag;
        return (d2 < d1);
    }

    bool operator> (const Complex &other) const {
        double  dt1, dt2;
        dt1 = other.real;
        dt2 = other.imag;
        double d1 = dt1 * dt1 + dt2 * dt2;
        double d2 = real * real + imag * imag;
        return (d2 > d1);
    }

// overload of == and != are done here as public class methods.  They
// could also be done as friends and stand alone functions.  In a 
// "real world" class, >= and <= would also be defined in similar
// fashion.

    bool operator== (const Complex &other) const {
        bool bret = false;
        if (real == other.real  &&  imag == other.imag) {
            bret = true;
        }
        return bret;
    }

    bool operator!= (const Complex &other) const {
        bool bret = true;
        if (real == other.real  &&  imag == other.imag) {
            bret = false;
        }
        return bret;
    }

// overload defre operator "*" to treturn complex conjugate
   
    Complex operator* () {
        Complex ctmp (real, -imag);
        return ctmp;
    }

    friend bool pcomp (Complex &a, Complex &b);

    void print() const {
        std::cout << "(" << real << " , " << imag << ")" << std::flush;
    }

// overload the << operator when it is use with ostream. If the
// operator needs access to the Complex class private stuff, the
// stand alone function must be defined as a friend here.  Note 
// that the actual operator<< function is NOT a member function of
// the Complex class.

    friend std::ostream &operator<< (std::ostream &out, const Complex &c);

// In a real world class, all the various possible arithmetic operator
// overloads could be prototyped here as friends and then implemented
// as stand alone functions outsife of the class.  As an example, I only
// implement the + operator.

    friend Complex operator+ (const Complex &c1, const Complex &c2);
    friend Complex operator+ (const Complex &c1, double dval);
    friend Complex operator+ (double dval, const Complex &c1);

};  // end of Complex class definition


bool pcomp (Complex &a, Complex &b) {
    return !(a < b);
}

// overloaded operator does not have to be in a class definition.

std::ostream &operator<< (std::ostream &out, const Complex &c) {
    out << "(" << c.real << " , " << c.imag << ")" << std::flush;
    return out;
}

Complex operator+ (const Complex &c1, const Complex &c2) {
    double dr, di;
    dr = c1.real + c2.real;
    di = c1.imag + c2.imag;
    Complex c3 (dr, di);
    return c3;
}

Complex operator+ (const Complex &c1, double dval) {
    double dr, di;
    dr = c1.real + dval;
    di = c1.imag;
    Complex c3 (dr, di);
    return c3;
}

Complex operator+ (double dval, const Complex &c1) {
    double dr, di;
    dr = c1.real + dval;
    di = c1.imag;
    Complex c3 (dr, di);
    return c3;
}

int main() {

    Complex   c1 (1.0, 0.0);
    Complex   c2 (0.0, 2.0);
    Complex   c3 (0.0, 0.5);
    
    std::cout << "c1 = " << c1 << std::endl;
    std::cout << "c2 = " << c2 << std::endl;
    std::cout << "c3 = " << c3 << std::endl;

    std::cout << "*c3 = " << *c3 << std::endl;

    if (c1 < c2) {
        std::cout << "c1 is less than c2" << std::endl;
    }
    else {
        std::cout << "c1 is greater than or equal to c2" << std::endl;
    }

    if (c1 < c3) {
        std::cout << "c1 is less than c3" << std::endl;
    }
    else {
        std::cout << "c1 is greater than or equal to c3" << std::endl;
    }

    std::cout << "c1 + c2 = " << c1 + c2 << std::endl;
    std::cout << "c1 + c3 = " << c1 + c3 << std::endl;
    std::cout << "c1 + 3.5 = " << c1 + 3.5 << std::endl;
    std::cout << "3.5 + c2 = " << 3.5 + c2 << std::endl;

    std::cout << 3.14159 + c1 + c3 + -3.14159 + c1 << std::endl;

    return 0;
}
