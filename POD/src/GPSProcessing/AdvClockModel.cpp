#include"AdvClockModel.h"

using namespace gpstk;

namespace pod
{

	void AdvClockModel::Prepare(gpstk::IRinex & gData)
	{

		// Update previous epoch
		setPreviousTime(currentTime);

		setCurrentTime(gData.getHeader().epoch);

		double d = std::abs(currentTime - previousTime);
		dt = isFirstTime || d < DBL_EPSILON ? 30.0 : d;
		isFirstTime = false;
	}

	ParametersSet AdvClockModel::getParameters() const
	{
		return types;
	}

	void AdvClockModel::updatePhi(gpstk::Matrix<double>& Phi, int & index) const
	{
		Phi(index, index) = 1.0;
		Phi(index, index + 1) = dt;
		Phi(index + 1, index) = 0;
		Phi(index, index) = 1.0;
		index += 2;
	}

	void AdvClockModel::updateQ(gpstk::Matrix<double>& Q, int & index) const
	{
		double dt2 = dt * dt;
		double dt3 = dt2 * dt;

		Q(index, index) = q1 * dt+ q2 * dt3 / 3.0;
		Q(index, index + 1) = Q(index + 1, index) = q2 * dt2 / 2.0;
		Q(index + 1, index + 1) = q2 * dt;
		index += 2;
	}
	
	void AdvClockModel::defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const
	{
		x(index) = 0;
		P(index, index) = 1e9;
		++index;
		x(index) = 0;
		P(index, index) = 1e9;
		++index;
	}

	void AdvClockModel::updateH(const gpstk::IRinex & gData, const gpstk::TypeIDSet & types, gpstk::Matrix<double>& H, int & col_0)
	{
		for (size_t i = 0; i < H.rows(); i++)
		{
			H(i, col_0) = 1.0;
			H(i, col_0 + 1) = dt;
		}
		col_0 += 2;
	}

	int AdvClockModel::getNumUnknowns() const
	{
		return 2;
	}
}