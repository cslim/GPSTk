#include"LinearCombination.h"
#include"GNSSconstants.hpp"

namespace pod
{
    /* Iono-Free wavelength according to equation 20.47 (pg. 591) in 
       "Peter J.G. Teunissen, Oliver Montenbruck (Eds.) 
       Springer Handbook of Global Navigation Satellite Systems"
     */
    double LinearCombination::getIonoFreeWaveLength(const gpstk::SatID &sv, int band1, int band2)
    {
        int fcn = sv.getGloFcn();
        double wlL1 = getWavelength(sv, 1, fcn);
        double wlL2 = getWavelength(sv, 2, fcn);

        return C_MPS / (wlL1 + wlL2);
    }

    #pragma region MWoubenna
    bool MWoubenna::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;
        const auto&  tC1 = useC1 ? TypeID::C1 : TypeID::P1;

        const auto&  itC1 = tvMap.find(tC1);
        if (itC1 == tvMap.end()) return false;

        const auto&  itC2 = tvMap.find(TypeID::P2);
        if (itC2 == tvMap.end()) return false;

        const auto&  itL1 = tvMap.find(TypeID::L1);
        if (itL1 == tvMap.end()) return false;

        const auto&  itL2 = tvMap.find(TypeID::L2);
        if (itL2 == tvMap.end()) return false;

        int fcn = sv.getGloFcn();
        double F1 = C_MPS / getWavelength(sv, 1, fcn);
        double F2 = C_MPS / getWavelength(sv, 2, fcn);

        value = (itL1->second * F1 - itL2->second * F2) / (F1 - F2) -
            (itC1->second * F1 + itC2->second * F2) / (F1 + F2);

        return true;
    }

    TypeID MWoubenna::getType() const
    {
        return TypeID::MWubbena;
    }
    #pragma endregion

    #pragma region PDelta
    bool PDelta::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        auto tC1 = useC1 ? TypeID::C1 : TypeID::P1;

        const auto&  itC1 = tvMap.find(tC1);
        if (itC1 == tvMap.end()) return false;

        const auto&  itC2 = tvMap.find(TypeID::P2);
        if (itC2 == tvMap.end()) return false;

        int fcn = sv.getGloFcn();
        double F1 = C_MPS / getWavelength(sv, 1, fcn);
        double F2 = C_MPS / getWavelength(sv, 2, fcn);

        double c = F1 / (F1 + F2);
        double d = F2 / (F1 + F2);

        value = c* itC1->second + d* itC2->second;

        return true;
    }

    TypeID PDelta::getType() const
    {
        return TypeID::Pdelta;
    }
    #pragma endregion

    #pragma region LDelta
    bool LDelta::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        const auto&  itL1 = tvMap.find(TypeID::L1);
        if (itL1 == tvMap.end()) return false;

        const auto&  itL2 = tvMap.find(TypeID::L2);
        if (itL2 == tvMap.end()) return false;

        int fcn = sv.getGloFcn();
        double F1 = C_MPS / getWavelength(sv, 1, fcn);
        double F2 = C_MPS / getWavelength(sv, 2, fcn);

        double e = F1 / (F1 - F2);
        double f = F2 / (F1 - F2);

        value = e* itL1->second + f* itL2->second;

        return true;
    }

    TypeID LDelta::getType() const
    {
        return TypeID::Ldelta;
    }
    #pragma endregion

    #pragma region PC
    bool PCCombimnation::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        const auto&  tC1 = useC1 ? TypeID::C1 : TypeID::P1;

        const auto&  itC1 = tvMap.find(tC1);
        if (itC1 == tvMap.end()) return false;

        const auto&  itC2 = tvMap.find(TypeID::P2);
        if (itC2 == tvMap.end()) return false;

        int fcn = sv.getGloFcn();
        double F1 = C_MPS / getWavelength(sv, 1, fcn);
        double F2 = C_MPS / getWavelength(sv, 2, fcn);

        F1 = F1 * F1;
        F2 = F2 * F2;

        value = (F1 * itC1->second - F2 * itC2->second) / (F1 - F2);

        return true;
    }

    TypeID PCCombimnation::getType() const
    {
        return TypeID::PC;
    }
    #pragma endregion

    #pragma region LC
    bool LCCombimnation::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        const auto&  itL1 = tvMap.find(TypeID::L1);
        if (itL1 == tvMap.end()) return false;

        const auto&  itL2 = tvMap.find(TypeID::L2);
        if (itL2 == tvMap.end()) return false;

        int fcn = sv.getGloFcn();
        double F1 = C_MPS / getWavelength(sv, 1, fcn);
        double F2 = C_MPS / getWavelength(sv, 2, fcn);

        F1 = F1 * F1;
        F2 = F2 * F2;

        value = (F1 * itL1->second - F2 * itL2->second) / (F1 - F2);

        return true;
    }

    TypeID LCCombimnation::getType() const
    {
        return TypeID::LC;
    }
    #pragma endregion

    #pragma region LI
    bool LICombimnation::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        const auto& itL1 = tvMap.find(TypeID::L1);
        if (itL1 == tvMap.end()) return false;

        const auto& itL2 = tvMap.find(TypeID::L2);
        if (itL2 == tvMap.end()) return false;
        value = itL1->second - itL2->second;

        return true;
    }

    TypeID LICombimnation::getType() const
    {
        return TypeID::LI;
    }
    #pragma endregion

    #pragma region PrefitC1
    bool PrefitC1::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        // C1 code pseudorange
        auto& it = tvMap.find(TypeID::C1);
        if (it == tvMap.end())
            return false;
        else
            value = it->second;

        // calculated distance
        it = tvMap.find(TypeID::rho);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //SV clock offset
        it = tvMap.find(TypeID::dtSat);
        if (it == tvMap.end())
            return false;
        else
            value += it->second;

        //rel 
        it = tvMap.find(TypeID::rel);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //grav Delay
        it = tvMap.find(TypeID::gravDelay);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //sv phase center 
        it = tvMap.find(TypeID::satPCenter);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //tropo
        it = tvMap.find(TypeID::tropoSlant);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //ionospheric delay on L1
        it = tvMap.find(TypeID::ionoL1);
        if (it != tvMap.end())
            value -= it->second;

        //wind Up
        it = tvMap.find(TypeID::instC1);
        if (it != tvMap.end())
            value -= it->second;//*getWavelength(sv, 1, sv.getGloFcn()) / TWO_PI;

        //multipath L1
        it = tvMap.find(TypeID::mpC1);
        if (it != tvMap.end())
            value -= it->second;

        return true;
    }

    TypeID PrefitC1::getType() const
    {
        return TypeID::prefitC;
    }
