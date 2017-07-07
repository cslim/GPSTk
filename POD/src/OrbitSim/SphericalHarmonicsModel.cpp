#include"SphericalHarmonicsModel.h"
#include"EarthRotation.h"

namespace POD
{
    SphericalHarmonicsModel::SphericalHarmonicsModel(const GravityModelData & gData)
        : GravityModel(gData)
    {
        const int size = gmData_.desiredDegree;

        V.resize(size + 3, size + 3, 0.0);
        W.resize(size + 3, size + 3, 0.0);

    }

    /* Evaluates the two harmonic functions V and W.
    * @param r ECEF position vector.
    */
    void SphericalHarmonicsModel::computeVW(Vector<double> r_bf)
    {
        const double R_ref = gmData_.refDistance;

        // Auxiliary quantities
        double r_sqr = dot(r_bf, r_bf);
        double rho = R_ref * R_ref / r_sqr;

        // Normalized coordinates
        double x0 = R_ref * r_bf(0) / r_sqr;
        double y0 = R_ref * r_bf(1) / r_sqr;
        double z0 = R_ref * r_bf(2) / r_sqr;

        //
        // Evaluate harmonic functions 
        //   V_nm = (R_ref/r)^(n+1) * P_nm(sin(phi)) * cos(m*lambda)
        // and 
        //   W_nm = (R_ref/r)^(n+1) * P_nm(sin(phi)) * sin(m*lambda)
        // up to degree and order n_max+1
        //

        // Calculate zonal terms V(n,0); set W(n,0)=0.0
        V[0][0] = R_ref / std::sqrt(r_sqr);
        W[0][0] = 0.0;

        V[1][0] = z0 * V[0][0];
        W[1][0] = 0.0;

        for (int n = 2; n <= (gmData_.desiredDegree + 2); n++)
        {
            V[n][0] = ((2 * n - 1) * z0 * V[n - 1][0] - (n - 1) * rho * V[n - 2][0]) / n;
            W[n][0] = 0.0;
        }

        // Calculate tesseral and sectorial terms
        for (int m = 1; m <= (gmData_.desiredOrder + 2); m++)
        {
            // Calculate V(m,m) .. V(n_max+1,m)

            V[m][m] = (2 * m - 1) * (x0 * V[m - 1][m - 1] - y0 * W[m - 1][m - 1]);
            W[m][m] = (2 * m - 1) * (x0 * W[m - 1][m - 1] + y0 * V[m - 1][m - 1]);

            if (m <= (gmData_.desiredDegree + 1))
            {
                V[m + 1][m] = (2 * m + 1) * z0 * V[m][m];
                W[m + 1][m] = (2 * m + 1) * z0 * W[m][m];
            }

            for (int n = (m + 2); n <= (gmData_.desiredDegree + 2); n++)
            {
                V[n][m] = ((2 * n - 1)*z0*V[n - 1][m] - (n + m - 1)*rho*V[n - 2][m]) / (n - m);
                W[n][m] = ((2 * n - 1)*z0*W[n - 1][m] - (n + m - 1)*rho*W[n - 2][m]) / (n - m);
            }

        }  // End 'for (int m = 1; m <= (desiredOrder + 2); m++) '

    }  // End of method 'GravityModel::computeVW()'

