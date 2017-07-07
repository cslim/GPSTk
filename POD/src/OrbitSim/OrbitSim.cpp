#include"OrbitSim.h"

#include"KeplerOrbit.hpp"
//using namespace gpstk;

namespace POD
{

    // Constructor
    OrbitSim::OrbitSim():
        //: pIntegrator(&defIntehrator),
        //pOrbit(&defOrbit),
        curT(0.0)
    {
        setDefaultIntegrator();
        setDefaultOrbit();
        setStepSize(3.0);

    }  // End of constructor 'OrbitSim::OrbitSim()'


       // Default destructor
    OrbitSim::~OrbitSim()
    {
        pIntegrator.reset();
        pOrbit.reset();

    }

    /* Take a single integration step.
    *
    * @param x     time or independent variable
    * @param y     containing needed inputs (usually the state)
    * @param tf    next time
    * @return      containing the new state
    */
    Vector<double> OrbitSim::integrateTo(double t, Vector<double> y, double tf)
    {
        try
        {
            curT = tf;

            curState = pIntegrator->integrateTo(t, y, pOrbit.get(), tf);

            updateMatrix();

            return curState;
        }
        catch (...)
        {
            Exception e("Error in OrbitPropagator::integrateTo()");
            GPSTK_THROW(e);
        }

    }  // End of method 'OrbitSim::integrateTo()'


    bool OrbitSim::integrateTo(double tf)
    {
        try
        {
            double t = curT;
            Vector<double> y = curState;

            curT = tf;
          
            curState = pIntegrator->integrateTo(t, y, pOrbit.get(), tf);

            updateMatrix();

            return true;
        }
        catch (Exception& e)
        {
            GPSTK_RETHROW(e);
        }

        catch (...)
        {
            Exception e("Unknown error in OrbitSim::integrateTo()");
            GPSTK_THROW(e);
        }

        return false;

    }  // End of method 'OrbitSim::integrateTo()'

       /*
       * set init state
       * utc0   init epoch
       * rv0    init state
       */
    OrbitSim& OrbitSim::setInitState(CommonTime t0, Vector<double> rv0)
    {
        const int np = 0; //= setFMT.size();

        curT = double(0.0);
        curState.resize(42 + 6 * np, 0.0);

        // position and velocity
        curState(0) = rv0(0);
        curState(1) = rv0(1);
        curState(2) = rv0(2);
        curState(3) = rv0(3);
        curState(4) = rv0(4);
        curState(5) = rv0(5);

        double I[9] = { 1.0, 0.0 ,0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };

        for (int i = 0; i < 9; i++)
        {
            curState(6 + i) = I[i];
            curState(33 + 3 * np + i) = I[i];
        }

        updateMatrix();

        // set reference epoch
        setRefEpoch(t0);

        return (*this);

    }  // End of method 'OrbitSim::setInitState()'


