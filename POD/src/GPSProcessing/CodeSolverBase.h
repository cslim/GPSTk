#ifndef POD_PR_SOLVER_BASE_H
#define POD_PR_SOLVER_BASE_H

#include"PowerSum.hpp"
#include "Rinex3ObsData.hpp"
#include "XvtStore.hpp"
#include "GPSEllipsoid.hpp"
#include"IonoModelStore.hpp"
#include"Matrix.hpp"
#include"TropModel.hpp"

#include"CodeProcSvData.h"

using namespace std;
using namespace gpstk;

namespace pod
{
    typedef unsigned char uchar;
    enum CodeIonoCorrType { NONE = 0, Klobuchar, IF };

    using namespace std;
    class CodeSolverBase
    {
    protected:

        static  double eps;
        static  GPSEllipsoid ellGPS;
        static void refreshSolution(Vector<double> &Sol, Vector<double> &dSol);

    public:

        CodeSolverBase();
        virtual ~CodeSolverBase()
        {};

        string virtual getName()
        {
            return "PRSolverBase";
        };
        void  selectObservables(
            const Rinex3ObsData &rod,
            const Rinex3ObsHeader& roh,
            const set<SatID::SatelliteSystem> &systems,
            const ObsTypes & typeMap,
            CodeProcSvData & svData,
            bool isApplyRCO = false
        );

        void prepare(
            const CommonTime &t,
            const XvtStore<SatID>& Eph,
            CodeProcSvData & svData
        );

        int solve(
            const CommonTime &t,
            const IonoModelStore &iono,
            CodeProcSvData & svData
        );

       // string printSolution(const CodeProcSvData &useSat);


        virtual NeillTropModel initTropoModel(const Position &nominalPos, int DoY) = 0;

        virtual double getTropoCorrection(
            const Position &rxPos,
            const Position &svPos,
            const CommonTime &t) const = 0;

    protected:
         int solveInter(
            const CommonTime &t,
             const IonoModelStore &iono,
            CodeProcSvData & svData,
            Matrix<double>& Cov

        );
         void CodeSolverBase::calcSigma(
             const Position& rxPos,
             const Matrix<double> & W,
             const Vector<double> & b,
             const CodeProcSvData &svsData);

        void calcStat(const Matrix<double>& Cov);

        int CodeSolverBase::catchSatByResid(
            const CommonTime & t,
            const IonoModelStore & iono,
            CodeProcSvData & svsData
        );

    public:
        CodeIonoCorrType ionoType;

        double maskSNR;
        double maskEl;

        int maxIter;
        int iter;

        Vector< double> Sol;

        double sigma;
        double RMS3D;
        double PDOP;
        double sigmaMax;

        ofstream dbg;

        friend  std::ostream& operator<<(std::ostream& strs, const CodeSolverBase& gdsMap);
    };
}

#endif // !POD_PR_SOLVER_BASE_H