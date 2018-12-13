#include "RinexEpoch.h"
using namespace std;
namespace gpstk
{


    RinexEpoch::
        RinexEpoch()
    { }

    RinexEpoch::
        RinexEpoch(const gpstk::gnssRinex & gRin)
        :rinex(gRin)
    { }

    RinexEpoch::
        ~RinexEpoch()
    { }

    void RinexEpoch::
        resetCurrData()
    {
        currData.clear();
        for (auto && it : rinex.body)
            currData.emplace(it.first, make_unique<TypeValueMapPtr>(it.second));
    }

    RinexEpoch RinexEpoch::
        extractSatID(const gpstk::SatID& satellite) const
    {
        return RinexEpoch(rinex.extractSatID(satellite));
    }

    RinexEpoch RinexEpoch::
        extractSatID(const int& p, const GpstkSatSystem& s) const
    {
        return RinexEpoch(rinex.extractSatID(p,s));
    }

    RinexEpoch RinexEpoch::
        extractSatID(const gpstk::SatIDSet& satSet) const
    {
        return RinexEpoch(rinex.extractSatID(satSet));
    }

    RinexEpoch RinexEpoch::
        extractSatSyst(const gpstk::SatSystSet& satSet) const
    {
        return RinexEpoch(rinex.extractSatSyst(satSet));
    }

    RinexEpoch& RinexEpoch::
        keepOnlySatID(const gpstk::SatID& satellite)
    {
        rinex.keepOnlySatID(satellite);
        return *this;
    }

    RinexEpoch& RinexEpoch::
        keepOnlySatID(const int& p, const GpstkSatSystem& s)
    {
        rinex.keepOnlySatID(p,s);
        return *this;
    }

    RinexEpoch& RinexEpoch::
        keepOnlySatID(const gpstk::SatIDSet& satSet)
    {
        rinex.keepOnlySatID(satSet);
        return *this;
    }

    RinexEpoch& RinexEpoch::
        keepOnlySatSystems(GpstkSatSystem satSyst)
    {
        rinex.keepOnlySatSystems(satSyst);
        return *this;
    }

    RinexEpoch& RinexEpoch::
        keepOnlySatSystems(const gpstk::SatSystSet& satSet)
    {
        rinex.keepOnlySatSystems(satSet);
        return *this;
    }

    RinexEpoch RinexEpoch::
        extractTypeID(const gpstk::TypeID& type) const
    {
        return RinexEpoch(rinex.extractTypeID(type));
    }

    RinexEpoch RinexEpoch::
        extractTypeID(const gpstk::TypeIDSet& typeSet) const
    {
        return RinexEpoch(rinex.extractTypeID(typeSet));
    }

    RinexEpoch& RinexEpoch::
        keepOnlyTypeID(const gpstk::TypeID& type)
    {
        rinex.keepOnlyTypeID(type);
        return *this;
    }

    RinexEpoch& RinexEpoch::
        keepOnlyTypeID(const gpstk::TypeIDSet& typeSet)
    {
        rinex.keepOnlyTypeID(typeSet);
        return *this;
    }
}