       /// update phiMatrix sMatrix and rvState from curState
    void OrbitSim::updateMatrix()
    {
        const int np = getNP();

        Vector<double> dr_dr0(9, 0.0);
        Vector<double> dr_dv0(9, 0.0);
        Vector<double> dr_dp0(3 * np, 0.0);
        Vector<double> dv_dr0(9, 0.0);
        Vector<double> dv_dv0(9, 0.0);
        Vector<double> dv_dp0(3 * np, 0.0);

        for (int i = 0; i < 9; i++)
        {
            dr_dr0(i) = curState(6 + i);
            dr_dv0(i) = curState(15 + i);

            dv_dr0(i) = curState(24 + 3 * np + i);
            dv_dv0(i) = curState(33 + 3 * np + i);
        }
        for (int i = 0; i < 3 * np; i++)
        {
            dr_dp0 = curState(24 + i);
            dv_dp0 = curState(42 + 3 * np + i);
        }

        // update phiMatrix
        phiMatrix.resize(6, 6, 0.0);

        // dr/dr0
        phiMatrix(0, 0) = dr_dr0(0);
        phiMatrix(0, 1) = dr_dr0(1);
        phiMatrix(0, 2) = dr_dr0(2);
        phiMatrix(1, 0) = dr_dr0(3);
        phiMatrix(1, 1) = dr_dr0(4);
        phiMatrix(1, 2) = dr_dr0(5);
        phiMatrix(2, 0) = dr_dr0(6);
        phiMatrix(2, 1) = dr_dr0(7);
        phiMatrix(2, 2) = dr_dr0(8);
        // dr/dv0
        phiMatrix(0, 3) = dr_dv0(0);
        phiMatrix(0, 4) = dr_dv0(1);
        phiMatrix(0, 5) = dr_dv0(2);
        phiMatrix(1, 3) = dr_dv0(3);
        phiMatrix(1, 4) = dr_dv0(4);
        phiMatrix(1, 5) = dr_dv0(5);
        phiMatrix(2, 3) = dr_dv0(6);
        phiMatrix(2, 4) = dr_dv0(7);
        phiMatrix(2, 5) = dr_dv0(8);
        // dv/dr0
        phiMatrix(3, 0) = dv_dr0(0);
        phiMatrix(3, 1) = dv_dr0(1);
        phiMatrix(3, 2) = dv_dr0(2);
        phiMatrix(4, 0) = dv_dr0(3);
        phiMatrix(4, 1) = dv_dr0(4);
        phiMatrix(4, 2) = dv_dr0(5);
        phiMatrix(5, 0) = dv_dr0(6);
        phiMatrix(5, 1) = dv_dr0(7);
        phiMatrix(5, 2) = dv_dr0(8);
        // dv/dv0
        phiMatrix(3, 3) = dv_dv0(0);
        phiMatrix(3, 4) = dv_dv0(1);
        phiMatrix(3, 5) = dv_dv0(2);
        phiMatrix(4, 3) = dv_dv0(3);
        phiMatrix(4, 4) = dv_dv0(4);
        phiMatrix(4, 5) = dv_dv0(5);
        phiMatrix(5, 3) = dv_dv0(6);
        phiMatrix(5, 4) = dv_dv0(7);
        phiMatrix(5, 5) = dv_dv0(8);

        // update sMatrix 6*np
        sMatrix.resize(6, np, 0.0);
        for (int i = 0; i<np; i++)
        {
            sMatrix(0, i) = dr_dp0(0 * np + i);
            sMatrix(1, i) = dr_dp0(1 * np + i);
            sMatrix(2, i) = dr_dp0(2 * np + i);

            sMatrix(3, i) = dv_dp0(0 * np + i);
            sMatrix(4, i) = dv_dp0(1 * np + i);
            sMatrix(5, i) = dv_dp0(2 * np + i);
        }

        // update rvVector
        rvVector.resize(6, 0.0);
        for (int i = 0; i < 6; i++)
        {
            rvVector(i) = curState(i);
        }

    }  // End of method 'OrbitSim::updateMatrix()'


       /* set initial state of the the integrator
       *
       *  v      3
       * dr_dr0    3*3
       * dr_dv0   3*3
       * dr_dp0   3*np
       * dv_dr0   3*3
       * dv_dv0   3*3
       * dv_dp0   3*np
       */
    void OrbitSim::setState(Vector<double> state)
    {
        int np = (state.size() - 42) / 6;
        if (np<0)
        {
            Exception e("The size of the imput state is not valid");
            GPSTK_THROW(e);
        }
        curT = 0;
        curState.resize(state.size(), 0.0);
        for (size_t i = 0; i<state.size(); i++)
        {
            curState(i) = state(i);
        }

        updateMatrix();

    }  // End of method 'OrbitSim::setState()'


    Vector<double> OrbitSim::rvState(bool isJ2k)
    {
        if (isJ2k)
            return rvVector;
        else
            return EarthRotation::eopStore.convertJ2k2Ecef(getCurTime(), rvVector);
    }  // End of method 'OrbitSim::rvState()'


