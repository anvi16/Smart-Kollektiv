
#include "ClassTest.h"


// Constructor
Temperature::Temperature(int h, int a, int n, int long_abs) :   home{h},
                                                                away{a},
                                                                night{n},
                                                                long_absence{long_abs}  {
 
}

void Temperature::printTemp(){
    std::cout<<"test";
}