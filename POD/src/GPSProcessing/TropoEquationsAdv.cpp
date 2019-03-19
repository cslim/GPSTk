#include "TropoEquationsAdv.hpp"
#include "WinUtils.h"

using namespace gpstk;

namespace pod
{

	void TropoEquationsAdv::updateH(const gpstk::IRinex& gData, const gpstk::TypeIDSet& obsTypes, gpstk::Matrix<double>& H, int& col_0)
	{

		int row(0);
		for (const auto& t : obsTypes)
		{
			for (const auto& it : gData.getBody())
			{
				double mf = it.second->get_value().at(typeId);
				H(row, col_0) = mf;
				H(row++, col_0 + 1) = mf * dt;
			}
		}
		col_0 += 2;
	}

	void TropoEquationsAdv::updatePhi(gpstk::Matrix<double>& Phi, int & index) const
	{
		Phi(index, index) = 1.0;
		Phi(index, index + 1) = dt;
		Phi(index+ 1, index ) = 0;
		Phi(index, index) = 1.0;
		index += 2;
	}

	void TropoEquationsAdv::updateQ(gpstk::Matrix<double>& Q, int & index) const
	{

		double dt2 = dt * dt;
		//double dt3 = dt2 * dt;

		Q(index, index) = q1 * dt;//+ q2 * dt3 / 3.0;
		Q(index, index + 1) = Q(index + 1, index) = q2 * dt2 / 2.0;
		Q(index + 1, index + 1) = q2 * dt;
		index += 2;

	}

	void TropoEquationsAdv::defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const
	{
		x(index) = 5.000e-02;
		P(index, index) = 5e-5;
		++index;
		x(index) = 0;
		P(index, index) = 1.667e-06;
		++index;
	}

	void TropoEquationsAdv::Prepare(IRinex& gData)
	{
		// Update previous epoch
		setPreviousTime(currentTime);

		setCurrentTime(gData.getHeader().epoch);

		double d = std::abs(currentTime - previousTime);
		dt = isFirstTime || d < DBL_EPSILON ? 30.0 : d;
		isFirstTime = false;
	}
}