
#pragma once
#include <iostream>

class Temperature{
    private:
        int home;
    public:
        int away;
        int night;
        int long_absence;    
        Temperature(int h, int a, int n, int long_abs);
        void printTemp();
};