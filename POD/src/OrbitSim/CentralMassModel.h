#ifndef POD_CENTRAL_MASS_MODEL_H
#define POD_CENTRAL_MASS_MODEL_H

#include "GravityModel.h"

using namespace gpstk;

namespace POD
{
    class CentralMassModel : public GravityModel
    {
    public:
        CentralMassModel(const GravityModelData &gMData);

        void doCompute(Epoch time, Spacecraft& sc);

        virtual void test() {};

    };
}

#endif //!POD_CENTRAL_MASS_MODEL_H