#ifndef POD_ORBIT_SIM_H
#define POD_ORBIT_SIM_H
#include<memory>

#include"RungeKuttaFehlberg.hpp"
#include"Integrator.hpp"
#include"EarthRotation.h"
#include"OrbitModel.h"

using namespace gpstk;
using namespace std;

namespace POD
{
    typedef unique_ptr<Integrator> IntegratorUniquePtr;
    typedef unique_ptr<OrbitModel> OrbitModelUniquePtr;
    class OrbitSim
    {

    public:
        
        /// Default constructor
        OrbitSim();

        /// Default destructor
        virtual ~OrbitSim();


        /// set integrator, default is Rungge-Kutta 78
        OrbitSim& setIntegrator(Integrator* pIntg)
        {
            pIntegrator.reset(pIntg);
            return (*this);
        }

        /// set the integrator to the default one
        OrbitSim& setDefaultIntegrator()
        {
            pIntegrator.reset(new RungeKuttaFehlberg);
            return (*this);
        }
        OrbitSim& setDefaultOrbit()
        {
            pOrbit.reset(new OrbitModel);
            return (*this);
        }
        /// set equation of motion of the orbit
        OrbitSim& setOrbit(OrbitModel *  orbit)
        {
            pOrbit.reset(orbit);
            return (*this);
        }

        /// set the orbit to the default one
        OrbitModel& getOrbit()
        {
            return (*pOrbit);
        }

        /// set step size of the integrator
        OrbitSim& setStepSize(double step_size = 10.0)
        {
            pIntegrator->setStepSize(step_size); 
            return (*this);
        }

        /**set init state
        * @param utc0   init epoch
        * @param rv0    init state
        * @return
        */
        OrbitSim& setInitState(CommonTime utc0, Vector<double> rv0);


        /** Take a single integration step.
        * @param tf    next time
        * @return      state of integration
        */
        virtual bool integrateTo(double tf);


        /// return the position and velocity , the dimension is 6
        Vector<double> rvState(bool bJ2k = true);

        /// return the rv state transition matrix 6*6
        Matrix<double> transitionMatrix()
        {
            return phiMatrix;
        }

        /// return the sensitivity matrix 6*np
        Matrix<double> sensitivityMatrix()
        {
            return sMatrix;
        }

        /// return the current epoch
        CommonTime getCurTime()
        {
            CommonTime utc = pOrbit->getRefEpoch();
            utc += curT; 
            return utc;
        }

        /// return the current state
        Vector<double> getCurState()
        {
            return curState;
        }

        OrbitSim& updateRefEpoch()
        {
            this->pOrbit->setRefEpoch(getCurTime());
            curT = 0;
            return (*this);
        }
        /// get numble of force model parameters
        int getNP() const
        {
            return (curState.size() - 42) / 6;
        }

        /// write curT curState to a file
        void writeToFile(std::ostream& s) const;


        ///tests methods
        void test();
        static void runTest();


    protected:

        /** Take a single integration step.
        *
        * @param x     time or independent variable
        * @param y     containing needed inputs (usually the state)
        * @param tf    next time
        * @return      containing the new state
        */
        virtual Vector<double> integrateTo(double t, Vector<double> y, double tf);

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
        void setState(Vector<double> state);

        /// set reference epoch
        void setRefEpoch(CommonTime utc)
        {
            pOrbit->setRefEpoch(utc);
        }

        /// update phiMatrix sMatrix and rvState from curState
        void updateMatrix();


        /// Pointer to an ode solver default is RungeKutta78
        IntegratorUniquePtr   pIntegrator;

        /// Pointer to the Equation Of Motion of a satellite
        OrbitModelUniquePtr  pOrbit;
       
        /// Default integrator
        RungeKuttaFehlberg defIntehrator;
        
        ///Default force model
        OrbitModel  defOrbit;

    private:

        static void testKepler();
        static void testFwBw();

        /// current time since reference epoch
        double curT;

        /// current state
        // r        3
        // v        3
        // dr_dr0   3*3
        // dr_dv0   3*3
        // dr_dp0   3*np
        // dv_dr0   3*3
        // dv_dv0   3*3
        // dv_dp0   3*np
        Vector<double> curState;         // 42+6*np

        /// the position and velocity
        Vector<double>   rvVector;      // 6

        /// state transition matrix
        Matrix<double> phiMatrix;      // 6*6

        /// the sensitivity matrix 
        Matrix<double> sMatrix;         // 6*np

    };
}

#endif // !POD_ORBIT_SIM_H