    void SphericalHarmonicsModel::computeNormVW(Vector<double> r_bf)
    {
        const double R_ref = gmData_.refDistance;

        // Auxiliary quantities
        double r_sqr = dot(r_bf, r_bf);
        double rho = R_ref * R_ref / r_sqr;

        // Normalized coordinates
        double x0 = R_ref * r_bf(0) / r_sqr;
        double y0 = R_ref * r_bf(1) / r_sqr;
        double z0 = R_ref * r_bf(2) / r_sqr;

        V[1][0] = normN01(1,0)*z0 * V[0][0];
        W[1][0] = 0.0;

        for (int n = 2; n <= (gmData_.desiredDegree + 2); n++)
        {
            V[n][0] = (normN01(n,0) * z0 * V[n - 1][0] - normN02(n, 0) * rho * V[n - 2][0]) ;
            W[n][0] = 0.0;
        }
        // Calculate tesseral and sectorial terms
        for (int m = 1; m <= (gmData_.desiredOrder + 2); m++)
        {
            // Calculate V(m,m) .. V(n_max+1,m)

            V[m][m] = normFactorNN(m) * (x0 * V[m - 1][m - 1] - y0 * W[m - 1][m - 1]);
            W[m][m] = normFactorNN(m) * (x0 * W[m - 1][m - 1] + y0 * V[m - 1][m - 1]);
    
            if (m <= (gmData_.desiredDegree + 1))
            {
                V[m + 1][m] = normN01(m + 1, m)*z0 *V[m][m];
                W[m + 1][m] = normN01(m + 1, m)*z0 *W[m][m];
            }
            for (int n = (m + 2); n <= (gmData_.desiredDegree + 2); n++)
            {
                V[n][m] = (normN01(n, m) * z0 * V[n - 1][m] - normN02(n, m) * rho * V[n - 2][m]);
                W[n][m] = (normN01(n, m) * z0 * W[n - 1][m] - normN02(n, m) * rho * W[n - 2][m]);
            }
        }  // End 'for (int m = 1; m <= (desiredOrder + 2); m++) '

    }
       /* Computes the acceleration due to gravity in m/s^2.
       * @param E ECI to ECEF transformation matrix.
       * @return ECI acceleration in m/s^2.
       */
    Vector<double> SphericalHarmonicsModel::gravity(const Matrix<double>& E)
    {
        // dimension should be checked here
        // I'll do it latter...
        if ((E.rows() != 3) || (E.cols() != 3))
        {
            Exception e("Wrong input for computeVW");
            GPSTK_THROW(e);
        }

        Matrix<double> cs = gmData_.unnormalizedCS;


        // Calculate accelerations ax,ay,az
        double ax(0.0), ay(0.0), az(0.0);

        for (int m = 0; m <= gmData_.desiredOrder; m++)
        {
            for (int n = m; n <= gmData_.desiredDegree; n++)
            {
                if (m == 0)
                {
                    double C = cs[n][0];               // = C_n,0

                    ax -= C * V[n + 1][1];
                    ay -= C * W[n + 1][1];
                    az -= (n + 1)*C * V[n + 1][0];
                }
                else
                {
                    double C = cs[n][m];   // = C_n,m
                    double S = cs[m - 1][n]; // = S_n,m
                    double Fac = 0.5 * (n - m + 1) * (n - m + 2);

                    ax += 0.5*(-C*V[n + 1][m + 1] - S*W[n + 1][m + 1]) + Fac*(C*V[n + 1][m - 1] + S*W[n + 1][m - 1]);
                    ay += 0.5*(-C*W[n + 1][m + 1] + S*V[n + 1][m + 1]) + Fac*(-C*W[n + 1][m - 1] + S*V[n + 1][m - 1]);
                    az += (n - m + 1)*(-C*V[n + 1][m] - S*W[n + 1][m]);
                }

            }  // End of 'for (int n = m; n <= (desiredDegree+1) ; n++)'

        }  // End of 'for (int m = 0; m <= (desiredOrder+1); m++)'

           // Body-fixed acceleration
        Vector<double> a_bf(3, 0.0);
        a_bf(0) = ax;
        a_bf(1) = ay;
        a_bf(2) = az;

        a_bf = a_bf * (gmData_.GM / (gmData_.refDistance * gmData_.refDistance));

        // Inertial acceleration
        Matrix<double> Etrans = transpose(E);
        Vector<double> out = Etrans * a_bf;            // this line may be wrong  matrix * vector

        return out;

    }  // End of method 'GravityModel::gravity'

