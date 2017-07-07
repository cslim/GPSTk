#ifndef POD_FORCE_MODEL_DATA_H
#define POD_FORCE_MODEL_DATA_H

#include<string>
#include"Matrix.hpp"

namespace POD
{
	struct GravityModelData
	{
		std::string modelName;

		double GM = 3.98600441500e+14;
		double refDistance;

		bool includesPermTide;

		double refMJD;

		double dotC20;
		double dotC21;
		double dotS21;

		int desiredDegree = 0;
		int desiredOrder = 0;

        int maxDegree = 0;
		int maxOrder = 0;

		bool solidTide = false;
		bool oceanTide = false;
		bool poleTide = false;

		//for debug purposes only
		gpstk:: Matrix<double> unnormalizedCS;

        gpstk:: Matrix<double> normalizedCS;

		bool isValid = true;

		void loadModel(const std::string &path);

	};

    struct ForceModelData
    {
        ForceModelData();
        ~ForceModelData();

        bool useGravEarh = true;
        bool useGravSun = false;
        bool useGravMoon = false;
        bool useAtmDrag = false;
        bool useRelEffect = false;
        bool useSolarPressure = false;

        GravityModelData gData;
    };
}

#endif // !POD_FORCE_MODEL_DATA_H
