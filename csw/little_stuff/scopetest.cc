
#include <iostream>

#include "csw/utils/private_include/csw_scope.h"


void tfunc2 ()
{
    int *iptr = NULL;
    iptr[0] = 1;
}



void tfunc () 
{
    auto fscope = [&]() {std::cout << "fscope lambda" << std::endl;};
    CSWScopeGuard  func_scope_guard (fscope);

    //tfunc2 ();
    return;
}


int main ()
{

    tfunc ();
    exit (0);

    return 0;
}


