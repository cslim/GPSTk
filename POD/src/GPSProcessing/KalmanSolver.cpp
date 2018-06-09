#include "KalmanSolver.h"

#include"WinUtils.h"
#include"AmbiguityHandler.h"

#include"PowerSum.hpp"
#include"ARSimple.hpp"
#include"ARMLambda.hpp"
#include <algorithm>

using namespace std;
using namespace gpstk;

namespace pod
{
    //maximum time interval (in seconds) without data
    double KalmanSolver::maxGap = 3600.0
        ;
    KalmanSolver::KalmanSolver():firstTime(true)
    { }
    KalmanSolver::KalmanSolver(eqComposer_sptr eqs) 
        :firstTime(true),equations(eqs)
    {}

    KalmanSolver::~KalmanSolver()
    {}

    gnssRinex& KalmanSolver::Process(gnssRinex& gData)
    {
        double dt = abs(t_pre - gData.header.epoch);
        t_pre = gData.header.epoch;
        if (dt > maxGap)
        {
            reset();
            DBOUT_LINE("dt= "<<dt<<"->RESET")
        }
        equations->Prepare(gData);
        Vector<double> floatSolution;

        while (true)
        {
            equations->updateH(gData, hMatrix);
            equations->updateMeas(gData, measVector);
            equations->updateW(gData, weigthMatrix);

            equations->updatePhi(phiMatrix);
            equations->updateQ(qMatrix);
            
          
            if (dt > maxGap)
                equations->initKfState(solution, covMatrix);
            else
                equations->updateKfState(solution, covMatrix);

            firstTime = false;

            DBOUT_LINE("----------------------------------------------------------------------------------------");
            
            //auto svset = gData.getSatID();
            //for (auto& it:svset)
            //    DBOUT(it<<" ");

            //DBOUT_LINE("measVector\n" << setprecision(10) << measVector);
             DBOUT_LINE("H\n" << hMatrix);
             DBOUT_LINE("Cov\n" << covMatrix);
             DBOUT_LINE("phiMatrix\n" << phiMatrix);
             DBOUT_LINE("qMatrix\n" << qMatrix);
            //prepare
            Matrix<double> hMatrixTr = transpose(hMatrix);
            Matrix<double> phiMatrixTr = transpose(phiMatrix);
            Matrix<double> hTrTimesW = hMatrixTr*weigthMatrix;

            //predict
            Matrix<double> Pminus = phiMatrix*covMatrix*phiMatrixTr + qMatrix;
            Vector<double> xminus = phiMatrix*solution;
            
            //correct
            Matrix<double> invPminus = inverseChol(Pminus);
            covMatrix = inverseChol(hTrTimesW*hMatrix + invPminus);
            solution = covMatrix*(hTrTimesW*measVector + (invPminus*xminus));

            postfitResiduals = measVector - hMatrix * solution;
            DBOUT_LINE("Solution\n" << solution);
            DBOUT_LINE("postfitResiduals\n" << postfitResiduals);
            //DBOUT_LINE("CovPost\n" << covMatrix);
 
            equations->saveResiduals(gData, postfitResiduals);
            floatSolution = solution;
            
            fixAmbiguities(gData);
           // storeAmbiguities(gData);

            break;

            if (!check(gData))
                break;
            //reject(gData, eqComposer().residTypes());

            if (gData.body.size() < equations->getNumUnknowns())
            {
                equations->initKfState(solution, covMatrix);
                break;
            }
        }

        equations->storeKfState(floatSolution, covMatrix);
        return gData;
    }
    
    void  KalmanSolver::fixAmbiguities(gnssRinex& gData)
    {
      /*  auto summ = Ambiguity::get_sv_by_ss( equations->currentAmb());
        for (auto &it : summ)
            std::cout << SatID::convertSatelliteSystemToString(it.first)
            << ": " << it.second.size() << endl;
*/
        if (equations->getSlnType() == SlnType::PD_Fixed && gData.body.size() > 5)
        {
            int core_num = equations->currentUnknowns().size();
            AmbiguityHandler ar(equations->currentAmb(), solution, covMatrix, core_num);
            ar.fixL1L2(gData);

            for (int k = 0; k < core_num; k++)
                solution(k) = ar.CoreParamFixed()(k);
        }
        else
            storeAmbiguities(gData);
            
    }

    void  KalmanSolver::storeAmbiguities(gnssRinex& gData) const
    {
        int i(0);
        for (const auto & amb : equations->currentAmb())
        {
            auto & it = gData.body.find(amb.sv);
            if (it != gData.body.end())
                gData.body[amb.sv][amb.type] = solution(equations->currentUnknowns().size() + i);
            ++i;
        }
    }

    int KalmanSolver::check(gnssRinex& gData)
    {
        Matrix<double> res(PostfitResiduals().size(), 1, 0.0);

        res = res.assignFrom(PostfitResiduals());
        
        //compute v'pv
        auto vpv = transpose(res)*weigthMatrix*res;

        double sigma = sqrt(vpv(0,0));

        double varX = getVariance(TypeID::dx);     // Cov dx    - #8
        double varY = getVariance(TypeID::dy);     // Cov dy    - #9
        double varZ = getVariance(TypeID::dz);     // Cov dz    - #10
        double stDev3D = sqrt(varX + varY + varZ);

        if (sigma / stDev3D > 3)
        {
            cout << "Epoch: " << CivilTime(gData.header.epoch) << " catched " << endl;
            cout << "sigma: " << sigma << " sigma: " << stDev3D << endl;
            return 1;
        }
        else
            return 0;
    }
    
    //reject by code postfit residual 
    gnssRinex& KalmanSolver::reject(gnssRinex& gData, const TypeIDSet&  typeIds)
    {
        using type = decltype(gnssRinex::body)::value_type;
        SatIDSet rejSat;
        ///!!!
        ///here we use only last type of postfit residuals, because in case of
        ///combined code/phase measurements processing this part contans carrier phase residuals
        //in case code/phase only processing we have only one element in @typeIds, which one will be used
        ///!!!
        const auto& id = typeIds.rbegin();

        //get the sv - typeMap pair with largest residual value

        auto svWithMaxResidual = std::max_element(
            gData.body.begin(), gData.body.end(),
            [&](const type& it1, const type& it2)-> bool
        {
            double val1 = ::abs(it1.second.at(*id));
            double val2 = ::abs(it2.second.at(*id));
            return(val1 < val2);
        }
        );

        //report detection
        cout << "Removed SV: " << svWithMaxResidual->first;
        cout << " with " << TypeID::tStrings[id->type] << " = ";
        cout << svWithMaxResidual->second[*id] << endl;
        
        //remove sv
        rejSat.insert(svWithMaxResidual->first);

        gData.removeSatID(rejSat);

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

        for (const auto it2 : equations->currentUnknowns())
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

        for (const auto it2 : equations->currentUnknowns())
        {
            if (it2 == type)
                return covMatrix(counter, counter);
            ++counter;
        }

        InvalidRequest e("Type: '" + TypeID::tStrings.at(type.type)+"' not found in current set of unknowns.");
        GPSTK_THROW(e);

    }  // End of method 'SolverLMS::getVariance()'
}
