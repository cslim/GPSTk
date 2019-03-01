#include "KalmanSolver.h"

#include"WinUtils.h"
#include"AmbiguityHandler.h"

#include"PowerSum.hpp"
#include"ARSimple.hpp"
#include"ARMLambda.hpp"
#include <algorithm>

using namespace std;
using namespace gpstk;

namespace pod
{

	//maximum time interval (in seconds) without data
	double KalmanSolver::maxGap = 500.;

	KalmanSolver::KalmanSolver()
		:firstTime(true), isValid_(false)
	{}

	KalmanSolver::KalmanSolver(eqComposer_sptr eqs)
		: firstTime(true), equations(eqs), isValid_(false)
	{}

	KalmanSolver::~KalmanSolver()
	{}

	IRinex& KalmanSolver::Process(IRinex& gData)
	{
		//invalidate solution
		isValid_ = false;

		double dt = abs(t_pre - gData.getHeader().epoch);
		t_pre = gData.getHeader().epoch;

		if (dt > maxGap)
		{
			reset();
			DBOUT_LINE("dt= " << dt << "->RESET")
		}
		//workaround: reset PPP engine every day 
		double  sec = gData.getHeader().epoch.getSecondOfDay();
		if ((int)sec % 86400 == 0 && equations->getSlnType() == SlnType::PPP_Float)
			equations->clearSvData();

		equations->Prepare(gData);
		Vector<double> floatSolution;

		//if number of satellies passed to processing is less than 'MIN_NUM_SV'
		//clear all SV data except observable
		if (gData.getBody().size() < minSatNumber)
		{
			equations->keepOnlySv(gData.getBody().getSatID());

			return gData;
		}

		equations->updateH(gData, hMatrix);
		equations->updateMeas(gData, measVector);
		equations->updateW(gData, weigthMatrix);

		equations->updatePhi(phiMatrix);
		equations->updateQ(qMatrix);

		if (dt > maxGap)
			equations->initKfState(solution, covMatrix);
		else
			equations->updateKfState(solution, covMatrix);

		firstTime = false;

		for (int i = 0; i < 2; i++)
		{
			//if number of satellies passed to processing is less than 'MIN_NUM_SV'
			//clear all SV data except observable
			if (gData.getBody().size() < minSatNumber)
			{
				equations->keepOnlySv(gData.getBody().getSatID());

				return gData;
			}

			//DBOUT_LINE("--" << i << "--");

			//for (auto& it : equations->currentUnknowns())
			//    DBOUT(it << " ");
			//DBOUT_LINE("")
		   // DBOUT_LINE("meas Vector\n" << setprecision(10) << measVector);
		   // DBOUT_LINE("H\n" << hMatrix);
		   //DBOUT_LINE("Cov\n" << covMatrix);
			//DBOUT_LINE("weigthMatrix\n" << weigthMatrix.diagCopy());
			//DBOUT_LINE("qMatrix\n" << qMatrix.diagCopy());
			//DBOUT_LINE("phiMatrix\n" << phiMatrix.diagCopy());

			//prepare
			Matrix<double> hMatrixTr = transpose(hMatrix);
			Matrix<double> phiMatrixTr = transpose(phiMatrix);
			Matrix<double> hTrTimesW = hMatrixTr * weigthMatrix;

			//predict
			Matrix<double> Pminus = phiMatrix * covMatrix*phiMatrixTr + qMatrix;
			Vector<double> xminus = phiMatrix * solution;

			//DBOUT_LINE("Pminus\n" << Pminus);
			//correct
			try
			{
				Matrix<double> invPminus = inverseChol(Pminus);
				covMatrix = inverseChol(hTrTimesW*hMatrix + invPminus);
				solution = covMatrix * (hTrTimesW*measVector + invPminus * xminus);
			}
			catch (const gpstk::MatrixException &e)
			{
				std::cerr << e << endl;

				Matrix<double> invPminus = inverseSVD(Pminus);
				covMatrix = inverseSVD(hTrTimesW*hMatrix + invPminus);
				solution = covMatrix * (hTrTimesW*measVector + invPminus * xminus);
			}

			postfitResiduals = measVector - hMatrix * solution;
			DBOUT_LINE("solution\n" << solution);
			DBOUT_LINE("postfit Residuals\n" << postfitResiduals);
			//DBOUT_LINE("CovPost\n" << covMatrix.diagCopy());
			//DBOUT_LINE("CorrPost\n" << corrMatrix(covMatrix));

			floatSolution = solution;

			fixAmbiguities(gData);
			//storeAmbiguities(gData);

			auto vpv = postfitResiduals * weigthMatrix*postfitResiduals;
			int numMeas = postfitResiduals.size();
			int numPar = solution.size();

			//sigma = sqrt(vpv(0) / (numMeas - numPar));
			sigma = vpv(0);

			if (i == 0 && checkPhase(gData) == 0)
				break;
			else
				DBOUT_LINE("Catched by residuals\n")
		}

		equations->saveResiduals(gData, postfitResiduals);

		equations->storeKfState(floatSolution, covMatrix);

		//everything is OK => set solutiuon status to VALID
		isValid_ = true;
		return gData;
	}
	struct resid
	{
		resid()
			:type(TypeID::dummy0), sv(SatID::dummy), value(0)
		{};

