#include "EquationComposer.h"
using namespace gpstk;

namespace pod
{
   const std::map<gpstk::TypeID, double> EquationComposer::weigthFactors{

        //code pseudorange weight factor
        { TypeID::prefitC, 1.0  }      ,
        { TypeID::prefitP1, 1.0 }      ,
        { TypeID::prefitP2, 1.0 }      ,

        //carrier phase weight factor
        { TypeID::prefitL,  10000.0 },
        { TypeID::prefitL1, 10000.0 },
        { TypeID::prefitL2, 10000.0 },
    };

    void EquationComposer::Prepare(gnssRinex& gData)
    {
        //clear ambiguities set
        currAmb.clear();
        for (auto& eq : equations)
        {
            //prepare equations objects state
            eq->Prepare(gData);

            // update current set of ambiguities
            auto  ambs = eq->getAmbSet();
            currAmb.insert(ambs.cbegin(), ambs.cend());
        }
    }

    void EquationComposer::updateH(gpstk::gnssRinex& gData, gpstk::Matrix<double>& H)
    {
        int numSVs = gData.body.size();
        int numMeasTypes = measTypes().size();
        //number of measurements are equals number of Satellites times observation types number 
        numMeas = numSVs*numMeasTypes;

        coreUnknowns.clear();
        for (auto& eq : equations)
            eq->updateEquationTypes(gData, coreUnknowns);

        int numCoreUnknowns = coreUnknowns.size();

        numUnknowns = getNumUnknowns();

        // set resize design matrix 
        H.resize(numMeas, numUnknowns, 0.0);

        //get the 'core' variables part of design matrix 
        auto hCore = gData.body.getMatrixOfTypes(coreUnknowns);
        
        /*
        form the design martix H:

             core          N1         N2     
           | d(core)/ |          |          | 
        P1 |  d(ro)   |    0     |     0    |
           |          |          |          |
           ----------------------------------
           | d(core)/ |          |          |
        P2 |  d(ro)   |    0     |     0    |
           |          |          |          |
           ----------------------------------
           | d(core)/ |          |          |
        L1 |  d(ro)   |lambda_1*E|     0    |
           |          |          |          |
           ----------------------------------
           | d(core)/ |          |          |
        L2 |  d(ro)   |    0     |lambda_2*E|
           |          |          |          |
        */

        //fill the core part of design matix
        for (size_t i = 0; i < numSVs; i++)
            for (size_t j = 0; j < numCoreUnknowns; j++)
                for (size_t k = 0; k < numMeasTypes; k++)
                    H(i + k*numSVs, j) = hCore(i, j);

        //fill the satellites dependent part of design matrix
        int row(numSVs*2);
        int col(numCoreUnknowns);
        for (auto& eq : equations)
            eq->updateH(gData, H, row, col);

    }

    void EquationComposer::updatePhi(Matrix<double>& Phi) const
    {
        int i = 0;
        Phi.resize(numUnknowns, numUnknowns, 0.0);
        for (auto& eq : equations)
            eq->updatePhi(Phi, i);
    }

    void EquationComposer::updateQ(Matrix<double>& Q) const
    {
        int i = 0;
        Q.resize(numUnknowns, numUnknowns, 0.0);
        for (auto& eq : equations)
            eq->updateQ(Q, i);
    }

    void EquationComposer::updateW(const gnssRinex& gData, gpstk::Matrix<double>& weigthMatrix)
    {
        size_t  numsv = gData.body.size();
        // Generate the appropriate weights matrix
        // Try to extract weights from GDS
        satTypeValueMap dummy(gData.body.extractTypeID(TypeID::weight));

        //prepare identy matrix
        weigthMatrix.resize(numMeas, numMeas, 0.0);
        for (size_t i = 0; i < numMeas; i++)
            weigthMatrix(i, i) = 1.0;

        // Check if weights match
        if (dummy.numSats() == numsv)
        {
            auto weigths = gData.getVectorOfTypeID(TypeID::weight);

            for (size_t i = 0; i < numsv; i++)
                weigthMatrix(i, i) = weigthMatrix(i, i) * weigths(i);
        }
        else
        {
            size_t n(0);
            for (const auto& observable : measTypes())
            {
                const auto weigthFactor = weigthFactors.find(observable);
                if (weigthFactor == weigthFactors.end())
                {
                    std::string msg = "Can't find weigth factor for TypeID:: " 
                        + TypeID::tStrings[observable.type];

                    InvalidRequest e(msg);
                    GPSTK_THROW(e)
                }

                for (size_t i = 0; i < numsv; i++)
                    weigthMatrix(i+ numsv*n, i+ numsv*n) *= weigthFactor->second;
                n++;
            }
        }
    }

    void EquationComposer::updateMeas(const gnssRinex& gData, gpstk::Vector<double>& measVector)
    {
        measVector.resize(numMeas, 0.0);
        int j = 0;
        for (const auto& it : measTypes())
        {
            auto meas = gData.getVectorOfTypeID(it);
            int numSat = meas.size();
            for (int i = 0; i <numSat; i++)
                measVector(i + j*numSat) = meas(i);
            j++;
        }
    }