       /// write curT curState to a file
    void OrbitSim::writeToFile(ostream& s) const
    {
        Epoch utcRef = pOrbit->getRefEpoch();
        utcRef += curT;

        const int np = getNP();

        s << fixed;
        s << "#" << utcRef << " "
            << setprecision(12) << utcRef << endl;

        for (int i = 0; i<6; i++)
        {
            s << setw(20) << setprecision(12) << rvVector(i) << " ";
        }
        s << endl;

        // [phi s]
        for (int i = 0; i<6; i++)
        {
            for (int j = 0; j<6; j++)
            {
                s << setw(20) << setprecision(12) << phiMatrix(i, j) << " ";
            }
            for (int j = 0; j<np; j++)
            {
                s << setw(20) << setprecision(12) << sMatrix(i, j) << " ";
            }

            s << endl;
        }
    }


    /*
    void OrbitPropagator::setForceModel(ForceModelSetting& fms)
    {
    if(pOrbit)
    {
    pOrbit->setForceModel(fms);
    }
    }*/

    /* For Testing and Debuging...
    */
    void OrbitSim::test()
    {
        cout << "testing OrbitPropagator[KeplerOrbit]" << endl;
        cout << fixed << setprecision(6);

        // load global data
      //  IERS::loadSTKFile("ERP\\COD17252.ERP");
        //ReferenceFrames::setJPLEphFile("InputData\\DE405\\jplde405");

        ofstream fout("outorbit.txt");

        CommonTime t0 = (CommonTime)CivilTime(2013, 1, 30, 0, 0, 0.0,TimeSystem::GPS);

        double state[42] = { 2682920.8943,4652720.5672,4244260.0400,2215.5999,4183.3573,-5989.0576,
            1,0,0,
            0,1,0,
            0,0,1,
            0,0,0,
            0,0,0,
            0,0,0,
            0,0,0,
            0,0,0,
            0,0,0,
            1,0,0,
            0,1,0,
            0,0,1 };

        Vector<double> y0(42, 0.0);
        y0 = state;

        Vector<double> yy0(6, 0.0);
        yy0(0) = y0(0);
        yy0(1) = y0(1);
        yy0(2) = y0(2);
        yy0(3) = y0(3);
        yy0(4) = y0(4);
        yy0(5) = y0(5);


        Vector<double> kep(6, 0.0);
      //  kep = KeplerOrbit::Elements(ASConstant::GM_Earth, yy0);


        OrbitSim op;

        op.setRefEpoch(t0);
        op.setStepSize(10.0);

        double tt = 3600.0 * 24;
        double step = 60.0;

        cout << fixed << setw(12) << setprecision(5);

        double t = 0.0;
        while (t < tt)
        {
            Vector<double> yy = op.integrateTo(t, y0, t + step);

            // Commented out because of compiler errors I can't be bothered to figure out
            //         fout << op;

            Vector<double> yy_prev(6, 0.0);
            Vector<double> yy_out(6, 0.0);
            for (int i = 0; i<6; i++)
            {
                yy_prev(i) = y0(i);
                yy_out(i) = yy(i);
            }

            Vector<double> yy_ref(6, 0.0);
            Matrix<double> phi_ref(6, 6, 0.0);
         //   KeplerOrbit::TwoBody(ASConstant::GM_Earth, yy0, t + step, yy_ref, phi_ref);
         //   Vector<double> checky0 = KeplerOrbit::State(ASConstant::GM_Earth, kep, t + step);

            Matrix<double> phi = op.transitionMatrix();

            Vector<double> diff = yy_out - yy_ref;

           // UTCTime utc = op.getCurTime();
           // cout << utc << " " << diff << endl;
            cout << phi - phi_ref << endl;

            t += step;
            y0 = yy;
        }

        fout.close();

    }