    Vector<double> SphericalHarmonicsModel::gravityNorm(const Matrix<double>& E)
    {
        // dimension should be checked here
        // I'll do it latter...
        if ((E.rows() != 3) || (E.cols() != 3))
        {
            Exception e("Wrong input for computeVW");
            GPSTK_THROW(e);
        }

        Matrix<double> cs = gmData_.unnormalizedCS;


        // Calculate accelerations ax,ay,az
        double ax(0.0), ay(0.0), az(0.0);

        for (int m = 0; m <= gmData_.desiredOrder; m++)
        {
            for (int n = m; n <= gmData_.desiredDegree; n++)
            {
                if (m == 0)
                {
                    double C = cs[n][0];               // = C_n,0
                    double norm = sqrt((2.0*n + 1)*(n + 2)*(n + 1) / (2.0*(2.0*n + 3)));
                    ax -= C * V[n + 1][1] * norm;
                    ay -= C * W[n + 1][1] * norm;
                    az -= C * V[n + 1][0] * norm*sqrt(2);
                }
                else
                {
                    double C = cs[n][m];   // = C_n,m
                    double S = cs[m - 1][n]; // = S_n,m
                    double Fac = 0.5 * (n - m + 1) * (n - m + 2);

                    ax += 0.5*(-C*V[n + 1][m + 1] - S*W[n + 1][m + 1]) + Fac*(C*V[n + 1][m - 1] + S*W[n + 1][m - 1]);
                    ay += 0.5*(-C*W[n + 1][m + 1] + S*V[n + 1][m + 1]) + Fac*(-C*W[n + 1][m - 1] + S*V[n + 1][m - 1]);
                    az += (n - m + 1)*(-C*V[n + 1][m] - S*W[n + 1][m]);
                }

            }  // End of 'for (int n = m; n <= (desiredDegree+1) ; n++)'

        }  // End of 'for (int m = 0; m <= (desiredOrder+1); m++)'

           // Body-fixed acceleration
        Vector<double> a_bf(3, 0.0);
        a_bf(0) = ax;
        a_bf(1) = ay;
        a_bf(2) = az;

        a_bf = a_bf * (gmData_.GM / (gmData_.refDistance * gmData_.refDistance));

        // Inertial acceleration
        Matrix<double> Etrans = transpose(E);
        Vector<double> out = Etrans * a_bf;            // this line may be wrong  matrix * vector

        return out;

    }  // End of method 'GravityModel::gravity'
       /* Computes the partial derivative of gravity with respect to position.
       * @return ECI gravity gradient matrix.
       * @param r ECI position vector.
       * @param E ECI to ECEF transformation matrix.
       */
    Matrix<double> SphericalHarmonicsModel::gravityGradient(const Matrix<double>& E)
    {
        // dimension should be checked here
        // I'll do it latter...
        if ((E.rows() != 3) || (E.cols() != 3))
        {
            Exception e("Wrong input for gravityGradient");
            GPSTK_THROW(e);
        }

        Matrix<double> cs = gmData_.unnormalizedCS;
        
        double xx = 0.0;
        double xy = 0.0;
        double xz = 0.0;
        double yy = 0.0;
        double yz = 0.0;
        double zz = 0.0;

        Matrix<double> out(3, 3, 0.0);

        for (int m = 0; m <= gmData_.desiredOrder; m++)
        {
            for (int n = m; n <= gmData_.desiredDegree; n++)
            {
                double Fac = (n - m + 2)*(n - m + 1);

                double C = cs[n][m];
                double S = (m == 0) ? 0.0 : cs[m - 1][n];   // yan changed
                                                            //S = (m==0)?Sn0(n):cs[m-1][n];   // yan changed

                zz += Fac*(C*V[n + 2][m] + S*W[n + 2][m]);

                if (m == 0)
                {
                    C = cs[n][0];   // = C_n,0

                    Fac = (n + 2)*(n + 1);
                    xx += 0.5 * (C*V[n + 2][2] - Fac*C*V[n + 2][0]);
                    xy += 0.5 * C * W[n + 2][2];

                    Fac = n + 1;
                    xz += Fac * C * V[n + 2][1];
                    yz += Fac * C * W[n + 2][1];
                }
                if (m > 0)
                {
                    C = cs[n][m];
                    S = cs[m - 1][n];

                    double f1 = 0.5*(n - m + 1);
                    double f2 = (n - m + 3)*(n - m + 2)*f1;

                    xz += f1*(C*V[n + 2][m + 1] + S*W[n + 2][m + 1]) - f2*(C*V[n + 2][m - 1] + S*W[n + 2][m - 1]);
                    yz += f1*(C*W[n + 2][m + 1] - S*V[n + 2][m + 1]) + f2*(C*W[n + 2][m - 1] - S*V[n + 2][m - 1]);         //* bug in JAT, I fix it

                    if (m == 1)
                    {
                        Fac = (n + 1)*n;
                        xx += 0.25*(C*V[n + 2][3] + S*W[n + 2][3] - Fac*(3.0*C*V[n + 2][1] + S*W[n + 2][1]));
                        xy += 0.25*(C*W[n + 2][3] - S*V[n + 2][3] - Fac*(C*W[n + 2][1] + S*V[n + 2][1]));
                    }
                    if (m > 1)
                    {
                        f1 = 2.0*(n - m + 2)*(n - m + 1);
                        f2 = (n - m + 4)*(n - m + 3)*f1*0.5;
                        xx += 0.25*(C*V[n + 2][m + 2] + S*W[n + 2][m + 2] - f1*(C*V[n + 2][m] + S*W[n + 2][m]) + f2*(C*V[n + 2][m - 2] + S*W[n + 2][m - 2]));

                        xy += 0.25*(C*W[n + 2][m + 2] - S*V[n + 2][m + 2] + f2*(-C*W[n + 2][m - 2] + S*V[n + 2][m - 2]));
                    }
                }
            }
            //consequence Laplace equation:
            //d2U/dx2 + d2U/dy2 + d2U/dz2 = 0
            yy = -xx - zz;

            out(0, 0) = xx;
            out(0, 1) = xy;
            out(0, 2) = xz;
            out(1, 0) = xy;
            out(1, 1) = yy;
            out(1, 2) = yz;
            out(2, 0) = xz;
            out(2, 1) = yz;
            out(2, 2) = zz;

        }  // for (int m = 0; m <= desiredOrder; m++) 

        const double R_ref = gmData_.refDistance;
        out = out * (gmData_.GM / (R_ref * R_ref * R_ref));

        // Rotate to ECI
        Matrix<double> Etrans = transpose(E);
        out = Etrans*(out*E);

        return out;         // the result should be checked

    }  // End of 'GravityModel::gravityGradient()'



