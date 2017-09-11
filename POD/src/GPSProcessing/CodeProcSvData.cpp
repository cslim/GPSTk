#include"CodeProcSvData.h"
#include"GeneralConstraint.hpp"

namespace pod
{
    ObsTypes CodeProcSvData ::obsTypes;
    CodeProcSvData::_init CodeProcSvData::_initializer;

     CodeProcSvData::_init::_init()
    {      
        RinexObsID idC1G(ObsID::ObservationType::otRange, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcCA);
        pair<TypeID, RinexObsID> pC1G (TypeID::C1, idC1G);

        RinexObsID idP1G(ObsID::ObservationType::otRange, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcW);
        pair<TypeID, RinexObsID> pP1G(TypeID::P1, idP1G);

        RinexObsID idP2G(ObsID::ObservationType::otRange, ObsID::CarrierBand::cbL2, ObsID::TrackingCode::tcW);
        pair<TypeID, RinexObsID> pP2G(TypeID::P2, idP2G);
       
        RinexObsID idS1G(ObsID::ObservationType::otSNR, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcCA);
        pair<TypeID, RinexObsID> pS1(TypeID::S1, idS1G);

        RinexObsID idS2G(ObsID::ObservationType::otSNR, ObsID::CarrierBand::cbL2, ObsID::TrackingCode::tcW);
        pair<TypeID, RinexObsID> pS2(TypeID::S2, idS2G);

        map<TypeID, RinexObsID> mG = { pC1G, pP1G, pP2G, pS1, pS2 };

        RinexObsID idC1R(ObsID::ObservationType::otRange, ObsID::CarrierBand::cbG1, ObsID::TrackingCode::tcGCA);
        pair<TypeID, RinexObsID> pC1R(TypeID::C1, idC1R);

        RinexObsID idP1R(ObsID::ObservationType::otRange, ObsID::CarrierBand::cbG1, ObsID::TrackingCode::tcGP);
        pair<TypeID, RinexObsID> pP1R(TypeID::P1, idP1R);

        RinexObsID idP2R(ObsID::ObservationType::otRange, ObsID::CarrierBand::cbG2, ObsID::TrackingCode::tcGP);
        pair<TypeID, RinexObsID> pP2R(TypeID::P2, idP2R);

        RinexObsID idS1R(ObsID::ObservationType::otSNR, ObsID::CarrierBand::cbG1, ObsID::TrackingCode::tcGCA);
        pair<TypeID, RinexObsID> pS1R(TypeID::S1, idS1R);

        RinexObsID idS2R(ObsID::ObservationType::otSNR, ObsID::CarrierBand::cbG2, ObsID::TrackingCode::tcGP);
        pair<TypeID, RinexObsID> pS2R(TypeID::S2, idS1R);

        map<TypeID, RinexObsID> mR = { pC1R, pP1R, pP2R, pS1R, pS2R };

        //
        pair<SatID::SatelliteSystem, map<TypeID, RinexObsID>> p1(SatID::SatelliteSystem::systemGPS, mG);
        pair<SatID::SatelliteSystem, map<TypeID, RinexObsID>> p2(SatID::SatelliteSystem::systemGlonass, mR);

        obsTypes = { p1, p2 };
    }


    bool CodeProcSvData::tryAdd(const SatID & id,const SvDataItem & item)
    {
        if (data.find(id) == data.end())
            data.insert(pair<SatID, SvDataItem>(id, item));
        else
            return false;

        satSyst.insert(id.system);
        return true;
    }
    bool CodeProcSvData::tryRemove(const SatID & id)
    {
        auto it = data.find(id);
        if (it != data.end())
            data.erase(it);
        else
            return false;

        satSyst.clear();

        for (auto & it : data)
            satSyst.insert(it.first.system);

        return true;
    }

    int CodeProcSvData::getNumUsedSv() const
    {
        int n = 0;
        for(auto & it :data)
            if (it.second.use)
                n++;
        
        return n;
    }
    int CodeProcSvData::getNumUsedSv(SatID::SatelliteSystem sys) const
    {
        int n = 0;
        for (auto & it : data)
            if (it.first.system == sys && it.second.use)
                n++;

        return n;
    }
    int CodeProcSvData::getParamNum() const
    {
        std::set<SatID::SatelliteSystem> ss;
        for (auto & it : data)
            if (it.second.use)
                ss.insert(it.first.system);
        return 3 + ss.size();
    }

    double CodeProcSvData::appendResid(Vector<double> sol, SatID::SatelliteSystem sys) const
    {
        if (getParamNum() > 4 && sys == SatID::SatelliteSystem::systemGlonass)
            return sol(4);
        else
            return 0.0;
    }
    int CodeProcSvData::getEquations(Matrix<double>& A, Matrix<double>& W, Vector<double>& resid)
    {

        /// find the number of good GLN satellites
        int numGLN = getNumUsedSv(SatID::SatelliteSystem::systemGlonass);
        //if number of GLN SV is lesss than 2 
        if (numGLN < 2)
        {
            // we will not use glonass satellites
            for (auto &it : data)
                if (it.first.system == SatID::SatelliteSystem::systemGlonass)
                    it.second.use = false;
        }

        // find the number of good satellites
        int N = getNumUsedSv();
        if (N < 4) return -1;
        // define for computation
        int K = getParamNum();
        A = Matrix<double>(N, K);
        resid = Vector<double>(N);
        
        //weight matrix
        W = Matrix<double>(N, N);
        ident(W);
        static const double elev0(30.0);
        static const double sin0(::sin(elev0 * DEG_TO_RAD));
        static const double glnSigmaFactor = 1;

        int i_eq = 0;
        for (auto &it :data)
        {
            if (!it.second.use) continue;

            for (size_t j = 0; j < 3; j++)
                A(i_eq, j) = it.second.alph[j];
            A(i_eq, 3) = 1.0;

            double invsig = 1.0;
            if (it.second.el >0 && it.second.el < elev0)
                 invsig = (::sin(it.second.el * DEG_TO_RAD) / sin0);

            W(i_eq, i_eq) = invsig*invsig;

            if (K > 4)
            {
                if (it.first.system == SatID::SatelliteSystem::systemGlonass)
                {
                    A(i_eq, 4) = 1.0;
                    W(i_eq, i_eq) /= glnSigmaFactor;
                }
                else
                    A(i_eq, 4) = 0;
            }
            
            resid(i_eq) = it.second.resid;
            i_eq++;
        }
        return 0;
    }

    void CodeProcSvData::resetUseFlags(bool newValue)
    {
        for (auto & it : data)
            it.second.use = newValue;
    }

    void CodeProcSvData::applyCNoMask(double CNoMask)
    {
        for (auto & it : data)
            it.second.use = (it.second.snr > CNoMask) ? true : false;
    }
    void CodeProcSvData::clear()
    {
        data.clear();
        satSyst.clear();
    }
    // stream output for CodeSolverBase
    std::ostream& operator<<(std::ostream& os, const CodeProcSvData& svData)
    {
        os << svData.data.size() << " " << svData.getNumUsedSv(SatID::SatelliteSystem::systemGPS) << " " << svData.getNumUsedSv(SatID::SatelliteSystem::systemGlonass);
        return os;
    }
}