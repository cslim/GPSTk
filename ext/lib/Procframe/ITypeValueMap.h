#pragma once
#include"DataStructures.hpp"

namespace gpstk
{
    class ITypeValueMap
    {

    public:
        virtual ~ITypeValueMap() {};
        virtual typeValueMap& get_value() = 0;
    };

    class TypeValueMapPtr : public ITypeValueMap
    {
    public:
        TypeValueMapPtr(typeValueMap * p) :ptr(p) {};
        TypeValueMapPtr(typeValueMap & p) :ptr(&p) {};

        virtual typeValueMap& get_value()
        {
            return *ptr;
        };

        std::shared_ptr<typeValueMap> ptr;
    };
}

