#include "GnssSolution.h"
#include"PRSolution2.hpp"
#include"Bancroft.hpp"
#include"PowerSum.hpp"
#include"KalmanSolver.h"

using namespace std;
using namespace gpstk;

namespace pod
{
     std::ostream& GnssSolution:: printMsg(const gpstk::CommonTime& time, const char* msg)
     {
        return( cout << "Epoch: " << CivilTime(time).asString() << " "<< msg << endl);
     }
   
     GnssSolution::GnssSolution(GnssDataStore_sptr gnssData, double sigma = 50.0)
         :data(gnssData), Equations(make_shared<EquationComposer>()), maxSigma(sigma)
     {
     }

    GnssSolution::~GnssSolution() {}

    int GnssSolution::computeApprPos(
        const gpstk::IRinex & gRin,
        const gpstk::XvtStore<gpstk::SatID>& Eph,
        gpstk::Position& pos)
    {
        auto svs = gRin.getBody().getVectorOfSatID().toStdVector();
        auto meas = gRin.getBody().getVectorOfTypeID(codeL1).toStdVector();
        Matrix<double> svp;
        if (PRSolution2::PrepareAutonomousSolution(gRin.getHeader().epoch, svs, meas, Eph, svp))
            return -1;
       
        Bancroft ban;
        Vector<double> res;
        if (ban.Compute(svp, res))
            return -2;
        pos = Position(res(0), res(1), res(2));

        return 0;
    }
    void GnssSolution::printSolution(const KalmanSolver& solver, const gpstk::CommonTime& time, GnssEpoch& gEpoch)
    {
       
        for (auto && it : Equations->currentUnknowns())
        {
            if (it.type == TypeID::dx || it.type == TypeID::dy || it.type == TypeID::dz)
                continue;

            if (it.sv == SatID::dummy)
            {
                if (it.type == TypeID::cdt)
                    gEpoch.slnData[TypeID::recCdt] = solver.getSolution(it);
                else if (it.type == TypeID::wetMap)
                    gEpoch.slnData[TypeID::recZTropo] = solver.getSolution(it);
                else
                    gEpoch.slnData[it.type] = solver.getSolution(it);
            }
            else
                gEpoch.satData[it.sv][it.type] = solver.getSolution(it);
        }
        Position newPos;
        double stDev3D(NAN);
        if (solver.isValid())
        {

            newPos[0] = nominalPos.X() + solver.getSolution(FilterParameter(TypeID::dx));    // dx    - #4
            newPos[1] = nominalPos.Y() + solver.getSolution(FilterParameter(TypeID::dy));    // dy    - #5
            newPos[2] = nominalPos.Z() + solver.getSolution(FilterParameter(TypeID::dz));    // dz    - #6

            double varX = solver.getVariance(FilterParameter(TypeID::dx));     // Cov dx    - #8
            double varY = solver.getVariance(FilterParameter(TypeID::dy));     // Cov dy    - #9
            double varZ = solver.getVariance(FilterParameter(TypeID::dz));     // Cov dz    - #10
            stDev3D = sqrt(varX + varY + varZ);
        }
        int numSats = gEpoch.satData.size();
       // int slnType = ( solver.getSigma() < getMaxSigma() && numSats >= 4) ? desiredSlnType() : 0;
        //if (solver.getSigma() > getMaxSigma())
        //{
         
       // }
        SlnType slnType = solver.isValid() ? desiredSlnType() : SlnType::NONE_SOLUTION;

        gEpoch.slnData.insert(make_pair(TypeID::recSlnType, slnType));

        gEpoch.slnData.insert(make_pair(TypeID::recX, newPos.X()));
        gEpoch.slnData.insert(make_pair(TypeID::recY, newPos.Y()));
        gEpoch.slnData.insert(make_pair(TypeID::recZ, newPos.Z()));

        gEpoch.slnData.insert(make_pair(TypeID::recStDev3D, stDev3D));

        gEpoch.slnData.insert(make_pair(TypeID::sigma, solver.getSigma()));

    };
}
