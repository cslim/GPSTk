#pragma once
#include"ProcessingClass.hpp"
namespace pod
{
    class PrefitResCatcher
        :public gpstk::ProcessingClass
    {
    public:
        PrefitResCatcher() {};
        PrefitResCatcher(const gpstk::TypeIDSet& types) :resTypes(types)
        {};
        virtual ~PrefitResCatcher() {};

        virtual gpstk::IRinex & Process(gpstk::IRinex & gData) override;

        virtual std::string getClassName(void) const override;

        gpstk::TypeIDSet resTypes;

    };
}