       /** Call the relevant methods to compute the acceleration.
       * @param utc Time reference class
       * @param rb  Reference body class
       * @param sc  Spacecraft parameters and state
       * @return the acceleration [m/s^s]
       */
    void SphericalHarmonicsModel::doCompute(Epoch time,  Spacecraft& sc)
    {

        //Matrix<double> C2T = ReferenceFrames::J2kToECEFMatrix(time);
        Matrix<double> C2T = EarthRotation::eopStore.getJ2k2Ecef00(time);

        /*
        // debuging
        -0.96093274494562253,0.27678089792921495,0.00077086494829907383
        -0.27678077751454710,-0.96093305341706370,0.00026086203590256260
        0.00081295123707397028,3.7310272463024317e-005,0.99999966885906000

        C2T(0,0) = -0.96093274494562253;
        C2T(0,1) = 0.27678089792921495;
        C2T(0,2) = 0.00077086494829907383;

        C2T(1,0) = -0.27678077751454710;
        C2T(1,1) = -0.96093305341706370;
        C2T(1,2) = 0.00026086203590256260;

        C2T(2,0) = 0.00081295123707397028;
        C2T(2,1) = 3.7310272463024317e-005;
        C2T(2,2) = 0.99999966885906000;*/

        // corrcet earth tides
        // correctCSTides(time, correctSolidTide, correctOceanTide, correctPoleTide);

        // inertial position
        Vector<double> r_in = sc.R();
        //terestrial position
        Vector<double> r_bf = C2T*r_in;

        // Evaluate harmonic functions
        computeVW(r_bf);         // update VM

        // a
        a = gravity(C2T);

        // da_dr
        da_dr = gravityGradient(C2T);

        //da_dv
        da_dv.resize(3, 3, 0.0);

        //da_dp

    }

