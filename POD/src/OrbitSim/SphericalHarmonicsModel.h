#ifndef POD_SHERICAL_HARMONICS_MODEL_H
#define POD_SHERICAL_HARMONICS_MODEL_H

#include"ForceModel.hpp"
#include "GravityModel.h"
#include "EarthSolidTide.hpp"
#include "EarthOceanTide.hpp"
#include "EarthPoleTide.hpp"

using namespace gpstk;

namespace POD
{
    class SphericalHarmonicsModel : public GravityModel
    {
    public: 
        SphericalHarmonicsModel(const GravityModelData &gData);
        
        /// Default destructor
        virtual ~SphericalHarmonicsModel() {};


        /** Computes the acceleration due to gravity in m/s^2.
        * @param E ECI to ECEF transformation matrix.
        * @return ECI acceleration in m/s^2.
        */
        Vector<double> gravity(const Matrix<double>& E);

        Vector<double> gravityNorm(const Matrix<double>& E);
        /** Computes the partial derivative of gravity with respect to position.
        * @return ECI gravity gradient matrix.
        * @param r ECI position vector.
        * @param E ECI to ECEF transformation matrix.
        */
        Matrix<double> gravityGradient(const Matrix<double>& E);


        /** Call the relevant methods to compute the acceleration.
        * @param utc Time reference class
        * @param rb  Reference body class
        * @param sc  Spacecraft parameters and state
        * @return the acceleration [m/s^s]
        */
        virtual void doCompute(Epoch time,  Spacecraft& sc) override;

        /// Return force model name
        virtual std::string modelName() const
        {
            return "SphericalHarmonicsModel";
        }

        /// return the force model index
        virtual int forceIndex() const
        {
            return ForceModel::FMI_GEOEARTH;
        }

    protected:

        /* Evaluates the two harmonic functions V and W.
        * @param r ECEF position vector.
        */
        void computeVW(Vector<double> r_bf);

        void computeNormVW(Vector<double> r_bf);

        /// Add tides to coefficients 
        void correctCSTides(Epoch t, bool solidFlag = false, bool oceanFlag = false, bool poleFlag = false);

        /// normalized coefficient
        double normFactor(int n, int m);
        double normN01(int n, int m);
        double normN02(int n, int m);
        double normFactorNN(int n);

        /// V W  (nmax+3)*(nmax+3)
        /// Harmonic function V and W
        Matrix<double> V, W;

        /// Objects to do earth tides correction
        EarthSolidTide  solidTide;
        EarthPoleTide   poleTide;
        EarthOceanTide  oceanTide;
    private :

        double delta(int n) 
        {
            return (n == 0) ? 1.0 : 0.0;
        }
        double E(int n)
        {
            return 2.0-delta(n);
        }
    };
}

#endif //!POD_SHERICAL_HARMONICS_MODEL_H