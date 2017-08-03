#pragma once

#include"TypeID.hpp"


using namespace gpstk;

class PrSmoother
{
public:
    PrSmoother();
    PrSmoother(const std::list<TypeID>& tList, int l);
    ~PrSmoother();

    void smooth(const char * path);
private:
    std::list<TypeID>  codes;
    int window;


};

