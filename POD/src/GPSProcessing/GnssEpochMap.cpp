#include"GnssEpochMap.h"

using namespace gpstk;

namespace pod
{

    GnssEpoch::GnssEpoch()
    { }

    GnssEpoch::GnssEpoch(const satTypeValueMap&  sData) :  satData(sData)
    { }
    
	GnssEpoch::GnssEpoch(const gpstk::gnssRinex& gRin) :satData(gRin.body)
    { }

	GnssEpoch::GnssEpoch(const gpstk::SatTypePtrMap&  stpMap) 
	{
		for (auto && it : stpMap)
			satData.emplace(it.first, it.second->get_value());
	}

    GnssEpoch::~GnssEpoch()
    { }

    /// Method to print data values
    std::ostream& GnssEpoch::dump(std::ostream& s, int precision )
    {
        s << std::fixed << std::setprecision(precision);
        s << "Satellite Data" << std::endl;
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
            s << std::endl;
        }
        s << "Solution Data" << std::endl;
        for (auto &itObs : this->slnData)
        {
                // Print type names and values
                s << (itObs).first << " ";
                s << (itObs).second << " ";
        }
        s << std::endl;
        return s;
    }

    //
    GnssEpochMap::GnssEpochMap()
    { }

    GnssEpochMap::~GnssEpochMap()
    { }



    void GnssEpochMap::updateTypes(const TypeIDSet & newTypes)
    {
        types.insert(newTypes.begin(), newTypes.end());
    }

    void GnssEpochMap::updateMetadata()
    {
        svs.clear();
        slnTypes.clear();
        types.clear();
        types.insert(TypeID::recSlnType);

        for (auto && epoch : data)
        {
            for (auto && svRcord : epoch.second.satData)
            {
                //update list of SV 
                svs.insert(svRcord.first);
                //update list of typeID 
                for (const auto & data : svRcord.second)
                    types.insert(data.first);
            }

            //update list of  typeID
            for (auto && data : epoch.second.slnData)
                types.insert(data.first);
            //update list of Solution types
            const auto& st = epoch.second.slnData.find(TypeID::recSlnType);

            if (st == epoch.second.slnData.end())
            {
                epoch.second.slnData[TypeID::recSlnType] = 0;
                slnTypes.insert(0);
            }
                
            else
                slnTypes.insert(st->second);
        }
    }

    /// Method to print data values
    std::ostream& GnssEpochMap::dump(std::ostream& s, int precision)
    {
        // Prepare for printing
        s << std::fixed << std::setprecision(precision);
        //for (auto it = data->begin(); it != this->end(); ++it)
        for (auto &it :data)
        {
            s << CivilTime(it.first) << std::endl;
            it.second.dump(s, precision);
        }
        return s;
    }
}