		TypeID type;
		SatID sv;
		double value;
	};

	void removeColumns(Matrix<double>& m, std::set<int> cols)
	{
		int newCols = m.cols() - cols.size();
		Matrix<double> m1(m.rows(), newCols, .0);
		int k = 0;

		for (size_t i = 0; i < m.cols(); i++)
		{
			if (cols.find(i) != cols.end())
				continue;

			for (size_t j = 0; j < m.rows(); j++)
				m1(j, k) = m(j, i);
			k++;
		}
		m = m1;
	}

	void removeRows(Matrix<double>& m, std::set<int> rows)
	{
		int newRows = m.rows() - rows.size();
		Matrix<double> m1(newRows, m.cols(), .0);
		int k = 0;

		for (size_t i = 0; i < m.rows(); i++)
		{
			if (rows.find(i) != rows.end())
				continue;

			for (size_t j = 0; j < m.cols(); j++)
				m1(k, j) = m(i, j);
			k++;
		}
		m = m1;
	}

	void removeElms(Vector<double>& v, std::set<int> elms)
	{
		int newSize = v.size() - elms.size();
		Vector<double> v1(newSize, .0);

		int k = 0;
		for (size_t i = 0; i < v.size(); i++)
		{
			if (elms.find(i) != elms.end())
				continue;
			v1(k) = v(i);
			k++;
		}
		v = v1;
	}

	int KalmanSolver::checkPhase(IRinex& gData)
	{
		static const double codeLim(DBL_MAX);
		static const double phaseLim(0.06);
		static const TypeIDSet phaseTypes{ TypeID::postfitL1, TypeID::postfitL2, TypeID::postfitLC };

		auto svSet = gData.getBody().getSatID();
		resid maxPhaseResid;

		int i_res = 0;
		for (const auto& type : equations->residTypes())
		{
			if (phaseTypes.find(type) == phaseTypes.end())
			{
				i_res += svSet.size();
				continue;
			}
			for (auto& sv : svSet)
			{
				double vali = ::abs(postfitResiduals(i_res));
				if (maxPhaseResid.value < vali)
				{
					maxPhaseResid.value = vali;
					maxPhaseResid.sv = sv;
					maxPhaseResid.type = type;
				}
				i_res++;
			}
		}
		if (maxPhaseResid.value < phaseLim)
		{
			return 0;
		}
		else
		{
			auto dist = std::distance(svSet.begin(), svSet.find(maxPhaseResid.sv));

			std::set<int> indeces;
			for (size_t i = 0; i < equations->residTypes().size(); i++)
				indeces.insert(i*svSet.size() + dist);

			//update H
			removeRows(hMatrix, indeces);

			//update observations
			removeElms(measVector, indeces);

			//update weigths
			removeRows(weigthMatrix, indeces);
			removeColumns(weigthMatrix, indeces);

			auto ambSet = equations->currentAmb();
			auto typeSet = FilterParameter::get_all_types(ambSet);

			int corParNum = equations->getNumUnknowns() - ambSet.size();

			//update Phi and Q marices
			for (size_t i = 0; i < typeSet.size(); i++)
			{
				int ind = corParNum + i * svSet.size();
				phiMatrix(ind, ind) = 0;
				qMatrix(ind, ind) = 4e14;
			}

			//remove sv 
			gData.getBody().removeSatID(maxPhaseResid.sv);

			return 1;
		}
	}

