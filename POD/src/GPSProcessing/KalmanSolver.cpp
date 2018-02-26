#include "KalmanSolver.h"
#include"WinUtils.h"
#include"PowerSum.hpp"
#include <algorithm>

using namespace std;
using namespace gpstk;

namespace pod
{

    KalmanSolver::KalmanSolver():firstTime(true)
    { }
    KalmanSolver::KalmanSolver(eqComposer_sptr eqs) 
        :firstTime(true),equations(eqs)
    {}
  /*  KalmanSolver::KalmanSolver(const EquationComposer& eqs)
        :equations(eqs),firstTime(true)
    {}*/
    KalmanSolver::~KalmanSolver()
    { }

    int KalmanSolver::Compute(const Vector<double>& prefitResiduals,
        const Matrix<double>& designMatrix,
        const Matrix<double>& rMatrix)
        throw(InvalidSolver)
    {

        // By default, results are invalid
        valid = false;

        if (!(rMatrix.isSquare()))
        {
            InvalidSolver e("Weight matrix is not square");
            GPSTK_THROW(e);
        }

        int wRow = static_cast<int>(rMatrix.rows());
        int pRow = static_cast<int>(prefitResiduals.size());
        if (!(wRow == pRow))
        {
            InvalidSolver e("prefitResiduals size does not match dimension of weightMatrix");

            GPSTK_THROW(e);
        }

        int gRow = static_cast<int>(designMatrix.rows());
        if (!(gRow == pRow))
        {
            InvalidSolver e("prefitResiduals size does not match dimension of designMatrix");

            GPSTK_THROW(e);
        }

        if (!(phiMatrix.isSquare()))
        {
            InvalidSolver e("phiMatrix is not square");

            GPSTK_THROW(e);
        }

        if (!(qMatrix.isSquare()))
        {
            InvalidSolver e("qMatrix is not square");

            GPSTK_THROW(e);
        }


        try
        {
            // Call the Kalman filter object.
            kFilter.Compute(phiMatrix,
                qMatrix,
                prefitResiduals,
                designMatrix,
                rMatrix);
        }
        catch (InvalidSolver& e)
        {
            GPSTK_RETHROW(e);
        }

        // Store the solution
        solution = kFilter.xhat;

        // Store the covariance matrix of the solution
        covMatrix = kFilter.P;

        // Compute the postfit residuals Vector
        postfitResiduals = prefitResiduals - designMatrix * solution;

        // If everything is fine so far, then the results should be valid
        valid = true;

        return 0;

    }  // End of method 'CodeKalmanSolver::Compute()'

    gnssRinex& KalmanSolver::Process(gnssRinex& gData)
    {
        equations->Prepare(gData);

        while (true)
        {
            equations->updateH(gData, hMatrix);
            equations->updateMeas(gData, measVector);
            equations->updateR(gData, rMatrix);

            equations->updatePhi(phiMatrix);
            equations->updateQ(qMatrix);

            if (firstTime)
                equations->initKfState(solution, covMatrix);
            else
                equations->updateKfState(solution, covMatrix);

            firstTime = false;


            kFilter.Reset(solution, covMatrix);

            Compute(measVector, hMatrix, rMatrix);

            equations->saveResiduals(gData, TypeID::postfitC, postfitResiduals);

            DBOUT_LINE("Epoch: ");
            DBOUT_LINE(CivilTime(gData.header.epoch));
            DBOUT_LINE("measVector");
            DBOUT_LINE(measVector);
            DBOUT_LINE("sln");
            DBOUT_LINE(solution);
            DBOUT_LINE("postfit");
            DBOUT_LINE(PostfitResiduals());

            if (!check(gData))
                break;
            reject(gData, TypeID::postfitC);

            if (gData.body.size() < equations->getNumUnknowns())
            {
                equations->initKfState(solution, covMatrix);
                break;
            }
        }

        equations->storeKfState(solution, covMatrix);
        return gData;
    }


    int KalmanSolver::check(gnssRinex& gData)
    {
        PowerSum psum;
        for (auto it : PostfitResiduals())
            psum.add(it);
        double sigma = sqrt(psum.variance());

        double varX = getVariance(TypeID::dx);     // Cov dx    - #8
        double varY = getVariance(TypeID::dy);     // Cov dy    - #9
        double varZ = getVariance(TypeID::dz);     // Cov dz    - #10
        double stDev3D = sqrt(varX + varY + varZ);

        if (sigma / stDev3D > 10)
            return 1;
        else
            return 0;

    }


    //reject by code postfit residual 
    gnssRinex& KalmanSolver::reject(gnssRinex& gData, const TypeID&  typeId)
    {
        using type = decltype(gnssRinex::body)::value_type;

        int imax = 0;

        auto svWithMaxResidual = std::max_element
        (
            gData.body.begin(), gData.body.end(),
            [&](const type& it1, const type& it2)-> bool
        {
            double val1 = ::abs(it1.second.at(typeId));
            double val2 = ::abs(it2.second.at(typeId));
            return(val1 < val2);
        }
        );
        DBOUT("Removed SV: ");
        DBOUT(svWithMaxResidual->first);
        DBOUT(" with " + TypeID::tStrings[typeId.type] + " = ");
        DBOUT_LINE(svWithMaxResidual->second[typeId]);

        gData.removeSatID(svWithMaxResidual->first);



        return gData;
    }

    gnssSatTypeValue& KalmanSolver::Process(gnssSatTypeValue& gData)
        throw(ProcessingException)
    {
        try
        {
            // Build a gnssRinex object and fill it with data
            gnssRinex g1;
            g1.header = gData.header;
            g1.body = gData.body;

            // Call the Process() method with the appropriate input object
            Process(g1);

            // Update the original gnssSatTypeValue object with the results
            gData.body = g1.body;

            return gData;

        }
        catch (Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e(getClassName() + ":" + u.what());

            GPSTK_THROW(e);
        }
    }

    double KalmanSolver::getSolution(const TypeID& type) const
    {
        // Define counter
        int counter(0);

        for (const auto it2 : equations->currentUnkNowns())
        {
            if (it2 == type)
                return solution(counter);
            counter++;
        }

        InvalidRequest e("Type: '" + TypeID::tStrings.at(type.type) + "' not found in  current set of unknowns.");
        GPSTK_THROW(e);

    }  // End of method 'SolverLMS::getSolution()'

    double KalmanSolver::getVariance(const TypeID& type) const
    {
        int counter(0);

        for (const auto it2 : equations->currentUnkNowns())
        {
            if (it2 == type)
                return covMatrix(counter, counter);
            ++counter;
        }

        InvalidRequest e("Type: '" + TypeID::tStrings.at(type.type)+"' not found in current set of unknowns.");
        GPSTK_THROW(e);

    }  // End of method 'SolverLMS::getVariance()'
}
