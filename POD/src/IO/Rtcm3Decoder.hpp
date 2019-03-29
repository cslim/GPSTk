#pragma once
#include"IdataSource.hpp"
#include"RinexEpoch.h"
#include"Rtcm3MessageBase.hpp"


namespace pod
{
	class Rtcm3MessageBase;
	typedef std::unique_ptr<Rtcm3MessageBase> rtcm3_msg_uptr;
	class Rtcm3Decoder
	{
	public:
		static const unsigned char PREAMBLE = 0b11010011;
		static const unsigned short MAX_MSG_LEN = 1024;

		Rtcm3Decoder(data_source_uptr ptr)
			:source(std::move(ptr)) {}
		~Rtcm3Decoder() = default;

		gpstk::irinex_uptr getObs();

	
	private:
		
		data_source_uptr source;
		std::set< rtcm3_msg_uptr> msgset;
	};

}
