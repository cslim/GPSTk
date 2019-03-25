#include "KalmanSolver.h"

#include"WinUtils.h"
#include"AmbiguityHandler.h"
#include"StringUtils.h"

#include"PowerSum.hpp"
#include"ARSimple.hpp"
#include"ARMLambda.hpp"
#include"MatrixExtensions.h"
#include"GnssSolution.h"

#include <algorithm>

using namespace std;
using namespace gpstk;

namespace pod
{
	//set of all possible TypeID for code pseudorange postfit residuals 
	const std::set<gpstk::TypeID> KalmanSolver::codeResTypes
	{
		TypeID::postfitC,
		TypeID::postfitP1,
		TypeID::postfitP2,
		TypeID::postfitPC,
	};

	//set of all possible TypeID for  carrier phase postfit residuals 
	const std::set<gpstk::TypeID> KalmanSolver::phaseResTypes
	{
		TypeID::postfitL,
		TypeID::postfitL1,
		TypeID::postfitL2,
		TypeID::postfitLC,
	};


	//maximum time interval (in seconds) without data
	double KalmanSolver::maxGap = 500;

	KalmanSolver::KalmanSolver()
		:firstTime(true), isValid(false)
	{}

	KalmanSolver::KalmanSolver(eqComposer_sptr eqs)
		: firstTime(true), equations(eqs), isValid(false)
	{}

	KalmanSolver::~KalmanSolver()
	{}

	IRinex& KalmanSolver::Process(IRinex& gData)
	{
		//invalidate solution
		isValid = false;
		isReset = false;
		double dt = abs(t_pre - gData.getHeader().epoch);
		

		if (dt > maxGap)
		{

			isReset = true;
			FilterData[t_pre] = getState();
			reset();
			DBOUT_LINE("dt= " << dt << "->RESET")
			//resetEpoches.insert(gData.getHeader().epoch);
		}
		t_pre = gData.getHeader().epoch;
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

			/*for (auto& it : equations->currentUnknowns())
			    DBOUT(it << " ");
			DBOUT_LINE("")*/
		    // DBOUT_LINE("meas Vector\n" << setprecision(10) << measVector);
		    // DBOUT_LINE("H\n" << hMatrix);
		    //DBOUT_LINE("Cov\n" << covMatrix);
			//DBOUT_LINE("weigthMatrix\n" << weigthMatrix.diagCopy());
			//DBOUT_LINE("qMatrix: " << qMatrix.diagCopy());
			//DBOUT_LINE("phiMatrix: " << phiMatrix.diagCopy());

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
			
			 DBOUT_LINE(StringUtils::formatTime(gData.getHeader().epoch) << " sln: " << solution)

		    //DBOUT_LINE("solution: " << solution);
			//DBOUT_LINE("postfit Residuals: " << postfitResiduals);
			//DBOUT_LINE("CovPost\n" << covMatrix.diagCopy());

			floatSolution = solution;

			fixAmbiguities(gData);
			//storeAmbiguities(gData);

			auto vpv = postfitResiduals * weigthMatrix*postfitResiduals;
			int numMeas = postfitResiduals.size();
			int numPar = solution.size();

			sigma = sqrt(vpv(0) / (numMeas - numPar));
			phaseSigma = getSigma(phaseResTypes);

			if (i == 0 && checkPhase(gData) == 0)
				break;
			else
				DBOUT_LINE("Catched by residuals\n")
		}

		equations->saveResiduals(gData, postfitResiduals);

		equations->storeKfState(floatSolution, covMatrix);

		//everything is OK => set solutiuon status to VALID
		isValid = true;
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

		std::string asString()
		{
			std::stringstream ss;
			ss << TypeID::tStrings[type.type] << " " << sv << " value: " << value  ;
			return ss.str();
		}

	};

	int KalmanSolver::checkPhase(IRinex& gData)
	{
		static const double codeLim(DBL_MAX);
		static const double phaseLim(0.1);
		
		auto svSet = gData.getBody().getSatID();
		resid maxPhaseResid;

		int i_res = 0;
		for (const auto& type : equations->residTypes())
		{
			if (phaseResTypes.find(type) == phaseResTypes.end())
			{
				i_res += svSet.size();
				continue;
			}
			for (auto&& sv : svSet)
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
			MatrixExtensions::removeRows(hMatrix, indeces);

			//update observations
			MatrixExtensions::removeElms(measVector, indeces);

			//update weigths
			MatrixExtensions::removeRows(weigthMatrix, indeces);
			MatrixExtensions::removeColumns(weigthMatrix, indeces);

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

			cout << StringUtils::formatTime(gData.getHeader().epoch) << " : " << maxPhaseResid.asString() << endl;

			return 1;
		}
	}

	double KalmanSolver::getSigma(const TypeIDSet& types) const
	{
		auto phaseRes = equations->getResiduals(PostfitResiduals(), types);
		Matrix<double> res(phaseRes.size(), 1, 0.0);

		res = res.assignFrom(phaseRes);

		//compute v'v
		auto vpv = transpose(res)*res;
		double sigma = sqrt(vpv(0, 0) / res.size());

		return sigma;
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

	bool KalmanSolver::ResetIfRequared(const gpstk::CommonTime& t, const KalmanSolver::filterHistory& data)
	{
		double dt = t - t_pre;

		if (::abs(dt) > maxGap)
		{
			auto it = data.find(t);
			if (it != data.end())
			{
				setState(it->second);
				t_pre = t;
				DBOUT_LINE(StringUtils::formatTime(t)<<"reset: "<< dt)

				return true;
			}
		}
		return false;
	}
}
