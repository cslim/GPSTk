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
        return( cout << "Epoch: " << CivilTime(time) << " "<< msg << endl);
     }
   
     GnssSolution::GnssSolution(GnssDataStore_sptr gnssData, double sigma = 50.0)
         :data(gnssData), Equations(make_shared<EquationComposer>()), maxSigma(sigma)
     {
     }

    GnssSolution::~GnssSolution() {}

    int GnssSolution::computeApprPos(
        const gpstk::gnssRinex & gRin,
        const gpstk::XvtStore<gpstk::SatID>& Eph,
        gpstk::Position& pos)
    {
        auto svs = gRin.getVectorOfSatID().toStdVector();
        auto meas = gRin.getVectorOfTypeID(codeL1).toStdVector();
        Matrix<double> svp;
        if (PRSolution2::PrepareAutonomousSolution(gRin.header.epoch, svs, meas, Eph, svp))
            return -1;
       
        Bancroft ban;
        Vector<double> res;
        if (ban.Compute(svp, res))
            return -2;
        pos = Position(res(0), res(1), res(2));

        return 0;
    }
    void GnssSolution::printSolution(std::ofstream& os, const KalmanSolver& solver, const gpstk::CommonTime& time, GnssEpoch& gEpoch)
    {
        PowerSum psum;
        for (auto it : solver.PostfitResiduals())
            psum.add(it);
        double sigma = sqrt(psum.variance());

        Position newPos;
        newPos[0] = nominalPos.X() + solver.getSolution(FilterParameter(TypeID::dx));    // dx    - #4
        newPos[1] = nominalPos.Y() + solver.getSolution(FilterParameter(TypeID::dy));    // dy    - #5
        newPos[2] = nominalPos.Z() + solver.getSolution(FilterParameter(TypeID::dz));    // dz    - #6

        double cdt = solver.getSolution(FilterParameter(TypeID::cdt));

        double varX = solver.getVariance(FilterParameter(TypeID::dx));     // Cov dx    - #8
        double varY = solver.getVariance(FilterParameter(TypeID::dy));     // Cov dy    - #9
        double varZ = solver.getVariance(FilterParameter(TypeID::dz));     // Cov dz    - #10
        double stDev3D = sqrt(varX + varY + varZ);

        int numSats = gEpoch.satData.size();
        int slnType = (sigma < getMaxSigma() && numSats >= 4) ? desiredSlnType() : 0;

        gEpoch.slnData.insert(make_pair(TypeID::recSlnType, slnType));

        gEpoch.slnData.insert(make_pair(TypeID::recX, newPos.X()));
        gEpoch.slnData.insert(make_pair(TypeID::recY, newPos.Y()));
        gEpoch.slnData.insert(make_pair(TypeID::recZ, newPos.Z()));

        gEpoch.slnData.insert(make_pair(TypeID::recStDev3D, stDev3D));
        gEpoch.slnData.insert(make_pair(TypeID::recCdt, cdt));
        gEpoch.slnData.insert(make_pair(TypeID::sigma, sigma));
        
        const auto& types = Equations->currentUnknowns();

        FilterParameter  ISB_GLN(TypeID::recISB_GLN);
        if (types.find(ISB_GLN) != types.end())
            gEpoch.slnData.insert(make_pair(TypeID::recISB_GLN, solver.getSolution(ISB_GLN)));

        FilterParameter IFB_GPS_L2(TypeID::recIFB_GPS_L2);
        if (types.find(IFB_GPS_L2) != types.end())
            gEpoch.slnData.insert(make_pair(TypeID::recIFB_GPS_L2, solver.getSolution(IFB_GPS_L2)));

        FilterParameter IFB_GLN_L2(TypeID::recIFB_GLN_L2);
        if (types.find(IFB_GLN_L2) != types.end())
            gEpoch.slnData.insert(make_pair(TypeID::recIFB_GLN_L2, solver.getSolution(IFB_GLN_L2)));

        os << setprecision(6) << CivilTime(time).printf("%02Y %02m %02d %02H %02M %02S %P") << " " << slnType << " ";
        os << setprecision(10) << newPos.X() << "  " << newPos.Y() << "  " << newPos.Z() << "  " << cdt << " ";
        os << setprecision(3) << sigma << " " << stDev3D << " " << numSats;
        os << endl;
    };
}
