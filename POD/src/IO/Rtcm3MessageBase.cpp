#include"Rtcm3MessageBase.hpp"
#include"GNSSconstants.hpp"
#include"WinUtils.h"

using namespace gpstk;
namespace pod
{
	const double CODE_RES = 0.02;
	const double PHASE_RES = 0.0005;
	//Rtcm3MessageBase::Initializer Rtcm3MessageBase::initializer;
	std::set<rtcm3_msg_uptr> Rtcm3MessageBase::
		init_msg_set()
	{
		std::set<rtcm3_msg_uptr> msgs;
		msgs.emplace(std::make_unique<Rtcm3_1008>());
		return msgs;
	}
	std::map<int, rtcm3_msg_uptr>  Rtcm3MessageBase::
		init_msg_map()
	{
		std::map<int, rtcm3_msg_uptr> msgs_map;
		msgs_map.emplace(std::make_pair(1008, std::make_unique<Rtcm3_1008>()));
		return msgs_map;
	}

	const std::set<rtcm3_msg_uptr> Rtcm3MessageBase::all_msgs = init_msg_set();

	const std::map<int, rtcm3_msg_uptr> Rtcm3MessageBase::id2msg = init_msg_map();


	bool Rtcm3_1008::parse(BitSetProxy &buffer, Rtcm3Decoder& decoder)
	{
		int ref_id = buffer.getUint32(12, 12);
		int n = buffer.getUint32(24, 8);
		auto ant_discr = buffer.getString(32,n*8);
		std::cout << ref_id << " " << ant_discr << std::endl;
		return true;
	}

	bool Rtcm3_GpsObs::parseHeader(BitSetProxy &buffer, Rtcm3Decoder& decoder)
	{
		int ref_id = buffer.getUint32(12, 12);
		int tow = buffer.getUint32(24, 30);
		bool syncFlag = buffer.bits()[54];
		nSats = buffer.getUint32(55, 5);
		std::cout <<"SyncFlag: "<<syncFlag<< " tow: " << tow << " ngps: " << nSats << std::endl;
		return true;
	}

	bool Rtcm3_1004::parse(BitSetProxy &buffer, Rtcm3Decoder& decoder)
	{
		if (parseHeader(buffer, decoder))
			for (size_t i = 0; i < nSats; i++)
			{
				if (!parseSatData(BitSetProxy(buffer.bits(), 64 + 125 * i, 125), decoder))
					return false;
			}

		else
			return false;

		return true;
	}

	bool Rtcm3_1004::parseSatData(BitSetProxy &buffer, Rtcm3Decoder& decoder)
	{
		int id = buffer.getUint32(0, 6);
		SatID satId(id, SatID::SatelliteSystem::systemGPS);
		TypeID typeIdL1 = buffer.bits()[6] ? TypeID::P1 : TypeID::C1;

		int prL1 = buffer.getUint32(7, 24);
		int dL1phase = buffer.getInt32(31, 20);
		int L1CodeAmb = buffer.getUint32(58, 8);
		int cnrL1 = buffer.getUint32(66, 8);
		 
		int typeL2code = buffer.getUint32(74, 2);
		TypeID typeIdL2 = typeL2code > 0 ? TypeID::P2 : TypeID::C2;

		int dL2code = buffer.getInt32(76, 14);
		int dL2phase = buffer.getInt32(90, 20);
		int cnrL2 = buffer.getUint32(117, 8);

		double codeL1 = prL1 * CODE_RES + L1CodeAmb * LIGTH_MS;
		double phaseL1 = codeL1 + dL1phase* PHASE_RES;
		double snrL1 = cnrL1 * 0.25;
		double snrL2 = cnrL2 * 0.25;

		typeValueMap tvm;
		if (satId.id == 16)
		{
			DBOUT("sv: " << satId << " ")
			DBOUT(typeIdL1 << " " << std::setprecision(12) << codeL1)
			DBOUT(" ph: " << std::setprecision(12) << phaseL1)
			DBOUT(" snr: " << std::setprecision(0) << snrL1)
			DBOUT(" "<<typeIdL2<<" ")
			DBOUT(" snr: " << std::setprecision(0) << snrL2)

			DBOUT_LINE("")
		}
		//DBOUT (" phase: "<<std::setprecision(12)<< phaseL1)
		

		//decoder.currEpoch();
		return true;
	}
}