    // Correct tides to coefficients 
    void SphericalHarmonicsModel::correctCSTides(Epoch t, bool solidFlag, bool oceanFlag, bool poleFlag)
    {
        // lower-case because 1) upper case is ugly and 2) name
        // collisions with macros.
        // copy CS
        Matrix<double> cs = gmData_.unnormalizedCS;
        Vector<double> Sn0(cs.rows(), 0.0);

        // 
        double mjd = t.MJD();
        double leapYears = (mjd - gmData_.refMJD) / 365.25;

        double detC20 = normFactor(2, 0)*leapYears*gmData_.dotC20;
        double detC21 = normFactor(2, 1)*leapYears*gmData_.dotC21;
        double detS21 = normFactor(2, 1)*leapYears*gmData_.dotS21;

        cs(2, 0) += detC20;
        cs(2, 1) += detC21;
        cs(0, 2) += detS21;

        // correct solid tide
        if (solidFlag)
        {
            // C20 C21 C22 C30 C31 C32 C33 C40 C41 C42
            double dc[10] = { 0.0 };
            double ds[10] = { 0.0 };
            solidTide.getSolidTide(t.MJD(), dc, ds);

            // c
            cs(2, 0) += normFactor(2, 0)*dc[0];
            cs(2, 1) += normFactor(2, 1)*dc[1];
            cs(2, 2) += normFactor(2, 2)*dc[2];
            cs(3, 0) += normFactor(3, 0)*dc[3];
            cs(3, 1) += normFactor(3, 1)*dc[4];
            cs(3, 2) += normFactor(3, 2)*dc[5];
            cs(3, 3) += normFactor(3, 3)*dc[6];
            cs(4, 0) += normFactor(4, 0)*dc[7];
            cs(4, 1) += normFactor(4, 1)*dc[8];
            cs(4, 2) += normFactor(4, 2)*dc[9];
            /// s
            Sn0(2) += normFactor(2, 0)*ds[0];   // s20
            cs(0, 2) += normFactor(2, 1)*ds[1];
            cs(1, 2) += normFactor(2, 2)*ds[2];
            Sn0(3) += normFactor(3, 0)*ds[3];   // s30
            cs(0, 3) += normFactor(3, 1)*ds[4];
            cs(1, 3) += normFactor(3, 2)*ds[5];
            cs(2, 3) += normFactor(3, 3)*ds[6];
            Sn0(4) += normFactor(4, 0)*ds[7];   // s40
            cs(0, 4) += normFactor(4, 1)*ds[8];
            cs(1, 4) += normFactor(4, 2)*ds[9];

        }

        // correct ocean tide
        if (oceanFlag)
        {
            // C20 C21 C22 C30 C31 C32 C33 C40 C41 C42 C43 C44
            double dc[12] = { 0.0 };
            double ds[12] = { 0.0 };
            oceanTide.getOceanTide(t.MJD(), dc, ds);

            // c
            cs(2, 0) += normFactor(2, 0)*dc[0];
            cs(2, 1) += normFactor(2, 1)*dc[1];
            cs(2, 2) += normFactor(2, 2)*dc[2];
            cs(3, 0) += normFactor(3, 0)*dc[3];
            cs(3, 1) += normFactor(3, 1)*dc[4];
            cs(3, 2) += normFactor(3, 2)*dc[5];
            cs(3, 3) += normFactor(3, 3)*dc[6];
            cs(4, 0) += normFactor(4, 0)*dc[7];
            cs(4, 1) += normFactor(4, 1)*dc[8];
            cs(4, 2) += normFactor(4, 2)*dc[9];
            cs(4, 3) += normFactor(4, 3)*dc[10];
            cs(4, 4) += normFactor(4, 4)*dc[11];


            /// s
            Sn0(2) += normFactor(2, 0)*ds[0];   // s20
            cs(0, 2) += normFactor(2, 1)*ds[1];
            cs(1, 2) += normFactor(2, 2)*ds[2];
            Sn0(3) += normFactor(3, 0)*ds[3];   // s30
            cs(0, 3) += normFactor(3, 1)*ds[4];
            cs(1, 3) += normFactor(3, 2)*ds[5];
            cs(2, 3) += normFactor(3, 3)*ds[6];
            Sn0(4) += normFactor(4, 0)*ds[7];   // s40
            cs(1, 4) += normFactor(4, 1)*ds[8];
            cs(2, 4) += normFactor(4, 2)*ds[9];
            cs(3, 4) += normFactor(4, 1)*ds[10];
            cs(4, 4) += normFactor(4, 2)*ds[11];

        }

        // correct pole tide
        if (poleFlag)
        {
            double dC21 = 0.0;
            double dS21 = 0.0;
            poleTide.getPoleTide(t.MJD(), dC21, dS21);

            cs(2, 1) += normFactor(2, 1)*dC21;
            cs(0, 2) += normFactor(2, 1)*dS21;
        }

    }  // End of method 'GravityModel::correctCSTides()'

