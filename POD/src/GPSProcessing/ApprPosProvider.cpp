#include"ApprPosProvider.hpp"
#include"PRSolution2.hpp"
#include"Bancroft.hpp"
#include"sqlite3.h"
#include"StringUtils.h"

#include<filesystem>

using namespace std;
using namespace gpstk;
namespace fs = std::experimental::filesystem;

namespace pod
{
	std::map<IApprPosProvider::PositionSource, std::string> IApprPosProvider::posSource2Str
	{
		{FromConfig,            "From ini file"               },
		{ComputeForEachEpoch,   "Statndalone for each epoch"  },
		{ComputeForFirstEpoch,  "Statndalone for first epoch" },
		{LoadFromFile,          "Load from file"              },
	};

	int IApprPosProvider::ComputeApprSol(const gpstk::IRinex & gRin, 
		const gpstk::XvtStore<gpstk::SatID>& eph,
		gpstk::Vector<double> & solution)
	{
		auto svs = gRin.getBody().getVectorOfSatID().toStdVector();
		auto meas = gRin.getBody().getVectorOfTypeID(TypeID::C1).toStdVector();

		Matrix<double> svp;
		if (PRSolution2::PrepareAutonomousSolution(gRin.getHeader().epoch, svs, meas, eph, svp))
			return -1;

		Bancroft ban;

		if (ban.Compute(svp, solution))
			return -2;

		return 0;
	}

	int ComputeApprPos::getPosition(const gpstk::IRinex & gRin, gpstk::Position& pos)
	{
		auto t = gRin.getHeader().epoch;
		auto it = pvtStore.find(t);
		if (it == pvtStore.end())
		{
			Vector<double> vect;
			int rc = IApprPosProvider::ComputeApprSol(gRin, *ephStore, vect);
			if (!rc)
			{
				pos = Position(vect[0], vect[1], vect[2]);
				Xvt xvt;
				xvt.frame = ReferenceFrame::WGS84;
				xvt.x = pos;
				xvt.clkbias = vect[3];
				pvtStore[t] = xvt;
			}
			else
				return rc;
		}
		else
			pos = Position(it->second.x[0], it->second.x[1], it->second.x[2]);

		return 0;
	}

	int ComputeOnePos::getPosition(const gpstk::IRinex & gRin, gpstk::Position & pos)
	{
		if (isFirstTime)
		{
			Vector<double> vect;
			int rc = IApprPosProvider::ComputeApprSol(gRin, *ephStore, vect);
			if (!rc)
			{
				apprPos = Position(vect[0], vect[1], vect[2]);
				isFirstTime = false;
			}
			else
				return rc;
		}
		pos = apprPos;
		return 0;
	}

	bool PositionFromFile::loadApprPos(const std::string & path)
	{
		pvtStore.clear();
		try
		{
			ifstream file(path);
			if (file.is_open())
			{
				string line;
				while (getline(file, line))
				{
					auto  words = StringUtils::split(line,";");
					if (words.size() > 5)
					{
						int sType = stoi(words[5]);
						CommonTime ct;
						if (sType && StringUtils::tryParseTime(words[0], ct))
						{
     						Xvt xvt;
							xvt.x = Triple(stod(words[1]), stod(words[2]), stod(words[3]));
							xvt.clkbias = stod(words[4]);
							pvtStore.insert(make_pair(ct, xvt));
						}
					}
				}
			}
			else
			{
				auto mess = "Can't load data from file: " + path;
				std::exception e(mess.c_str());
				throw e;
			}
		}
		catch (const std::exception& e)
		{
			cout << e.what() << endl;
			throw e;
		}
		return true;
	}

	int PositionFromFile::getPosition(const gpstk::IRinex & gRin, gpstk::Position & pos)
	{
		auto it = pvtStore.find(gRin.getHeader().epoch);
		if (it == pvtStore.end())
			return -1;
		pos = Position(it->second.x[0], it->second.x[1], it->second.x[2]);
		return 0;
	}
}