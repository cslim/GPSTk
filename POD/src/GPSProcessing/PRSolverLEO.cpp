
#include"Matrix.hpp"
#include"PRSolverLEO.h"
namespace POD
{
    int  PRSolverLEO::solve(
        const CommonTime &t,
        const Matrix<double> &SVP,
        vector<bool> &useSat,
        Matrix<double>& Cov,
        Vector<double>& Resid,
        IonoModelStore &iono)
    {
        int   n, N;
        size_t i;
        this->iter = 0;
        double conv = DBL_MAX;
        do
        {
            // find the number of good satellites
            for (N = 0, i = 0; i < useSat.size(); i++)
            {
                if (useSat[i]) N++;
            }

            // define for computation
            Vector<double> CRange(N), dX(4);
            Matrix<double> P(N, 4), PT, G(4, N), PG(N, N);

            //Sol.resize(4);
            //Cov.resize(4, 4);
            Resid.resize(N);

            Position PosRX(Sol(0), Sol(1), Sol(2));
            ps.clear();

            for (n = 0, i = 0; i < useSat.size(); i++)
            {
                if (!useSat[i]) continue;

                // time of flight (sec)
                double rho(0.0);
                if (iter == 0)
                    rho = 0.070;             // initial guess: 70ms
                else
                    rho = RSS(SVP(i, 0) - Sol(0), SVP(i, 1) - Sol(1), SVP(i, 2) - Sol(2))
                    / ellGPS.c();

                // correct for earth rotation
                double  wt = ellGPS.angVelocity()*rho;  //radians
                double  svxyz[3];

                svxyz[0] = ::cos(wt)*SVP(i, 0) + ::sin(wt)*SVP(i, 1);
                svxyz[1] = -::sin(wt)*SVP(i, 0) + ::cos(wt)*SVP(i, 1);
                svxyz[2] = SVP(i, 2);

                double iodel(0.0);
                if (conv < 100 && useSat[i])
                {
                    Position SVpos(SVP(i, 0), SVP(i, 1), SVP(i, 2));
                    double elv = PosRX.elevationGeodetic(SVpos);
                    if (elv < maskEl)
                    {
                        useSat[i] = false;
                        continue;
                    }
                    if (ionoType == PRIonoCorrType::Klobuchar)
                    {
                        double azm = PosRX.azimuth(SVpos);
                        double iodel = iono.getCorrection(t, PosRX, elv, azm);
                    }
                }

                rho = RSS(svxyz[0] - Sol(0), svxyz[1] - Sol(1), svxyz[2] - Sol(2));

                // corrected pseudorange (m)
                CRange(n) = SVP(i, 3);

                // partials matrix
                P(n, 0) = (Sol(0) - svxyz[0]) / rho; // x direction cosine
                P(n, 1) = (Sol(1) - svxyz[1]) / rho; // y direction cosine
                P(n, 2) = (Sol(2) - svxyz[2]) / rho; // z direction cosine
                P(n, 3) = 1.0;

                // data vector: corrected range residual
                Resid(n) = CRange(n) - rho - Sol(3) - iodel;
                ps.add(Resid(n));
                n++;
            }
            if (n < 4) return -1;

            PT = transpose(P);
            Cov = PT * P;

            try
            {
                Cov = inverseSVD(Cov);
            }
            catch (SingularMatrixException& sme)
            {
                return -2;
            }
            // generalized inverse
            G = Cov * PT;
            dX = G * Resid;
            Sol += dX;
            // test for convergence
            conv = norm(dX);
            this->iter++;

            if (this->iter == maxIter) break;
        } while (eps < conv);

        calcStat(Resid, Cov);

        if (sigma > sigmaMax)
            return catchSatByResid(t, SVP, useSat, iono);

        return 0;
    };

    int PRSolverLEO::catchSatByResid(
        const CommonTime &t,
        const Matrix<double> &SVP,
        vector<bool> &useSat,
        IonoModelStore &iono)
    {

        for (int k = 0; k < useSat.size(); k++)
        {
            if (!useSat[k]) continue;

            useSat[k] = false;

            double conv = DBL_MAX;
            int iter_ = 0;

            int N = 0;
            // find the number of good satellites
            for (int i = 0; i < useSat.size(); i++)
            {
                if (useSat[i]) N++;
            }

            // define for computation
            Vector<double> CRange(N), dX(4), Resid(N);
            Matrix<double> P(N, 4), PT(4, N), G(4, N), Cov(4, 4);

            //
            do
            {
                Position PosRX(Sol(0), Sol(1), Sol(2));
                ps.clear();
                int n = 0;
                for (int i = 0; i < useSat.size(); i++)
                {
                    if (!useSat[i]) continue;
                    double rho(0.0);
                    // time of flight (sec)
                    if (iter == 0)
                        rho = 0.070;             // initial guess: 70ms
                    else
                        rho = RSS(SVP(i, 0) - Sol(0), SVP(i, 1) - Sol(1), SVP(i, 2) - Sol(2))
                        / ellGPS.c();

                    // correct for earth rotation
                    double  wt = ellGPS.angVelocity()*rho;  //radians
                    double  svxyz[3];

                    svxyz[0] = ::cos(wt)*SVP(i, 0) + ::sin(wt)*SVP(i, 1);
                    svxyz[1] = -::sin(wt)*SVP(i, 0) + ::cos(wt)*SVP(i, 1);
                    svxyz[2] = SVP(i, 2);

                    double iodel(0.0);
                    if (conv < 100 && ionoType == PRIonoCorrType::Klobuchar)
                    {
                        Position SVpos(SVP(i, 0), SVP(i, 1), SVP(i, 2));
                        double elv = PosRX.elevationGeodetic(SVpos);
                        double azm = PosRX.azimuth(SVpos);
                        double iodel = iono.getCorrection(t, PosRX, elv, azm);
                    }

                    rho = RSS(svxyz[0] - Sol(0), svxyz[1] - Sol(1), svxyz[2] - Sol(2));

                    // corrected pseudorange (m)
                    CRange(n) = SVP(i, 3);

                    // partials matrix
                    P(n, 0) = (Sol(0) - svxyz[0]) / rho; // x direction cosine
                    P(n, 1) = (Sol(1) - svxyz[1]) / rho; // y direction cosine
                    P(n, 2) = (Sol(2) - svxyz[2]) / rho; // z direction cosine
                    P(n, 3) = 1.0;

                    // data vector: corrected range residual
                    Resid(n) = CRange(n) - rho - Sol(3) - iodel;
                    ps.add(Resid(n));
                    n++;
                }

                if (n < 3) return -1;

                PT = transpose(P);
                Cov = PT * P;

                try
                {
                    Cov = inverseSVD(Cov);
                }
                catch (SingularMatrixException& sme)
                {
                    return -2;
                }
                // generalized inverse
                G = Cov * PT;
                dX = G * Resid;
                Sol += dX;
                // test for convergence
                conv = norm(dX);
                iter_++;
                if (iter_ == maxIter) break;

            } while (eps < conv);

            calcStat(Resid, Cov);

            if (this->sigma < sigmaMax)
                return 0;
            else
                useSat[k] = true;
        }
        return 1;
    }
}
