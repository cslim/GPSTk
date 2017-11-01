#include"LinearCombination.h"
#include"GNSSconstants.hpp"

namespace pod
{
    bool MWoubenna::getCombination(const SatID & sv, typeValueMap tvMap, double & value) const
    {
        value = NAN;
        auto tC1 = useC1 ? TypeID::C1 : TypeID::P1;

        auto itC1 = tvMap.find(tC1);
        if (itC1 == tvMap.end()) return false;

        auto itC2 = tvMap.find(TypeID::P2);
        if (itC2 == tvMap.end()) return false;

        auto itL1 = tvMap.find(TypeID::L1);
        if (itL1 == tvMap.end()) return false;

        auto itL2 = tvMap.find(TypeID::L2);
        if (itL2 == tvMap.end()) return false;

        int fcn = sv.getGloFcn();
        double F1 = C_MPS/  getWavelength(sv, 1, fcn);
        double F2 = C_MPS/  getWavelength(sv, 2, fcn);

        value = (itL1->second * F1 - itL2->second * F2) / (F1 - F2) - 
            (itC1->second * F1 + itC2->second * F2) / (F1 + F2);
        
        return true;
    }

    TypeID MWoubenna::getType() const
    {
        return TypeID::MWubbena;
    }

    bool PDelta::getCombination(const SatID & sv, typeValueMap tvMap, double & value) const
    {
        value = NAN;

        auto tC1 = useC1 ? TypeID::C1 : TypeID::P1;

        auto itC1 = tvMap.find(tC1);
        if (itC1 == tvMap.end()) return false;

        auto itC2 = tvMap.find(TypeID::P2);
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
    bool LDelta::getCombination(const SatID & sv, typeValueMap tvMap, double & value) const
    {
        value = NAN;

        auto itL1 = tvMap.find(TypeID::L1);
        if (itL1 == tvMap.end()) return false;

        auto itL2 = tvMap.find(TypeID::L2);
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

    bool PCCombimnation::getCombination(const SatID & sv, typeValueMap tvMap, double & value) const
    {
        value = NAN;

        auto tC1 = useC1 ? TypeID::C1 : TypeID::P1;

        auto itC1 = tvMap.find(tC1);
        if (itC1 == tvMap.end()) return false;

        auto itC2 = tvMap.find(TypeID::P2);
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

    bool LCCombimnation::getCombination(const SatID & sv, typeValueMap tvMap, double & value) const
    {
        value = NAN;

        auto itL1 = tvMap.find(TypeID::L1);
        if (itL1 == tvMap.end()) return false;

        auto itL2 = tvMap.find(TypeID::L2);
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
    bool LICombimnation::getCombination(const SatID & sv, typeValueMap tvMap, double & value) const
    {
        value = NAN;

        auto itL1 = tvMap.find(TypeID::L1);
        if (itL1 == tvMap.end()) return false;

        auto itL2 = tvMap.find(TypeID::L2);
        if (itL2 == tvMap.end()) return false;
        value = itL1->second -  itL2->second;

        return true;
    }

    TypeID LICombimnation::getType() const
    {
        return TypeID::LI;
    }
}




