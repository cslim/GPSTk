#ifndef POD_PR_SOLVER_BASE_H
#define POD_PR_SOLVER_BASE_H

#include"typenames.hpp"
#include"PowerSum.hpp"
#include "Rinex3ObsData.hpp"
#include "XvtStore.hpp"
#include "GPSEllipsoid.hpp"
#include"IonoModelStore.hpp"
#include"Matrix.hpp"
#include"TropModel.hpp"
#include"NeillTropModel.hpp"
#include"CodeProcSvData.h"
#include"GnssDataStore.hpp"


namespace pod
{
    class CodeSolverBase
    {
    protected:

        static  double eps;
        static  gpstk::GPSEllipsoid ellGPS;
        static void refreshSolution(gpstk::Vector<double> &Sol, gpstk::Vector<double> &dSol);

    public:

        CodeSolverBase(GnssDataStore_sptr data );
        virtual ~CodeSolverBase()
        {};

        std::string virtual getName()
        {
            return "PRSolverBase";
        };
        void  selectObservables(
            const gpstk::Rinex3ObsData &rod,
            const gpstk::Rinex3ObsHeader& roh,
            const std::set<gpstk::SatID::SatelliteSystem> &systems,
            const ObsTypes & typeMap,
            CodeProcSvData & svData,
            bool isApplyRCO = false
        );

        void prepare(
            const gpstk::CommonTime &t,
            const gpstk::XvtStore<gpstk::SatID>& Eph,
            CodeProcSvData & svData
        );

        int solve(
            const gpstk::CommonTime &t,
            const gpstk::IonoModelStore &iono,
            CodeProcSvData & svData
        );

       // string printSolution(const CodeProcSvData &useSat);


        virtual gpstk::NeillTropModel initTropoModel(const gpstk::Position &nominalPos, int DoY) = 0;

        virtual double getTropoCorrection(
            const gpstk::Position &rxPos,
            const gpstk::Position &svPos,
            const gpstk::CommonTime &t) const = 0;

    protected:
         int solveInter(
            const gpstk::CommonTime &t,
             const gpstk::IonoModelStore &iono,
            CodeProcSvData & svData,
			 gpstk::Matrix<double>& Cov

        );
         void CodeSolverBase::calcSigma(
             const gpstk::Position& rxPos,
             const gpstk::Matrix<double> & W,
             const gpstk::Vector<double> & b,
             const CodeProcSvData &svsData);

        void calcStat(const gpstk::Matrix<double>& Cov);

        int CodeSolverBase::catchSatByResid(
            const gpstk::CommonTime & t,
            const gpstk::IonoModelStore & iono,
            CodeProcSvData & svsData
        );

    public:
		gpstk::ComputeIonoModel::IonoModelType ionoType;

        double maskSNR;
        double maskEl;

        int maxIter;
        int iter;

		gpstk::Vector< double> Sol;

        double sigma;
        double RMS3D;
        double PDOP;
        double sigmaMax;

        std::ofstream dbg;

        friend  std::ostream& operator<<(std::ostream& strs, const CodeSolverBase& gdsMap);
    };
}

#endif // !POD_PR_SOLVER_BASE_H