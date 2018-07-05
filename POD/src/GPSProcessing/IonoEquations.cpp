#include "IonoEquations.h"

using namespace gpstk;
namespace pod
{
    const double IonoEquations::SQR_L1_WL_GPS = L1_WAVELENGTH_GPS*L1_WAVELENGTH_GPS;

     std::map<gpstk::TypeID, int> IonoEquations::obsType2Band
     {
         { TypeID::prefitL1,1 },
         { TypeID::prefitL2,2 },
         { TypeID::prefitP1,1 },
         { TypeID::prefitC,1 },
         { TypeID::prefitP2,2 }
     };

     std::map<gpstk::TypeID, int> IonoEquations::obsType2Sign
     {
         { TypeID::prefitL1,-1 },
         { TypeID::prefitL2,-1 },
         { TypeID::prefitP1,1 },
         { TypeID::prefitC,1 },
         { TypeID::prefitP2,1 }
     };
    #pragma region Stochasic model initializers
   
    gpstk::StochasticModel_uptr  IonoEquations::constantModel(double sigma)
    {
        return std::make_unique<StochasticModel>();
    }

    gpstk::StochasticModel_uptr  IonoEquations::rWalkModel(double qPrime)
    {
        return std::make_unique<RandomWalkModel>(qPrime);
    }

    gpstk::StochasticModel_uptr  IonoEquations::whiteNoiseModel(double sigma)
    {
        return std::make_unique<WhiteNoiseModel>(sigma);
    }

    gpstk::StochasticModel_uptr  IonoEquations::ionoModel(double sigma)
    {
        return std::make_unique<IonoStochasticModel>(sigma);
    }

    #pragma endregion

    IonoEquations::IonoEquations():
        eqType(TypeID::ionoL1),
        sigma(300000.0),
         stModelInitializer(&IonoEquations::constantModel)
    {};

    IonoEquations::IonoEquations(double qPrime)
        :eqType(TypeID::ionoL1),
        sigma(qPrime),
        stModelInitializer(&IonoEquations::constantModel)
    {};

    void IonoEquations::Prepare(gpstk::gnssRinex & gData)
    {
        currParameters.clear();
        auto&& currentSatSet = gData.getSatID();

        for (auto&& it : currentSatSet)
        {
            auto it_model = stochModels.find(it);
            if (it_model == stochModels.end())
            {
                auto&&  newModel = (this->*stModelInitializer)(sigma);
                it_model = stochModels.insert(std::make_pair(it, std::move(newModel))).first;
            }
            
            it_model->second->Prepare(it, gData);
            currParameters.insert(FilterParameter(eqType, it));
        }
    }

    void IonoEquations::updateH(const gpstk::gnssRinex& gData, const gpstk::TypeIDSet& types, gpstk::Matrix<double>& H, int& col_0)
    {
        int nSv = currParameters.size();
        Matrix<double>mI = ident<double>(nSv);
       
        int row(0);
        for (const auto & it : types)
        {
            int band = obsType2Band.at(it);
            int sign = obsType2Sign.at(it);
            int i(0);
            for (const auto& sv: currParameters)
            {
                int fcn = sv.sv.getGloFcn();
                double wl = getWavelength(sv.sv, band, fcn);
                wl *= wl;
                H(row, col_0 + i) = sign*wl / SQR_L1_WL_GPS;
                row++;
                i++;
            }
        }
        col_0 += nSv;

    }
    
    void IonoEquations::updatePhi(gpstk::Matrix<double>& Phi, int & index) const
    {
        for (const auto & it : currParameters)
        {
            Phi(index, index) = stochModels.at(it.sv)->getPhi();
            ++index;
        }
    }

    void IonoEquations::updateQ(gpstk::Matrix<double>& Q, int & index) const
    {
        for (const auto & it : currParameters)
        {
            Q(index, index) = stochModels.at(it.sv)->getQ();
            ++index;
        }
    }

    void IonoEquations::defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const
    {
        for (const auto & it : currParameters)
        {
            x(index) = 0.0;
            P(index, index) = 1000;

            ++index;
        }
    }

    int IonoEquations::getNumUnknowns() const
    {
        return currParameters.size();
    }
}
