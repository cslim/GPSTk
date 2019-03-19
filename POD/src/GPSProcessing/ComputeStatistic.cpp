#include "ComputeStatistic.h"
#include"WinUtils.h"

using namespace gpstk;

namespace pod
{
    //method to compute correlation matrix from variance-covariance one
    Matrix<double> ComputeStatistic::corrMatrix(const  Matrix<double>& covar)
    {
        Matrix<double> corr(covar.rows(), covar.cols(), .0);

        for (size_t i = 0; i < covar.rows(); i++)
            for (size_t j = 0; j < covar.cols(); j++)
                corr(i, j) = covar(i, j) / sqrt(covar(i, i)) / sqrt(covar(j, j));
        return corr;
    }

    void ComputeStatistic::compute(const GnssEpochMap & data, gpstk::Vector<double>& sln, gpstk::Matrix<double>& covar)
    {
        int s = types.size();
        std::list<Vector<double>> res;
        sln.resize(s, .0);
        covar = Matrix<double>(s, s, .0);
        int i(0);

        //for (auto ep = begin(data); ep != end(data); ep++)
        for (auto && ep : data)
        {
            SlnType curSt = (SlnType)(int)ep.second.slnData.getValue(TypeID::recSlnType);

            if (curSt == slnType)
            {
                i++;
                Vector<double> resi(s, .0);
                int j(0);
                for (auto && it : types)
                {
                    resi[j] = ep.second.slnData.getValue(it);
                    sln[j] = sln[j] * (i - 1) / i + resi[j] / i;
                    j++;
                }
                res.push_back(resi);
            }
        }
		goodEpochs = i;
		totalEpochs = data.size();

        //calculate sum squares 
        for (auto&& it : res)
        {
            for (size_t k = 0; k < s; k++)
                for (size_t l = 0; l <= k; l++)
                {
                    double d1 = it[k] - sln[k];
                    double d2 = it[l] - sln[l];
                    covar(k, l) += d1 * d2;
                }
        }

        for (size_t k = 0; k < s; k++)
            for (size_t l = 0; l <= k; l++)
                covar(k, l) = covar(k, l) / (res.size() - 1);

    }
}