    int EquationComposer::getNumUnknowns() const
    {
        int res = 0;
        for (auto& eq : equations)
            res += eq->getNumUnknowns();
        return res;
    }

    void EquationComposer::updateKfState(gpstk::Vector<double>& currState, gpstk::Matrix<double>& currErrorCov) const
    {
        initKfState(currState, currErrorCov);

        int numCoreUnks = coreUnknowns.size();

        int row = 0;

        //update 'core' state and 'core' X 'core' covarince
        for (const auto& it_row : coreUnknowns)
        {
            const auto& typeRow = coreData.find(it_row);
            if (typeRow != coreData.end())
            {
                currState(row) = coreData.at(it_row).value;
                int col = 0;
                for (const auto& it_col : coreUnknowns)
                {
                    const auto& typeCol = (typeRow->second).valCov.find(it_col);
                    if (typeCol != (typeRow->second).valCov.end())
                        currErrorCov(col, row) = currErrorCov(row, col) = typeCol->second;
                    ++col;
                }
            }
            ++row;
        }

        int i_amb1(0);
        //for each current ambiguity
        for (const auto& amb : currAmb)
        {
            //try to find ambiguity data, processed before
            const auto& it = ambiguityData.find(amb);

            //if found
            if (it != ambiguityData.end())
            {
                // first, update ambiguity value
                currState(i_amb1 + numCoreUnks) = it->second.ambiguity;

                // update ambiguities vs 'core' variables covariance
                int i_core = 0;
                for (const auto& core_var : coreUnknowns)
                {
                    const auto& amb_sv_core = it->second.coreCov.find(core_var);
                    //if found
                    if (amb_sv_core != it->second.coreCov.end())
                    {
                        currErrorCov(i_core, i_amb1 + numCoreUnks) =
                            currErrorCov(i_amb1 + numCoreUnks, i_core) = amb_sv_core->second;
                    }
                    i_core++;
                }

                int i_amb2(0);
                // update ambiguity vs ambiguity covariance
                for (const auto& sv2 : currAmb)
                {
                    //try to find ambiguity vs ambiguity covarince in data processed before
                    const auto& amb_sv_amb = it->second.ambCov.find(sv2);

                    //if found
                    if (amb_sv_amb != it->second.ambCov.end())
                    {
                        currErrorCov(numCoreUnks + i_amb1, numCoreUnks + i_amb2) =
                            currErrorCov(numCoreUnks + i_amb2, numCoreUnks + i_amb1) = amb_sv_amb->second;
                    }
                    i_amb2++;
                }
            }
            i_amb1++;

        }

    }

    void EquationComposer::storeKfState(const gpstk::Vector<double>& currState, const gpstk::Matrix<double>& currErrorCov)
    {
        int numCoreUnkns(coreUnknowns.size());
        //store 'core' variables data  
        int row = 0;
        for (const auto& it_row : coreUnknowns)
        {
            coreData[it_row].value = currState(row);

            int col = 0;
            for (const auto& it_col : coreUnknowns)
            {
                coreData[it_row].valCov[it_col] = currErrorCov(row, col);
                ++col;
            }
            ++row;
        }

        //store 'ambiguities' variables data
        int i_amb1(0);
        for (const auto& amb : currAmb)
        {
            ambiguityData[amb].ambiguity = currState(numCoreUnkns+ i_amb1);

            int i_core(0);
            for (const auto& it_col : coreUnknowns)
            {
                ambiguityData[amb].coreCov[it_col] = currErrorCov(i_core, numCoreUnkns + i_amb1);
                i_core++;
            }

            int i_amb2(0);
            for (const auto& amb2 : currAmb)
            {
                ambiguityData[amb].ambCov[amb2] = currErrorCov(numCoreUnkns + i_amb1, numCoreUnkns + i_amb2);
                i_amb2++;
            }
            i_amb1++;
        }
    }

    void EquationComposer::initKfState(gpstk::Vector<double>& state, gpstk::Matrix<double>& cov) const
    {
        state.resize(numUnknowns, 0.0);
        cov.resize(numUnknowns, numUnknowns, 0.0);

        int i = 0;
        for (auto& eq : equations)
            eq->defStateAndCovariance(state, cov, i);
    }

    void EquationComposer::saveResiduals(gpstk::gnssRinex& gData, gpstk::Vector<double>& postfitResiduals) const
    {
        int resNum = postfitResiduals.size();
        int satNum = gData.body.size();
        int numResTypes = (residTypes()).size();

        assert(satNum * numResTypes == resNum);

        int i_res = 0;
        for (const auto& resType : residTypes())
            for (auto& itSat : gData.body)
            {
                itSat.second[resType] = postfitResiduals(i_res);
                i_res++;
            }
    }
}