#include"ForceModelData.h"
#include<iostream>

using namespace std;

namespace POD
{
    ForceModelData::ForceModelData()
    {
        useGravEarh = true;
        useGravSun = useGravMoon = false;
        useAtmDrag = false;
        useRelEffect = false;
        useSolarPressure = false;

        gData = GravityModelData();
    }

    ForceModelData::~ForceModelData()
    {
    }

    void GravityModelData:: loadModel(const std::string &path)
    {
        isValid = false;
        ifstream inpStream(path);
        if (!inpStream.is_open())
        {
            gpstk::Exception e("failed to open gravity model file: " + path);
            GPSTK_THROW(e);
        }
        double factor(0), refd(0.0), mu(0.0);
        int isNormalize(0);
        int nMax(0), mMax(0);
        string buff;

        modelName = path;
        inpStream >> refd;
        refDistance = refd;
        inpStream >> mu;
        GM = mu;
        inpStream >> factor;
        inpStream >> maxDegree;
        inpStream >> maxOrder;
        inpStream >> isNormalize;
        getline(inpStream, buff);

        if (maxOrder < desiredOrder || maxDegree < desiredDegree)
        {
            gpstk::Exception e("desired gravity field degree(order) is lover than maximum degree(order) avalable in " + path + " gravity model file.");
            GPSTK_THROW(e);
        }

        gpstk::Matrix<double>  &CS = (isNormalize) ? normalizedCS : unnormalizedCS;

        CS.resize(desiredOrder + 1, desiredDegree + 1);

        while (!inpStream.eof())
        {
            int n(0), m(0);
            double cnm(0.0), snm(0.0);
            inpStream >> n >> m >> cnm >> snm;
            getline(inpStream, buff);

            if (n > desiredDegree && m > desiredOrder)
            {
                gpstk:: Exception e("Unexpected gravity field degree(order) in: " + path);
                GPSTK_THROW(e);
            }
            //CS[n][m] = C[n][m], CS[m-1][n] = S[n][m].
            CS[n][m] = cnm;
            if (m != 0)
                CS[m - 1][n] = snm;

            if (n == desiredDegree && m == desiredOrder) break;

        }
        isValid = true;
    }

}