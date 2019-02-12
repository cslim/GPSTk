#pragma once
#include "EquationBase.h"
#include"StochasticModel.hpp"

namespace pod
{
	class TropoEquationsAdv :
		public EquationBase
	{
	public:
		TropoEquationsAdv()
			:typeSet{ FilterParameter( gpstk::TypeID::wetMap), FilterParameter(gpstk::TypeID::wetMapDot) },
			previousTime(gpstk::CommonTime::BEGINNING_OF_TIME),
			currentTime(gpstk::CommonTime::END_OF_TIME),
			dt(DBL_MAX), isFirstTime(true)
		{};
		TropoEquationsAdv(double q_1, double q_2)
			:typeSet{ FilterParameter(gpstk::TypeID::wetMap), FilterParameter(gpstk::TypeID::wetMapDot) },
			previousTime(gpstk::CommonTime::BEGINNING_OF_TIME),
			currentTime(gpstk::CommonTime::END_OF_TIME),
			dt(DBL_MAX), isFirstTime(true), q1(q_1), q2(q_2)
		{};

		virtual ~TropoEquationsAdv() {};

#pragma region Inherited via EquationBase

		virtual void Prepare(gpstk::IRinex & gData) override;

		virtual void updateH(const gpstk::IRinex& gData, const gpstk::TypeIDSet& types, gpstk::Matrix<double>& H, int& col_0) override;

		virtual  ParametersSet getParameters() const override
		{
			return typeSet;
		}

		virtual void updatePhi(gpstk::Matrix<double>& Phi, int & index) const override;

		virtual void updateQ(gpstk::Matrix<double>& Q, int & index) const override;

		virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int & index) const override;

		virtual int getNumUnknowns() const override
		{
			return typeSet.size();
		}

#pragma endregion

		virtual TropoEquationsAdv& setPreviousTime(const gpstk::CommonTime& prevTime)
		{
			previousTime = prevTime; return (*this);
		}

		/** Set the value of current epoch
		*
		* @param currTime   Value of current epoch
		*
		*/
		virtual TropoEquationsAdv& setCurrentTime(const gpstk::CommonTime& currTime)
		{
			currentTime = currTime;
			return (*this);
		}


#pragma region Fields

		//gpstk::StochasticModel_uptr pStochasticModel;

		ParametersSet typeSet;

		///the diffusion coefficients, denoting the impact
		/// of white frequency noise
		double q1;

		///the diffusion coefficients, denoting the impact
		/// of random walk frequency noise
		double q2;

		/// Epoch of previous measurement
		gpstk::CommonTime previousTime;

		/// Epoch of current measurement
		gpstk::CommonTime currentTime;

		double dt;

		gpstk::TypeID typeId = gpstk::TypeID::wetMap;
		 
		mutable bool isFirstTime;

#pragma endregion

	};
}