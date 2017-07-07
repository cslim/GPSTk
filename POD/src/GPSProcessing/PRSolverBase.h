#ifndef POD_PR_SOLVER_BASE_H
#define POD_PR_SOLVER_BASE_H

#include"PowerSum.hpp"
#include "Rinex3ObsData.hpp"
#include "XvtStore.hpp"
#include "GPSEllipsoid.hpp"
#include"IonoModelStore.hpp"
#include"Matrix.hpp"

using namespace std;
using namespace gpstk;
namespace POD
{
    typedef unsigned char uchar;
    enum PRIonoCorrType { NONE = 0, Klobuchar, IF };

    using namespace std;
    class PRSolverBase
    {
    public:
        PRSolverBase() :
            maskEl(0.0), maskSNR(0.0), Sol(4), maxIter(15),
            sigmaMax(25), ionoType(IF), RMS3D(DBL_MAX), PDOP(DBL_MAX), ps()
        {};
        virtual ~PRSolverBase()
        {};
        string virtual getName()
        {
            return "PRSolverBase";
        };
        void  selectObservables(
            const Rinex3ObsData &rod,
            int iL1Code,
            int iL2Code,
            int iL1SNR,
            vector<SatID> & PRNs,
            vector<double> &L1PRs,
            vector<uchar> & SNRs,
            bool isApplyRCO = false
        );

        void prepare(
            const CommonTime &t,
            vector<SatID> &IDs,
            const vector<double> &PRs,
            const XvtStore<SatID>& Eph,
            vector<bool> &useSat,
            Matrix<double> &SVP
        );

        virtual int solve(
            const CommonTime &t,
            const Matrix<double> &SVP,
            vector<bool> &useSat,
            Matrix<double>& Cov,
            Vector<double>& Resid,
            IonoModelStore &iono
        ) = 0;

        string printSolution(const vector<bool> &UseSat);

        PRIonoCorrType ionoType;

        uchar maskSNR;
        double maskEl;

        int maxIter;
        int iter;

        Vector< double> Sol;
        PowerSum ps;

        double sigma;
        double RMS3D;
        double PDOP;
        double sigmaMax;

        ofstream dbg;

        static double eps;

    protected:
        static  GPSEllipsoid ellGPS;

        void calcStat(Vector<double> resid, Matrix<double> Cov);

        virtual int catchSatByResid(
            const CommonTime &t,
            const Matrix<double> &SVP,
            vector<bool> &useSat,
            IonoModelStore &iono) = 0;

    };
}

#endif // !POD_PR_SOLVER_BASE_H