    double SphericalHarmonicsModel::normFactor(int n, int m)
    {
        // The input should be n >= m >= 0

        double fac(1.0);
        for (int i = (n - m + 1); i <= (n + m); i++)
        {
            fac = fac * double(i);
        }

        //  double delta = (m == 0) ? 1.0 : 0.0;

        double num = (2.0 * n + 1.0) * (2.0 - delta(m));

        // We should make sure fac!=0, but it won't happen on the case,
        // so we just skip handling it
        double out = std::sqrt(num / fac);

        return out;

    }  // End of method 'GravityModel::normFactor())
 

    double SphericalHarmonicsModel:: normN01(int n, int m)
    {
        double res = (4.0*n*n - 1) / (n*n - m*m);
        return std::sqrt(res);
    }

    //
    double SphericalHarmonicsModel::normN02(int n, int m)
    {
        int nsq = (n - 1)*(n - 1);

        double nom = (2.0*n+1)*(nsq-m*m);
        double denom = (2.0*n - 3)*(n*n - m*m);

        return std::sqrt(nom/denom);
    }

    //
    double  SphericalHarmonicsModel::normFactorNN(int n)
    {
        double nom = (2.0*n + 1)*(2 - delta(n));
        double denom = 2.0*n *(2 - delta(n - 1));

        return std::sqrt(nom / denom);
    }
     //
    //void SphericalHarmonicsModel::test()
    //{
    //    Vector<double> r(3, 0.0);
    //    Matrix<double> E(3, 3, 0.0);

    //    r(0) = 6525.919e3;
    //    r(1) = 1710.416e3;
    //    r(2) = 2508.886e3;

    //    E = ident<double>(3);

    //    computeVW(r, E);         // update VM

    //                             // a
    //    Vector<double> a = gravity(r, E);

    //    Matrix<double> da_dr = gravityGradient(r, E);

    //    std::cout << std::setprecision(12) << a << std::endl;
    //    std::cout << da_dr << std::endl;

    //}  // End of method 'GravityModel::test()'
}