	int KalmanSolver::check(IRinex& gData)
	{
		Matrix<double> res(PostfitResiduals().size(), 1, 0.0);

		res = res.assignFrom(PostfitResiduals());

		//compute v'pv
		auto vpv = transpose(res)*weigthMatrix*res;

		double sigma = sqrt(vpv(0, 0));

		double varX = getVariance(FilterParameter(TypeID::dx));     // Cov dx    - #8
		double varY = getVariance(FilterParameter(TypeID::dy));     // Cov dy    - #9
		double varZ = getVariance(FilterParameter(TypeID::dz));     // Cov dz    - #10
		double stDev3D = sqrt(varX + varY + varZ);

		if (sigma / stDev3D > 3)
		{
			cout << "Epoch: " << CivilTime(gData.getHeader().epoch) << " catched " << endl;
			cout << "sigma: " << sigma << " sigma: " << stDev3D << endl;
			return 1;
		}
		else
			return 0;
	}

	void  KalmanSolver::fixAmbiguities(IRinex& gData)
	{
		if (equations->getSlnType() == SlnType::PD_Fixed && gData.getBody().size() > 5)
		{
			int core_num = equations->currentUnknowns().size() - equations->currentAmb().size();

			AmbiguityHandler ar(equations->currentAmb(), solution, covMatrix, core_num);
			ar.fixL1L2(gData);

			for (int k = 0; k < core_num; k++)
				solution(k) = ar.CoreParamFixed()(k);
		}
	}

	//reject by code postfit residual 
	IRinex& KalmanSolver::reject(IRinex& gData, const TypeIDSet&  typeIds)
	{
		typedef SatTypePtrMap::value_type type;
		SatIDSet rejSat;
		///!!!
		///here we use only last type of postfit residuals, because in case of
		///combined code/phase measurements processing this part contans carrier phase residuals
		//in case code/phase only processing we have only one element in @typeIds, which one will be used
		///!!!
		const auto& id = typeIds.rbegin();

		//get the sv - typeMap pair with largest residual value

		auto svWithMaxResidual = std::max_element(
			gData.getBody().begin(), gData.getBody().end(),
			[&](const type& it1, const type& it2)-> bool
		{
			double val1 = ::abs(it1.second->get_value().at(*id));
			double val2 = ::abs(it2.second->get_value().at(*id));
			return(val1 < val2);
		}
		);

		//report detection
		cout << "Removed SV: " << svWithMaxResidual->first;
		cout << " with " << TypeID::tStrings[id->type] << " = ";
		cout << svWithMaxResidual->second->get_value()[*id] << endl;

		//remove sv
		rejSat.insert(svWithMaxResidual->first);

		gData.getBody().removeSatID(rejSat);

		return gData;
	}

	int KalmanSolver::getUnknownIndex(const FilterParameter& parameter) const
	{
		auto it = equations->currentUnknowns().find(parameter);
		if (it == equations->currentUnknowns().end())
		{
			InvalidRequest e("Type: '" + parameter.toString() + "' not found in  current set of unknowns.");
			GPSTK_THROW(e);
		}
		return std::distance(equations->currentUnknowns().begin(), it);
	}

	double KalmanSolver::getSolution(const FilterParameter& parameter) const
	{
		int i = getUnknownIndex(parameter);
		return solution(i);

	}  // End of method 'SolverLMS::getSolution()'

	double KalmanSolver::getVariance(const FilterParameter& parameter) const
	{
		int i = getUnknownIndex(parameter);
		return covMatrix(i, i);

	}  // End of method 'SolverLMS::getVariance()'
}
