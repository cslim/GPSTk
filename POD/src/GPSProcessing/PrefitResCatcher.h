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


        // Inherited via ProcessingClass
        virtual gpstk::gnssSatTypeValue & Process(gpstk::gnssSatTypeValue & gData) override;

        virtual gpstk::gnssRinex & Process(gpstk::gnssRinex & gData) override;

        virtual std::string getClassName(void) const override;

        gpstk::TypeIDSet resTypes;

    };
}
