#ifndef POD_EARTH_ROTATION_H
#define POD_EARTH_ROTATION_H

#include"EOPDataStore.hpp"
#include"Matrix.hpp"
#include"CivilTime.hpp"

using namespace gpstk;
using namespace std;

namespace POD
{
    class EarthRotation
	{
    public:
        static  EarthRotation eopStore;

        EarthRotation();
        EarthRotation(const EOPDataStore & eop);
        bool loadEOP(const string &  file, EOPDataStore::EOPSource source= EOPDataStore::EOPSource::IERS)
            throw(InvalidRequest,FileMissingException);
        /// get Inertial to terestriel matrix by IAU 2000A, CIO based, using classical angles
        Matrix<double> getJ2k2Ecef00(const CommonTime & t);
        Matrix<double> getEcef2J2k00(const CommonTime & t);
        Vector<double> convertJ2k2Ecef(const CommonTime & t, const Vector<double> pos);
        Vector<double> convertEcef2J2k(const CommonTime & t, const Vector<double> pos);

        virtual ~EarthRotation()
        {
            eopData.clear();
        }

        bool test();
    protected:
        EOPDataStore eopData;
		
    private:
		static CivilTime toTAI(const CommonTime & t) throw(InvalidParameter);
    };
}
#endif // !POD_EARTH_ROTATION_H