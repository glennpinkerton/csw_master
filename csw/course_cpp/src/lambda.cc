/*
 * I (Glenn) think of lambda expressions as chunks of code
 * that can be passed around and used.  The syntax is a bit
 * of a "bracket fest" but essentially it is:
 *
 *   [](){}
 *
 * The square barckets "[]" are the definitive part signalling
 * a lambda.  The parentheses "()" are used very much like the
 * () in a function definition.  The optional list of parameters
 * used by the lambda are listed here.  The curly braces "{}"
 * have the body of the lambda, very much like the body of a
 * function.  
 */

#include <iostream>
#include <functional>

// The lambda can be passed as a function object
// The <functional> header is needed for this.

void LambdaTest1 (std::function<void()> f) 
{
    f();
}

// The tutorial uses straight function pointer syntax:
// (void(*f)())

void LambdaTest2 (void(*f)())
{
    f();
}


// Receive divide lambda as function object or "old style"
// function pointer.

void LambdaTest3 (std::function<double(double, double)> f) 
{
    std::cout << "From lambda test 3: " << f(12.0, 4.0) << std::endl;
}

void LambdaTest4 (double (*f) (double a, double b))
{
    double rval = f(10.0, 4.0);
    std::cout << "From lambda test 4, " << rval << std::endl;
}



// I put this here to show that a free standing function
// can be called from inside a lambda expression.

bool NearZero (double dval) {
    if (dval > -1.e-20  &&  dval < 1.e-20) {
        return true;
    }
    return false;
}


void LambdaTest6 (std::function<void()> f) {
    f ();
}



/*
 * Example of "this" in capture expression.  This means that all the 
 * object variables are captured by reference for use by the lambda.
 */
class LC {

  private:

    int   iv2 = 200;

  public:

    int   iv1 = 100;

    void run ()
    {
        double  dv1 = 3.579;
      // this in the capture expression.  Do not mix = for
      // capture by value with "this".  The mix will not compile.
      // It will at least generate a warning.  I (Glenn) think
      // I will always just use the "&, this" expression when I 
      // want to capture the object variables.
        auto lclamb = [&, this]() {
            std::cout << "this iv2 = " << iv2 << std::endl;
            std::cout << "local dv1 = " << dv1 << std::endl;
            iv1 = 9999;
            dv1 = 1.234;
        };
        LambdaTest6 (lclamb);
        std::cout << "after LambdaTest6 dv1 = " << dv1 << std::endl;
    }

};
    



int main() {

    auto lambda_1 = [](){std::cout << "from lambda 1" << std::endl;};

    LambdaTest1 (lambda_1);
    LambdaTest2 (lambda_1);

// The compiler will try to infer the return type based on
// any return statements in the body of the code.  If there
// is more than one return statement, and they return different
// data types, the compiler cannot infer the return type.
// The -> double explicitly says to return double.

    auto lambda_divide = [](double a, double b) -> double
    {
        if (NearZero (b)) {
            return 0;  // inferred return of int
        }
        return a / b;  // inferred return of double
    };

    LambdaTest3 (lambda_divide);
    LambdaTest4 (lambda_divide);

/*
 * The square brackets "[]" can be empty or they can have a
 * "capture expression" inside.  The capture expressions are 
 * a way of making "local" variables available inside the
 * lambda.  The most popular capture expressions are [=] to
 * capture all local variables by value  and [&] to capture 
 * all local variables by reference.  For example:
 */
    int    iv1 = 5;
    int    iv2 = 10;
    double dv1 = 100.0;
    double dv2 = 10.0;

    auto lambda_cap_ref = [&]() {
        std::cout << "capture all by reference: " <<
                     dv1 / dv2 << " , " << iv1 * iv2 << std::endl;
    };
    LambdaTest1 (lambda_cap_ref);

/*
 * Notice that the "local" variables are all available to the chunk of
 * code even though the chunk is run by a function that knows nothing
 * about iv1, iv2 etc.  This is a bit magical and it works.  I have no
 * need to determine the "under the hood" details.
 */

/*
 * The following does the same stuff, but by value.  The values are
 * also const (cannot be changed, even locally in the lambda),
 */
    auto lambda_cap_val = [=]() {
        std::cout << "capture all by value: " <<
                     dv1 / dv2 << " , " << iv1 * iv2 << std::endl;
    };
    LambdaTest1 (lambda_cap_val);

/*
 * To let variables captured by value be changed, the mutable 
 * keyword must be specified between the () and the {}.  The
 * value is only changed in the scope of the lambda.
 */
    auto lambda_cap_val2 = [=]() mutable {
        dv1 = 1.234;
        std::cout << "capture all by mutable value: " <<
                     dv1 / dv2 << " , " << iv1 * iv2 << std::endl;
    };
    LambdaTest1 (lambda_cap_val);
    LambdaTest1 (lambda_cap_val2);

/*
 * Ref and value can be mixed by declaring all to be one or the
 * other by default and then listing exceptions.  For example:
 *   [=, &iv1]  all except iv1 by value, iv1 by ref
 *   [&, dv2]  all except dv2 by ref, dv2 by value
 */


    std::cout << std::endl;

    LC  lc_obj;

    std::cout << "old LC iv1 value = " << lc_obj.iv1 << std::endl;

    lc_obj.run ();

    std::cout << "new LC iv1 value = " << lc_obj.iv1 << std::endl;

    return 0;
}
