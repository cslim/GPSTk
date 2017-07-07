
#include"EarthRotation.h"
#include"EOPDataStore.hpp"
#include"sofa.h"

using namespace gpstk;
using namespace std;

namespace POD
{
    ///
    EarthRotation EarthRotation::eopStore;

    EarthRotation::EarthRotation():eopData(10)
    {
    }
    EarthRotation::EarthRotation(const EOPDataStore & eop): eopData(eop)
    {
    }

    bool EarthRotation:: loadEOP(const string &  file, EOPDataStore::EOPSource source)
    {
        try
        {
            eopData.loadFile(file,source);
        }
        catch (const std::exception&)
        {
            return false;
        }
		return true;
    }
	
    Matrix<double> EarthRotation::getJ2k2Ecef00(const CommonTime & t)
    {
		//first, let's convert input time to TT;
		CivilTime ct_TAI = toTAI(t);

		//convert TT to MJD and MJD0
        double djmjd0, date;
		iauCal2jd(ct_TAI.year, ct_TAI.month, ct_TAI.day, &djmjd0, &date);
		double timeTAI = (60.0*(double)(60 * ct_TAI.hour + ct_TAI.minute) + ct_TAI.second) / DAYSEC;
		
		double tai = date + timeTAI;
		double tt = tai + 32.184 / DAYSEC;
		
		// let's calculate UTC for EOP interpolation
		double utc1,  utc2;
		iauTaiutc(djmjd0, tai, &utc1, &utc2);
		
		int iy, im, id;
		double timeUTC;
		iauJd2cal(utc1, utc2, &iy, &im, &id, &timeUTC);

        double dat;
		iauDat(iy, im, id, timeUTC, &dat);
		
		CommonTime tUTC = (CommonTime)ct_TAI;
		tUTC.addSeconds(-dat);
        tUTC.setTimeSystem(TimeSystem::UTC);

		//let's interpolate EOP
		EOPDataStore:: EOPData eop = eopData.getEOPData(tUTC);
        
        //
        //EOP data from SOFA cookbook
        //
        //eop.xp = 0.0349282;
        //eop.yp = 0.4833163;
        //eop.UT1mUTC = -0.072073685;
        //eop.dX = 0.1725;
        //eop.dY = -0.2650;

		double x, y, s;
		/* CIP and CIO, IAU 2000A. */
		iauXys00a(djmjd0, tt, &x, &y, &s);

		/* Add CIP corrections. */
        x += eop.dX*DMAS2R;
	    y += eop.dY*DMAS2R;

		/* GCRS to CIRS matrix. */
		double rc2i[3][3];
		iauC2ixys(x, y, s, rc2i);

		/* UT1. */
		double tut = timeUTC + eop.UT1mUTC / DAYSEC;
		double ut1 = date + tut;
		
		/* Earth rotation angle. */
        double era = iauEra00(djmjd0 + date, tut);

		/* Form celestial-terrestrial matrix (no polar motion yet). */
		double rc2ti[3][3];
		iauCr(rc2i, rc2ti);
		iauRz(era, rc2ti);

		double xp = eop.xp*DAS2R;
		double yp = eop.yp*DAS2R;
        
		/* Polar motion matrix (TIRS->ITRS, IERS 2003). */
		double rpom[3][3];
		iauPom00(xp, yp, iauSp00(djmjd0, tt), rpom);
		/* Form celestial-terrestrial matrix (including polar motion). */
		double rc2it[3][3];
		iauRxr(rpom, rc2ti, rc2it);
		Matrix<double> J2k2ECEF(3, 3);
	
		for(int i=0;i<3;i++)
			for (int j = 0; j < 3; j++)
			{
				J2k2ECEF(i, j) = rc2it[i][j];
			}

		return J2k2ECEF;
    }
    
    Matrix<double> EarthRotation::getEcef2J2k00(const CommonTime & t)
	{
		return transpose(getJ2k2Ecef00(t));
	}

    Vector<double> EarthRotation:: convertJ2k2Ecef(const CommonTime & t, const Vector<double> pos)
    {
        return getJ2k2Ecef00(t)*pos;
    }

    Vector<double> EarthRotation:: convertEcef2J2k(const CommonTime & t, const Vector<double> pos)
    {
        return getEcef2J2k00(t)*pos;
    }
	 CivilTime EarthRotation::toTAI(const CommonTime & t)
	{
		TimeSystem inTS = t.getTimeSystem();
		if (inTS == TimeSystem::Any || inTS == TimeSystem::Unknown) 
			throw InvalidParameter("TimeSystem is invalid (Any or Unknown)");

		CivilTime ct = (CivilTime)t;
		double dTAI = TimeSystem::Correction(inTS, TimeSystem::TAI, ct.year, ct.month, ct.day);

		CommonTime TAI = t;
		TAI.addSeconds(dTAI);
		
		return (CivilTime)TAI;
	}

     bool EarthRotation::test()
     {
         ofstream os("EarthRotation_test.txt");
         int iy, im, id, ih, min, j;
         double sec, djmjd0, date, time, dat;
        
         /* UTC. */
         iy = 2007;
         im = 4;
         id = 5;
         ih = 12;
         min = 0;
         sec = 0.0;

         j = iauCal2jd(iy, im, id, &djmjd0, &date);
         if (j < 0) return j;
         time = (60.0*(double)(60 * ih + min) + sec) / DAYSEC;
         double utc = date + time;
         j = iauDat(iy, im, id, time, &dat);
         if (j < 0) return j;
         double  tai = utc + dat / DAYSEC;
         CivilTime ctTAI(iy, im, id, ih, min, sec, TimeSystem::TAI);

         CommonTime TAI = (CommonTime)ctTAI;
         TAI = TAI.addSeconds(dat);

         Matrix<double>J2k2ECEF = getJ2k2Ecef00(TAI);

         os << setprecision(18) << J2k2ECEF <<endl;
     }

};
