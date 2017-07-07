#ifndef POD_ORBIT_MODEL_H
#define POD_ORBIT_MODEL_H

#include"ForceModelData.h"

#include"ForceList.h"
#include"EquationOfMotion.hpp"


using namespace gpstk;
namespace POD
{
    class OrbitModel : public EquationOfMotion
    {
    public:

        /// Default constructor
        OrbitModel();

        OrbitModel(const ForceModelData& fmc);

        /// Default destructor
        virtual ~OrbitModel()
        {
            forceList.clear();
        }

        virtual Vector<double> getDerivatives(const double&t, const Vector<double>& y);

        /// Restore the default setting
        OrbitModel& reset(const ForceModelData& fmc);

        /// set reference epoch
        OrbitModel& setRefEpoch(const Epoch & t)
        {
            t0 = t; 
            return (*this);
        }

        /// get reference epoch
        Epoch getRefEpoch() const
        {
            return t0;
        }

    protected:

        /// Reference epoch
        Epoch t0;

        /// Spacecraft object
        Spacecraft sc;

        /// Force Model List
        ForceList forceList;

    };
}
#endif // !POD_ORBIT_MODEL_H
