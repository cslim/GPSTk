#include "AmbiguitySdEquations.h"
#include"GNSSconstants.hpp"
#include"LinearCombination.h"

using namespace gpstk;

namespace pod
{
    ParametersSet AmbiguitySdEquations::getAmbSet() const
    {
        ParametersSet ambSet;
        for (const auto& it : csFlags)
            ambSet.insert(FilterParameter(type, it.first));
        return ambSet;
    }

    void AmbiguitySdEquations::Prepare(gpstk::gnssRinex & gData)
    {
        currentSatSet = gData.getSatID();
        
        //update satellites set
        satSet.insert(currentSatSet.begin(), currentSatSet.end());
        
        csFlags.clear();

        for (const auto& it :satSet)
        {
            stochModel.Prepare(it, gData);
            csFlags[it] = stochModel.getCS();
        }

        satSet = currentSatSet;
    }
    
    void AmbiguitySdEquations::updateEquationTypes(gpstk::gnssRinex & gData, gpstk::TypeIDSet & eqTypes)
    {
        //do nothing
    }
    
    gpstk::TypeIDSet AmbiguitySdEquations::getEquationTypes() const
    {
        return gpstk::TypeIDSet();
    }

    void AmbiguitySdEquations::updatePhi(gpstk::Matrix<double>& Phi, int & index) const
    {
        for (auto &it : csFlags)
        {
            stochModel.setCS(it.second);
            Phi(index, index) = stochModel.getPhi();
            ++index;
        }
    }
    
    void AmbiguitySdEquations::updateQ(gpstk::Matrix<double>& Q, int & index) const
    {
        for (auto &it : csFlags)
        {
            stochModel.setCS(it.second);
            Q(index, index) = stochModel.getQ();
            ++index;
        }
    }
    
    void AmbiguitySdEquations::defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const
    {
        for (auto &it : csFlags)
        {
            x(index) = 0;
            P(index, index) = 4e14;
            ++index;
        }
    }

    void AmbiguitySdEquations::updateH(gpstk::gnssRinex& gData, gpstk::Matrix<double>& H, int& row_0, int& col_0)
    {
        // Now, fill the coefficients related to phase biases
        // We must be careful because not all processed satellites
        // are currently visible
        currentSatSet = gData.getSatID();
        for (const auto& itSat : currentSatSet)
        {
            // Find in which position of 'satSet' is the current '(*itSat)'
            // Please note that 'currSatSet' is a subset of 'satSet'
            int j(0);
            auto& itSat2 = csFlags.begin();
            while ((itSat2->first) != (itSat))
            {
                ++j;
                ++itSat2;
            }

            double wavelength(0);
            int fcn = itSat.getGloFcn();
            switch (type.type)
            {
            case TypeID::BL1:
                wavelength = getWavelength(itSat, 1, fcn);
                break;
            case TypeID::BL2:
                wavelength = getWavelength(itSat, 2, fcn);
                break;
            case TypeID::BLC:
                wavelength = LinearCombination::getIonoFreeWaveLength(itSat, 1, 2);
                break;
            default:
                break;
            }

            // Put coefficient in the right place
            H(row_0, j + col_0) = wavelength;
           
            ++row_0;
        }

        col_0 += csFlags.size();
        satSet = currentSatSet;
    }

    int AmbiguitySdEquations::getNumUnknowns() const
    {
        return csFlags.size();
    }
}