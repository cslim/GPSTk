#include "IonoStochasticModel.h"

using namespace gpstk;
using namespace std;

namespace pod
{
    void IonoStochasticModel::Prepare(const gpstk::SatID & sat, gpstk::IRinex & gData)
    {
        RandomWalkModel::Prepare(sat, gData);

        try
        {
            csFlag = (gData.getBody().getValue(sat, TypeID::CSL1) > 1) ? true : false;
			el = gData.getBody().getValue(sat, TypeID::elevation);
            return;
        }
        catch (SatIDNotFound &e)
        { }
        catch (TypeIDNotFound &e)
        { }

        el = 0;
        csFlag = true;
    }

    double IonoStochasticModel::getQ() const
    {
        if (csFlag)
        {
            return 1;
        }
        else
        {
            double k = cos(el*PI / 180.0);
            return   RandomWalkModel::getQ()*k*k;
        }
    }

    double IonoStochasticModel::getPhi() const
    {
        return   0;//csFlag ? 0 : 1;
    }

}