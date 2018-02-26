#include "EquationComposer.h"
using namespace gpstk;

namespace pod
{
    void EquationComposer::Prepare(gnssRinex& gData)
    {
        for (auto& eq : equations)
            eq->Prepare(gData);

    }
    
    void EquationComposer::updateH(gpstk::gnssRinex& gData, gpstk::Matrix<double>& H)
    {


        numMeas = gData.body.size();

        currUnknowns.clear();
        for (auto& eq : equations)
            eq->updateEquationTypes(gData,currUnknowns);

        numUnknowns = getNumUnknowns();

        H = gData.body.getMatrixOfTypes(currUnknowns);
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
    
    void EquationComposer::updateR(const gnssRinex& gData, gpstk::Matrix<double>& R)
    {
        size_t  numsv = gData.body.size();
        // Generate the appropriate weights matrix
        // Try to extract weights from GDS
        satTypeValueMap dummy(gData.body.extractTypeID(TypeID::weight));
        
        //prepare identy matrix
        R.resize(numMeas, numMeas, 0.0);
        for (size_t i = 0; i < numsv; i++)
            R(i, i) = 1.0;

        // Check if weights match
        if (dummy.numSats() == numsv)
        {
            auto weigths = gData.getVectorOfTypeID(TypeID::weight);

            for (size_t i = 0; i < numsv; i++)
                R(i, i) = R(i, i) / weigths(i);
        }
    }

    void EquationComposer::updateMeas(const gnssRinex& gData, gpstk::Vector<double>& measVector)
    {
        measVector = gData.getVectorOfTypeID(measurmentsType);
    }

    int EquationComposer::getNumUnknowns() const
    {
        int res = 0;
        for (auto& eq : equations)
            res += eq->getNumUnknowns();
        return res;
    }


    void EquationComposer::updateKfState( gpstk::Vector<double>& currState, gpstk::Matrix<double>& currErrorCov) const
    {
        currErrorCov.resize(numUnknowns, numUnknowns, 0.0);
        int row = 0;
        for (const auto& it_row : currUnknowns)
        {
            const auto& type = coreData.find(it_row);
            if (type != coreData.end())
            {
                currState(row) = coreData.at(it_row).value;
                int col = 0;
                for (const auto& it_col : currUnknowns)
                {
                    currErrorCov(col, row) = currErrorCov(row, col) = coreData.at(it_row).valCov.at(it_col);
                    ++col;
                }
            }
            else
            {
                currState(row) = 0;
                currErrorCov(row, row) = 1e9;
            }
            ++row;
        }
    }

    void EquationComposer::storeKfState(const gpstk::Vector<double>& currState, const gpstk::Matrix<double>& currErrorCov)
    {
        int row = 0;
        for (const auto& it_row : currUnknowns)
        {
            coreData[it_row].value = currState(row);

            int col = 0;
            for (const auto& it_col : currUnknowns)
            {
                coreData[it_row].valCov[it_col] = currErrorCov(row, col);
                ++col;
            }
            ++row;
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

    void EquationComposer::saveResiduals(gpstk::gnssRinex& gData, gpstk::TypeID type, gpstk::Vector<double>& postfitResiduals)
    {
        gData.insertTypeIDVector(type, postfitResiduals);
    }
}