#pragma endregion

    #pragma region PrefitP1

    bool PrefitP1::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        // P1 code pseudorange
        auto& it = tvMap.find(TypeID::P1);
        if (it == tvMap.end())
            return false;
        else
            value = it->second;

        // calculated distance
        it = tvMap.find(TypeID::rho);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //SV clock offset
        it = tvMap.find(TypeID::dtSat);
        if (it == tvMap.end())
            return false;
        else
            value += it->second;

        //rel 
        it = tvMap.find(TypeID::rel);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //grav Delay
        it = tvMap.find(TypeID::gravDelay);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //sv phase center 
        it = tvMap.find(TypeID::satPCenter);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //tropo
        it = tvMap.find(TypeID::tropoSlant);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //ionospheric delay on L1
        it = tvMap.find(TypeID::ionoL1);
        if (it != tvMap.end())
            value -= it->second;

        //wind Up
        it = tvMap.find(TypeID::instC1);
        if (it != tvMap.end())
            value -= it->second;//*getWavelength(sv, 1, sv.getGloFcn()) / TWO_PI;

                                //multipath L1
        it = tvMap.find(TypeID::mpC1);
        if (it != tvMap.end())
            value -= it->second;

        return true;
    }

    TypeID PrefitP1::getType() const
    {
        return TypeID::prefitP1;
    }

    #pragma endregion

    #pragma region PrefitP2

    bool PrefitP2::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        // P1 code pseudorange
        auto& it = tvMap.find(TypeID::P2);
        if (it == tvMap.end())
            return false;
        else
            value = it->second;

        // calculated distance
        it = tvMap.find(TypeID::rho);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //SV clock offset
        it = tvMap.find(TypeID::dtSat);
        if (it == tvMap.end())
            return false;
        else
            value += it->second;

        //rel 
        it = tvMap.find(TypeID::rel);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //grav Delay
        it = tvMap.find(TypeID::gravDelay);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //sv phase center 
        it = tvMap.find(TypeID::satPCenter);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //tropo
        it = tvMap.find(TypeID::tropoSlant);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //ionospheric delay on L2
        it = tvMap.find(TypeID::ionoL2);
        if (it != tvMap.end())
            value -= it->second;

        
        //multipath P2/C2
        it = tvMap.find(TypeID::mpC2);
        if (it != tvMap.end())
            value -= it->second;

        return true;
    }

    TypeID PrefitP2::getType() const
    {
        return TypeID::prefitP2;
    }

   #pragma endregion

    #pragma region PrefitL1
    bool PrefitL1::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        // L1 phase pseudorange
        auto& it = tvMap.find(TypeID::L1);
        if (it == tvMap.end())
            return false;
        else
            value = it->second;

        // calculated distance
        it = tvMap.find(TypeID::rho);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //SV clock offset
        it = tvMap.find(TypeID::dtSat);
        if (it == tvMap.end())
            return false;
        else
            value += it->second;

        //rel 
        it = tvMap.find(TypeID::rel);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //grav Delay
        it = tvMap.find(TypeID::gravDelay);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //sv phase center 
        it = tvMap.find(TypeID::satPCenter);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //tropo
        it = tvMap.find(TypeID::tropoSlant);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //ionospheric delay on L1
        it = tvMap.find(TypeID::ionoL1);
        if (it != tvMap.end())
            value += it->second;

        //wind Up
        it = tvMap.find(TypeID::windUp);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second*getWavelength(sv, 1, sv.getGloFcn()) / TWO_PI;

        //multipath L1
        it = tvMap.find(TypeID::mpL1);
        if (it != tvMap.end())
            value -= it->second;

        return true;
    }

    TypeID PrefitL1::getType() const
    {
        return TypeID::prefitL1;
    }
    #pragma endregion

    #pragma region PrefitL2
    bool PrefitL2::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        // L1 phase pseudorange
        auto& it = tvMap.find(TypeID::L2);
        if (it == tvMap.end())
            return false;
        else
            value = it->second;

        // calculated distance
        it = tvMap.find(TypeID::rho);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //SV clock offset
        it = tvMap.find(TypeID::dtSat);
        if (it == tvMap.end())
            return false;
        else
            value += it->second;

        //rel 
        it = tvMap.find(TypeID::rel);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //grav Delay
        it = tvMap.find(TypeID::gravDelay);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //sv phase center 
        it = tvMap.find(TypeID::satPCenter);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //tropo
        it = tvMap.find(TypeID::tropoSlant);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //ionospheric delay on L1
        it = tvMap.find(TypeID::ionoL2);
        if (it != tvMap.end())
            value += it->second;

        //wind Up
        it = tvMap.find(TypeID::windUp);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second*getWavelength(sv, 2, sv.getGloFcn()) / TWO_PI;

        //multipath L1
        it = tvMap.find(TypeID::mpL2);
        if (it != tvMap.end())
            value -= it->second;

        return true;
    }

    TypeID PrefitL2::getType() const
    {
        return TypeID::prefitL2;
    }

    #pragma endregion

    #pragma region PrefitLC
    bool PrefitLC::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        // L1 phase pseudorange
        auto& it = tvMap.find(TypeID::LC);
        if (it == tvMap.end())
            return false;
        else
            value = it->second;

        // calculated distance
        it = tvMap.find(TypeID::rho);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //SV clock offset
        it = tvMap.find(TypeID::dtSat);
        if (it == tvMap.end())
            return false;
        else
            value += it->second;

        //rel 
        it = tvMap.find(TypeID::rel);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //grav Delay
        it = tvMap.find(TypeID::gravDelay);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //sv phase center 
        it = tvMap.find(TypeID::satPCenter);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //tropo
        it = tvMap.find(TypeID::tropoSlant);
        if (it == tvMap.end())
            return false;
        else
            value -= it->second;

        //wind Up
        it = tvMap.find(TypeID::windUp);
        if (it == tvMap.end())
            return false;
        else
        {
            double wu_wl = getIonoFreeWaveLength(sv, 1, 2);
            value -= it->second*wu_wl / TWO_PI;
        }
        return true;
    }

    TypeID PrefitLC::getType() const
    {
        return TypeID::prefitL;
    }
    #pragma endregion

    #pragma region CodeIonoDelayL1

    bool CodeIonoDelayL1::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        const auto&  tC1 = useC1 ? TypeID::C1 : TypeID::P1;

        const auto&  itC1 = tvMap.find(tC1);
        if (itC1 == tvMap.end()) return false;

        const auto& itP2 = tvMap.find(TypeID::P2);
        if (itP2 == tvMap.end()) return false;

        int fcn = sv.getGloFcn();
        double F1 = C_MPS / getWavelength(sv, 1, fcn);
        double F2 = C_MPS / getWavelength(sv, 2, fcn);

        F1 *= F1;
        F2 *= F2;

        value =(itC1->second - itP2->second) * F2  / (F2 - F1);

        return true;
    }

    TypeID CodeIonoDelayL1::getType() const
    {
        return TypeID::ionoL1;
    }


   #pragma endregion

    #pragma region PhaseIonoDelayL1


    bool PhaseIonoDelayL1::getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const
    {
        value = NAN;

        const auto&  itC1 = tvMap.find(TypeID::L1);
        if (itC1 == tvMap.end()) return false;

        const auto& itP2 = tvMap.find(TypeID::L2);
        if (itP2 == tvMap.end()) return false;

        int fcn = sv.getGloFcn();
        double F1 = C_MPS / getWavelength(sv, 1, fcn);
        double F2 = C_MPS / getWavelength(sv, 2, fcn);

        F1 *= F1;
        F2 *= F2;

        value = (itC1->second - itP2->second)* F2 / (F2 - F1);

        return true;
    }

    TypeID PhaseIonoDelayL1::getType() const
    {
        return TypeID::ionoL1;
    }

#pragma endregion

}