    void OrbitSim:: runTest()
    {
      /*  cout << EarthRotation::eopStore.loadEOP("finals2000A.data") << endl;
        cout << "ERP loading..."<<endl;*/
        testFwBw();

    }
    void OrbitSim::testKepler()
    {
        cout << "Test kepler motion" << endl;

        ofstream os("Integr_test.out");

        OrbitSim op;
        ForceModelData fmd;
        fmd.gData.loadModel("GEN\\EGM2008_TideFree_nm150.txt");

        op.setOrbit(new OrbitModel(fmd));
        op.setStepSize(60);

        Vector<double> elts(6, 0.0);
        double step(5200.0), tt(86400.0 * 10);

        elts(0) = 6487264.0502067700; //A, m
        elts(1) = 0.001;              //ecc
        elts(2) = M_PI_2;                //i, rad
        elts(3) = 2.0;                //OMG, rad
        elts(4) = 3.0;                //omg, rad
        elts(5) = 0.0;                //M, rad

        CommonTime t0 = (CommonTime)(CivilTime(2013, 01, 30, 0, 0, 0.0, TimeSystem::TT));
        double mu = 3.98600441500e+14;
        Vector<double> sv = KeplerOrbit::State(mu, elts, 0);

        op.setInitState(t0, sv);
      
        auto sv0 = op.getCurState();
        os << fixed << setw(12) << setprecision(6);
        os << op.getCurTime() << " " << op.getCurState() << endl;

        //
        double t = 0, dMax = 0;

        //
        while (t < tt)
        {
            if (!op.integrateTo(t + step))
            {
                cout << "failed to integrate\n";
                break;
            }

            t += step;

            auto svi = op.getCurState();
            auto dsv = svi - sv0;
            
            double d = 0;
            for (size_t i = 0; i < 3; i++)
                d += dsv[i] * dsv[i];
            d = sqrt(d) * 1000;
            dMax = (d > dMax) ? d : dMax;
            os << op.getCurTime() << " " << d << endl;
        }
        os << "max Err"<< " " << dMax << endl;
        os.close();
    }
    void OrbitSim::testFwBw()
    {
        cout << "Test Fwd-Bwd" << endl;

        ofstream os("Integr_test.out");

        OrbitSim op;
        ForceModelData fmd;
        fmd.gData.loadModel("GEN\\EGM2008_TideFree_nm150.txt");
        op.setOrbit(new OrbitModel(fmd));
        op.setStepSize(5);

        Vector<double> elts(6, 0.0);
        double step(5200.0), tt(10*86400);

        elts(0) = 10000000; //6487264.0502067700; //A, m
        elts(1) = 0.000;              //ecc
        elts(2) = 0;                  //i, rad
        elts(3) = 0.0;                //OMG, rad
        elts(4) = 0.0;                //omg, rad
        elts(5) = 0.0;                //M, rad

        CommonTime t0 = (CommonTime)(CivilTime(2013, 01, 30, 0, 0, 0.0, TimeSystem::TT));
        double mu = 3.98600441500e+14;
        Vector<double> sv = KeplerOrbit::State(mu, elts, 0);

        op.setInitState(t0, sv);

        auto sv0 = op.getCurState();
        os << fixed << setw(12) << setprecision(6);
        os << op.getCurTime() << " " << op.getCurState() << endl;

        //
        double t = 0, dMax = 0;

        if (!op.integrateTo( tt))
        {
            cout << "failed to  fwd integrate\n";
        }
        os << op.getCurTime() << " " << op.getCurState() << endl;
        op.updateRefEpoch();
        os << op.getCurTime() << " " << op.getCurState() << endl;
        if (!op.integrateTo(-tt))
        {
            cout << "failed to bwd integrate\n";
        }

        os << op.getCurTime() << " " << op.getCurState() << endl;
        auto svi = op.getCurState();
        auto dsv = svi - sv0;

        double d = 0;
        for (size_t i = 0; i < 3; i++)
            d += dsv[i] * dsv[i];
        d = sqrt(d) * 1000;
        dMax = (d > dMax) ? d : dMax;
       
        os << op.getCurTime() << " " << d << endl;

        //  os << "max Err" << " " << dMax << endl;
        os.close();
    }
}