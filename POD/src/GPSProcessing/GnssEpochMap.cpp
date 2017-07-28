#include"GnssEpochMap.h"

using namespace std;

namespace pod
{

    GnssEpoch::GnssEpoch()
    { }

    GnssEpoch::GnssEpoch(const satTypeValueMap&  sData) :  satData(sData)
    { }
    GnssEpoch::GnssEpoch(const gpstk::gnssRinex& gRin) :satData(gRin.body)
    { }

    GnssEpoch::~GnssEpoch()
    { }

    /// Method to print data values
    std::ostream& GnssEpoch::dump(std::ostream& s, int precision )
    {
        s << fixed << setprecision(precision);
        s << "Satellite Data" << endl;
        for (auto &it : this->satData)
        {
            //print satellite
            s << (it).first << " ";
            for (auto itObs : (it).second)
            {
                // Print type names and values
                s << (itObs).first << " ";
                s << (itObs).second << " ";

            }  
            s << endl;
        }
        s << "Solution Data" << endl;
        for (auto &itObs : this->slnData)
        {
                // Print type names and values
                s << (itObs).first << " ";
                s << (itObs).second << " ";
        }
        s << endl;     
        return s;
    }

    //
    GnssEpochMap::GnssEpochMap()
    { }

    GnssEpochMap::~GnssEpochMap()
    { }

    /// Method to print data values
    std::ostream& GnssEpochMap::dump(std::ostream& s, int precision)
    {
        // Prepare for printing
        s << fixed << setprecision(precision);
        //for (auto it = data->begin(); it != this->end(); ++it)
        for (auto &it :data)
        {
            s << CivilTime(it.first) << endl;
            it.second.dump(s, precision);
        }
        return s;
    }
}