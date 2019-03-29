#include"Rtcm3Decoder.hpp"
#include"RtcmUtils.hpp"
#include"BitSetProxy.hpp"

using namespace gpstk;

namespace pod
{

	gpstk::irinex_uptr Rtcm3Decoder::getObs()
	{
		irinex_uptr res(std::make_unique<RinexEpoch>());
		uchar buff[MAX_MSG_LEN];

		while (true)
		{
			uchar c = source->readByte();
			if (c == PREAMBLE)
			{
				std::cout << c <<  std::endl;
				uchar  lenchars[2]  ;
				source->readBytes(lenchars, 2);
				BitSetProxy bsp(lenchars, 0, 2);
				std::cout << bsp << std::endl;
				ushort len = bsp.getUint32(6, 10);
				//std::memcpy(&len, lenchars, 2);
				
				std::cout << (int)(lenchars[0]) <<' '<< (int)lenchars[1] << std::endl;
				std::cout << (len) << std::endl;
				if (len < MAX_MSG_LEN)
				{
					source->readBytes(buff, 3, len + 3);
					buff[0] = PREAMBLE;
					buff[1] = lenchars[0];
					buff[2] = lenchars[1];

					bool check = RtcmUtils::crc24q_check(buff, len + 6);
					std::cout << check << ": " << len << std::endl;

				}

			}
		}

		return res;
	}
}