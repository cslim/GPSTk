#include "AmbiguitiesEquations.h"
#include"GNSSconstants.hpp"
#include"LinearCombination.h"

using namespace gpstk;

namespace pod
{
    std::map< gpstk::TypeID, gpstk::TypeID> AmbiguitySdEquations::typeMap;

    AmbiguitySdEquations::Initializer AmbiguitySdEquations::initializer;

    AmbiguitySdEquations::Initializer::Initializer()
    {
        typeMap[TypeID::BL1] = TypeID::prefitL1;
        typeMap[TypeID::BL2] = TypeID::prefitL2;
        typeMap[TypeID::BLC] = TypeID::prefitLC;
    }

    ParametersSet AmbiguitySdEquations::getParameters() const
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

    void AmbiguitySdEquations::updateH(const gpstk::gnssRinex& gData, const gpstk::TypeIDSet& types, gpstk::Matrix<double>& H, int& col_0)
    {
        //total number of  ambiguities
        int numAmbs(csFlags.size());

        //sv in view
        auto currentSatSet = gData.getSatID();

        //total number of sv in view
        int numSv(currentSatSet.size());

        int row(0);
        for (const auto& t : types)
        {
            if (obsType != t)
            {
                row += numSv;
                continue;
            }
            
            // Now, fill the coefficients related to phase biases
            // We must be careful because not all processed satellites
            // are currently visible
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
                    wavelength = 1.0;// LinearCombination::getIonoFreeWaveLength(itSat, 1, 2);
                    break;
                default:
                    break;
                }

                // Put coefficient in the right place
                H(row, j + col_0) = wavelength;

                ++row;
            }

            col_0 += numAmbs;
        }
        satSet = currentSatSet;
    }

    int AmbiguitySdEquations::getNumUnknowns() const
    {
        return csFlags.size();
    }
}